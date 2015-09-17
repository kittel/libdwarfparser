#include "instance.h"

#include <cassert>
#include <iostream>

#include "helpers.h"

#include "structured.h"
#include "structuredmember.h"

#include "consttype.h"

#include "array.h"

#include "libvmiwrapper/vmiinstance.h"

Instance::Instance(BaseType* type, uint64_t va, Instance *parent):
	parent(parent), type(type), address(va){
}

Instance::~Instance(){}

BaseType *Instance::getType(){
	return this->type;
}

uint64_t Instance::getAddress(){
	return this->address;
}

uint64_t Instance::getLength(){
	Array* array = dynamic_cast<Array*>(this->type);
	if(!array) return 1;
	else return array->getLength();
}

bool Instance::isNULL(){
	return (this->address == 0);
}


Instance Instance::changeBaseType(const std::string &newType, 
		const std::string &fieldname){
	BaseType *structModule = BaseType::findBaseTypeByName(newType);
	assert(structModule);
	uint64_t newAddress = this->address;
	BaseType *newBT = this->type;

	if(this->type->getName() == "list_head"){
		Structured *structured = dynamic_cast<Structured*>(structModule);
		assert(structured);
		StructuredMember *sm = structured->memberByName(fieldname);
		assert(sm);
		newAddress -= sm->getMemberLocation();
	}
	newBT = structModule;
	return Instance(newBT, 
			newAddress,
			this);
}

Instance Instance::memberByName(const std::string &name,
                                bool ptr,
                                bool expectZeroPtr){
	uint64_t newAddress;
	assert(address);
	BaseType* bt = this->type;
	while(dynamic_cast<RefBaseType*>(bt)){
		bt = (dynamic_cast<RefBaseType*>(bt))->getBaseType();
	}
	Structured *structured = dynamic_cast<Structured*>(bt);
	assert(structured);
	StructuredMember* member = structured->memberByName(name);
	assert(member);
	bt = member->getBaseType();
	assert(bt);

	newAddress = address + member->getMemberLocation();
	assert(newAddress);
	if(ptr){
		VMIInstance *vmi = VMIInstance::getInstance();
		Pointer* ptr_type;
		if((ptr_type = dynamic_cast<Pointer*>(bt))){
			bt = ptr_type->getBaseType();
			newAddress = vmi->read64FromVA(newAddress);
			//Dereferencing NULL Ptr?
			if(!expectZeroPtr){
				assert(newAddress);
			}
		}
	}
	return Instance(bt, 
			newAddress,
			this);
}

Instance Instance::memberByOffset(uint64_t offset, bool ptr){
	uint64_t newAddress;
	if(offset > this->type->getByteSize()){
		assert(false);
	}
	Structured* structured = dynamic_cast<Structured*>(this->type);
	assert(structured);
	StructuredMember *member = structured->memberByOffset(offset);
	assert(member);
	BaseType *bt = member->getBaseType();
	assert(bt);
	newAddress = address + member->getMemberLocation();
	if(ptr){
		VMIInstance *vmi = VMIInstance::getInstance();
		Pointer* ptr_type;
		while((ptr_type = dynamic_cast<Pointer*>(bt))){
			bt = ptr_type->getBaseType();
			newAddress = vmi->read64FromVA(newAddress);
		}
	}
	return Instance(bt, 
			newAddress,
			this);
}

std::string Instance::memberName(uint64_t offset){
	assert(address);
	BaseType* bt = this->type;
	while(dynamic_cast<RefBaseType*>(bt)){
		bt = (dynamic_cast<RefBaseType*>(bt))->getBaseType();
	}
	Structured *structured = dynamic_cast<Structured*>(bt);
	assert(structured);
	return structured->memberNameByOffset(offset);
}

uint32_t Instance::size(){
	return this->type->getByteSize();
}

Instance Instance::arrayElem(uint64_t element){
	if(element >= this->getLength() && 0 != this->getLength()){
		//TODO throw exception
		std::cout << "Array only got length of " << this->getLength() << 
			" element " << element << " requested" << std::endl;
		assert(false);
	}
	Array* array = dynamic_cast<Array*>(this->type);
	assert(array);
	BaseType *childType = array->getBaseType();
	if(!childType->getByteSize()){
		std::cout << "Type with id " << std::hex << childType->getID() << 
			std::dec << " has no ByteSize" << std::endl;
	}
	assert(childType->getByteSize());
	return Instance(childType, 
			this->address + (element * childType->getByteSize()),
			this);
}

uint32_t Instance::memberOffset(const std::string &name) const{
	Structured* structured = dynamic_cast<Structured*>(this->type);
	assert(structured);
	return structured->memberOffset(name);
}

Instance Instance::dereference(){
	uint64_t newAddress = this->address;
	VMIInstance *vmi = VMIInstance::getInstance();
	RefBaseType* ptr_type;
	BaseType *bt = this->type;
	while((ptr_type = dynamic_cast<RefBaseType*>(bt))){
		bt = ptr_type->getBaseType();
		newAddress = vmi->read64FromVA(newAddress);
	}
	return Instance(bt, 
			newAddress,
			this);
}

bool Instance::operator==(const Instance& instance) const{
	if (this->type != instance.type) return false;
	if (this->address != instance.address) return false;

	return true;
}

bool Instance::operator!=(const Instance& instance) const{
	return !(*this == instance);
}

void Instance::print(){
	std::cout << "Instance @       " << 
		std::hex << this->address << std::dec << std::endl;
	this->getType()->print();
}
