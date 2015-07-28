#ifndef _UNION_H_
#define _UNION_H_

#include "structured.h"

class Union: public Structured {
public:
	Union(DwarfParser *parser, 
			const Dwarf_Die &object, 
			const std::string &name);
	virtual ~Union();
};

#endif  /* _UNION_H_ */
