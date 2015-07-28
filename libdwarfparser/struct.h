#ifndef _STRUCT_H_
#define _STRUCT_H_

#include "structured.h"

class Struct: public Structured {
public:
	Struct(DwarfParser *parser, const Dwarf_Die &object, const std::string &name);
	virtual ~Struct();
};

#endif  /* _STRUCT_H_ */

