#ifndef _UNION_H_
#define _UNION_H_

#include "structured.h"

class Union: public Structured {
public:
	Union(DwarfParser *parser, Dwarf_Die object, std::string name);
	virtual ~Union();
};

#endif  /* _UNION_H_ */
