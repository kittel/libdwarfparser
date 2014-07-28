#include "structured.h"

#include "structuredmember.h"

Structured::Structured(Dwarf_Die object):
	BaseType(object){

}

Structured::~Structured(){

}

void Structured::addMember(StructuredMember *member){
	this->members.push_back(member);
}

StructuredMember* Structured::getMemberByName(std::string name){
	for (uint16_t i=0; i<members.size(); ++i){
		if(members[i]->getName() == name){
			return members[i];
		}
	}
	return NULL;
}
