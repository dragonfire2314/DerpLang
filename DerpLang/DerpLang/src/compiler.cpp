#include "compiler.h"
#include "scanner.h"

#include <iostream>

void print();
CallEntry lookupRule(Token_Type type);
void error(const char* message);
void number();
void print();
void binary();
void emitShort(uint16_t data);
void emitOpCode(Byte_Code opcode);
void emitByte(uint8_t byte);
void consume(Token_Type type, const char* message);
void execute();
void parsePrecedence(Precedence precedence);

//Rule table
CallEntry rules[] = {
	{NULL, NULL, PREC_NONE},		//TOKEN_NONE

	{NULL, binary, PREC_TERM},		//TOKEN_PLUS
	{NULL, binary, PREC_TERM},		//TOKEN_MINUS
	{NULL, binary, PREC_FACTOR},	//TOKEN_STAR
	{NULL, binary, PREC_FACTOR},	//TOKEN_SLASH

	{NULL, NULL, PREC_NONE},		//TOKEN_WHILE
	{NULL, NULL, PREC_NONE},		//TOKEN_FOR
	{NULL, NULL, PREC_NONE},		//TOKEN_IF
	{NULL, NULL, PREC_NONE},		//TOKEN_ELSE

	{NULL, NULL, PREC_NONE},		//TOKEN_FUNCTION
	{NULL, NULL, PREC_NONE},		//TOKEN_LEFT_PAREN
	{NULL, NULL, PREC_TERM},		//TOKEN_RIGHT_PAREN
	{NULL, NULL, PREC_NONE},		//TOKEN_LEFT_BRACE
	{NULL, NULL, PREC_NONE},		//TOKEN_RIGHT_BRACE
	{NULL, NULL, PREC_NONE},		//TOKEN_SEMICOLON

	{number, NULL, PREC_NONE},		//TOKEN_NUMBER

	{print, NULL, PREC_NONE},		//TOKEN_PRINT
	 
	{NULL, NULL, PREC_NONE},		//TOKEN_EOF
	{NULL, NULL, PREC_NONE},		//TOKEN_GENERAL_ID
	{NULL, NULL, PREC_NONE},		//TOKEN_ERROR
};

Parser parser;

Chunk compile(const char* data) 
{
	InitScanner(data);
	initChunk();
	//Init the parser
	parser.current = scanToken();
	//Call execute untill eof

	for (;;) {
		//parse the line
		execute();
		//Expected semi colon at the end
		consume(TOKEN_SEMICOLON, "Semicolon not found.");
		if (parser.current.type == TOKEN_EOF)
			break;
	}

	return getChunk();
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
	printf("An error has occured: %s\n", message);
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

void number() 
{
	//Get the constant
	float value = strtof(parser.previous.start, NULL);
	//Emit the opcode
	emitOpCode(OP_CONSTANT);
	//Store the constant in the chunk
	uint16_t loc = writeChunkConstantData((uint32_t)value);
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

/*********************************************
	Emiting functions
*********************************************/
void emitShort(uint16_t data)
{
	emitByte((data & 0xFF00) >> 8);
	emitByte(data & 0xFF);
}

void emitOpCode(Byte_Code opcode)
{
	emitByte(opcode);
}

void emitByte(uint8_t byte) 
{
	writeChunkByteCode(byte);
}