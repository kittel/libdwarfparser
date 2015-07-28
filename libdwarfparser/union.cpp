#include "union.h"

Union::Union(DwarfParser *parser, const Dwarf_Die &object, 
		const std::string &name):
	Structured(parser, object, name){

}

Union::~Union(){

}

