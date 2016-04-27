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

	/**
	 * @return Location of Symbol this Variable points to.
	 */
	uint64_t getLocation();
	void update(DwarfParser *parser, const Dwarf_Die &object);

	/**
	 * @param location New location of Symbol this Variable points to.
	 * Set a new location for this Variable.
	 */
	void setLocation(uint64_t location);

	/**
	 * @return Instance of this Variable.
	 */
	Instance getInstance();

	void print() const override;

private:
	uint64_t location; ///< Location of referenced Symbol.
};

#endif /* _VARIABLE_H_ */
