#include "dwarfexception.h"

DwarfException::DwarfException() throw() {}

DwarfException::DwarfException(const char* reason) throw() : reason(reason){}

DwarfException::~DwarfException() throw() {}

const char* DwarfException::what() const throw(){
	std::string result = std::string("DwarfException: ");
	return result.append(this->reason).c_str();
}
