#include "struct.h"

Struct::Struct(SymbolManager *mgr,
               DwarfParser *parser,
               const Dwarf_Die &object,
               const std::string &name)
	:
	Structured(mgr, parser, object, name) {}

Struct::~Struct() {}
