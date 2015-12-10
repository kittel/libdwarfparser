#include "referencingtype.h"

#include <cassert>
#include <iostream>

#include "symbolmanager.h"

ReferencingType::ReferencingType(SymbolManager *mgr,
                                 DwarfParser *parser,
                                 const Dwarf_Die &object)
	:
	manager{mgr} {

	if (parser->dieHasAttr(object, DW_AT_type)) {
		uint64_t dwarfType = parser->getDieAttributeNumber(object, DW_AT_type);
		uint32_t fileID    = parser->getFileID();
		this->type = this->manager->getID(dwarfType, fileID);
		if (!this->type)
			assert(false);
	}
}

ReferencingType::~ReferencingType() {}

BaseType *ReferencingType::getBaseType() {
	this->base = this->manager->findBaseTypeByID(this->type);
	assert(this->base);
	return this->base;
}

void ReferencingType::print() const {
	std::cout << "\t ReferenceType:" << this->type << std::endl;
}

