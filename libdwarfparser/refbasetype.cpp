#include "refbasetype.h"

#include "dwarfparser.h"

RefBaseType::RefBaseType(Dwarf_Die object):
	BaseType(object){
	DwarfParser* parser = DwarfParser::getInstance();
	if(parser->dieHasAttr(object, DW_AT_type)){
		this->type = parser->getDieAttributeNumber(object, DW_AT_type);
	}
	this->base = NULL;
}

RefBaseType::~RefBaseType(){

}

void RefBaseType::resolveBaseType(){
	this->base = BaseType::findBaseTypeById(this->type);
}
