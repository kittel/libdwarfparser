#include "refbasetype.h"

#include "dwarfparser.h"

#include <cassert>

RefBaseType::RefBaseTypeNameMap RefBaseType::refBaseTypeNameMap;

RefBaseType::RefBaseType(Dwarf_Die object):
	BaseType(object), type(0), base(0){
	DwarfParser* parser = DwarfParser::getInstance();
	if(parser->dieHasAttr(object, DW_AT_type)){
		this->type = parser->getDieAttributeNumber(object, DW_AT_type);
	}
	this->base = NULL;
	if(this->name.compare("") != 0){
		refBaseTypeNameMap[this->name] = this;
	}
}

RefBaseType::~RefBaseType(){

}

BaseType* RefBaseType::getBaseType(){
	if(!base) this->resolveBaseType();
	return this->base;
}

void RefBaseType::resolveBaseType(){
	this->base = BaseType::findBaseTypeByID(this->type);
}

uint32_t RefBaseType::getByteSize(){
	if(!base) this->resolveBaseType();
	return base->getByteSize();
}

RefBaseType* RefBaseType::findRefBaseTypeByID(uint64_t id){
	RefBaseType* base;
	Symbol *symbol = Symbol::findSymbolByID(id);
	assert(symbol);
	base = dynamic_cast<RefBaseType*>(symbol);
	assert(base);
	return base;
}

RefBaseType* RefBaseType::findRefBaseTypeByName(std::string name){
	auto rbt = refBaseTypeNameMap.find(name);
	if(rbt != refBaseTypeNameMap.end()){
		return rbt->second;
	}
	else return NULL;
}

