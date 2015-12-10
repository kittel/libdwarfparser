#ifndef _POINTER_H_
#define _POINTER_H_

#include "refbasetype.h"

class Pointer : public RefBaseType {
public:
	Pointer(SymbolManager *mgr,
	        DwarfParser *parser,
	        const Dwarf_Die &object,
	        const std::string &name);
	virtual ~Pointer();

	virtual void print() const;
};

#endif /* _POINTER_H_ */
