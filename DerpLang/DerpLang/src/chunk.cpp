#include "chunk.h"

#include <vector>

Chunk chunk;

void initChunk() 
{
	chunk.byteCode.clear();
	chunk.constantData.clear();
}

Chunk getChunk() 
{
	return chunk;
}

void writeChunkByteCode(uint8_t data) 
{
	chunk.byteCode.push_back(data);
}

uint16_t writeChunkConstantData(uint32_t data)
{
	if (chunk.constantData.size() + 1 > UINT16_MAX) {
		printf("Error the constant data storage for this chunk is full.\n");
	}
	chunk.constantData.push_back(data);
	return (uint16_t)(chunk.constantData.size() - 1);
}