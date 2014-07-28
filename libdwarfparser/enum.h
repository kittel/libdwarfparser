#ifndef _ENUM_H_
#define _ENUM_H_

#include "basetype.h"

#include <map>

typedef std::map<uint32_t, std::string> EnumValues;

class Enum: public BaseType {
public:
	Enum(Dwarf_Die object);
	virtual ~Enum();

	void addEnum(Dwarf_Die object);
	std::string enumName(uint32_t value);
	uint32_t enumValue(std::string name);
	void printEnumMembers(std::ostream &stream);

private:

	EnumValues enumValues;
	
};

#endif  /* _ENUM_H_ */

