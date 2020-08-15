#pragma once

#include "global.h"

uint16_t addVariable(Variable v);
uint16_t addFunction(const char* name);

uint32_t writeChunkByteCode(uint8_t data);
void writeChunkByteCodeLocation(uint8_t data, uint32_t location);
uint32_t getChuckByteCodeCurrentLocation();

Variable makeVariable(double number);
Variable makeVariable(bool boolean);
Variable makeVariable(std::string str);
Variable makeVariable();

uint16_t writeChunkConstantData(Variable data);
Program getProgram();
void clearProgram();

void clearLocalVarList();
uint32_t createLocalVar(Variable data);