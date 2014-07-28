#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include "funcpointer.h"

class Function : public FuncPointer {
public:
	Function(Dwarf_Die object);
	virtual ~Function();
};

#endif  /* _FUNCTION_H_ */

