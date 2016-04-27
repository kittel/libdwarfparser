#ifndef _ENUM_H_
#define _ENUM_H_

#include "basetype.h"

#include <map>

typedef std::map<uint32_t, std::string> EnumValues;

class Enum : public BaseType {
public:
	Enum(SymbolManager *mgr, DwarfParser *parser,
	     const Dwarf_Die &object, const std::string &name);
	virtual ~Enum();

	void addEnum(SymbolManager *mgr,
	             DwarfParser *parser,
	             const Dwarf_Die &object,
	             const std::string &name);
	std::string enumName(uint32_t value);
	uint32_t enumValue(const std::string &name);
	void printEnumMembers(std::ostream &stream);
	void print() const override;

private:
	EnumValues enumValues;
	std::mutex enumMutex;
};

#endif /* _ENUM_H_ */
