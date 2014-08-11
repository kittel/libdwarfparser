#include "basetype.h"

#include <cassert>

#include <libdwarf/libdwarf.h>

#include "dwarfparser.h"
#include "instance.h"

BaseType::BaseTypeNameMap BaseType::baseTypeNameMap;

BaseType::BaseType(Dwarf_Die object):
	Symbol(object), encoding(0){
	DwarfParser *parser = DwarfParser::getInstance();
	if(parser->dieHasAttr(object, DW_AT_encoding)){
		this->encoding = parser->getDieAttributeNumber(object, DW_AT_encoding);
	}
	if(this->name.size() != 0){
		baseTypeNameMap.insert ( std::pair<std::string, BaseType*>(this->name, this));
	}

}

BaseType::~BaseType(){

}

BaseType* BaseType::findBaseTypeByID(uint64_t id){
	BaseType* base;
	Symbol *symbol = Symbol::findSymbolByID(id);
	if(!symbol) std::cout << "No symbol with id: " << std::hex << id << std::dec << std::endl;
	assert(symbol);
	base = dynamic_cast<BaseType*>(symbol);
	assert(base);
	return base;
}

BaseType* BaseType::findBaseTypeByName(std::string name){
	auto bt = baseTypeNameMap.find(name);
	if(bt != baseTypeNameMap.end()){
		return bt->second;
	}
	else return NULL;
}


uint64_t BaseType::getEncoding(){
	return this->encoding;
}

Instance BaseType::getInstance(uint64_t va){
	Instance instance = Instance(this, va);
	return instance;
}

