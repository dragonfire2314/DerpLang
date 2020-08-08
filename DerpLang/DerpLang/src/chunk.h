#pragma once

#include "global.h"

uint16_t addVariable(Variable v);
uint16_t addFunction(const char* name);

void writeChunkByteCode(uint8_t data);

Variable makeVariable(double number);
Variable makeVariable(bool boolean);
Variable makeVariable();

uint16_t writeChunkConstantData(Variable data);
Program getProgram();
void clearProgram();

void clearLocalVarList();
void createLocalVar(std::string name, Variable data);
bool isLocalVar(std::string varName, uint16_t& index);