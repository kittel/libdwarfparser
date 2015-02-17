#include "function.h"

#include "dwarfparser.h"

#include <algorithm>

Function::FuncList Function::funcList;

Function::Function(Dwarf_Die object):
	BaseType(object),
	rettype(0), address(0),
	paramList(),paramsFinal(false){
	this->update(object);
	this->paramsFinal = false;
	funcList.push_back(this);
}

Function::~Function(){

}

void Function::addParam(Dwarf_Die object){
	if (this->paramsFinal) return;
	DwarfParser* parser = DwarfParser::getInstance();
	if(parser->dieHasAttr(object, DW_AT_type)){
		uint64_t dwarfType = parser->getDieAttributeNumber(object, DW_AT_type);
		uint32_t fileID = parser->getFileID();
		uint64_t paramType = IDManager::getID(dwarfType, fileID);
		if(!paramType) assert(false);
		paramList.push_back(paramType);
	}
}

void Function::update(Dwarf_Die object){
	DwarfParser* parser = DwarfParser::getInstance();
	if(this->rettype == 0 && parser->dieHasAttr(object, DW_AT_type)){
		uint64_t dwarfType = parser->getDieAttributeNumber(object, DW_AT_type);
		uint32_t fileID = parser->getFileID();
		this->rettype = IDManager::getID(dwarfType, fileID);
		if(!this->rettype) assert(false);
	}
	if(this->address == 0 && parser->dieHasAttr(object, DW_AT_low_pc)){
		this->address = parser->getDieAttributeNumber(object, DW_AT_low_pc);
	}
	this->paramsFinal = true;
}


bool Function::operator< (const Function& func) const{
	if (this->rettype != func.rettype) 
		return this->rettype < func.rettype;
	if (this->paramList.size() != func.paramList.size()) 
		return this->paramList.size() < func.paramList.size();

	for (size_t i = 0 ; i < this->paramList.size() ; i++){
		if (this->paramList[i] != func.paramList[i]) 
			return this->paramList[i] < func.paramList[i];
	}
	if (this->id != func.id) 
		return this->id < func.id;
	return false;

}

bool Function::operator==(const Function& func) const{
	if (this->rettype != func.rettype) return false;
	if (this->paramList.size() != func.paramList.size()) return false;

	for (size_t i = 0 ; i < this->paramList.size() ; i++){
		if (this->paramList[i] != func.paramList[i]) return false;
	}
	if(this->name != func.name) return false;
	return true;
}

void Function::updateTypes(){
	Symbol *sym;
	if(this->rettype){
		sym = Symbol::findSymbolByID(this->rettype);
		this->rettype = sym->getID();
	}
	for (size_t i = 0 ; i < this->paramList.size() ; i++){
		sym = Symbol::findSymbolByID(this->paramList[i]);
		assert(sym);
		this->paramList[i] = sym->getID();
	}
}

void Function::cleanFunctions(){
	for(auto item = funcList.begin(); item != funcList.end(); item++){
		assert((*item));
		(*item)->updateTypes();
	}

	auto item = funcList.begin();
	std::sort(item, funcList.end(), [](const Function* a, const Function* b) {return *a < *b; });
	item = funcList.begin();

	if (item == funcList.end()){
		return;
	}

	Function *oldPtr = (*item);
	Function *delPtr = 0;
	item++;

	while(item != funcList.end()){
		if (*oldPtr == *(*item)){
			delPtr = *item;
#if 0
			std::cout << "Keeping Function: " << std::endl;
			oldPtr->print();
			std::cout << "Removing Function: " << std::endl;
			delPtr->print();
#endif
			oldPtr->addAlternativeID(delPtr->id);
			item = funcList.erase(item);
			delete delPtr;
		}else{
			oldPtr = (*item);
			item++;
		}
	}
	funcList.shrink_to_fit();
	//TODO Eigentlich brauchen wir den Vector hier nicht mehr.
}

void Function::print(){
	BaseType::print();
	std::cout << "\t Address:      " << std::hex << 
	                         this->address << std::dec << std::endl;
	for (auto& param : this->paramList) {
		std::cout << "\t Param:        " << std::hex <<
	                         param << std::dec << std::endl;
	}
}
