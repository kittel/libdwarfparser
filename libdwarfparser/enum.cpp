#include "enum.h"

#include <iostream>
#include <string>

#include "dwarfparser.h"
#include "dwarfexception.h"
#include "helpers.h"


Enum::Enum(SymbolManager *mgr,
           DwarfParser *parser,
           const Dwarf_Die &object,
           const std::string &name)
	:
	BaseType(mgr, parser, object, name) {}

Enum::~Enum() {}

void Enum::addEnum(SymbolManager * /*mgr*/,
                   DwarfParser *parser,
                   const Dwarf_Die &object,
                   const std::string &name) {
	// TODO get ENUM Value
	uint32_t enumValue = parser->getDieAttributeNumber(object, DW_AT_const_value);

	this->enumMutex.lock();
	this->enumValues[enumValue] = name;
	this->enumMutex.unlock();
}

std::string Enum::enumName(uint32_t value) {
	EnumValues::const_iterator it;
	it = this->enumValues.find(value);
	if (it != this->enumValues.end()) {
		return it->second;
	}
	throw DwarfException("Enum value not found");
}

uint32_t Enum::enumValue(const std::string &name) {
	for (auto &it : this->enumValues) {
		if (it.second == name) {
			return it.first;
		}
	}
	throw DwarfException("Enum name not found");
}

void Enum::printEnumMembers(std::ostream &stream) {
	for (auto &it : this->enumValues) {
		stream << it.second << ": " << it.first << std::endl;
	}
}

void Enum::print() const {
	BaseType::print();
}
