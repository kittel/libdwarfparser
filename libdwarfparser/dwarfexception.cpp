#include "dwarfexception.h"
#include <iostream>

DwarfException::DwarfException() throw() {}

DwarfException::DwarfException(const char* reason) throw() : reason(reason){}

DwarfException::~DwarfException() throw() {}

const char* DwarfException::what() const throw(){
	std::cout << "DwarfException: " << this->reason << std::endl;
	return "";
}

DwarfTypeException::DwarfTypeException() throw() {}

DwarfTypeException::DwarfTypeException(const char* reason) throw() : reason(reason){}

DwarfTypeException::~DwarfTypeException() throw() {}

const char* DwarfTypeException::what() const throw(){
	std::cout << "DwarfTypeException: " << this->reason << std::endl;
	return "";
}
