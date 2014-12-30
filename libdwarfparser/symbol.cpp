#include "symbol.h"

#include "dwarfparser.h"

#include <iostream>
#include <cassert>

uint64_t IDManager::nextID = 1;
IDManager::IDMap IDManager::idMap;
IDManager::IDRevMap IDManager::idRevMap;

IDManager::IDManager(){};
IDManager::~IDManager(){};

uint64_t IDManager::getID(uint64_t dwarfID, uint64_t fileID){
	if(idMap[std::make_pair(dwarfID, fileID)] == 0){

		//Search for ID in idRevMap
		//TODO
	}

	return dwarfID;
}

Symbol::SymbolNameMap Symbol::symbolNameMap;
Symbol::SymbolIDMap Symbol::symbolIDMap;
Symbol::SymbolIDAliasMap Symbol::symbolIDAliasMap;
Symbol::SymbolIDAliasReverseList Symbol::symbolIDAliasReverseList;

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
	if (symbolIDAliasReverseList[this->id].size() > 0){
		std::cout << "Warning removing symbol with aliases" << std::endl;
		std::cout << std::hex << this->id << std::dec << std::endl;
		assert(false);
	}
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
	sym = symbolIDMap.find(id)->second;
	if(sym) return sym;
	//Look in reverseList to find symbol
	auto revList = symbolIDAliasReverseList[id];
	for(auto i = revList.begin();
			i != revList.end();
			i++){
		sym = symbolIDMap.find(*i)->second;
		if(sym) return sym;
	}
	assert(false);
}
void Symbol::addAlternativeID(uint64_t id){
	symbolIDAliasMap[id] = this->id;
	symbolIDAliasReverseList[this->id].push_back(id);
}

std::list<uint64_t> Symbol::getAliases(uint64_t id){
	return symbolIDAliasReverseList[id];
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

