#include "structured.h"

#include "structuredmember.h"

#include <iostream>

#include <CppUTest/MemoryLeakDetectorNewMacros.h>

Structured::Structured(DwarfParser *parser, 
		const Dwarf_Die &object, 
		const std::string &name):
	BaseType(parser, object, name),
	memberNameMap(), memberMutex(){

}

Structured::~Structured(){
	for(auto& i : memberNameMap){
		delete i.second;
	}
}

StructuredMember* Structured::addMember(DwarfParser *parser, 
		const Dwarf_Die &object, const std::string &memberName){
	StructuredMember* member;
	memberMutex.lock();
	auto memberIter = memberNameMap.find(memberName);
	if ( memberNameMap.find(memberName) != memberNameMap.end()){
		member = memberIter->second;
	}else{
		member = new StructuredMember(parser, object, memberName, this);
		memberNameMap[memberName] = member;
	}
	memberMutex.unlock();
	return member;
}

StructuredMember* Structured::memberByName(const std::string &name){
	return memberNameMap[name];
}

void Structured::listMembers(){
	std::cout << "Members of " << this->name << ": " << std::endl;
	for(auto& i : memberNameMap){
		std::cout << i.second->getName() << std::endl;
	}
}

StructuredMember *Structured::memberByOffset(uint32_t offset){
	StructuredMember * result = NULL;
	for (auto& i : memberNameMap){
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
	for(auto& i : memberNameMap){
		if(i.second->getMemberLocation() == offset){
			return i.first;
		}
	}
	return "";
}

uint32_t Structured::memberOffset(const std::string &member) const{
	for(auto& i : memberNameMap){
		if(i.second->getName() == member){
			return i.second->getMemberLocation();   
		}
	}
	return -1;
}

void Structured::print(){
	std::map<uint32_t, std::string> localMemberMap;
	for(auto& i : memberNameMap){
		localMemberMap[i.second->getMemberLocation()] = i.first;
	}

	BaseType::print();
	std::cout << "\t Members:      " << std::endl;
	for(auto& i : localMemberMap){
		std::cout << "\t\t 0x" << 
			std::hex << i.first << std::dec <<
			"\t" << i.second << std::endl;
	}
}
