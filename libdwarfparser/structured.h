#ifndef _STRUCTURED_H_
#define _STRUCTURED_H_

#include "basetype.h"

#include <map>

class StructuredMember;

class Structured: public BaseType {
public:
	Structured(DwarfParser *parser, Dwarf_Die object, std::string name);
	virtual ~Structured();

	virtual StructuredMember *addMember(DwarfParser *parser, 
			Dwarf_Die object, std::string memberName);
	StructuredMember* memberByName(std::string name);

	void listMembers();
	StructuredMember *memberByOffset(uint32_t offset);
	std::string memberNameByOffset(uint32_t offset);
	uint32_t memberOffset(std::string member) const;

	void print();

private:
	
	typedef std::map<std::string, StructuredMember*> MemberNameMap;
	MemberNameMap memberNameMap;
};

#endif  /* _STRUCTURED_H_ */

