#include "libdwarfparser/libdwarfparser.h"

#include <iostream>

	
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
    return 0;
}
