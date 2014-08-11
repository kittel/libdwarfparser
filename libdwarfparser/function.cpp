#include "function.h"

#include "dwarfparser.h"

#include <algorithm>

Function::FuncList Function::funcList;

Function::Function(Dwarf_Die object):
	BaseType(object),
	rettype(0), paramList(){
	DwarfParser* parser = DwarfParser::getInstance();
	if(parser->dieHasAttr(object, DW_AT_type)){
		this->rettype = parser->getDieAttributeNumber(object, DW_AT_type);
	}
	funcList.push_back(this);
}

Function::~Function(){

}

void Function::addParam(Dwarf_Die object){
	DwarfParser* parser = DwarfParser::getInstance();
	if(parser->dieHasAttr(object, DW_AT_type)){
		paramList.push_back(parser->getDieAttributeNumber(object, DW_AT_type));
	}
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
	return false;

}

bool Function::operator==(const Function& func) const{
	if (this->rettype != func.rettype) return false;
	if (this->paramList.size() != func.paramList.size()) return false;

	for (size_t i = 0 ; i < this->paramList.size() ; i++){
		if (this->paramList[i] != func.paramList[i]) return false;
	}
	return true;

}

bool Function::operator()(const Function *f1, const Function *f2){
	return *f1 < *f2;
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
	std::sort(item, funcList.end(), [](Function* a, Function* b) {return *a < *b; });
	item = funcList.begin();

	if (item == funcList.end()){
		return;
	}

	Function *oldPtr = (*item);
	Function *delPtr = 0;
	item++;

	while(item != funcList.end()){
		if (*oldPtr == *(*item)){
			oldPtr->addAlternativeID((*item)->id);
			delPtr = *item;
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
