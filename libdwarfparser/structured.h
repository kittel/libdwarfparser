#ifndef _STRUCTURED_H_
#define _STRUCTURED_H_

#include "basetype.h"

#include <unordered_map>

class StructuredMember;

class Structured : public BaseType {
public:
	Structured(SymbolManager *mgr,
	           DwarfParser *parser,
	           const Dwarf_Die &object,
	           const std::string &name);
	virtual ~Structured();

	virtual StructuredMember *addMember(SymbolManager *mgr,
	                                    DwarfParser *parser,
	                                    const Dwarf_Die &object,
	                                    const std::string &memberName);
	StructuredMember *memberByName(const std::string &name);

	void listMembers();
	StructuredMember *memberByOffset(uint32_t offset);
	std::string memberNameByOffset(uint32_t offset);
	uint32_t memberOffset(const std::string &member) const;

	virtual void print() const;

private:
	typedef std::unordered_map<std::string, StructuredMember *> MemberNameMap;
	MemberNameMap memberNameMap;
	std::mutex memberMutex;
};

#endif /* _STRUCTURED_H_ */
