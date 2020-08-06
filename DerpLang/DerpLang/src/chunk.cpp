#include "chunk.h"
#include "global.h"

#include <vector>

std::vector<Variable> vars;

Chunk* currentChunk;

uint16_t addVariable(Variable v)
{
	vars.push_back(v);
	return vars.size() - 1;
}

uint16_t addFunction(const char* name)
{
	Variable v;
	v.type = VAR_OBJ;
	v.data.obj = new Function();
	((Function*)v.data.obj)->name = name;
	((Function*)v.data.obj)->type = OBJ_FUNCTION;
	currentChunk = &((Function*)v.data.obj)->chunk;

	return addVariable(v);
}

void initChunk() 
{
	
}

Chunk getChunk() 
{
	return *currentChunk;
}

void writeChunkByteCode(uint8_t data)
{
	currentChunk->byteCode.push_back(data);
}

uint16_t writeChunkConstantData(uint32_t data)
{
	if (currentChunk->constantData.size() + 1 > UINT16_MAX) {
		printf("Error the constant data storage for this chunk is full.\n");
	}
	currentChunk->constantData.push_back(data);
	return (uint16_t)(currentChunk->constantData.size() - 1);
}




/************************************
	Byte Code output generator
************************************/
void copyStandardVariable(std::vector<uint8_t>* dest, Variable* src);
void copyFunctionObject(std::vector<uint8_t>* dest, Variable* src);

std::vector<uint8_t> generateByteCode() 
{
	std::vector<uint8_t> ret;

	for (auto x : vars) 
	{
		if (x.type == VAR_OBJ)
		{
			switch (x.data.obj->type) 
			{
			case OBJ_FUNCTION:
				copyFunctionObject(&ret, &x);
				break;
			case OBJ_STRING:
				break;
			}
		}
		else 
		{
			//Copy var to output array
			copyStandardVariable(&ret, &x);
		}
	}

	return ret;
}

/*
Function btyecode header as fallows
	-4Bytes (Offset to code)
	-4Bytes (Offset to constant data)
	-?Bytes (cstring of funtion name)
*/
void copyFunctionObject(std::vector<uint8_t>* dest, Variable* src) 
{
	//Push back header for the function
	std::string str = ((Function*)src->data.obj)->name;
	uint32_t codeOffset = 4 + str.length();
	//Code Offset
	dest->push_back((codeOffset & 0xFF000000) >> 24);
	dest->push_back((codeOffset & 0x00FF0000) >> 16);
	dest->push_back((codeOffset & 0x0000FF00) >> 8);
	dest->push_back((codeOffset & 0x000000FF));
	//Const Offset
	uint32_t constOffset = codeOffset + ((Function*)src->data.obj)->chunk.byteCode.size();
	dest->push_back((constOffset & 0xFF000000) >> 24);
	dest->push_back((constOffset & 0x00FF0000) >> 16);
	dest->push_back((constOffset & 0x0000FF00) >> 8);
	dest->push_back((constOffset & 0x000000FF));
	//Name
	for (int i = 0; i < str.length(); i++)
	{
		dest->push_back(str[i]);
	}
	//Push back the code
	Chunk* c = &((Function*)src->data.obj)->chunk;
	for (int i = 0; i < c->byteCode.size(); i++)
	{
		dest->push_back(c->byteCode[i]);
	}
	//Push back the constant data
	for (int i = 0; i < c->constantData.size(); i++)
	{
		dest->push_back((uint8_t)(((uint32_t)(c->constantData[i])) & 0xFF000000) >> 24);
		dest->push_back((uint8_t)(((uint32_t)(c->constantData[i])) & 0x00FF0000) >> 16);
		dest->push_back((uint8_t)(((uint32_t)(c->constantData[i])) & 0x0000FF00) >> 8);
		dest->push_back((uint8_t)(((uint32_t)(c->constantData[i])) & 0x000000FF));
	}
}

void copyStandardVariable(std::vector<uint8_t>* dest, Variable* src)
{
	for (int i = 0; i < sizeof(Variable); i++) 
	{
		dest->push_back(((uint8_t*)src)[i]);
	}
}