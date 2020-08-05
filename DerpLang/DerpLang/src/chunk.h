#pragma once

#include <stdint.h>
#include <vector>

struct Chunk {
	std::vector<uint8_t> byteCode;
	std::vector<float> constantData;
};

void writeChunkByteCode(uint8_t data);
uint16_t writeChunkConstantData(uint32_t data);
Chunk getChunk();

void initChunk();