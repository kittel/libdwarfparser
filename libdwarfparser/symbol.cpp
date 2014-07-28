#include "symbol.h"

#include "dwarfparser.h"

Symbol::SymbolMap Symbol::symbolMap;

Symbol::Symbol(Dwarf_Die object){
	DwarfParser* instance = DwarfParser::getInstance();
	this->byteSize = instance->getDieByteSize(object);
	this->offset = instance->getDieOffset(object);
	this->name = instance->getDieName(object);

	if(this->name.size() != 0){
		symbolMap[this->name] = this;
	}
	
}

Symbol::~Symbol(){}

Symbol* Symbol::findSymbolByName(std::string name){
	return symbolMap.find(name)->second;
}

uint32_t Symbol::getByteSize(){
	return this->byteSize;
}

std::string Symbol::getName(){
	return this->name;
}

