#include "vmiexception.h"

VMIException::VMIException() throw() {}
VMIException::VMIException(const char* reason) throw() : reason(reason){}
VMIException::~VMIException() throw() {}
const char* VMIException::what() const throw(){
	std::string result = std::string("VMIException: ");
	return result.append(this->reason).c_str();
}

