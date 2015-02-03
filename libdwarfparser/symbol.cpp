#include "symbol.h"

#include "dwarfparser.h"

#include <iostream>
#include <cassert>

uint64_t IDManager::nextID = 0;
IDManager::IDMap IDManager::idMap;
IDManager::IDRevMap IDManager::idRevMap;

IDManager::IDManager(){};
IDManager::~IDManager(){};

uint64_t IDManager::getID(uint64_t dwarfID, uint32_t fileID){
	auto pair = std::make_pair(dwarfID, fileID);
	if(idMap[pair] == 0){
		//return new ID, as this ID was not yet seen.
		uint64_t newID = ++nextID;

		idMap[pair] = newID;
		idRevMap[newID] = pair;

		return newID;
	}else{
		return idMap[pair];
	}
}

std::pair<uint64_t, uint32_t> IDManager::getRevID(uint64_t id){
	return idRevMap[id];
}

Symbol::SymbolNameMap Symbol::symbolNameMap;
Symbol::SymbolIDMap Symbol::symbolIDMap;
Symbol::SymbolIDAliasMap Symbol::symbolIDAliasMap;
Symbol::SymbolIDAliasReverseList Symbol::symbolIDAliasReverseList;

Symbol::Symbol(Dwarf_Die object){
	DwarfParser* instance = DwarfParser::getInstance();
	this->byteSize = instance->getDieByteSize(object);
	this->id = IDManager::getID(instance->getDieOffset(object),
								instance->getFileID());
	this->name = instance->getDieName(object);

	if(this->name.size() != 0){
		symbolNameMap.insert ( std::pair<std::string, Symbol*>(this->name, this));
	}
	symbolIDMap[this->id] = this;
	
}

Symbol::~Symbol(){

	if (symbolIDAliasReverseList[this->id].size() > 0){
		std::cout << "Warning removing symbol with aliases" << std::endl;
		std::cout << std::hex << this->id << std::dec << std::endl;

		std::cout << std::endl << "offenting symbols: " << std::endl;

		auto oTypes = symbolIDAliasReverseList[this->id];
		for(auto iter = oTypes.begin();
				iter!=oTypes.end();
				iter++){
			std::cout << "ID: " << std::hex << *iter << std::dec << std::endl;
			Symbol::findSymbolByID(*(iter))->print();
			//TODO
			break;
		}
		
		std::cout << "This Symbol: " << std::endl;
		this->print();
		
		assert(false);
	}
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

	symbolIDAliasReverseList[this->id].insert(id);
}

void Symbol::addAlternativeDwarfID(uint64_t id, uint32_t fileID){
	uint64_t internalID = IDManager::getID(id, fileID);
	this->addAlternativeID(internalID);
}

void Symbol::print(){
	auto dwarfID = IDManager::getRevID(this->id);
	std::cout << "Symbolname:      " << this->name << std::endl;
	std::cout << "\t ID:           " << std::hex << this->id << 
										std::dec << std::endl;
	std::cout << "\t Dwarf ID:     " << std::hex << dwarfID.first << 
										std::dec << std::endl;
	std::cout << "\t Dwarf File:   " << std::hex << dwarfID.second << 
										std::dec << std::endl;
	std::cout << "\t Bytesize:     " << this->byteSize << std::endl;
}

std::set<uint64_t> Symbol::getAliases(uint64_t id){
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

