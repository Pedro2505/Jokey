#include "Boolean.h"
#include <string>

Boolean::Boolean() {
	booleans = { "True", "False" };
}

bool Boolean::isBoolean(std::string& str) {
	return booleans.count(str) > 0;
}