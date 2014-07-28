#ifndef _REFBASETYPE_H_
#define _REFBASETYPE_H_

#include "basetype.h"

class RefBaseType: public BaseType {
public:
	RefBaseType(Dwarf_Die object);
	virtual ~RefBaseType();

	void resolveBaseType();
	BaseType* getBaseType();

protected:
	uint64_t type;
	BaseType* base;
};


#endif  /* _REFBASETYPE_H_ */

