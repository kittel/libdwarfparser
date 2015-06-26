#ifndef _STRUCTUREDMEMBER_H_
#define _STRUCTUREDMEMBER_H_

#include "symbol.h"
#include "referencingtype.h"

#include "structured.h"

class StructuredMember: public Symbol, public ReferencingType {
public:
	StructuredMember(DwarfParser *parser, Dwarf_Die object, 
			std::string name, Structured *parent);
	virtual ~StructuredMember();

	uint32_t getByteSize();
	uint32_t getBitSize();
	uint32_t getBitOffset();
	uint32_t getMemberLocation();


protected:
	uint32_t bitSize;
	uint32_t bitOffset;
	uint32_t memberLocation;

	Structured* parent;
};

#endif  /* _STRUCTUREDMEMBER_H_ */

