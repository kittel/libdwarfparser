#include "array.h"

#include <algorithm>
#include <cassert>
#include <iostream>

#include "dwarfparser.h"
#include "helpers.h"
#include "symbolmanager.h"

Array::Array(SymbolManager *mgr,
             DwarfParser *parser,
             const Dwarf_Die &object,
             const std::string &name)
	:
	Pointer(mgr, parser, object, name),
	length(0),
	lengthType(0),
	lengthTypeBT(0) {

	this->manager->addArray(this);
}

Array::~Array() {}

uint64_t Array::getLength() {
	return this->length;
}

uint32_t Array::getByteSize() {
	Symbol *symbol = this->manager->findSymbolByID(this->type);
	return this->length * symbol->getByteSize();
}

void Array::update(DwarfParser *parser, const Dwarf_Die &object) {
	if (parser->dieHasAttr(object, DW_AT_type)) {
		uint64_t dwarfType = parser->getDieAttributeNumber(object, DW_AT_type);
		uint32_t fileID    = parser->getFileID();
		this->lengthType = this->manager->getID(dwarfType, fileID);
		if (!this->lengthType)
			assert(false);
	}
	if (parser->dieHasAttr(object, DW_AT_upper_bound)) {
		this->length = parser->getDieAttributeNumber(object,
		                                             DW_AT_upper_bound) + 1;
	}
}

bool Array::operator <(const Array &array) const {
	if (this->type != array.type)
		return this->type < array.type;
	if (this->length != array.length)
		return this->length < array.length;
	if (this->id != array.id)
		return this->id < array.id;
	return false;
}

bool Array::operator ==(const Array &array) const {
	if (this->type != array.type)
		return false;
	if (this->length != array.length)
		return false;

	return true;
}

void Array::updateTypes() {
	Symbol *sym;
	if (this->type) {
		sym        = this->manager->findSymbolByID(this->type);
		this->type = sym->getID();
	}
}

void Array::print() const {
	Pointer::print();
	std::cout << "\t Array Type:   " << std::hex << this->type << std::dec
	          << std::endl;
	std::cout << "\t Length        " << this->length << std::endl;
}
