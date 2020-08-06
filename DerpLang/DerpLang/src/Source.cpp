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
	Chunk c = compile(input.c_str());

	std::vector<uint8_t> p = generateByteCode();
	for (int i = 0; i < p.size(); i++)
		printf("%02x ", p[i]);

	//Virtual Machine
	/*if (!checkErrorStatus()) {
		std::string ret = runChunk(c);
		printf("%s", ret.c_str());
	}*/
}