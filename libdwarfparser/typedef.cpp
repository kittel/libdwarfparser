#include "typedef.h"

Typedef::Typedef(DwarfParser *parser, const Dwarf_Die &object, 
		const std::string &name):
	RefBaseType(parser, object, name){

}

Typedef::~Typedef(){

}
