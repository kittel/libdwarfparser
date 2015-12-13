#include "structured.h"

#include "structuredmember.h"

#include <iostream>

Structured::Structured(SymbolManager *mgr,
                       DwarfParser *parser,
                       const Dwarf_Die &object,
                       const std::string &name)
	:
	BaseType(mgr, parser, object, name) {}

Structured::~Structured() {}

StructuredMember *Structured::addMember(SymbolManager *mgr,
                                        DwarfParser *parser,
                                        const Dwarf_Die &object,
                                        const std::string &memberName) {
	StructuredMember *member;
	this->memberMutex.lock();
	auto memberIter = this->memberNameMap.find(memberName);
	if (this->memberNameMap.find(memberName) != this->memberNameMap.end()) {
		member = memberIter->second;
	} else {
		member = new StructuredMember(mgr, parser, object, memberName, this);
		this->memberNameMap[memberName] = member;
	}
	this->memberMutex.unlock();
	return member;
}

StructuredMember *Structured::memberByName(const std::string &name) {
	return this->memberNameMap[name];
}

void Structured::listMembers() {
	std::cout << "Members of " << this->name << ": " << std::endl;
	for (auto &i : this->memberNameMap) {
		std::cout << i.second->getName() << std::endl;
	}
}

StructuredMember *Structured::memberByOffset(uint32_t offset) {
	StructuredMember *result = nullptr;
	for (auto &i : this->memberNameMap) {
		if (i.second->getMemberLocation() == offset)
			return i.second;
		if (i.second->getMemberLocation() < offset &&
		    (!result ||
		     i.second->getMemberLocation() > result->getMemberLocation())) {
			result = i.second;
		}
	}
	return result;
}

std::string Structured::memberNameByOffset(uint32_t offset) {
	for (auto &i : this->memberNameMap) {
		if (i.second->getMemberLocation() == offset) {
			return i.first;
		}
	}
	return "";
}

uint32_t Structured::memberOffset(const std::string &member) const {
	for (auto &i : this->memberNameMap) {
		if (i.second->getName() == member) {
			return i.second->getMemberLocation();
		}
	}
	return -1;
}

void Structured::print() const {
	std::map<uint32_t, std::string> localMemberMap;
	for (auto &i : this->memberNameMap) {
		localMemberMap[i.second->getMemberLocation()] = i.first;
	}

	BaseType::print();
	std::cout << "\t Members:      " << std::endl;
	for (auto &i : localMemberMap) {
		std::cout << "\t\t 0x" << std::hex << i.first << std::dec << "\t"
		          << i.second << std::endl;
	}
}
