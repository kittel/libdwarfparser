#ifndef _VMIEXCEPTION_H
#define _VMIEXCEPTION_H

#include <exception>
#include <string>

class VMIException : std::exception {
	public:
		VMIException() throw();
		VMIException(const char* reason) throw();
		virtual ~VMIException() throw();
		virtual const char* what() const throw();

	private:
		std::string reason;
};



#endif  /* _VMIEXCEPTION_H */
