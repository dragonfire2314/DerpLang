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
    EXPECT_EQ(scanToken().type, TOKEN_GENERAL_ID);
    EXPECT_EQ(scanToken().type, TOKEN_MINUS);
    EXPECT_EQ(scanToken().type, TOKEN_NUMBER);
    EXPECT_EQ(scanToken().type, TOKEN_SEMICOLON);
}

TEST(SingleLine, print_addition) {
    const char* program = "print 1 + 5;";

    Chunk c = compile(program);

    EXPECT_EQ(c.byteCode[0], OP_CONSTANT);
    EXPECT_EQ(c.byteCode[3], OP_CONSTANT);
    EXPECT_EQ(c.byteCode[6], OP_ADD);
    EXPECT_EQ(c.byteCode[7], OP_PRINT);

    EXPECT_EQ(runChunk(c), "6.000000\n");
}

TEST(SingleLine, multiple_binary_operators) {
    const char* program = "print 1 + 2 + 3 + 4;";

    Chunk c = compile(program);

    EXPECT_EQ(runChunk(c), "10.000000\n");
}