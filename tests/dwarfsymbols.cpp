#include "libdwarfparser/libdwarfparser.h"

#include <iostream>
#include <cassert>

void checkEnum(SymbolManager *mgr) {
	BaseType *bt = mgr->findBaseTypeByName("testenum");
	assert(bt);
	assert(bt->getName() == std::string("testenum"));

	Enum *enumPtr = dynamic_cast<Enum *>(bt);
	assert(enumPtr);
	assert(enumPtr->enumValue(std::string("testenumzero")) == 0);
	assert(enumPtr->enumName(0) == std::string("testenumzero"));
}

void checkStruct(SymbolManager *mgr) {
	BaseType *bt;
	bt = mgr->findBaseTypeByName("teststruct1");
	assert(bt);
	assert(bt->getName() == std::string("teststruct1"));
	assert(bt->getByteSize() == 16);

	Struct *structPtr;
	structPtr = dynamic_cast<Struct *>(bt);
	assert(structPtr);
	StructuredMember *sm;
	sm = structPtr->memberByName("testmember1");
	assert(sm);

	bt = mgr->findBaseTypeByName("teststruct2");
	assert(bt);
	structPtr = dynamic_cast<Struct *>(bt);
	assert(structPtr);
	sm = structPtr->memberByName("testmember3");
	assert(sm);
	assert(sm->getByteSize() == 4);
	assert(sm->getBitSize() == 1);
	assert(sm->getBitOffset() == 23);
	assert(sm->getMemberLocation() == 8);
}

int main(int argc, char **argv) {
	SymbolManager mgr;

	if (argc < 2) {
		printf("Expecting filename as argument\n");
		return 0;
	} else {
		// printf("Using %s as file\n", argv[1]);
		try {
			DwarfParser::parseDwarfFromFilename(std::string(argv[1]), &mgr);
		} catch (DwarfException &e) {
			std::cout << e.what() << std::endl;
		}
	}
	checkEnum(&mgr);
	checkStruct(&mgr);

	return 0;
}
