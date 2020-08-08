#include "pch.h"
#include "../DerpLang/src/compiler.h"
#include "../DerpLang/src/scanner.h"
#include "../DerpLang/src/chunk.h"
#include "../DerpLang/src/derpLangVM.h"

#include <string>
#include <fstream>
#include <streambuf>

TEST(Basic, print) {

	std::ifstream t("../../Tests/Basic/print.dlang");
	std::string input((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());

	//Compiler
	Program p = compile(input.c_str());

	//Virtual Machine
	EXPECT_FALSE(checkErrorStatus());

	std::string ret = runProgram(p);

	std::string expected = "1.000000\n";

	EXPECT_EQ(ret, expected);
}

TEST(Basic, print_mult) {

	std::ifstream t("../../Tests/Basic/print_mult.dlang");
	std::string input((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());

	//Compiler
	Program p = compile(input.c_str());

	//Virtual Machine
	EXPECT_FALSE(checkErrorStatus());

	std::string ret = runProgram(p);

	std::string expected = "9.000000\n";

	EXPECT_EQ(ret, expected);
}

TEST(Variables, vars1) {

	std::ifstream t("../../Tests/Variables/vars1.dlang");
	std::string input((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());

	//Compiler
	Program p = compile(input.c_str());

	//Virtual Machine
	EXPECT_FALSE(checkErrorStatus());

	std::string ret = runProgram(p);

	std::string expected = "2.000000\n";

	EXPECT_EQ(ret, expected);
}

TEST(Variables, vars2) {

	std::ifstream t("../../Tests/Variables/vars2.dlang");
	std::string input((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());

	//Compiler
	Program p = compile(input.c_str());

	//Virtual Machine
	EXPECT_FALSE(checkErrorStatus());

	std::string ret = runProgram(p);

	std::string expected = "4.000000\n8.000000\n";

	EXPECT_EQ(ret, expected);
}