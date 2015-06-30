#include "array.h"
#include <cassert>
#include <iostream>

#include "helpers.h"
#include "dwarfparser.h"

#include <algorithm>

Array::ArrayTypeMap Array::arrayTypeMap;
Array::ArrayVector Array::arrayVector;
std::mutex Array::arrayVectorMutex;
std::mutex Array::arrayTypeMapMutex;

Array::Array(DwarfParser *parser, Dwarf_Die object, std::string name):
	Pointer(parser, object, name), 
	length(0), lengthType(0), lengthTypeBT(0){

	arrayVectorMutex.lock();
	arrayVector.push_back(this);
	arrayVectorMutex.unlock();
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

void Array::update(DwarfParser *parser, Dwarf_Die object){
	if(parser->dieHasAttr(object, DW_AT_type)){
		uint64_t dwarfType = parser->getDieAttributeNumber(object, DW_AT_type);
		uint32_t fileID = parser->getFileID();
		this->lengthType = IDManager::getID(dwarfType, fileID);
		if(!this->lengthType) assert(false);
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
	arrayTypeMapMutex.lock();
	auto bt = arrayTypeMap.find(id);
	arrayTypeMapMutex.unlock();
	while(bt != arrayTypeMap.end()){
		if(bt->second->length == length){
			return bt->second;
		}
		bt++;
	}
	//Search for all aliases of this type
	auto revList = Symbol::getAliases(id);
	for( auto i : revList ){
		arrayTypeMapMutex.lock();
		bt = arrayTypeMap.find(i);
		arrayTypeMapMutex.unlock();
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
	if (this->id != array.id) 
		return this->id < array.id;
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

void Array::print(){
	Pointer::print();
	std::cout << "\t Array Type:   " << std::hex << this->type << 
										std::dec << std::endl;
	std::cout << "\t Length        " << this->length << std::endl;
}

void Array::cleanArrays(){
	arrayVectorMutex.lock();
	for(auto item : arrayVector){
		assert(item);
		item->updateTypes();
	}

	auto item = arrayVector.begin();
	std::sort(item, arrayVector.end(), [](const Array *a, const Array *b) {return *a < *b; });
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
			//std::cout << "Deleting array: " << std::endl;
			//oldPtr->print();
			//delPtr->print();
			item = arrayVector.erase(item);
			delete delPtr;
		}else{
			oldPtr = (*item);
			item++;
		}
	}
	arrayVector.shrink_to_fit();

	arrayTypeMapMutex.lock();
	for (auto item : arrayVector){
		arrayTypeMap.insert(std::pair<uint64_t, Array*>(item->type, item));
	}
	arrayTypeMapMutex.unlock();
	arrayVectorMutex.unlock();
}
