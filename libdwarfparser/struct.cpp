#include "struct.h"

Struct::Struct(DwarfParser *parser, 
		const Dwarf_Die &object, 
		const std::string &name):
	Structured(parser, object, name){

}

Struct::~Struct(){

}
