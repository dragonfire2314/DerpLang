#include "compiler.h"
#include "scanner.h"

#include <iostream>
#include <unordered_map>
#include <string>

void print();
CallEntry lookupRule(Token_Type type);
void error(const char* message);
void number();
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

	{print, NULL, PREC_NONE},		//TOKEN_PRINT
	{localVar, NULL, PREC_NONE},	//TOKEN_VAR
	 
	{NULL, NULL, PREC_NONE},		//TOKEN_EOF
	{prefixVar, NULL, PREC_NONE},	//TOKEN_VAR_IDENTIFIER
	{NULL, NULL, PREC_NONE},		//TOKEN_ERROR
};

Parser parser;

std::unordered_map<std::string, uint16_t> stringToVarLocation;

Program compile(const char* data) 
{
	clearProgram();
	stringToVarLocation.clear();

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
	//Check if var already exists globally
	if (stringToVarLocation.find(name) != stringToVarLocation.end()) {
		//found
		error("Variable was redefined");
	}
	//Make the variable
	Variable v;
	v.type = VAR_NONE;
	createLocalVar(name, v);
	//Determine if data should be stored in the var
	if (parser.current.type == TOKEN_EQUAL) {
		consume(TOKEN_EQUAL, "Should not be displayed, lacalVar().");
		execute();
		emitOpCode(OP_STORE_LOCAL_VAR);
		uint16_t index;
		isLocalVar(name, index);
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

void prefixVar() 
{
	//Determine Var type
	std::string s(parser.previous.start, parser.previous.length);
	if (stringToVarLocation.find(s) == stringToVarLocation.end()) {
		uint16_t index;
		if (isLocalVar(s, index)) {
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
	if (parser.current.type == TOKEN_LEFT_BRACE) {
		compileBrace();
	}
	else //No braces 
	{
		execute();
		consume(TOKEN_SEMICOLON, "Semicolon not found.");
	}
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
	//Compile code in braces
	if (parser.current.type == TOKEN_LEFT_BRACE) {
		compileBrace();
	}
	else //No braces 
	{
		execute();
		consume(TOKEN_SEMICOLON, "Semicolon not found.");
	}

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
	if (parser.current.type == TOKEN_LEFT_BRACE) {
		compileBrace();
	}
	else //No braces 
	{
		execute();
		consume(TOKEN_SEMICOLON, "Semicolon not found.");
	}
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
	//Consume name token
	std::string name(parser.current.start, parser.current.length);
	consume(TOKEN_VAR_IDENTIFIER, "Expected function name.");
	//Consume open paren
	consume(TOKEN_LEFT_PAREN, "Function not fallowed by '('.");
	//Variables? Not doing this yet, will come back to
	//Consume close paren
	consume(TOKEN_RIGHT_PAREN, "Expected ')'.");
	//Add the function variable
	uint16_t i = addFunction(name.c_str());
	stringToVarLocation.insert({ name, i});
	//Compile expressions untill '}' is hit
	compileBrace();
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