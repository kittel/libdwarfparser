#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_

#include "refbasetype.h"

class Typedef: public RefBaseType {
public:
	Typedef(DwarfParser *parser, Dwarf_Die object, std::string name);
	virtual ~Typedef();
	
};

#endif  /* _TYPEDEF_H_ */

