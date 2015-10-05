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
	static std::mutex funcMutex;
	
	auto pair = std::make_pair(dwarfID, fileID);
	funcMutex.lock();
	if(idMap[pair] == 0){
		//return new ID, as this ID was not yet seen.
		uint64_t newID = ++nextID;

		idMap[pair] = newID;
		idRevMap[newID] = pair;

		funcMutex.unlock();
		return newID;
	}else{
		funcMutex.unlock();
		return idMap[pair];
	}
}

std::pair<uint64_t, uint32_t> IDManager::getRevID(uint64_t id){
	return idRevMap[id];
}

Symbol::SymbolNameMap Symbol::symbolNameMap;
std::mutex Symbol::symbolNameMapMutex;
Symbol::SymbolIDMap Symbol::symbolIDMap;
std::mutex Symbol::symbolIDMapMutex;
Symbol::SymbolIDAliasMap Symbol::symbolIDAliasMap;
std::mutex Symbol::symbolIDAliasMapMutex;
Symbol::SymbolIDAliasReverseList Symbol::symbolIDAliasReverseList;
std::mutex Symbol::symbolIDAliasReverseListMutex;

Symbol::Symbol(DwarfParser *parser, const Dwarf_Die &object, const std::string &name){
	this->byteSize = parser->getDieByteSize(object);
	this->id = IDManager::getID(parser->getDieOffset(object),
								parser->getFileID());
	this->name = name;


	if(this->name.size() != 0){
		symbolNameMapMutex.lock();
		symbolNameMap.insert ( std::pair<std::string, Symbol*>(this->name, this));
		symbolNameMapMutex.unlock();
	}

	symbolNameMapMutex.lock();
	symbolIDMap[this->id] = this;
	symbolNameMapMutex.unlock();
}

Symbol::~Symbol(){

	if (symbolIDAliasReverseList[this->id].size() > 0){
		std::cout << "Warning removing symbol with aliases" << std::endl;
		std::cout << std::hex << this->id << std::dec << std::endl;

		std::cout << std::endl << "offenting symbols: " << std::endl;

		symbolIDAliasReverseListMutex.lock();
		auto oTypes = symbolIDAliasReverseList[this->id];
		symbolIDAliasReverseListMutex.unlock();
		for(auto& iter : oTypes){
			std::cout << "\tID: " << std::hex << iter << std::dec << std::endl;
			Symbol::findSymbolByID(iter)->print();
			//TODO
			break;
		}
		
		std::cout << "This Symbol: " << std::endl;
		this->print();
		
		assert(false);
	}
	symbolIDMapMutex.lock();
	symbolIDMap.erase(this->id);
	symbolIDMapMutex.unlock();
}

Symbol* Symbol::findSymbolByName(const std::string &name){
	symbolNameMapMutex.lock();
	auto ret = symbolNameMap.find(name)->second;
	symbolNameMapMutex.unlock();
	return ret;
}

Symbol* Symbol::findSymbolByID(uint64_t id){
	symbolIDMapMutex.lock();
	auto symbol = symbolIDMap.find(id);
	symbolIDMapMutex.unlock();
	if(symbol != symbolIDMap.end()){
		return symbol->second;
	}
	
	//Look in aliasMap to find symbol
	symbolIDAliasMapMutex.lock();
	auto symbolIDAlias = symbolIDAliasMap.find(id);
	symbolIDAliasMapMutex.unlock();
	if(symbolIDAlias != symbolIDAliasMap.end()){
		id = symbolIDAlias->second;
		symbolIDMapMutex.lock();
		symbol = symbolIDMap.find(id);
		symbolIDMapMutex.unlock();
		if(symbol != symbolIDMap.end()){
			return symbol->second;
		}
	}
	
	//Look in reverseList to find symbol
	symbolIDAliasReverseListMutex.lock();
	auto revList = symbolIDAliasReverseList[id];
	symbolIDAliasReverseListMutex.unlock();
	for(auto& i : revList){
		symbol = symbolIDMap.find(i);
		if(symbol != symbolIDMap.end()){
			return symbol->second;
		}
	}
	assert(false);
}

void Symbol::addAlternativeID(uint64_t id){
	symbolIDAliasMapMutex.lock();
	symbolIDAliasMap[id] = this->id;
	symbolIDAliasMapMutex.unlock();

	symbolIDAliasReverseListMutex.lock();
	symbolIDAliasReverseList[this->id].insert(id);
	symbolIDAliasReverseListMutex.unlock();
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

