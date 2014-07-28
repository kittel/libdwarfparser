#include "variable.h"

#include "helpers.h"

Variable::Variable(Dwarf_Die object):
	Symbol(object), ReferencingType(object){
	
	//DwarfParser *parser = DwarfParser::getInstance();
	
	//Get location from elf
	this->location = 0;
}

Variable::~Variable(){}

uint64_t Variable::getLocation(){
	return this->location;
}

void Variable::setLocation(uint64_t location){
	this->location = location;
}

