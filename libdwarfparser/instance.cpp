#include "instance.h"

#include <cassert>
#include <iostream>

#include "helpers.h"


#include "structured.h"
#include "structuredmember.h"

#include "array.h"

Instance::Instance(BaseType* type, uint64_t va, Instance *parent):
	parent(parent), type(type), address(va){
}

Instance::~Instance(){}

BaseType *Instance::getType(){
	return this->type;
}

Instance Instance::getMember(std::string name){
	Structured *structured = dynamic_cast<Structured*>(this->type);
	assert(structured);
	StructuredMember* member = structured->getMemberByName(name);
	assert(member);
	BaseType *bt = member->getBaseType();
	assert(bt);
	return Instance(bt, 
			address + member->getMemberLocation(),
			this);
}

uint32_t Instance::size(){
	return this->type->getByteSize();
}

uint64_t Instance::getLength(){
	Array* array = dynamic_cast<Array*>(this->type);
	if(!array) return 1;
	else return array->getLength();
}

Instance Instance::arrayElem(uint64_t element){
	if(element >= this->getLength()){
		//TODO throw exception
		std::cout << "Array only got length of " << this->getLength() << 
			" element " << element << " requested" << std::endl;
		assert(false);
	}
	Array* array = dynamic_cast<Array*>(this->type);
	assert(array);
	BaseType *type = array->getBaseType();
	return Instance(type, 
			this->address + (element * type->getByteSize()),
			this);
}

Instance Instance::memberByOffset(uint64_t member){
	if(member > this->type->getByteSize()){
		assert(false);
	}
	Structured* structured = dynamic_cast<Structured*>(this->type);
	assert(structured);
	StructuredMember *type = structured->memberByOffset(member);
	return Instance(type->getBaseType(), 
			this->address + type->getMemberLocation(),
			this);
}

uint32_t Instance::memberOffset(std::string name) const{
	Structured* structured = dynamic_cast<Structured*>(this->type);
	assert(structured);
	return structured->memberOffset(name);
}
