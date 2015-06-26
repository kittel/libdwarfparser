#include "consttype.h"

ConstType::ConstType(DwarfParser *parser, Dwarf_Die object, std::string name):
	RefBaseType(parser, object, name){

}

ConstType::~ConstType(){

}
