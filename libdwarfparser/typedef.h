#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_

#include "refbasetype.h"

class Typedef : public RefBaseType {
public:
	Typedef(SymbolManager *mgr,
	        DwarfParser *parser,
	        const Dwarf_Die &object,
	        const std::string &name);
	virtual ~Typedef();
};

#endif /* _TYPEDEF_H_ */
