#ifndef _BASETYPE_H_
#define _BASETYPE_H_

#include "symbol.h"

class BaseType: public Symbol {
public:
	BaseType(Dwarf_Die object);
	virtual ~BaseType();

	static BaseType* findBaseTypeById(uint64_t id);
	static BaseType* findBaseTypeByName(std::string name);
	uint64_t getEncoding();

private:
	uint64_t encoding;
	
	typedef std::map<std::string, BaseType*> BaseTypeNameMap;
	static BaseTypeNameMap baseTypeNameMap;
	
};

#endif  /* _BASETYPE_H_ */

