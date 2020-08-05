#include "scanner.h"

#include <map>

Token makeToken(Token_Type type);
Token errorToken(const char* message);
bool isEOF();
void skipWhitespace();
char advance();
char peek();
char peekNext();
bool isDigit(char c);
Token number();
Token identifier();
Token_Type identifierType();
bool isAlpha(char c);

struct Scanner {
	const char* start;
	const char* current;
	int line;
};

Scanner scanner;

std::map<std::string, Token_Type> identifierMap;


//Inits the scanner
void InitScanner(const char* line)
{
	scanner.start = line;
	scanner.current = line;
	scanner.line = 1;
	//Build the identifier map
	identifierMap.insert(std::pair<std::string, Token_Type>("while",    TOKEN_WHILE));
	identifierMap.insert(std::pair<std::string, Token_Type>("for",      TOKEN_FOR));
	identifierMap.insert(std::pair<std::string, Token_Type>("if",       TOKEN_IF));
	identifierMap.insert(std::pair<std::string, Token_Type>("else",     TOKEN_ELSE));
	identifierMap.insert(std::pair<std::string, Token_Type>("function", TOKEN_FUNCTION));
	identifierMap.insert(std::pair<std::string, Token_Type>("print",    TOKEN_PRINT));
}

//Scans for the next token
Token scanToken() 
{
	skipWhitespace();

	scanner.start = scanner.current;

	if (isEOF())
		return makeToken(TOKEN_EOF);

	char c = advance();

	if (isAlpha(c)) return identifier();
	if (isDigit(c)) return number();

	//Single character tokens
	switch (c) {
	case '(': return makeToken(TOKEN_LEFT_PAREN);
	case ')': return makeToken(TOKEN_RIGHT_PAREN);
	case '+': return makeToken(TOKEN_PLUS);
	case '-': return makeToken(TOKEN_MINUS);
	case '/': return makeToken(TOKEN_SLASH);
	case '*': return makeToken(TOKEN_STAR);
	case ';': return makeToken(TOKEN_SEMICOLON);
	}

	return errorToken("No matching token type.");
}

//Parses for an identifier
Token identifier() 
{
	while (isAlpha(peek()) || isDigit(peek()))
		advance();

	return makeToken(identifierType());
}

Token_Type identifierType() 
{
	std::string s;
	for (int i = 0; i < scanner.current - scanner.start; i++)
		s.push_back(scanner.start[i]);
	if (identifierMap.find(s) == identifierMap.end()) {
		// not found
		return TOKEN_GENERAL_ID;
	}
	else {
		// found
		return identifierMap[s];
	}
}

//Parses a number and makes it into a token
Token number() 
{
	//Find number up to possible '.'
	while (isDigit(peek()))
		advance();
	//Find possible '.'
	if (peek() == '.' && isDigit(peekNext()))
		advance();
	//Find remaining number
	while (isDigit(peek()))
		advance();

	return makeToken(TOKEN_NUMBER);
}

//Checks if something is a alhpa charater
bool isAlpha(char c) 
{
	return (
		(c >= 'a' && c <= 'z') ||
		(c >= 'A' && c <= 'Z') ||
		c == '_'
	);
}

//Checks if someting is a digit
bool isDigit(char c) 
{
	return (c >= '0' && c <= '9');
}

//Advances to the next charater and returns it
char advance() 
{
	scanner.current++;
	return scanner.current[-1];
}

//returns the next charater
char peek()
{
	return scanner.current[0];
}

//returns 2 characters ahead
char peekNext() 
{
	if (isEOF()) return '\0';
	return scanner.current[1];
}

//Loops thourgh and removes all whitespace
void skipWhitespace() 
{
	for (;;) {
		char c = peek();
		switch (c) {
		case ' ':
		case '\r':
		case '\t':
			advance();
			break;
		//NewLine
		case '\n':
			scanner.line++;
			advance();
			break;
		//Comment removal
		case '/':
			if (peekNext() == '/') {
				while (peek() != '\n' && !isEOF()) advance();
			}
			else {
				return;
			}
			break;
		default:
			return;
		}
	}
}

//Checks if we have reached the end of the file
bool isEOF()
{
	return (*scanner.current == '\0');
}

//Builds a token with a provided type
Token makeToken(Token_Type type) 
{
	Token t;
	t.start = scanner.start;
	t.length = scanner.current - scanner.start;
	t.line = scanner.line;
	t.type = type;
	return t;
}

//Creates an error token with a message
Token errorToken(const char* message)
{
	Token t;
	t.start = message;
	t.length = (int)strlen(message);
	t.line = scanner.line;
	t.type = TOKEN_ERROR;
	return t;
}