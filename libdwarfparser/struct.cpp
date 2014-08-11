#include "struct.h"

#include <dwarf.h>
#include <libdwarf.h>

Struct::Struct(Dwarf_Die object):
	Structured(object){

}

Struct::~Struct(){

}
