#include "array.h"
#include <cassert>
#include <iostream>

#include "helpers.h"
#include "dwarfparser.h"

#include <algorithm>

Array::ArrayTypeMap Array::arrayTypeMap;
Array::ArrayVector Array::arrayVector;

Array::Array(Dwarf_Die object):
	Pointer(object), 
	length(0), lengthType(0), lengthTypeBT(0){

	arrayVector.push_back(this);
}

Array::~Array(){
}

uint64_t Array::getLength(){
	return this->length;
}

uint32_t Array::getByteSize(){
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
	//Search for array with type
	auto bt = arrayTypeMap.find(id);
	while(bt != arrayTypeMap.end()){
		if(bt->second->length == length){
			return bt->second;
		}
		bt++;
	}
	//Search for all aliases of this type
	auto revList = Symbol::getAliases(id);
	for(auto i = revList.begin();
			i != revList.end();
			i++){
		bt = arrayTypeMap.find(*i);
		while(bt != arrayTypeMap.end()){
			if(bt->second->length == length){
				return bt->second;
			}
			bt++;
		}
	}
	return NULL;
}

bool Array::operator< (const Array& array) const{
	if (this->type != array.type) 
		return this->type < array.type;
	if (this->length != array.length) 
		return this->length < array.length;
	return false;

}

bool Array::operator==(const Array& array) const{
	if (this->type != array.type) return false;
	if (this->length != array.length) return false;

	return true;

}

void Array::updateTypes(){
	Symbol *sym;
	if(this->type){
		sym = Symbol::findSymbolByID(this->type);
		this->type = sym->getID();
	}
}

void Array::cleanArrays(){
	for(auto item = arrayVector.begin(); item != arrayVector.end(); item++){
		assert((*item));
		(*item)->updateTypes();
	}

	auto item = arrayVector.begin();
	std::sort(item, arrayVector.end(), [](Array* a, Array* b) {return *a < *b; });
	item = arrayVector.begin();

	if (item == arrayVector.end()){
		return;
	}

	Array *oldPtr = (*item);
	Array *delPtr = 0;
	item++;

	while(item != arrayVector.end()){
		if (*oldPtr == *(*item)){
			oldPtr->addAlternativeID((*item)->id);
			delPtr = *item;
			item = arrayVector.erase(item);
			delete delPtr;
		}else{
			oldPtr = (*item);
			item++;
		}
	}
	arrayVector.shrink_to_fit();

	for (item = arrayVector.begin(); item != arrayVector.end(); item++){
		arrayTypeMap.insert(std::pair<uint64_t, Array*>((*item)->type, (*item)));
	}
}
