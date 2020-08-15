#include "chunk.h"
#include "global.h"

#include <vector>
#include <unordered_map>

void clearLocalVarList();

Program prog;

Chunk* currentChunk;

Program getProgram() 
{
	//Set the main pointer
	for (int i = 0; i < prog.vars.size(); i++)
		if (prog.vars[i].type == VAR_OBJ && prog.vars[i].data.obj->type == OBJ_FUNCTION)
			if (((Function*)prog.vars[i].data.obj)->name == "main")
				prog.mainIndex = i;

	return prog;
}

void clearProgram() 
{
	prog.vars.clear();
}

uint16_t addVariable(Variable v)
{
	prog.vars.push_back(v);
	return prog.vars.size() - 1;
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

uint32_t writeChunkByteCode(uint8_t data)
{
	currentChunk->byteCode.push_back(data);
	return currentChunk->byteCode.size() - 1;
}

void writeChunkByteCodeLocation(uint8_t data, uint32_t location) 
{
	currentChunk->byteCode[location] = data;
}

uint32_t getChuckByteCodeCurrentLocation()
{
	return currentChunk->byteCode.size();
}

Variable makeVariable(double number)
{
	Variable v;
	v.data.number = number;
	v.type = VAR_DOUBLE;
	return v;
}

Variable makeVariable(bool boolean)
{
	Variable v;
	v.data.boolean = boolean;
	v.type = VAR_BOOLEAN;
	return v;
}

Variable makeVariable(std::string str)
{
	Variable v;
	v.type = VAR_OBJ;
	v.data.obj = new String();
	v.data.obj->type = OBJ_STRING;
	((String*)v.data.obj)->str = str;
	return v;
}

Variable makeVariable()
{
	Variable v;
	v.type = VAR_NONE;
	return v;
}


uint16_t writeChunkConstantData(Variable data)
{
	if (currentChunk->constantData.size() + 1 > UINT16_MAX) {
		printf("Error the constant data storage for this chunk is full.\n");
	}
	currentChunk->constantData.push_back(data);
	return (uint16_t)(currentChunk->constantData.size() - 1);
}

/***********************************
	Local Variables
***********************************/

void clearLocalVarList() 
{
	
}

uint32_t createLocalVar(Variable data) 
{
	currentChunk->variableData.push_back(data);
	return currentChunk->variableData.size() - 1;
}