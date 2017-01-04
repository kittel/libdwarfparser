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

	/**
	 * Add a member to this Structured type
	 */
	virtual StructuredMember *addMember(SymbolManager *mgr,
	                                    DwarfParser *parser,
	                                    const Dwarf_Die &object,
	                                    const std::string &memberName);
	/**
	 * @return Pointer to Member of Structured Type by member name
	 */
	StructuredMember *memberByName(const std::string &name);

	/**
	 * Print all members to stdout
	 */
	void listMembers();

	/**
	 * @return Pointer to Member of Structured Type by member offset
	 */
	StructuredMember *memberByOffset(uint32_t offset);

	/**
	 * @return Name of Member of Structured Type by member offset
	 */
	std::string memberNameByOffset(uint32_t offset);

	/**
	 * @return Offset of Member of Structured Type by member name
	 */
	uint32_t memberOffset(const std::string &member) const;

	virtual void print() const override;

private:
	typedef std::unordered_map<std::string, StructuredMember *> MemberNameMap;
	MemberNameMap memberNameMap;
	std::mutex memberMutex;
};

#endif /* _STRUCTURED_H_ */
