#include "union.h"

Union::Union(SymbolManager *mgr,
             DwarfParser *parser,
             const Dwarf_Die &object,
             const std::string &name)
	:
	Structured(mgr, parser, object, name) {}

Union::~Union() {}
