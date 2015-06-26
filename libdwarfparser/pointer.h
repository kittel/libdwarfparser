#ifndef _POINTER_H_
#define _POINTER_H_

#include "refbasetype.h"

class Pointer: public RefBaseType {
public:
	Pointer(DwarfParser *parser, Dwarf_Die object, std::string name);
	virtual ~Pointer();

	void print();
};

#endif  /* _POINTER_H_ */

