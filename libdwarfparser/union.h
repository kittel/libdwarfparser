#ifndef _UNION_H_
#define _UNION_H_

#include "structured.h"

class Union: public Structured {
public:
	Union(Dwarf_Die object);
	virtual ~Union();
};

#endif  /* _UNION_H_ */
