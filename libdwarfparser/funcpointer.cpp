#include "funcpointer.h"

FuncPointer::FuncPointer(DwarfParser *parser, 
		const Dwarf_Die &object, 
		const std::string &name):
	RefBaseType(parser, object, name){

}

FuncPointer::~FuncPointer(){

}

