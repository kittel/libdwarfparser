#ifndef _STRUCT_H_
#define _STRUCT_H_

#include "structured.h"

class Struct: public Structured {
public:
	Struct(DwarfParser *parser, Dwarf_Die object, std::string name);
	virtual ~Struct();
};

#endif  /* _STRUCT_H_ */

