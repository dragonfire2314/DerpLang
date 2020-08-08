#pragma once

enum Obj_Type {
	OBJ_STRING,
	OBJ_FUNCTION
};

struct Obj {
	Obj_Type type;
};

enum Variable_Type {
	VAR_NONE,
	VAR_DOUBLE,
	VAR_BOOLEAN,
	VAR_OBJ
};

union Variable_Data {
	double number;
	bool boolean;
	Obj* obj;
};

struct Variable {
	Variable_Type type;
	Variable_Data data;

	Variable();
	Variable(Variable_Type type, double data);
	Variable(Variable_Type type, bool data);
	Variable operator*(const Variable& rhs);
};
