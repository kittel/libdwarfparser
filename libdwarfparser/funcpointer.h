#ifndef _FUNCPOINTER_H_
#define _FUNCPOINTER_H_

#include "refbasetype.h"

class FuncPointer: public RefBaseType {
public:
	FuncPointer(DwarfParser *parser, Dwarf_Die object, std::string name);
	virtual ~FuncPointer();
};

#endif  /* _FUNCPOINTER_H_ */

