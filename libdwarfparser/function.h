#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include "basetype.h"

#include <vector>

class Function : public BaseType {
public:
	Function(SymbolManager *mgr,
	         DwarfParser *parser,
	         const Dwarf_Die &object,
	         const std::string &name);
	virtual ~Function();

	void addParam(DwarfParser *parser,
	              const Dwarf_Die &object);

	bool operator <(const Function &func) const;
	bool operator ==(const Function &func) const;
	void update(DwarfParser *parser, const Dwarf_Die &object);
	virtual void print() const;

	uint64_t getAddress();

	void updateTypes();

protected:
	uint64_t rettype;
	uint64_t address;

	typedef std::vector<uint64_t> ParamList;
	ParamList paramList;
	bool paramsFinal;
};

#endif /* _FUNCTION_H_ */
