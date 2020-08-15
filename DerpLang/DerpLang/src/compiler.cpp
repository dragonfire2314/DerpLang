#include "compiler.h"
#include "scanner.h"

#include <iostream>
#include <unordered_map>
#include <string>
#include <chrono>
#include <ctime> 

void print();
CallEntry lookupRule(Token_Type type);
void error(const char* message);
void number();
void string();
void print();
void binary();
void emitShort(uint16_t data);
uint32_t emitOpCode(Byte_Code opcode);
uint32_t emitByte(uint8_t byte);
void consume(Token_Type type, const char* message);
void execute();
void parsePrecedence(Precedence precedence);
void grouping();
void function();
void advance();
void declaration();
void brace();
void prefixVar();
void localVar();
void globalVar();
void prefixIf();
void prefixWhile();
void comparison();
void compileBrace();
void call();
void time();
bool isVarGlobal(std::string name);
bool doesVarExist(std::string name);
uint16_t getVarIndex(std::string name);

//Rule table
CallEntry rules[] = {
	{NULL, NULL, PREC_NONE},		//TOKEN_NONE
	{NULL, NULL, PREC_NONE},		//TOKEN_EQUAL

	{NULL, binary, PREC_TERM},		//TOKEN_PLUS
	{NULL, binary, PREC_TERM},		//TOKEN_MINUS
	{NULL, binary, PREC_FACTOR},	//TOKEN_STAR
	{NULL, binary, PREC_FACTOR},	//TOKEN_SLASH

	{prefixWhile, NULL, PREC_NONE},	//TOKEN_WHILE
	{NULL, NULL, PREC_NONE},		//TOKEN_FOR
	{prefixIf, NULL, PREC_NONE},	//TOKEN_IF
	{NULL, NULL, PREC_NONE},		//TOKEN_ELSE

	{NULL, NULL, PREC_NONE},		//TOKEN_FUNCTION
	{grouping, NULL, PREC_NONE},	//TOKEN_LEFT_PAREN
	{NULL, NULL, PREC_NONE},		//TOKEN_RIGHT_PAREN
	{brace, NULL, PREC_NONE},		//TOKEN_LEFT_BRACE
	{NULL, NULL, PREC_NONE},		//TOKEN_RIGHT_BRACE
	{NULL, NULL, PREC_NONE},		//TOKEN_SEMICOLON

	{NULL, comparison, PREC_EQUALITY},//TOKEN_EQUALIY
	{NULL, comparison, PREC_EQUALITY},//TOKEN_NOT_EQUAL

	{number, NULL, PREC_NONE},		//TOKEN_NUMBER
	{string, NULL, PREC_NONE},		//TOKEN_STRING

	{print, NULL, PREC_NONE},		//TOKEN_PRINT
	{call, NULL, PREC_NONE},		//TOKEN_CALL
	{time, NULL, PREC_NONE},		//TOKEN_TIME

	{localVar, NULL, PREC_NONE},	//TOKEN_VAR
	 
	{NULL, NULL, PREC_NONE},		//TOKEN_EOF
	{prefixVar, NULL, PREC_NONE},	//TOKEN_VAR_IDENTIFIER
	{NULL, NULL, PREC_NONE},		//TOKEN_ERROR
};

Parser parser;

std::unordered_map<std::string, uint16_t> stringToVarLocation;
std::vector<std::unordered_map<std::string, uint16_t>> localVarNameToIndexArray;

Program compile(const char* data) 
{
	clearProgram();
	for (auto x : localVarNameToIndexArray)
		x.clear();
	stringToVarLocation.clear();
	localVarNameToIndexArray.clear();

	parser.isError = false;

	InitScanner(data);
	//Init the parser
	parser.current = scanToken();
	//Call execute untill eof

	//Compile all declarations

	for (;;) {
		declaration();
		if (parser.current.type == TOKEN_EOF)
			break;
	}

	return getProgram();
}

void declaration() 
{
	switch (parser.current.type) 
	{
	case TOKEN_FUNCTION: advance(); function(); break;
	case TOKEN_VAR: advance(); globalVar();
	}
}

bool checkErrorStatus() 
{
	return parser.isError;
}

void advance() 
{
	parser.previous = parser.current;

	parser.current = scanToken();
	if (parser.current.type == TOKEN_ERROR) {
		error("Error Token was scanned");
	}
}

void consume(Token_Type type, const char* message) 
{
	if (parser.current.type == type) {
		advance();
		return;
	}

	error(message);
}

void error(const char* message)
{
	parser.isError = true;
	printf("An error has occured on line %i: %s\n", getLine(), message);
}

void execute() 
{
	parsePrecedence(PREC_ASSIGNMENT);
}

void parsePrecedence(Precedence precedence)
{
	//Call prefix
	advance();
	fnCall prefixEntry = lookupRule(parser.previous.type).prefix;
	if (prefixEntry == NULL) {
		//Error stop compiling and report problem
		error("Prefix error!");
	}

	prefixEntry();

	//Call the infix if precedence is valid
	while (precedence <= lookupRule(parser.current.type).precedence) {
		advance();
		fnCall infixEntry = lookupRule(parser.previous.type).infix;
		infixEntry();
	}
}

CallEntry lookupRule(Token_Type type)
{
	return rules[type];
}

/*********************************************
	Functions for tokens
*********************************************/
void print() 
{
	//Compile the next operand
	execute();

	//Emit the opcode for print
	emitOpCode(OP_PRINT);
}

void localVar() 
{
	//Consume name
	std::string name(parser.current.start, parser.current.length);
	consume(TOKEN_VAR_IDENTIFIER, "Expected an indentifier.");
	//Make the variable
	Variable v;
	v.type = VAR_NONE;
	//Check if var already exists
	if (doesVarExist(name)) {
		error("Variable redefinition.");
	}
	else {
		//Create the variable and store the reference
		localVarNameToIndexArray.back().insert({ name, createLocalVar(v) });
	}
	//Determine if data should be stored in the var
	if (parser.current.type == TOKEN_EQUAL) {
		consume(TOKEN_EQUAL, "Should not be displayed, lacalVar().");
		execute();
		emitOpCode(OP_STORE_LOCAL_VAR);
		uint16_t index = getVarIndex(name);
		emitShort(index);
	}
}

void number() 
{
	//Get the constant
	double value = strtof(parser.previous.start, NULL);
	//Emit the opcode
	emitOpCode(OP_CONSTANT);
	//Store the constant in the chunk
	uint16_t loc = writeChunkConstantData(makeVariable(value));
	//Emit the loaction in the chunk constant data
	emitShort(loc);
}

void string() 
{
	std::string s(parser.previous.start, parser.previous.length - 1);
	emitOpCode(OP_CONSTANT);

	uint16_t loc = writeChunkConstantData(makeVariable(s));
	emitShort(loc);
}

void binary() 
{
	Token_Type operator_type = parser.previous.type;

	Precedence p = lookupRule(parser.previous.type).precedence;
	parsePrecedence((Precedence)(p + 1));

	switch (operator_type) {
		case TOKEN_PLUS:	emitOpCode(OP_ADD);		break;
		case TOKEN_MINUS:	emitOpCode(OP_SUB);		break;
		case TOKEN_STAR:	emitOpCode(OP_MULT);	break;
		case TOKEN_SLASH:	emitOpCode(OP_DIV);		break;
		default:
			error("unreachable code has been hit in Binary().");
	}
}

void call() 
{
	//consume(TOKEN_CALL, "Should not happen call()");
	std::string s(parser.current.start, parser.current.length);
	consume(TOKEN_VAR_IDENTIFIER, "No variable identifier was found");

	if (doesVarExist(s)) {
		if (isVarGlobal(s)) {
			emitOpCode(OP_LOAD_GLOBAL_VAR);
			emitShort(stringToVarLocation[s]);
		}
		else {
			error("Call made to non function");
		}
	}
	else {
		error("function does not exist");
	}
	emitOpCode(OP_CALL);
}

void time() 
{
	//Emit the opcode
	emitOpCode(OP_TIME);
}

void prefixVar() 
{
	//Determine Var type
	std::string s(parser.previous.start, parser.previous.length);
	if (stringToVarLocation.find(s) == stringToVarLocation.end()) {
		if (doesVarExist(s)) {
			uint16_t index = getVarIndex(s);
			//Is local
			//Check if next token is an equallity
			if (parser.current.type == TOKEN_EQUAL) { //Storing
				consume(TOKEN_EQUAL, "Should never happen prefixVar().");
				execute();
				emitOpCode(OP_STORE_LOCAL_VAR);
				emitShort(index);
			}
			else { //Loading
				emitOpCode(OP_LOAD_LOCAL_VAR);
				emitShort(index);
			}
		}
		else error("The variable was not declared");
	}
	else {
		//Is global
		if (parser.current.type == TOKEN_EQUAL) { //Storing
			consume(TOKEN_EQUAL, "Should never happen prefixVar().");
			execute();
			emitOpCode(OP_STORE_GLOBAL_VAR);
			emitShort(stringToVarLocation[s]);
		}
		else { //Loading
			emitOpCode(OP_LOAD_GLOBAL_VAR);
			emitShort(stringToVarLocation[s]);
		}
	}
}

void prefixWhile() 
{
	uint32_t topOfWhile = getChuckByteCodeCurrentLocation();
	//Check for left paren
	consume(TOKEN_LEFT_PAREN, "Expected '('");
	execute();
	//Check for right paren
	consume(TOKEN_RIGHT_PAREN, "Expected ')'");
	//Emit the branch if false opcode and location
	emitOpCode(OP_BRANCH_ON_FALSE);
	uint32_t highLoc = emitByte(0x00);
	uint32_t lowLoc = emitByte(0x00);
	uint32_t currentLoc;
	//Check for left brace
	compileBrace();
	//Add jump back to top of while
	emitOpCode(OP_JUMP);
	emitByte((topOfWhile & 0xFF00) << 8);
	emitByte(topOfWhile & 0xFF);
	//Fix branch location
	currentLoc = getChuckByteCodeCurrentLocation();
	writeChunkByteCodeLocation((currentLoc & 0xFF00) >> 8, highLoc);
	writeChunkByteCodeLocation(currentLoc & 0xFF, lowLoc);
}

void prefixIf()
{
	consume(TOKEN_LEFT_PAREN, "Expected '('");
	//Compile condition
	execute();
	consume(TOKEN_RIGHT_PAREN, "Expected ')'");
	//Emit the branch if false opcode and location
	emitOpCode(OP_BRANCH_ON_FALSE);
	uint32_t highLoc = emitByte(0x00);
	uint32_t lowLoc = emitByte(0x00);
	uint32_t currentLoc;
	//Compile code in braces
	compileBrace();

	//Check for else
	if (parser.current.type != TOKEN_ELSE) {
		//Set branch location to here when else dosen't exist
		currentLoc = getChuckByteCodeCurrentLocation();
		writeChunkByteCodeLocation((currentLoc & 0xFF00) >> 8, highLoc);
		writeChunkByteCodeLocation(currentLoc & 0xFF, lowLoc);
		return;
	}
	//Is else
	uint16_t lowOld = lowLoc;
	uint16_t highOld = highLoc;
	//Push code to jump over else
	emitOpCode(OP_JUMP);
	highLoc = emitByte(0x00);
	lowLoc = emitByte(0x00);
	//Fix location for else jump
	currentLoc = getChuckByteCodeCurrentLocation();
	writeChunkByteCodeLocation((currentLoc & 0xFF00) >> 8, highOld);
	writeChunkByteCodeLocation(currentLoc & 0xFF, lowOld);
	//Consume the stuff for the else and compile it
	consume(TOKEN_ELSE, "Should not happen perfixIf().");
	compileBrace();
	//Set jump location to here
	currentLoc = getChuckByteCodeCurrentLocation();
	writeChunkByteCodeLocation((currentLoc & 0xFF00) >> 8, highLoc);
	writeChunkByteCodeLocation(currentLoc & 0xFF, lowLoc);
}

void comparison() 
{
	Token_Type operator_type = parser.previous.type;

	Precedence p = lookupRule(parser.previous.type).precedence;
	parsePrecedence((Precedence)(p + 1));

	switch (operator_type) {
	case TOKEN_EQUALIY:		emitOpCode(OP_EQUAL);		break;
	case TOKEN_NOT_EQUAL:	emitOpCode(OP_NOT_EQUAL);	break;
	default:
		error("unreachable code has been hit in comparison().");
	}
}

void function() 
{
	std::string name(parser.current.start, parser.current.length);
	consume(TOKEN_VAR_IDENTIFIER, "Expected function name.");
	consume(TOKEN_LEFT_PAREN, "Function not fallowed by '('.");
	//Variables? Not doing this yet, will come back to

	consume(TOKEN_RIGHT_PAREN, "Expected ')'.");
	//Add the function variable
	uint16_t i = addFunction(name.c_str());
	stringToVarLocation.insert({ name, i});
	//Compile expressions untill '}' is hit
	compileBrace();

	emitOpCode(OP_RETURN);
}

void globalVar() 
{
	//Consume name token
	const char* name = parser.current.start;
	int name_length = parser.current.length;
	int varLoc;
	Variable v;
	consume(TOKEN_VAR_IDENTIFIER, "Expected variable name name.");
	//Check if a value should be assigned
	if (parser.current.type == TOKEN_EQUAL) {
		advance();
		switch (parser.current.type) {
		case TOKEN_NUMBER: 
			v.type = VAR_DOUBLE;
			v.data.number = strtod(parser.current.start, NULL);
			break;
		}
		advance();
	} else {
		v.type = VAR_NONE;
	}

	uint16_t i = addVariable(v);
	stringToVarLocation.insert({ std::string(name, name_length), i });

	consume(TOKEN_SEMICOLON, "Expected ';'");
}

void compileBrace()
{
	//Because we are entering a deeper scope the local variable vector map must be updated
	std::unordered_map<std::string, uint16_t> m;
	localVarNameToIndexArray.push_back(m);


	if (parser.current.type == TOKEN_LEFT_BRACE) {
		//Consume open brace
		consume(TOKEN_LEFT_BRACE, "Expected '{'.");
		while (parser.current.type != TOKEN_RIGHT_BRACE)
		{
			switch (parser.current.type) {
			case TOKEN_WHILE:	consume(TOKEN_WHILE, "expected while?");	prefixWhile();	break;
			case TOKEN_IF:	consume(TOKEN_IF, "expected if?");	prefixIf();		break;
			default:
				execute();
				consume(TOKEN_SEMICOLON, "Semicolon not found.");
			}
		}
		consume(TOKEN_RIGHT_BRACE, "Expected '}'.");
	}
	else //No braces 
	{
		execute();
		if (parser.current.type != TOKEN_RIGHT_BRACE)
			consume(TOKEN_SEMICOLON, "Semicolon not found.");
	}

	//Since we have returned from a brace we must pop the variables off the vector
	localVarNameToIndexArray.pop_back();
}

void grouping() 
{
	execute();
	consume(TOKEN_RIGHT_PAREN, "Right paran not found.");
}

void brace() 
{
	execute();
	consume(TOKEN_RIGHT_BRACE, "Right brace not found.");
}

/*********************************************
	Variable maps
*********************************************/
bool isVarGlobal(std::string name)
{
	if (stringToVarLocation.find(name) != stringToVarLocation.end()) {
		//found
		return true;
	}
	return false;
}

bool doesVarExist(std::string name)
{ 
	//Check through all variable lists starting with the highest scope
	if (stringToVarLocation.find(name) != stringToVarLocation.end()) {
		//found
		return true;
	}
	for (auto x : localVarNameToIndexArray) {
		if (x.find(name) == x.end()) {
			continue;
		}
		else {
			return true;
		}
	}
	return false;
}

uint16_t getVarIndex(std::string name)
{
	for (auto x : localVarNameToIndexArray) {
		if (x.find(name) == x.end()) {
			continue;
		}
		else {
			return x[name];
		}
	}
	return 0;
}

/*********************************************
	Emiting functions
*********************************************/
void emitShort(uint16_t data)
{
	emitByte((data & 0xFF00) >> 8);
	emitByte(data & 0xFF);
}

uint32_t emitOpCode(Byte_Code opcode)
{
	return emitByte(opcode);
}

uint32_t emitByte(uint8_t byte) 
{
	return writeChunkByteCode(byte);
}