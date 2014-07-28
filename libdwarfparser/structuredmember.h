#ifndef _STRUCTUREDMEMBER_H_
#define _STRUCTUREDMEMBER_H_

#include "symbol.h"
#include "referencingtype.h"

#include "structured.h"

class StructuredMember: public Symbol, public ReferencingType {
public:
	StructuredMember(Dwarf_Die object, Structured *parent);
	virtual ~StructuredMember();

protected:
	uint32_t bitoffset;

};

#endif  /* _STRUCTUREDMEMBER_H_ */

