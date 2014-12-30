#include "variable.h"

#include <cassert>

#include "helpers.h"
#include <iostream>

#include "libvmiwrapper/vmiinstance.h"

#include "instance.h"

Variable::VariableNameMap Variable::variableNameMap;

Variable::Variable(Dwarf_Die object):
	Symbol(object), ReferencingType(object){
	this->location = 0;
	if(this->name.size() != 0){
		variableNameMap[this->name] = this;
	}

	DwarfParser *parser = DwarfParser::getInstance();
	if(parser->dieHasAttr(object, DW_AT_location)){
		this->location = parser->getDieAttributeNumber(object, DW_AT_location);
	}
}

Variable::~Variable(){}

uint64_t Variable::getLocation(){
	return this->location;
}

void Variable::update(Dwarf_Die object){
	if(this->location != 0) return;
	DwarfParser *parser = DwarfParser::getInstance();
	if(parser->dieHasAttr(object, DW_AT_location)){
		this->location = parser->getDieAttributeNumber(object, DW_AT_location);
	}

}

void Variable::setLocation(uint64_t location){
	this->location = location;
}

Instance Variable::getInstance(){
	assert(this->location);
	Instance instance = Instance(this->getBaseType(), this->location);
	return instance;
}

Variable* Variable::findVariableByID(uint64_t id){
	Variable* var;
	Symbol *symbol = Symbol::findSymbolByID(id);
	assert(symbol);
	var = dynamic_cast<Variable*>(symbol);
	assert(var);
	return var;
}

Variable* Variable::findVariableByName(std::string name){
	return variableNameMap.find(name)->second;
}
