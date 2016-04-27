#ifndef _DWARFEXCEPTION_H_
#define _DWARFEXCEPTION_H_

#include <exception>
#include <string>

class DwarfException : std::exception {
public:
	DwarfException() throw();
	DwarfException(const char* reason) throw();
	virtual ~DwarfException() throw();
	const char* what() const throw() override;

private:
	std::string reason;
};

class DwarfTypeException : DwarfException {
public:
	DwarfTypeException() throw();
	DwarfTypeException(const char* reason) throw();
	virtual ~DwarfTypeException() throw();
	const char* what() const throw() override;

private:
	std::string reason;
};

#endif  /* _DWARFEXCEPTION_H_ */

