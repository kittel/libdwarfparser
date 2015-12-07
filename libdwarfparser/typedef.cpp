#include "typedef.h"

Typedef::Typedef(SymbolManager *mgr,
                 DwarfParser *parser,
                 const Dwarf_Die &object,
                 const std::string &name)
	:
	RefBaseType(mgr, parser, object, name) {}

Typedef::~Typedef() {}
