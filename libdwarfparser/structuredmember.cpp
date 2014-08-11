#include "structuredmember.h"

#include <iostream>
#include "dwarfexception.h"

#include <dwarf.h>
#include <libdwarf.h>


StructuredMember::StructuredMember(Dwarf_Die object, Structured *parent):
	Symbol(object), ReferencingType(object),
	bitSize(0), bitOffset(0), memberLocation(0),
	parent(parent){
	if(parent == NULL){
		std::cout << "parent not set" << std::endl;
		throw DwarfException("Parent not set");
	}
	DwarfParser *parser = DwarfParser::getInstance();
	if(parser->dieHasAttr(object, DW_AT_bit_size)){
		this->bitSize = parser->getDieAttributeNumber(object, DW_AT_bit_size);
	}
	if(parser->dieHasAttr(object, DW_AT_bit_offset)){
		this->bitOffset = parser->getDieAttributeNumber(object, DW_AT_bit_offset);
	}
	if(parser->dieHasAttr(object, DW_AT_data_member_location)){
		this->memberLocation = parser->getDieAttributeNumber(object, DW_AT_data_member_location);
	}
}

StructuredMember::~StructuredMember(){

}

uint32_t StructuredMember::getByteSize(){
	if(this->byteSize == 0){	
		this->getBaseType();
		return this->base->getByteSize();
	}
	return this->byteSize;
}

uint32_t StructuredMember::getBitSize(){
	return this->bitSize;
}

uint32_t StructuredMember::getBitOffset(){
	return this->bitOffset;
}

uint32_t StructuredMember::getMemberLocation(){
	return this->memberLocation;
}

