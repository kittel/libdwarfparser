#include "referencingtype.h"

ReferencingType::ReferencingType(Dwarf_Die object){
	DwarfParser *parser = DwarfParser::getInstance();
	if(parser->dieHasAttr(object, DW_AT_type)){
		this->type = parser->getDieAttributeNumber(object, DW_AT_type);
	}
}

ReferencingType::~ReferencingType(){}

