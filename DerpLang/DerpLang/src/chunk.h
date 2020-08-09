#pragma once

#include "global.h"

uint16_t addVariable(Variable v);
uint16_t addFunction(const char* name);

uint32_t writeChunkByteCode(uint8_t data);
void writeChunkByteCodeLocation(uint8_t data, uint32_t location);
uint32_t getChuckByteCodeCurrentLocation();

Variable makeVariable(double number);
Variable makeVariable(bool boolean);
Variable makeVariable();

uint16_t writeChunkConstantData(Variable data);
Program getProgram();
void clearProgram();

void clearLocalVarList();
void createLocalVar(std::string name, Variable data);
bool isLocalVar(std::string varName, uint16_t& index);