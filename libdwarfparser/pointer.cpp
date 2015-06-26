#include "pointer.h"

Pointer::Pointer(DwarfParser *parser, Dwarf_Die object, std::string name):
	RefBaseType(parser, object, name){
	this->byteSize = 8;

}

Pointer::~Pointer(){

}

void Pointer::print(){
	RefBaseType::print();
	std::cout << "\t Pointer Size  " << this->byteSize << std::endl;
}
