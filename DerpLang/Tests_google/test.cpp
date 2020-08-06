#include "pch.h"
#include "../DerpLang/src/compiler.h"
#include "../DerpLang/src/scanner.h"
#include "../DerpLang/src/chunk.h"
#include "../DerpLang/src/derpLangVM.h"

#include <string>
#include <fstream>
#include <streambuf>
TEST(Scanner, print_addition) {
    const char* program = "print 1 + 2;";
    
    InitScanner(program);

    EXPECT_EQ(scanToken().type, TOKEN_PRINT);
    EXPECT_EQ(scanToken().type, TOKEN_NUMBER);
    EXPECT_EQ(scanToken().type, TOKEN_PLUS);
    EXPECT_EQ(scanToken().type, TOKEN_NUMBER);
    EXPECT_EQ(scanToken().type, TOKEN_SEMICOLON);
}

TEST(Scanner, print_subtration) {
    const char* program = "print apple - 2;";

    InitScanner(program);

    EXPECT_EQ(scanToken().type, TOKEN_PRINT);
    EXPECT_EQ(scanToken().type, TOKEN_VAR_IDENTIFIER);
    EXPECT_EQ(scanToken().type, TOKEN_MINUS);
    EXPECT_EQ(scanToken().type, TOKEN_NUMBER);
    EXPECT_EQ(scanToken().type, TOKEN_SEMICOLON);
}

TEST(Scanner, paren) {
    const char* program = "(1 + 2)";

    InitScanner(program);

    EXPECT_EQ(scanToken().type, TOKEN_LEFT_PAREN);
    EXPECT_EQ(scanToken().type, TOKEN_NUMBER);
    EXPECT_EQ(scanToken().type, TOKEN_PLUS);
    EXPECT_EQ(scanToken().type, TOKEN_NUMBER);
    EXPECT_EQ(scanToken().type, TOKEN_RIGHT_PAREN);
}

TEST(Single_function, multiple_binary_operators) {
    const char* program = "function test(){print 1 + 2 + 3 + 4;}";

    Chunk c = compile(program);

    EXPECT_FALSE(checkErrorStatus());
    EXPECT_EQ(runChunk(c), "10.000000\n");
}