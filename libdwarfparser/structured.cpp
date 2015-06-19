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
	StructuredMember* member;
	auto memberIter = memberNameMap.find(memberName);
	if ( memberNameMap.find(memberName) != memberNameMap.end()){
		member = memberIter->second;
	}else{
		member = new StructuredMember(object, this);
		memberNameMap[memberName] = member;
	}
	return member;
}

StructuredMember* Structured::memberByName(std::string name){
	return memberNameMap[name];
}

void Structured::listMembers(){
	std::cout << "Members of " << this->name << ": " << std::endl;
	for(auto i : memberNameMap){
		std::cout << i.second->getName() << std::endl;
	}
}

StructuredMember *Structured::memberByOffset(uint32_t offset){
	StructuredMember * result = NULL;
	for (auto i : memberNameMap){
		if ( i.second->getMemberLocation() == offset)
			return i.second;
		if ( i.second->getMemberLocation() < offset &&
			(!result || 
			 i.second->getMemberLocation() > result->getMemberLocation())){
			result = i.second;
		}
	}
	return result;
}

std::string Structured::memberNameByOffset(uint32_t offset){
	for(auto i : memberNameMap){
		if(i.second->getMemberLocation() == offset){
			return i.first;
		}
	}
	return "";
}

uint32_t Structured::memberOffset(std::string member) const{
	for(auto i : memberNameMap){
		if(i.second->getName() == member){
			return i.second->getMemberLocation();   
		}
	}
	return -1;
}

void Structured::print(){
	std::map<uint32_t, std::string> localMemberMap;
	for(auto i : memberNameMap){
		localMemberMap[i.second->getMemberLocation()] = i.first;
	}

	BaseType::print();
	std::cout << "\t Members:      " << std::endl;
	for(auto i : localMemberMap){
		std::cout << "\t\t 0x" << 
			std::hex << i.first << std::dec <<
			"\t" << i.second << std::endl;
	}
}
