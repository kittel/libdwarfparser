#ifndef _REFBASETYPE_H_
#define _REFBASETYPE_H_

#include "basetype.h"

class RefBaseType : public BaseType {
public:
	RefBaseType(SymbolManager *mgr,
	            DwarfParser *parser,
	            const Dwarf_Die &object,
	            const std::string &name);
	virtual ~RefBaseType();

	void resolveBaseType();
	BaseType *getBaseType();

	virtual uint32_t getByteSize();

	uint64_t getType();

	void print();

protected:
	uint64_t type;
	BaseType *base;
};

#endif /* _REFBASETYPE_H_ */
