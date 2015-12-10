#ifndef _VARIABLE_H_
#define _VARIABLE_H_

#include "symbol.h"
#include "referencingtype.h"

class Instance;

class Variable : public Symbol, public ReferencingType {
public:
	Variable(SymbolManager *mgr,
	         DwarfParser *parser,
	         const Dwarf_Die &object,
	         const std::string &name);
	virtual ~Variable();

	uint64_t getLocation();
	void update(DwarfParser *parser, const Dwarf_Die &object);
	void setLocation(uint64_t location);

	Instance getInstance();

	virtual void print() const;

private:
	uint64_t location;
};

#endif /* _VARIABLE_H_ */
