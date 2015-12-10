#include "refbasetype.h"

#include <cassert>

#include "dwarfparser.h"
#include "symbolmanager.h"

#include "helpers.h"


RefBaseType::RefBaseType(SymbolManager *mgr,
                         DwarfParser *parser,
                         const Dwarf_Die &object,
                         const std::string &name)
	:
	BaseType(mgr, parser, object, name),
	type(0),
	base(nullptr) {

	if (parser->dieHasAttr(object, DW_AT_type)) {
		uint64_t dwarfType = parser->getDieAttributeNumber(object, DW_AT_type);
		uint32_t fileID    = parser->getFileID();
		this->type = this->manager->getID(dwarfType, fileID);
		if (!this->type) {
			assert(false);
		}
	}

	this->manager->addRefBaseType(this);
}

RefBaseType::~RefBaseType() {}

BaseType *RefBaseType::getBaseType() {
	if (!this->base) {
		this->resolveBaseType();
	}
	return this->base;
}

void RefBaseType::resolveBaseType() {
	if (!this->type) {
		std::cout << "DO NOT DEREFERENCE void *" << std::endl;
	}
	assert(this->type);
	this->base = this->manager->findBaseTypeByID(this->type);
}

uint32_t RefBaseType::getByteSize() {
	if (!this->base) {
		this->resolveBaseType();
	}
	return base->getByteSize();
}

void RefBaseType::print() const {
	BaseType::print();
	std::cout << "\t Ref Type      " << std::hex
	          << this->type << std::dec << std::endl;
}

uint64_t RefBaseType::getType() {
	return this->type;
}
