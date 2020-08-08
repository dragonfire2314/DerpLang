#pragma once

#include <stdint.h>
#include <vector>
#include <string>

enum Byte_Code {
	OP_NOP,
	//Arithmetic
	OP_ADD,
	OP_SUB,
	OP_DIV,
	OP_MULT,
	//Constants
	OP_CONSTANT, //3 bytes (OP_CONSTANT w/ 2 byes as look up for value)
	//Variable
	OP_LOAD_GLOBAL_VAR,
	OP_LOAD_LOCAL_VAR,
	OP_STORE_LOCAL_VAR,
	//Functions
	OP_PRINT
	//Control
};

enum Token_Type {
	TOKEN_NONE,
	TOKEN_EQUAL,

	//Binary
	TOKEN_PLUS, TOKEN_MINUS, TOKEN_STAR, TOKEN_SLASH,

	//Unary

	//Flow
	TOKEN_WHILE, TOKEN_FOR, TOKEN_IF, TOKEN_ELSE,

	//Structure
	TOKEN_FUNCTION,
	TOKEN_LEFT_PAREN,
	TOKEN_RIGHT_PAREN,
	TOKEN_LEFT_BRACE,
	TOKEN_RIGHT_BRACE,
	TOKEN_SEMICOLON,

	//Constant
	TOKEN_NUMBER,

	//Commands
	TOKEN_PRINT,

	//Variables
	TOKEN_VAR,

	//Other
	TOKEN_EOF,
	TOKEN_VAR_IDENTIFIER,
	TOKEN_ERROR
};

struct Token {
	Token_Type type;
	const char* start;
	int length;
	int line;
};

enum Precedence {
	PREC_NONE,
	PREC_ASSIGNMENT,  // =
	PREC_OR,          // or
	PREC_AND,         // and
	PREC_EQUALITY,    // == !=
	PREC_COMPARISON,  // < > <= >=
	PREC_TERM,        // + -
	PREC_FACTOR,      // * /
	PREC_UNARY,       // ! -
	PREC_CALL,        // . ()
	PREC_PRIMARY
};

typedef void (*fnCall)();

struct CallEntry {
	fnCall prefix;
	fnCall infix;
	Precedence precedence;
};

struct Parser {
	Token current;
	Token previous;

	bool isError = false;
};

/**************************************
	Variable Stuff
**************************************/

#include "variable.h"

struct Program {
	std::vector<Variable> vars;
	uint32_t mainIndex;
};

struct Chunk {
	std::vector<uint8_t> byteCode;
	std::vector<Variable> constantData;
	std::vector<Variable> variableData;
};

struct String : Obj {
	int length;
	char* data;
};

struct Function : Obj
{
	Chunk chunk;

	std::string name;
};
