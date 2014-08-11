#include "consttype.h"

#include <dwarf.h>
#include <libdwarf.h>

ConstType::ConstType(Dwarf_Die object):
	RefBaseType(object){

}

ConstType::~ConstType(){

}
