#include "pointer.h"

Pointer::Pointer(SymbolManager *mgr,
                 DwarfParser *parser,
                 const Dwarf_Die &object,
                 const std::string &name)
	:
	RefBaseType(mgr, parser, object, name) {

	this->byteSize = 8;
}

Pointer::~Pointer() {}

void Pointer::print() const {
	RefBaseType::print();
	std::cout << "\t Pointer Size  " << this->byteSize << std::endl;
}
