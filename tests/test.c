#include <stdio.h>
#include <inttypes.h>


struct teststruct{
	uint8_t testmember1;
	uint64_t testmember2;
};

typedef struct teststruct teststruct;

int main(int argc, char** argv){
	teststruct test;
	test.testmember1 = 0;

	printf("7\n");
}
