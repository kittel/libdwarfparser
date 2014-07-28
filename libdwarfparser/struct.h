#ifndef _STRUCT_H_
#define _STRUCT_H_

#include "structured.h"

class Struct: public Structured {
public:
	Struct(Dwarf_Die object);
	virtual ~Struct();
};

#endif  /* _STRUCT_H_ */

