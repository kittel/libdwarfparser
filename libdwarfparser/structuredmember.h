#ifndef _STRUCTUREDMEMBER_H_
#define _STRUCTUREDMEMBER_H_

#include "symbol.h"
#include "referencingtype.h"

#include "structured.h"

class StructuredMember : public Symbol, public ReferencingType {
public:
	StructuredMember(SymbolManager *mgr, DwarfParser *parser,
	                 const Dwarf_Die &object,
	                 const std::string &name,
	                 Structured *parent);
	virtual ~StructuredMember();

	uint32_t getByteSize() override;
	uint32_t getBitSize();
	uint32_t getBitOffset();
	uint32_t getMemberLocation();

protected:
	uint32_t bitSize;
	uint32_t bitOffset;
	uint32_t memberLocation;

	Structured *parent;
};

#endif /* _STRUCTUREDMEMBER_H_ */
