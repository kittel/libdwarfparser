#ifndef _VARIABLE_H_
#define _VARIABLE_H_

#include "symbol.h"
#include "referencingtype.h"

class Variable : public Symbol, public ReferencingType {
public:
	Variable(Dwarf_Die object);
	virtual ~Variable();

	uint64_t getLocation();
	void setLocation(uint64_t location);

private:
	uint64_t location;
};

#endif  /* _VARIABLE_H_ */

