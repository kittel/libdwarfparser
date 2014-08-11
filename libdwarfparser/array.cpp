#include "array.h"
#include <cassert>
#include <iostream>

#include "helpers.h"
#include "dwarfparser.h"

Array::ArrayTypeMap Array::arrayTypeMap;

Array::Array(Dwarf_Die object):
	Pointer(object), 
	length(0), lengthType(0), lengthTypeBT(0){

	arrayTypeMap.insert( std::pair<uint64_t, Array*>(this->type, this));

}

Array::~Array(){
}

uint64_t Array::getLength(){
	return this->length;
}

uint64_t Array::getByteSize(){
	Symbol* symbol = Symbol::findSymbolByID(this->type);
	return this->length * symbol->getByteSize();
}

void Array::update(Dwarf_Die object){
	DwarfParser *parser = DwarfParser::getInstance();
	if(parser->dieHasAttr(object, DW_AT_type)){
		this->lengthType = parser->getDieAttributeNumber(object, DW_AT_type);
	}
	if(parser->dieHasAttr(object, DW_AT_upper_bound)){
		this->length = parser->getDieAttributeNumber(object, DW_AT_upper_bound) + 1;
	}
	//if(this->type != 0){
	//	arrayTypeMap.insert ( std::pair<uint64_6, Array*>(this->type, this));
	//}
}

Array* Array::findArrayByID(uint64_t id){
	Array* base;
	Symbol *symbol = Symbol::findSymbolByID(id);
	assert(symbol);
	base = dynamic_cast<Array*>(symbol);
	assert(base);
	return base;
}

Array* Array::findArrayByTypeID(uint64_t id, uint64_t length){
	auto bt = arrayTypeMap.find(id);
	if(bt != arrayTypeMap.end()){
		if(bt->second->length == length){
			return bt->second;
		}
	}
	return NULL;
}

void Array::cleanArrays(){
	auto item = arrayTypeMap.begin();
	auto end  = arrayTypeMap.end();

	uint64_t type = 0;
	Symbol* symbol = 0;
	Array* oldArray = 0;

	while(item != end){
		type = item->second->type;
		symbol = Symbol::findSymbolByID(type);
		if(type != symbol->getID()){
			oldArray = findArrayByTypeID(type, item->second->length);
			assert(oldArray);
			oldArray->addAlternativeID(item->second->id);
			delete item->second;
			arrayTypeMap.erase(item++);
			continue;
		}
		item++;
	}

}
