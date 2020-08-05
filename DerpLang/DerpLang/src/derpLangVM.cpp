#include "derpLangVM.h"

#include "global.h"

#include <stack>
#include <string>

std::string runChunk(Chunk chunk) 
{
	//return part
	std::string ret;

	std::stack<float> st;

	float f1, f2;
	uint16_t index;

	for (int ip = 0; ip < chunk.byteCode.size();) 
	{
		//Fetch
		uint8_t opcode = chunk.byteCode[ip];
		//Decode
		//Execute
		switch (opcode)
		{
		case OP_CONSTANT:
			index = (chunk.byteCode[ip + 1] << 8) | (chunk.byteCode[ip + 2]);	//Get index to constant
			st.push(chunk.constantData[index]);											//Push the constant to the stack
			ip += 3;																//Move ip over up two
			break;
		case OP_ADD:
			f1 = st.top(); st.pop();
			f2 = st.top(); st.pop();
			st.push(f1 + f2);
			ip++;
			break;
		case OP_PRINT:
			f1 = st.top(); st.pop();
			ret += std::to_string(f1);
			ret += '\n';
			ip++;
			break;
		}
	}

	return ret;
}