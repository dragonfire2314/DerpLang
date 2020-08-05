#include <iostream>
#include <string>

#include "compiler.h"
#include "derpLangVM.h"

int main(int argc, char* argv[])
{
	//Simple line compiler
	std::cout << "Enter line to be compiled: " << std::endl;

	std::string input;
	std::getline(std::cin, input);

	//Compiler
	Chunk c = compile(input.c_str());
	//Virtual Machine
	std::string ret = runChunk(c);

	printf("%s", ret.c_str());
}