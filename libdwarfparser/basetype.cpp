#include "basetype.h"

#include "dwarfparser.h"

#include <cassert>

BaseType::BaseTypeNameMap BaseType::baseTypeNameMap;

BaseType::BaseType(Dwarf_Die object):
	Symbol(object){
	DwarfParser *parser = DwarfParser::getInstance();
	if(parser->dieHasAttr(object, DW_AT_encoding)){
		this->encoding = parser->getDieAttributeNumber(object, DW_AT_encoding);
	}
	if(this->name.size() != 0){
		baseTypeNameMap[this->name] = this;
	}

}

BaseType::~BaseType(){

}

BaseType* BaseType::findBaseTypeByID(uint64_t id){
	BaseType* base;
	Symbol *symbol = Symbol::findSymbolByID(id);
	assert(symbol);
	base = dynamic_cast<BaseType*>(symbol);
	assert(base);
	return base;
}

BaseType* BaseType::findBaseTypeByName(std::string name){
	return baseTypeNameMap.find(name)->second;
}


uint64_t BaseType::getEncoding(){
	return this->encoding;
}

