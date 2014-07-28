#include "libdwarfparser/libdwarfparser.h"

#include <iostream>
#include <cassert>


void checkEnum(){
	BaseType* bt;
	bt = BaseType::findBaseTypeByName("testenum");
	assert(bt);
	assert(bt->getName() == std::string("testenum"));

	Enum *enumPtr = dynamic_cast<Enum*>(bt);
	assert(enumPtr);
	assert(enumPtr->enumValue(std::string("testenumzero")) == 0);
	assert(enumPtr->enumName(0) == std::string("testenumzero"));
}
	
int main(int argc, char **argv){
    if(argc < 2) {
		printf("Expecting filename as argument\n");
		return 0;
    } else {
        printf("Using %s as file\n", argv[1]);
		try{
			DwarfParser::parseDwarf(std::string(argv[1]));
		}catch(DwarfException &e){
			std::cout << e.what() << std::endl;
		}
    }
	checkEnum();

    return 0;
}
