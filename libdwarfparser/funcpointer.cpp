#include "funcpointer.h"

FuncPointer::FuncPointer(DwarfParser *parser, Dwarf_Die object, std::string name):
	RefBaseType(parser, object, name){

}

FuncPointer::~FuncPointer(){

}

