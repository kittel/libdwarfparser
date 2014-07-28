#ifndef _STRUCTURED_H_
#define _STRUCTURED_H_

#include "basetype.h"

#include <vector>

class StructuredMember;

class Structured: public BaseType {
public:
	Structured(Dwarf_Die object);
	virtual ~Structured();

	virtual void addMember(StructuredMember *member);
	StructuredMember* getMemberByName(std::string name);

private:
	std::vector<StructuredMember*> members;
};

#endif  /* _STRUCTURED_H_ */

