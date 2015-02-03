#include "pointer.h"

Pointer::Pointer(Dwarf_Die object):
	RefBaseType(object){
	this->byteSize = 8;

}

Pointer::~Pointer(){

}

void Pointer::print(){
	RefBaseType::print();
	std::cout << "\t Pointer Size  " << this->byteSize << std::endl;
}
