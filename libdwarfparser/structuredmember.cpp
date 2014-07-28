#include "structuredmember.h"

#include <iostream>
#include "dwarfexception.h"


StructuredMember::StructuredMember(Dwarf_Die object, Structured *parent):
	Symbol(object), ReferencingType(object){
	if(parent == NULL){
		std::cout << "parent not set" << std::endl;
		throw DwarfException("Parent not set");
	}
	parent->addMember(this);
	DwarfParser *parser = DwarfParser::getInstance();
	if(parser->dieHasAttr(object, DW_AT_data_member_location)){
		this->bitoffset = parser->getDieAttributeNumber(object, DW_AT_data_member_location);
	}
}

StructuredMember::~StructuredMember(){

}
