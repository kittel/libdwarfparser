#include "symbol.h"

#include "dwarfparser.h"

#include <iostream>

Symbol::SymbolNameMap Symbol::symbolNameMap;
Symbol::SymbolIDMap Symbol::symbolIDMap;
Symbol::SymbolIDAliasMap Symbol::symbolIDAliasMap;

Symbol::Symbol(Dwarf_Die object){
	DwarfParser* instance = DwarfParser::getInstance();
	this->byteSize = instance->getDieByteSize(object);
	this->id = instance->getDieOffset(object);
	this->name = instance->getDieName(object);

	if(this->name.size() != 0){
		symbolNameMap.insert ( std::pair<std::string, Symbol*>(this->name, this));
	}
	symbolIDMap[this->id] = this;
	
}

Symbol::~Symbol(){
	symbolIDMap.erase(this->id);
}

Symbol* Symbol::findSymbolByName(std::string name){
	return symbolNameMap.find(name)->second;
}

Symbol* Symbol::findSymbolByID(uint64_t id){
	Symbol *sym;
	sym = symbolIDMap.find(id)->second;
	if(sym) return sym;
	//Look in aliasMap to find symbol
	id = symbolIDAliasMap.find(id)->second;
	return symbolIDMap.find(id)->second;
}
void Symbol::addAlternativeID(uint64_t id){
	symbolIDAliasMap[id] = this->id;
}

uint64_t Symbol::numberOfSymbols(){
	return symbolIDMap.size();
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

