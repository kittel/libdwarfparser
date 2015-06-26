#include "typedef.h"

Typedef::Typedef(DwarfParser *parser, Dwarf_Die object, std::string name):
	RefBaseType(parser, object, name){

}

Typedef::~Typedef(){

}
