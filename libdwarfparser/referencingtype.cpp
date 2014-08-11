#include "referencingtype.h"

#include <cassert>

#include <iostream>

ReferencingType::ReferencingType(Dwarf_Die object){
	DwarfParser *parser = DwarfParser::getInstance();
	if(parser->dieHasAttr(object, DW_AT_type)){
		this->type = parser->getDieAttributeNumber(object, DW_AT_type);
	}
}

ReferencingType::~ReferencingType(){}

BaseType *ReferencingType::getBaseType(){
	this->base = BaseType::findBaseTypeByID(this->type);
	assert(this->base);
	return this->base;
}

