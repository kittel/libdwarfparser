#include "symbol.h"

#include "dwarfparser.h"

Symbol::SymbolNameMap Symbol::symbolNameMap;
Symbol::SymbolIDMap Symbol::symbolIDMap;

Symbol::Symbol(Dwarf_Die object){
	DwarfParser* instance = DwarfParser::getInstance();
	this->byteSize = instance->getDieByteSize(object);
	this->id = instance->getDieOffset(object);
	this->name = instance->getDieName(object);

	if(this->name.size() != 0){
		symbolNameMap[this->name] = this;
	}
	symbolIDMap[this->id] = this;
	
}

Symbol::~Symbol(){}

Symbol* Symbol::findSymbolByName(std::string name){
	return symbolNameMap.find(name)->second;
}

Symbol* Symbol::findSymbolByID(uint64_t id){
	return symbolIDMap.find(id)->second;
}

uint32_t Symbol::getByteSize(){
	return this->byteSize;
}

uint64_t Symbol::getID(){
	return this->id;
}

std::string Symbol::getName(){
	return this->name;
}

