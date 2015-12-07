#include "CppUTest/TestHarness.h"
#include "libdwarfparser/libdwarfparser.h"
#include "libdwarfparser/symbol.h"

#include <iostream>

TEST_GROUP(DwarfTests)
{
	//ClassName* className;

	void setup() {
		printf("Setup\n");
        //printf("Using %s as file\n", argv[1]);
		try{
			DwarfParser::parseDwarfFromFilename("./tests/dwarftestunit");
		}catch(DwarfException &e){
			FAIL("Could not load dwarf test unit");
		}
	}
	void teardown()
	{
		//delete className;
		printf("Teardown\n");
		Symbol::deleteAllSymbols();
	}
};

//TEST(DwarfTests, Create)
//{
//	//CHECK(0 != className);
//	CHECK(true);
//	CHECK_EQUAL(1,1);
//	LONGS_EQUAL(1,1);
//	DOUBLES_EQUAL(1.000, 1.001, .01);
//	STRCMP_EQUAL("hello", "hello");
//	FAIL("The prior tests pass, but this one doesn't");
//}

TEST(DwarfTests, checkFindBaseTypeByName) {
	//BaseType* bt;
	//bt = BaseType::findBaseTypeByName("testenum");
	//CHECK_TEXT(bt, "Could not find BaseType");
}

TEST(DwarfTests, checkEnum) {
	//BaseType* bt;
	//bt = BaseType::findBaseTypeByName("testenum");
	//CHECK_TEXT(bt, "Could not find BaseType");
	//CHECK(bt->getName() == std::string("testenum"));

	//Enum *enumPtr = dynamic_cast<Enum*>(bt);
	//CHECK_TEXT(enumPtr, "Casting to Enum");
	//CHECK(enumPtr->enumValue(std::string("testenumzero")) == 0);
	//CHECK(enumPtr->enumName(0) == std::string("testenumzero"));
}

TEST(DwarfTests, checkStruct) {
	//BaseType *bt;
	//bt = BaseType::findBaseTypeByName("teststruct1");
	//CHECK(bt);
	//CHECK(bt->getName() == std::string("teststruct1"));
	//CHECK(bt->getByteSize() == 16);

	//Struct *structPtr;
	//structPtr = dynamic_cast<Struct*>(bt);
	//CHECK(structPtr);
	//StructuredMember* sm;
	//sm = structPtr->memberByName("testmember1");
	//CHECK(sm);

	//bt = BaseType::findBaseTypeByName("teststruct2");
	//CHECK(bt);
	//structPtr = dynamic_cast<Struct*>(bt);
	//CHECK(structPtr);
	//sm = structPtr->memberByName("testmember3");
	//CHECK(sm);
	//CHECK(sm->getByteSize() == 4);
	//CHECK(sm->getBitSize() == 1);
	//CHECK(sm->getBitOffset() == 23);
	//CHECK(sm->getMemberLocation() == 8);
}
