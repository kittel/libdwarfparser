#include "enum.h"
#include "dwarfparser.h"
#include "dwarfexception.h"

#include <string>
#include <iostream>

#include "helpers.h"

#include <libdwarf/dwarf.h>
#include <libdwarf/libdwarf.h>

Enum::Enum(DwarfParser *parser, Dwarf_Die object, std::string name):
	BaseType(parser, object, name), enumValues(){
}

Enum::~Enum(){}

void Enum::addEnum(DwarfParser *parser, Dwarf_Die object, std::string name){
//TODO get ENUM Value
	uint32_t enumValue = parser->getDieAttributeNumber(object, DW_AT_const_value);

	enumValues[enumValue] = name;
}

std::string Enum::enumName(uint32_t value){
	EnumValues::const_iterator it;
	it = enumValues.find(value);	
	if(it != enumValues.end()) { return it->second; }
	throw DwarfException("Enum value not found");
}

uint32_t Enum::enumValue(std::string name){
	for (auto it : enumValues)	{
		if (it.second == name){
			return it.first;
		}
	}
	throw DwarfException("Enum name not found");
}

void Enum::printEnumMembers(std::ostream &stream){
	for (auto it : enumValues)	{
		stream << it.second << ": " << it.first << std::endl;
	}
}
