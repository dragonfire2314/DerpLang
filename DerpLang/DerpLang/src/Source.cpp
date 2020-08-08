#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>

#include "compiler.h"
#include "derpLangVM.h"

int main(int argc, char* argv[])
{
	//Simple line compiler
	//std::cout << "Enter line to be compiled: " << std::endl;

	//std::getline(std::cin, input);

	std::ifstream t("test.dlang");
	std::string input((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());

	printf("%s\n\n\n", input.c_str());

	//Compiler
	Program p = compile(input.c_str());

	//Virtual Machine
	if (!checkErrorStatus()) {
		std::string ret = runProgram(p);
		printf("%s", ret.c_str());
	}
}