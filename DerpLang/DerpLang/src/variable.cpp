#include "variable.h"

Variable::Variable()
{
}

Variable::Variable(Variable_Type type, double data)
{
	this->type = type;
	this->data.number = data;
}

Variable::Variable(Variable_Type type, bool data)
{
	this->type = type;
	this->data.boolean = data;
}

Variable Variable::operator*(const Variable& rhs)
{
	//Needs to be worked on, including runtime crashes
	if (type == rhs.type) {
		switch (type) {
		case VAR_DOUBLE:
			return Variable(type, data.number * rhs.data.number);
		}
	}
}

Variable Variable::operator-(const Variable& rhs)
{
	//Needs to be worked on, including runtime crashes
	if (type == rhs.type) {
		switch (type) {
		case VAR_DOUBLE:
			return Variable(type, data.number - rhs.data.number);
		}
	}
}

Variable Variable::operator+(const Variable& rhs)
{
	//Needs to be worked on, including runtime crashes
	if (type == rhs.type) {
		switch (type) {
		case VAR_DOUBLE:
			return Variable(type, data.number + rhs.data.number);
		}
	}
}

Variable Variable::operator/(const Variable& rhs)
{
	//Needs to be worked on, including runtime crashes
	if (type == rhs.type) {
		switch (type) {
		case VAR_DOUBLE:
			return Variable(type, data.number / rhs.data.number);
		}
	}
}

Variable Variable::operator==(const Variable& rhs)
{
	//Needs to be worked on, including runtime crashes
	if (type == rhs.type) {
		switch (type) {
		case VAR_DOUBLE:
			return Variable(VAR_BOOLEAN, data.number == rhs.data.number);
		}
	}
}

Variable Variable::operator!=(const Variable& rhs)
{
	//Needs to be worked on, including runtime crashes
	if (type == rhs.type) {
		switch (type) {
		case VAR_DOUBLE:
			return Variable(VAR_BOOLEAN, data.number != rhs.data.number);
		}
	}
}