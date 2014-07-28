#include <stdio.h>
#include <inttypes.h>

#include "libdwarfparser/helpers.h"


struct teststruct{
	uint8_t testmember1;
	uint64_t testmember2;
};

enum testenum{
	testenumzero = 0,
	testenumone,
	testenumtwo
};

int main(int argc, char** argv){
	UNUSED(argc);
	UNUSED(argv);
	struct teststruct test;
	test.testmember1 = testenumzero;
	test.testmember2 = testenumone;

	printf("%lu\n", test.testmember2);

	return 0;
}
