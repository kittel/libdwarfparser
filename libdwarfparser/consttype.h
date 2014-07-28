#ifndef _CONSTTYPE_H_
#define _CONSTTYPE_H_

#include "refbasetype.h"

class ConstType: public RefBaseType {
public:
	ConstType(Dwarf_Die object);
	virtual ~ConstType();
};

#endif  /* _CONSTTYPE_H_ */

