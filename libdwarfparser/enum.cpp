#include "enum.h"
#include "dwarfparser.h"
#include "dwarfexception.h"

#include <string>
#include <iostream>

#include "helpers.h"

#include <dwarf.h>
#include <libdwarf.h>

Enum::Enum(Dwarf_Die object):
	BaseType(object), enumValues(){
}

Enum::~Enum(){}

void Enum::addEnum(Dwarf_Die object){
//TODO get ENUM Value
	UNUSED(object);
	DwarfParser* parser = DwarfParser::getInstance();
	std::string enumName = parser->getDieName(object);
	uint32_t enumValue = parser->getDieAttributeNumber(object, DW_AT_const_value);

	enumValues[enumValue] = enumName;
}

std::string Enum::enumName(uint32_t value){
	EnumValues::const_iterator it;
	it = enumValues.find(value);	
	if(it != enumValues.end()) { return it->second; }
	throw DwarfException("Enum value not found");
}

uint32_t Enum::enumValue(std::string name){
	EnumValues::const_iterator it;
	for (it = enumValues.begin(); it != enumValues.end(); ++it)	{
		if (it->second == name){
			return it->first;
		}
	}
	throw DwarfException("Enum name not found");
}

void Enum::printEnumMembers(std::ostream &stream){
	EnumValues::const_iterator it;
	for (it = enumValues.begin(); it != enumValues.end(); ++it)	{
		stream << it->second << ": " << it->first << std::endl;
	}
}
