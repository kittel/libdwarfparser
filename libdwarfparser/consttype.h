#ifndef _CONSTTYPE_H_
#define _CONSTTYPE_H_

#include "refbasetype.h"

class ConstType: public RefBaseType {
public:
	ConstType(DwarfParser *parser, const Dwarf_Die &object, 
			const std::string &name);
	virtual ~ConstType();
};

#endif  /* _CONSTTYPE_H_ */

