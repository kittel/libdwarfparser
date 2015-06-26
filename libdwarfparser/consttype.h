#ifndef _CONSTTYPE_H_
#define _CONSTTYPE_H_

#include "refbasetype.h"

class ConstType: public RefBaseType {
public:
	ConstType(DwarfParser *parser, Dwarf_Die object, std::string name);
	virtual ~ConstType();
};

#endif  /* _CONSTTYPE_H_ */

