#ifndef _VARIABLE_H_
#define _VARIABLE_H_

#include "symbol.h"
#include "referencingtype.h"

class Instance;

class Variable : public Symbol, public ReferencingType {
public:
	Variable(DwarfParser *parser, const Dwarf_Die &object, 
			const std::string &name);
	virtual ~Variable();

	uint64_t getLocation();
	void update(DwarfParser *parser, const Dwarf_Die &object);
	void setLocation(uint64_t location);

	static Variable* findVariableByID(uint64_t id);
	static Variable* findVariableByName(std::string name);
	Instance getInstance();

private:
	uint64_t location;
	
	typedef std::map<std::string, Variable*> VariableNameMap;
	static VariableNameMap variableNameMap;
};

#endif  /* _VARIABLE_H_ */

