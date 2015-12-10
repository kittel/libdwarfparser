#include "symbol.h"

#include <iostream>
#include <cassert>

#include "dwarfparser.h"
#include "symbolmanager.h"


Symbol::Symbol(SymbolManager *manager,
               DwarfParser *parser,
               const Dwarf_Die &object,
               const std::string &name)
	:
	name{name},
	manager{manager} {

	this->byteSize = parser->getDieByteSize(object);
	this->id = this->manager->getID(parser->getDieOffset(object),
	                                parser->getFileID());
	this->manager->addSymbol(this);
}

Symbol::~Symbol() {}


void Symbol::addAlternativeDwarfID(uint64_t id, uint32_t fileID) {
	uint64_t internalID = this->manager->getID(id, fileID);
	this->manager->addAlternativeID(id, internalID);
}

void Symbol::print() const {
	auto dwarfID = this->manager->getRevID(this->id);
	std::cout << "Symbolname:      " << this->name << std::endl;
	std::cout << "\t ID:           " << std::hex << this->id << std::dec
	          << std::endl;
	std::cout << "\t Dwarf ID:     " << std::hex << dwarfID.first << std::dec
	          << std::endl;
	std::cout << "\t Dwarf File:   " << std::hex << dwarfID.second << std::dec
	          << std::endl;
	std::cout << "\t Bytesize:     " << this->byteSize << std::endl;
}

uint32_t Symbol::getByteSize() {
	return this->byteSize;
}

uint64_t Symbol::getID() {
	return this->id;
}

const std::string &Symbol::getName() {
	return this->name;
}
