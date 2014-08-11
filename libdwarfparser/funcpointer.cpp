#include "funcpointer.h"

#include <dwarf.h>
#include <libdwarf.h>

FuncPointer::FuncPointer(Dwarf_Die object):
	RefBaseType(object){

}

FuncPointer::~FuncPointer(){

}

