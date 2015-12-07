#include "funcpointer.h"

FuncPointer::FuncPointer(SymbolManager *mgr,
                         DwarfParser *parser,
                         const Dwarf_Die &object,
                         const std::string &name)
	:
	RefBaseType(mgr, parser, object, name) {}

FuncPointer::~FuncPointer() {}
