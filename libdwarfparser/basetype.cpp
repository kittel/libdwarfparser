#include "basetype.h"

#include <cassert>
#include <libdwarf/libdwarf.h>

#include <CppUTest/MemoryLeakDetectorNewMacros.h>

#include "instance.h"
#include "symbolmanager.h"

BaseType::BaseType(SymbolManager *manager,
                   DwarfParser *parser,
                   const Dwarf_Die &object,
                   const std::string &name)
	:
	Symbol(manager, parser, object, name), encoding(0) {

	if (parser->dieHasAttr(object, DW_AT_encoding)) {
		this->encoding = parser->getDieAttributeNumber(object,
		                                               DW_AT_encoding);
	}
	if (this->name.size() != 0) {
		baseTypeNameMap.insert(std::pair<std::string, BaseType*>(this->name, this));
	}

	this->manager->addBaseType(this);
}

BaseType::~BaseType() {}

uint64_t BaseType::getEncoding() {
	return this->encoding;
}

Instance BaseType::getInstance(uint64_t va) {
	Instance instance = Instance{this->manager, this, va};
	return instance;
}

void BaseType::print() {
	Symbol::print();
	std::cout << "\t BaseType:" << std::endl;
	std::cout << "\t Encoding:     " << this->encoding << std::endl;
}
