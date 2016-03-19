#include "consttype.h"

ConstType::ConstType(SymbolManager *mgr,
                     DwarfParser *parser,
                     const Dwarf_Die &object,
                     const std::string &name)
	:
	RefBaseType(mgr, parser, object, name) {}

ConstType::~ConstType() {}

void ConstType::print() const {
	RefBaseType::print();
	std::cout << "\t ConstType:" << std::endl;
}
