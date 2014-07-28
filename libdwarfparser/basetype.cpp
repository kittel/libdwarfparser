#include "basetype.h"

#include "dwarfparser.h"

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

BaseType* BaseType::findBaseTypeById(uint64_t id){
	//TODO Implement function
	id = id;
	return 0;
}

BaseType* BaseType::findSymbolByName(std::string name){
	return baseTypeNameMap.find(name)->second;
}


uint64_t BaseType::getEncoding(){
	return this->encoding;
}

