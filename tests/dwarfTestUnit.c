#include <stdio.h>
#include <inttypes.h>

#include "helpers.h"


struct teststruct1{
	uint8_t testmember1;
	uint64_t testmember2;
};

struct teststruct2{
	uint8_t testmember1;
	uint64_t testmember2;
	uint32_t testmember3 : 1;
} __attribute__((packed));

enum testenum{
	testenumzero = 0,
	testenumone,
	testenumtwo
};

int main(int argc, char** argv){
	UNUSED(argc);
	UNUSED(argv); 
	struct teststruct1 teststruct1;
	teststruct1.testmember1 = testenumzero;
	teststruct1.testmember2 = testenumone;
	struct teststruct2 teststruct2;
	UNUSED(teststruct2);

	printf("%lu\n", teststruct1.testmember2);

	return 0;
}
