#include "struct.h"

Struct::Struct(DwarfParser *parser, Dwarf_Die object, std::string name):
	Structured(parser, object, name){

}

Struct::~Struct(){

}
