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

	std::ifstream i("../../Tests/Variables/vars1.dlang");
	std::string input((std::istreambuf_iterator<char>(i)),
		std::istreambuf_iterator<char>());
	std::ifstream o("../../Tests/Variables/Expected/vars1");
	std::string expected((std::istreambuf_iterator<char>(o)),
		std::istreambuf_iterator<char>());

	//Compiler
	Program p = compile(input.c_str());

	//Virtual Machine
	EXPECT_FALSE(checkErrorStatus());

	std::string ret = runProgram(p);

	EXPECT_EQ(ret, expected);
}

TEST(Variables, vars2) {

	std::ifstream i("../../Tests/Variables/vars2.dlang");
	std::string input((std::istreambuf_iterator<char>(i)),
		std::istreambuf_iterator<char>());
	std::ifstream o("../../Tests/Variables/Expected/vars2");
	std::string expected((std::istreambuf_iterator<char>(o)),
		std::istreambuf_iterator<char>());

	//Compiler
	Program p = compile(input.c_str());

	//Virtual Machine
	EXPECT_FALSE(checkErrorStatus());

	std::string ret = runProgram(p);

	EXPECT_EQ(ret, expected);
}

TEST(Variables, vars3) {

	std::ifstream i("../../Tests/Variables/vars3.dlang");
	std::string input((std::istreambuf_iterator<char>(i)),
		std::istreambuf_iterator<char>());
	std::ifstream o("../../Tests/Variables/Expected/vars3");
	std::string expected((std::istreambuf_iterator<char>(o)),
		std::istreambuf_iterator<char>());

	//Compiler
	Program p = compile(input.c_str());

	//Virtual Machine
	EXPECT_FALSE(checkErrorStatus());

	std::string ret = runProgram(p);

	EXPECT_EQ(ret, expected);
}

TEST(Control_Flow, whileTest) {

	std::ifstream i("../../Tests/Control_Flow/while.dlang");
	std::string input((std::istreambuf_iterator<char>(i)),
		std::istreambuf_iterator<char>());
	std::ifstream o("../../Tests/Control_Flow/Expected/while");
	std::string expected((std::istreambuf_iterator<char>(o)),
		std::istreambuf_iterator<char>());

	//Compiler
	Program p = compile(input.c_str());

	//Virtual Machine
	EXPECT_FALSE(checkErrorStatus());

	std::string ret = runProgram(p);

	EXPECT_EQ(ret, expected);
}

TEST(Control_Flow, ifelseTest) {

	std::ifstream i("../../Tests/Control_Flow/if_else.dlang");
	std::string input((std::istreambuf_iterator<char>(i)),
		std::istreambuf_iterator<char>());
	std::ifstream o("../../Tests/Control_Flow/Expected/if_else");
	std::string expected((std::istreambuf_iterator<char>(o)),
		std::istreambuf_iterator<char>());

	//Compiler
	Program p = compile(input.c_str());

	//Virtual Machine
	EXPECT_FALSE(checkErrorStatus());

	std::string ret = runProgram(p);

	EXPECT_EQ(ret, expected);
}