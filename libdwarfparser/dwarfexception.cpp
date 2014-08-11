#include "dwarfexception.h"

DwarfException::DwarfException() throw() {}

DwarfException::DwarfException(const char* reason) throw() : reason(reason){}

DwarfException::~DwarfException() throw() {}

const char* DwarfException::what() const throw(){
	std::string result = std::string("DwarfException: ");
	return result.append(this->reason).c_str();
}

DwarfTypeException::DwarfTypeException() throw() {}

DwarfTypeException::DwarfTypeException(const char* reason) throw() : reason(reason){}

DwarfTypeException::~DwarfTypeException() throw() {}

const char* DwarfTypeException::what() const throw(){
	std::string result = std::string("DwarfTypeException: ");
	return result.append(this->reason).c_str();
}
