#pragma once

#include "global.h"

uint16_t addVariable(Variable v);
uint16_t addFunction(const char* name);

void writeChunkByteCode(uint8_t data);
uint16_t writeChunkConstantData(uint32_t data);
Chunk getChunk();

void initChunk();

std::vector<uint8_t> generateByteCode();