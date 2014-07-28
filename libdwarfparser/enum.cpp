#include "enum.h"

#include <string>

#include "helpers.h"

Enum::Enum(Dwarf_Die object):
	BaseType(object), enumValues(){
}

Enum::~Enum(){}

void Enum::addEnum(Dwarf_Die object){
//TODO get ENUM Value
	UNUSED(object);

	enumValues[0] = std::string("Foobar");
}

std::string Enum::enumValue(uint32_t value){
	EnumValues::const_iterator it;
	it = enumValues.find(value);	
	if(it != enumValues.end()) { return it->second; }
	return 0;
}

uint32_t Enum::enumName(std::string name){
	EnumValues::const_iterator it;

	for (it = enumValues.begin(); it != enumValues.end(); ++it)	{
		if (it->second == name){
			return it->first;
		}
	}
	
	return 0;
}

