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

SymbolManager::SymbolManager()
	:
	currentID{0} {}

SymbolManager::~SymbolManager() {
	for (auto& it : this->symbolIDMap ) {
		delete (it.second);
	}
}

std::pair<uint64_t, uint32_t> SymbolManager::getRevID(uint64_t id) {
	return this->idRevMap[id];
}

uint64_t SymbolManager::getID(uint64_t dwarfID, uint32_t fileID) {
	std::lock_guard<std::mutex> lock(this->mapMutex);
	auto pair = std::make_pair(dwarfID, fileID);

	if (this->idMap[pair] == 0) {
		uint64_t newID = ++this->currentID;

		this->idMap[pair]     = newID;
		this->idRevMap[newID] = pair;

		return newID;
	}
	return this->idMap[pair];
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
	std::cout << "Could not find symbol with id: " << id
	          << " DwarfID: " << std::hex << this->getRevID(id).first
	          << std::dec << std::endl;
	assert(false);
}

uint64_t SymbolManager::numberOfSymbols() {
	return this->symbolIDMap.size();
}


void SymbolManager::addSymbol(Symbol *sym) {
	if (sym->getName().size() != 0) {
		this->symbolNameMapMutex.lock();
		this->symbolNameMap.insert(std::pair<std::string, Symbol *>(sym->getName(), sym));
		this->symbolNameMapMutex.unlock();
	}

	this->symbolIDMap[sym->getID()] = sym;
}

void SymbolManager::addBaseType(BaseType *bt) {
	if (bt->getName().size() != 0) {
		this->baseTypeNameMap.insert(std::make_pair(bt->getName(), bt));
	}
}

void SymbolManager::addRefBaseType(RefBaseType *bt) {
	if (bt->getName().compare("") != 0) {
		this->refBaseTypeNameMap[bt->getName()] = bt;
	}
}

void SymbolManager::addFunction(Function *fun) {
	if (fun->getName().size() != 0 &&
	    this->functionNameMap.find(fun->getName()) == this->functionNameMap.end()) {
		this->functionNameMapMutex.lock();
		this->functionNameMap[fun->getName()] = fun;
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
	if (var->getName().size() != 0) {
		variableNameMap[var->getName()] = var;
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

	FuncList tmp;

	while (item != funcList.end()) {
		if (*oldPtr == *(*item)) {
			delPtr = *item;
			this->addAlternativeID(oldPtr->getID(), delPtr->getID());
			delete delPtr;
		} else {
			tmp.push_back(*item);
			oldPtr = (*item);
		}
		item++;
	}

	tmp.shrink_to_fit();
	this->funcList.swap(tmp);
	tmp.clear();
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

	ArrayVector tmp;

	while (item != this->arrayVector.end()) {
		if (*oldPtr == *(*item)) {
			this->addAlternativeID(oldPtr->getID(), (*item)->getID());
			delPtr = *item;
			delete delPtr;
		} else {
			oldPtr = (*item);
			tmp.push_back(*item);
		}
		item++;
	}

	tmp.shrink_to_fit();
	this->arrayVector.swap(tmp);
	tmp.clear();

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

#define enum_bit_test(source, input_enum) \
	static_cast<uint64_t>(src) & static_cast<uint64_t>(input_enum)

uint64_t SymbolManager::getSymbolAddress(const std::string &symbolName,
                                         symbol_source src) {
	uint64_t address;

	if (enum_bit_test(src, symbol_source::system_map)) {
		address = this->getSystemMapAddress(symbolName);
		if (address != 0) {
			return address;
		}
	}

	if (enum_bit_test(src, symbol_source::modules)) {
		address = this->getElfSymbolAddress(symbolName);
		if (address != 0) {
			return address;
		}
	}

	if (enum_bit_test(src, symbol_source::functions)) {
		address = this->getFunctionAddress(symbolName);
		if (address != 0) {
			return address;
		}
	}

	if (enum_bit_test(src, symbol_source::dwarf_function)) {
		Function *func = this->findFunctionByName(symbolName);
		if (func) {
			uint64_t ret = func->getAddress();
			if (ret) {
				return ret;
			}
		}
	}

	if (enum_bit_test(src, symbol_source::dwarf_variable)) {
		Variable *var = this->findVariableByName(symbolName);
		if (var) {
			uint64_t ret = var->getLocation();
			if (ret) {
				return ret;
			}
		}
	}

	std::cout << std::endl << COLOR_RED << COLOR_BOLD
	          << "Could not find address for variable " << symbolName
	          << COLOR_NORM << COLOR_BOLD_OFF << std::endl;

	return 0;
}

#undef enum_bit_test

void SymbolManager::updateRevMaps() {
	this->elfSymbolRevMap.clear();
	this->functionSymbolRevMap.clear();

	for (auto &i : this->elfSymbolMap) {
		this->elfSymbolRevMap[i.second] = i.first;
	}
	for (auto &i : this->functionSymbolMap) {
		this->functionSymbolRevMap[i.second] = i.first;
	}
}


uint64_t SymbolManager::getSystemMapAddress(const std::string &name,
                                            bool priv) {
	auto symbol = this->sysMapSymbols.find(name);
	if (symbol != this->sysMapSymbols.end()) {
		return symbol->second;
	}

	if (priv) {
		symbol = this->sysMapSymbolsPrivate.find(name);
		if (symbol != this->sysMapSymbolsPrivate.end()) {
			return symbol->second;
		}
	}

	return 0;
}

bool SymbolManager::addSymbolAddress(const std::string &name,
                                     uint64_t address, bool replace) {
	bool duplicate = false;
	auto it = this->elfSymbolMap.find(name);

	std::string newName = name;

	while (it != this->elfSymbolMap.end()) {
		if (it->second == address) {
			return false;
		}

		duplicate = true;
		if (not replace) {
			newName = newName.append("_");
		} else {
			break;
		}
		it = this->elfSymbolMap.find(newName);
	}
	this->elfSymbolMap[newName] = address;

	return duplicate;
}

uint64_t SymbolManager::getElfSymbolAddress(const std::string &name) {
	auto symbol = this->elfSymbolMap.find(name);
	if (symbol != this->elfSymbolMap.end()) {
		return symbol->second;
	}
	return 0;
}

std::string SymbolManager::getElfSymbolName(uint64_t address) {
	auto symbol = this->elfSymbolRevMap.find(address);
	if (symbol != this->elfSymbolRevMap.end()) {
		return symbol->second;
	}
	return "";
}

bool SymbolManager::isSymbol(uint64_t address) {
	if (this->elfSymbolRevMap.find(address) != this->elfSymbolRevMap.end()) {

		return true;
	}
	return false;
}

uint64_t SymbolManager::getContainingSymbol(uint64_t address) {
	auto iter = this->elfSymbolRevMap.upper_bound(address);
	if (iter != this->elfSymbolRevMap.end() &&
	    iter-- != this->elfSymbolRevMap.begin()) {

		return iter->first;
	}
	return 0;
}

void SymbolManager::addFunctionAddress(const std::string &name,
                                       uint64_t address) {
	std::string newName = name;
	while (this->functionSymbolMap.find(newName) !=
	       this->functionSymbolMap.end()) {

		newName = newName.append("_");
	}
	this->functionSymbolMap[newName] = address;
}

uint64_t SymbolManager::getFunctionAddress(const std::string &name) {
	auto function = this->functionSymbolMap.find(name);
	if (function != this->functionSymbolMap.end()) {
		return function->second;
	}
	return 0;
}

std::string SymbolManager::getFunctionName(uint64_t address) {
	auto function = this->functionSymbolRevMap.find(address);
	if (function != this->functionSymbolRevMap.end()) {
		return function->second;
	}
	return "";
}

bool SymbolManager::isFunction(uint64_t address) {
	if (this->functionSymbolRevMap.find(address) !=
	    this->functionSymbolRevMap.end()) {
		return true;
	}
	return false;
}

void SymbolManager::addSysmapSymbol(const std::string &name, uint64_t address, bool priv) {
	if (priv) {
		this->sysMapSymbols[name] = address;
	}
	else {
		this->sysMapSymbolsPrivate[name] = address;
	}
}
