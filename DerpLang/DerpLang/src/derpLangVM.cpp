#include "derpLangVM.h"

#include "global.h"

#include <stack>
#include <string>
#include <iostream>
#include <chrono>

//#define MAKE_VAR(x)

void printVariable(std::string& out, Variable v);

struct ReturnInfo {
	Chunk* chunk;
	uint32_t ip;
};

std::string runProgram(Program program)
{
	//return part
	std::string ret;

	std::stack<Variable> varStack;
	std::stack<ReturnInfo> retInfo;

	uint32_t ip = 0;
	Chunk* chunk = &((Function*)program.vars[program.mainIndex].data.obj)->chunk;

	//Variables for the opcodes to use
	Variable v1, v2;
	uint8_t ub1, ub2;
	ReturnInfo ri;

	//Find main
	for (;;) 
	{
		if (ip >= chunk->byteCode.size())
			break;

		uint8_t opcode = chunk->byteCode[ip];

		switch (opcode) 
		{
		case OP_CONSTANT:
			ub1 = chunk->byteCode[ip + 1];
			ub2 = chunk->byteCode[ip + 2];
			varStack.push(chunk->constantData[(ub1 << 8) | ub2]);
			ip += 3;
			break;
		case OP_PRINT:
			v1 = varStack.top();
			varStack.pop();
			printVariable(ret, v1);
			ip += 1;
			break;
		case OP_TIME:
			std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::system_clock::now().time_since_epoch()
			);
			varStack.push(makeVariable((double)ms.count()));
			ip += 1;
			break;
		case OP_MULT:
			v1 = varStack.top();
			varStack.pop();
			v2 = varStack.top();
			varStack.pop();
			varStack.push(v2 * v1);
			ip += 1;
			break;
		case OP_DIV:
			v1 = varStack.top();
			varStack.pop();
			v2 = varStack.top();
			varStack.pop();
			varStack.push(v2 / v1);
			ip += 1;
			break;
		case OP_ADD:
			v1 = varStack.top();
			varStack.pop();
			v2 = varStack.top();
			varStack.pop();
			varStack.push(v2 + v1);
			ip += 1;
			break;
		case OP_SUB:
			v1 = varStack.top();
			varStack.pop();
			v2 = varStack.top();
			varStack.pop();
			varStack.push(v2 - v1);
			ip += 1;
			break;
		case OP_LOAD_GLOBAL_VAR:
			ub1 = chunk->byteCode[ip + 1];
			ub2 = chunk->byteCode[ip + 2];
			varStack.push(program.vars[(ub1 << 8) | ub2]);
			ip += 3;
			break;
		case OP_LOAD_LOCAL_VAR:
			ub1 = chunk->byteCode[ip + 1];
			ub2 = chunk->byteCode[ip + 2];
			varStack.push(chunk->variableData[(ub1 << 8) | ub2]);
			ip += 3;
			break;
		case OP_STORE_LOCAL_VAR:
			ub1 = chunk->byteCode[ip + 1];
			ub2 = chunk->byteCode[ip + 2];
			v1 = varStack.top();
			varStack.pop();
			chunk->variableData[(ub1 << 8) | ub2] = v1;
			ip += 3;
			break;
		case OP_STORE_GLOBAL_VAR:
			ub1 = chunk->byteCode[ip + 1];
			ub2 = chunk->byteCode[ip + 2];
			v1 = varStack.top();
			varStack.pop();
			program.vars[(ub1 << 8) | ub2] = v1;
			ip += 3;
			break;
		case OP_EQUAL:
			v1 = varStack.top();
			varStack.pop();
			v2 = varStack.top();
			varStack.pop();
			varStack.push(v2 == v1);
			ip += 1;
			break;
		case OP_NOT_EQUAL:
			v1 = varStack.top();
			varStack.pop();
			v2 = varStack.top();
			varStack.pop();
			varStack.push(v2 != v1);
			ip += 1;
			break;
		case OP_BRANCH_ON_FALSE:
			v1 = varStack.top();
			varStack.pop();
			if (v1.type == VAR_BOOLEAN && v1.data.boolean == false) {
				ub1 = chunk->byteCode[ip + 1];
				ub2 = chunk->byteCode[ip + 2];
				ip = (ub1 << 8) | ub2;
				break;
			}
			ip += 3;
			break;
		case OP_JUMP:
			ub1 = chunk->byteCode[ip + 1];
			ub2 = chunk->byteCode[ip + 2];
			ip = (ub1 << 8) | ub2;
			break;
		case OP_CALL:
			v1 = varStack.top();
			varStack.pop();
			if (v1.type == VAR_OBJ)
				if (v1.data.obj->type == OBJ_FUNCTION) {
					ri.chunk = chunk;
					ri.ip = ip + 1;
					retInfo.push(ri);
					ip = 0;
					chunk = &((Function*)v1.data.obj)->chunk;
				}
			break;
		case OP_RETURN:
			//Check if ret from main
			if (&((Function*)program.vars[program.mainIndex].data.obj)->chunk == chunk) {
				ip++;
				break;
			}
			ri = retInfo.top();
			retInfo.pop();
			ip = ri.ip;
			chunk = ri.chunk;
			break;
		default:
			printf("Opcode not implemented: %x.\n", opcode);
			std::cin.get();
		}
	}

	return ret;
}

void printVariable(std::string& out, Variable v) 
{
	switch (v.type) 
	{
	case VAR_DOUBLE: out += std::to_string(v.data.number) + '\n'; break;
	case VAR_BOOLEAN: if (v.data.boolean) out += "true\n"; else  out += "false\n"; break;
	case VAR_NONE: out += "NULL\n"; break;
	case VAR_OBJ: 
		switch (v.data.obj->type) 
		{
		case OBJ_STRING: out += ((String*)v.data.obj)->str + '\n'; break;
		}
		break;
	}
}

/*
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
			st.push(f2 + f1);
			ip++;
			break;
		case OP_SUB:
			f1 = st.top(); st.pop();
			f2 = st.top(); st.pop();
			st.push(f2 - f1);
			ip++;
			break;
		case OP_MULT:
			f1 = st.top(); st.pop();
			f2 = st.top(); st.pop();
			st.push(f2 * f1);
			ip++;
			break;
		case OP_DIV:
			f1 = st.top(); st.pop();
			f2 = st.top(); st.pop();
			st.push(f2 / f1);
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
*/