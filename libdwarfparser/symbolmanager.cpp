#include "symbolmanager.h"

#include <algorithm>
#include <cassert>

#include "array.h"
#include "basetype.h"
#include "symbol.h"
#include "refbasetype.h"
#include "function.h"
#include "variable.h"

#include "helpers.h"

#include <CppUTest/MemoryLeakDetectorNewMacros.h>

SymbolManager::SymbolManager()
	:
	currentID{0} {}

SymbolManager::~SymbolManager() {
	for (auto& it : symbolIDMap ) {
		delete (it.second);
	}
}

std::pair<uint64_t, uint32_t> SymbolManager::getRevID(uint64_t id) {
	return this->idRevMap[id];
}

uint64_t SymbolManager::getID(uint64_t dwarfID, uint32_t fileID) {
	auto pair = std::make_pair(dwarfID, fileID);

	this->mapMutex.lock();
	if (this->idMap[pair] == 0) {
		// return new ID, as this ID was not yet seen.
		this->currentID += 1;
		uint64_t newID = this->currentID;

		this->idMap[pair]     = newID;
		this->idRevMap[newID] = pair;

		this->mapMutex.unlock();
		return newID;
	} else {
		this->mapMutex.unlock();
		return this->idMap[pair];
	}
}


std::set<uint64_t> SymbolManager::getAliases(uint64_t id) {
	return this->symbolIDAliasReverseList[id];
}

Symbol *SymbolManager::findSymbolByID(uint64_t id) {
	this->symbolIDMapMutex.lock();
	auto symbol = this->symbolIDMap.find(id);
	this->symbolIDMapMutex.unlock();
	if (symbol != symbolIDMap.end()) {
		return symbol->second;
	}

	// Look in aliasMap to find symbol
	this->symbolIDAliasMapMutex.lock();
	auto symbolIDAlias = symbolIDAliasMap.find(id);
	this->symbolIDAliasMapMutex.unlock();

	if (symbolIDAlias != this->symbolIDAliasMap.end()) {
		uint64_t new_id = symbolIDAlias->second;

		this->symbolIDMapMutex.lock();
		symbol = this->symbolIDMap.find(new_id);
		this->symbolIDMapMutex.unlock();

		if (symbol != this->symbolIDMap.end()) {
			assert(new_id == symbol->second->getID());
			return symbol->second;
		}
	}

	// Look in reverseList to find symbol
	this->symbolIDAliasReverseListMutex.lock();
	auto revList = this->symbolIDAliasReverseList[id];
	this->symbolIDAliasReverseListMutex.unlock();

	for (auto &i : revList) {
		this->symbolIDMapMutex.lock();
		symbol = this->symbolIDMap.find(i);
		this->symbolIDMapMutex.unlock();

		if (symbol != this->symbolIDMap.end()) {
			assert(id == symbol->second->getID());
			return symbol->second;
		}
	}
	assert(false);
}

uint64_t SymbolManager::numberOfSymbols() {
	return this->symbolIDMap.size();
}


void SymbolManager::addSymbol(Symbol *sym) {
	if (sym->name.size() != 0) {
		this->symbolNameMapMutex.lock();
		this->symbolNameMap.insert(std::pair<std::string, Symbol *>(sym->name, sym));
		this->symbolNameMapMutex.unlock();
	}

	this->symbolIDMap[sym->getID()] = sym;
}

void SymbolManager::addBaseType(BaseType *bt) {
	if (bt->name.size() != 0) {
		this->baseTypeNameMap.insert(std::make_pair(bt->name, bt));
	}
}

void SymbolManager::addRefBaseType(RefBaseType *bt) {
	if (bt->name.compare("") != 0) {
		this->refBaseTypeNameMap[bt->name] = bt;
	}
}

void SymbolManager::addFunction(Function *fun) {
	if (fun->name.size() != 0 &&
	    this->functionNameMap.find(fun->name) == this->functionNameMap.end()) {
		this->functionNameMapMutex.lock();
		this->functionNameMap[fun->name] = fun;
		this->functionNameMapMutex.unlock();
	}
	this->funcListMutex.lock();
	this->funcList.push_back(fun);
	this->funcListMutex.unlock();
}

void SymbolManager::addArray(Array *ar) {
	this->arrayVectorMutex.lock();
	this->arrayVector.push_back(ar);
	this->arrayVectorMutex.unlock();
}

void SymbolManager::addVariable(Variable *var) {
	if (var->name.size() != 0) {
		variableNameMap[var->name] = var;
	}
}

void SymbolManager::addAlternativeID(uint64_t id, uint64_t new_id) {
	this->symbolIDAliasMapMutex.lock();
	this->symbolIDAliasMap[new_id] = id;
	this->symbolIDAliasMapMutex.unlock();

	this->symbolIDAliasReverseListMutex.lock();
	this->symbolIDAliasReverseList[id].insert(new_id);
	this->symbolIDAliasReverseListMutex.unlock();
}

void SymbolManager::removeSymbol(Symbol *sym) {
	this->removeSymbol(sym->getID());
}

void SymbolManager::removeSymbol(uint64_t id) {
	if (this->symbolIDAliasReverseList[id].size() > 0) {
		std::cout << "Warning removing symbol with aliases" << std::endl;
		std::cout << std::hex << id << std::dec << std::endl;

		std::cout << std::endl << "offenting symbols: " << std::endl;

		this->symbolIDAliasReverseListMutex.lock();
		auto oTypes = this->symbolIDAliasReverseList[id];
		this->symbolIDAliasReverseListMutex.unlock();

		for (auto &iter : oTypes) {
			std::cout << "\tID: " << std::hex << iter << std::dec << std::endl;
			this->findSymbolByID(iter)->print();
			// TODO
			break;
		}

		std::cout << "This Symbol: " << std::endl;
		this->symbolIDMap[id]->print();

		assert(false);
	}

	this->symbolIDMapMutex.lock();
	this->symbolIDMap.erase(id);
	this->symbolIDMapMutex.unlock();
}

BaseType *SymbolManager::findBaseTypeByID(uint64_t id) {
	BaseType *base;
	Symbol *symbol = this->findSymbolByID(id);
	if (!symbol)
		std::cout << "No symbol with id: " << std::hex << id << std::dec
		          << std::endl;
	assert(symbol);
	base = dynamic_cast<BaseType *>(symbol);
	assert(base);
	return base;
}

BaseType *SymbolManager::findBaseTypeByName(const std::string &name) {
	auto bt = this->baseTypeNameMap.find(name);
	if (bt != this->baseTypeNameMap.end()) {
		return bt->second;
	} else {
		return nullptr;
	}
}

Function *SymbolManager::findFunctionByID(uint64_t id) {
	Function *var;
	Symbol *symbol = this->findSymbolByID(id);
	assert(symbol);
	var = dynamic_cast<Function *>(symbol);
	assert(var);
	return var;
}

Function *SymbolManager::findFunctionByName(const std::string &name) {
	return returnPtrInMap(this->functionNameMap, name);
}

void SymbolManager::cleanFunctions() {
	this->funcListMutex.lock();
	for (auto &item : this->funcList) {
		assert(item);
		item->updateTypes();
	}

	auto item = this->funcList.begin();
	std::sort(
		item,
		funcList.end(),
		[](const Function *a, const Function *b) {
			return *a < *b;
		}
	);

	item = this->funcList.begin();

	if (item == this->funcList.end()) {
		return;
	}

	Function *oldPtr = *item;
	Function *delPtr = nullptr;
	item++;

	while (item != funcList.end()) {
		if (*oldPtr == *(*item)) {
			delPtr = *item;
			this->addAlternativeID(oldPtr->getID(), delPtr->getID());
			item = funcList.erase(item);
			delete delPtr;
		} else {
			oldPtr = (*item);
			item++;
		}
	}
	this->funcList.shrink_to_fit();
	// TODO actually we don't need the vector any more at this point
	this->funcListMutex.unlock();
}

RefBaseType *SymbolManager::findRefBaseTypeByID(uint64_t id) {
	RefBaseType *base;
	Symbol *symbol = this->findSymbolByID(id);
	assert(symbol);
	base = dynamic_cast<RefBaseType *>(symbol);
	assert(base);
	return base;
}

RefBaseType *SymbolManager::findRefBaseTypeByName(const std::string &name) {
	auto rbt = this->refBaseTypeNameMap.find(name);
	if (rbt != this->refBaseTypeNameMap.end()) {
		return rbt->second;
	} else {
		return nullptr;
	}
}

Array *SymbolManager::findArrayByID(uint64_t id) {
	Array *base;
	Symbol *symbol = this->findSymbolByID(id);
	assert(symbol);
	base = dynamic_cast<Array *>(symbol);
	assert(base);
	return base;
}

Array *SymbolManager::findArrayByTypeID(uint64_t id, uint64_t length) {
	// Search for array with type
	this->arrayTypeMapMutex.lock();
	auto bt = arrayTypeMap.find(id);
	this->arrayTypeMapMutex.unlock();
	while (bt != this->arrayTypeMap.end()) {
		if (bt->second->getLength() == length) {
			return bt->second;
		}
		bt++;
	}
	// Search for all aliases of this type
	auto revList = this->getAliases(id);
	for (auto &i : revList) {
		this->arrayTypeMapMutex.lock();
		bt = this->arrayTypeMap.find(i);
		this->arrayTypeMapMutex.unlock();
		while (bt != this->arrayTypeMap.end()) {
			if (bt->second->getLength() == length) {
				return bt->second;
			}
			bt++;
		}
	}
	return nullptr;
}

void SymbolManager::cleanArrays() {
	this->arrayVectorMutex.lock();
	for (auto &item : this->arrayVector) {
		assert(item);
		item->updateTypes();
	}

	auto item = this->arrayVector.begin();
	std::sort(
		item,
		arrayVector.end(),
		[](const Array *a, const Array *b) {
			return *a < *b;
		}
	);
	item = this->arrayVector.begin();

	if (item == this->arrayVector.end()) {
		return;
	}

	Array *oldPtr = *item;
	Array *delPtr = nullptr;
	item++;

	while (item != this->arrayVector.end()) {
		if (*oldPtr == *(*item)) {
			this->addAlternativeID(oldPtr->getID(), (*item)->getID());
			delPtr = *item;
			item = this->arrayVector.erase(item);
			delete delPtr;
		} else {
			oldPtr = (*item);
			item++;
		}
	}
	this->arrayVector.shrink_to_fit();

	this->arrayTypeMapMutex.lock();
	for (auto &item : arrayVector) {
		this->arrayTypeMap.insert(std::make_pair(item->getType(), item));
	}
	this->arrayTypeMapMutex.unlock();
	this->arrayVectorMutex.unlock();
}


Variable *SymbolManager::findVariableByID(uint64_t id) {
	Variable *var;
	Symbol *symbol = this->findSymbolByID(id);
	assert(symbol);
	var = dynamic_cast<Variable *>(symbol);
	assert(var);
	return var;
}

Variable *SymbolManager::findVariableByName(const std::string &name) {
	return returnPtrInMap(this->variableNameMap, name);
}

std::vector<std::string> SymbolManager::getVarNames() {
	std::vector<std::string> ret;
	for (auto& it : this->variableNameMap) {
		ret.push_back(it.first);
	}
	return ret;
}
