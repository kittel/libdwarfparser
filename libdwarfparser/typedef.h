#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_

#include "refbasetype.h"

class Typedef: public RefBaseType {
public:
	Typedef(Dwarf_Die object);
	virtual ~Typedef();
};

#endif  /* _TYPEDEF_H_ */

