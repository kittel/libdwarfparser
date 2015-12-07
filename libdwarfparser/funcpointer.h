#ifndef _FUNCPOINTER_H_
#define _FUNCPOINTER_H_

#include "refbasetype.h"

class FuncPointer : public RefBaseType {
public:
	FuncPointer(SymbolManager *mgr,
	            DwarfParser *parser,
	            const Dwarf_Die &object,
	            const std::string &name);
	virtual ~FuncPointer();
};

#endif /* _FUNCPOINTER_H_ */
