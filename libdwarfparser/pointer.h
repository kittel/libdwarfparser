#ifndef _POINTER_H_
#define _POINTER_H_

#include "refbasetype.h"

class Pointer: public RefBaseType {
public:
	Pointer(Dwarf_Die object);
	virtual ~Pointer();
};

#endif  /* _POINTER_H_ */

