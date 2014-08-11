#include "pointer.h"

Pointer::Pointer(Dwarf_Die object):
	RefBaseType(object){
	this->byteSize = 8;

}

Pointer::~Pointer(){

}

