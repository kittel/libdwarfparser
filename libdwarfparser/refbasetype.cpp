#include "refbasetype.h"

#include "dwarfparser.h"

#include <cassert>

RefBaseType::RefBaseTypeNameMap RefBaseType::refBaseTypeNameMap;

RefBaseType::RefBaseType(DwarfParser *parser, 
		Dwarf_Die object, std::string name):
	BaseType(parser, object, name), type(0), base(0){
	if(parser->dieHasAttr(object, DW_AT_type)){
		uint64_t dwarfType = parser->getDieAttributeNumber(object, DW_AT_type);
		uint32_t fileID = parser->getFileID();
		this->type = IDManager::getID(dwarfType, fileID);
		if(!this->type) assert(false);
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
	if(!this->type){
		std::cout << "DO NOT DEREFERENCE void *" << std::endl;
	}
	assert(this->type);
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

void RefBaseType::print(){
	BaseType::print();
	std::cout << "\t Ref Type      " << this->type << std::endl;
}
