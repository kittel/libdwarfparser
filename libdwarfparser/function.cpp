#include "function.h"

#include <algorithm>

#include "dwarfparser.h"
#include "symbolmanager.h"

Function::Function(SymbolManager *mgr,
                   DwarfParser *parser,
                   const Dwarf_Die &object,
                   const std::string &name)
	:
	BaseType(mgr, parser, object, name),
	rettype(0),
	address(0),
	paramsFinal(false) {

	this->update(parser, object);

	this->manager->addFunction(this);
	this->paramsFinal = false;
}

Function::~Function() {}

void Function::addParam(DwarfParser *parser,
                        const Dwarf_Die &object) {
	if (this->paramsFinal) {
		return;
	}

	if (parser->dieHasAttr(object, DW_AT_type)) {
		uint64_t dwarfType = parser->getDieAttributeNumber(object, DW_AT_type);
		uint32_t fileID    = parser->getFileID();
		uint64_t paramType = this->manager->getID(dwarfType, fileID);
		if (!paramType) {
			assert(false);
		}
		this->paramList.push_back(paramType);
	}
}

void Function::update(DwarfParser *parser,
                      const Dwarf_Die &object) {
	if (this->rettype == 0 && parser->dieHasAttr(object, DW_AT_type)) {
		uint64_t dwarfType = parser->getDieAttributeNumber(object, DW_AT_type);
		uint32_t fileID    = parser->getFileID();
		this->rettype = this->manager->getID(dwarfType, fileID);
		if (!this->rettype) {
			assert(false);
		}
	}
	if (this->address == 0 && parser->dieHasAttr(object, DW_AT_low_pc)) {
		this->address = parser->getDieAttributeNumber(object, DW_AT_low_pc);
	}
	this->paramsFinal = true; // < why??
}

bool Function::operator <(const Function &func) const {
	if (this->rettype != func.rettype)
		return this->rettype < func.rettype;
	if (this->paramList.size() != func.paramList.size())
		return this->paramList.size() < func.paramList.size();

	for (size_t i = 0; i < this->paramList.size(); i++) {
		if (this->paramList[i] != func.paramList[i])
			return this->paramList[i] < func.paramList[i];
	}
	if (this->id != func.id)
		return this->id < func.id;
	return false;
}

bool Function::operator ==(const Function &func) const {
	if (this->rettype != func.rettype) {
		return false;
	}

	if (this->paramList.size() != func.paramList.size()) {
		return false;
	}

	for (size_t i = 0; i < this->paramList.size(); i++) {
		if (this->paramList[i] != func.paramList[i])
			return false;
	}
	if (this->name != func.name) {
		return false;
	}
	return true;
}

void Function::updateTypes() {
	Symbol *sym;
	if (this->rettype) {
		sym           = this->manager->findSymbolByID(this->rettype);
		this->rettype = sym->getID();
	}
	for (size_t i = 0; i < this->paramList.size(); i++) {
		sym = this->manager->findSymbolByID(this->paramList[i]);
		assert(sym);
		this->paramList[i] = sym->getID();
	}
}


void Function::print() {
	BaseType::print();
	std::cout << "\t Address:      " << std::hex << this->address << std::dec
	          << std::endl;
	for (auto &param : this->paramList) {
		std::cout << "\t Param:        " << std::hex << param << std::dec
		          << std::endl;
	}
}

uint64_t Function::getAddress() {
	return this->address;
}
