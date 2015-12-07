#ifndef _CONSTTYPE_H_
#define _CONSTTYPE_H_

#include "refbasetype.h"

class ConstType : public RefBaseType {
public:
	ConstType(SymbolManager *mgr,
	          DwarfParser *parser,
	          const Dwarf_Die &object,
	          const std::string &name);
	virtual ~ConstType();

	void print();
};

#endif /* _CONSTTYPE_H_ */
