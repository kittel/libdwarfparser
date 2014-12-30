#include "structured.h"

#include "structuredmember.h"

#include <iostream>

Structured::Structured(Dwarf_Die object):
	BaseType(object),
	memberNameMap(){

}

Structured::~Structured(){

}

StructuredMember* Structured::addMember(Dwarf_Die object){
	DwarfParser* parser = DwarfParser::getInstance();
	std::string memberName = parser->getDieName(object);
	StructuredMember* member = memberNameMap[memberName];
	if(member){
		return member;
	}
	member = new StructuredMember(object, this);
	memberNameMap[memberName] = member;
	return member;
}

StructuredMember* Structured::memberByName(std::string name){
	return memberNameMap[name];
}

void Structured::listMembers(){
	std::cout << "Members of " << this->name << ": " << std::endl;
	for(auto i = memberNameMap.begin(); i != memberNameMap.end(); i++){
		std::cout << i->second->getName() << std::endl;
	}
}

StructuredMember *Structured::memberByOffset(uint32_t offset){
	for(auto i = memberNameMap.begin(); i != memberNameMap.end(); i++){
		if(i->second->getMemberLocation() <= offset){
			return i->second;
		}
	}
	return NULL;
}

uint32_t Structured::memberOffset(std::string member) const{
	for(auto i = memberNameMap.begin(); i != memberNameMap.end(); i++){
		if(i->second->getName() == member){
			return i->second->getMemberLocation();   
		}
	}
	return -1;
}
