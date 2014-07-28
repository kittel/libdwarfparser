#ifndef _FUNCPOINTER_H_
#define _FUNCPOINTER_H_

#include "refbasetype.h"

class FuncPointer: public RefBaseType {
public:
	FuncPointer(Dwarf_Die object);
	virtual ~FuncPointer();
};

#endif  /* _FUNCPOINTER_H_ */

