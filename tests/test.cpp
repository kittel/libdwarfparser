#include "CppUTest/CommandLineTestRunner.h"

int main(int argc, char** argv)
{
	  return CommandLineTestRunner::RunAllTests(argc, argv);
}

IMPORT_TEST_GROUP(DwarfTests);
