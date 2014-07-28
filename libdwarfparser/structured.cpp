#include "structured.h"

Structured::Structured(Dwarf_Die object):
	BaseType(object){

}

Structured::~Structured(){

}

void Structured::addMember(StructuredMember *member){
	this->members.push_back(member);
}
