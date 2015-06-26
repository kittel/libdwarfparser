#include "referencingtype.h"

#include <cassert>

#include <iostream>

ReferencingType::ReferencingType(DwarfParser *parser, Dwarf_Die object){
	if(parser->dieHasAttr(object, DW_AT_type)){
		uint64_t dwarfType = parser->getDieAttributeNumber(object, DW_AT_type);
		uint32_t fileID = parser->getFileID();
		this->type = IDManager::getID(dwarfType, fileID);
		if(!this->type) assert(false);
	}
}

ReferencingType::~ReferencingType(){}

BaseType *ReferencingType::getBaseType(){
	this->base = BaseType::findBaseTypeByID(this->type);
	assert(this->base);
	return this->base;
}
