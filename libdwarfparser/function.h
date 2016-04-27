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
	void print() const override;

	uint64_t getAddress();

	void updateTypes();

	std::vector<std::pair<std::string,uint64_t>> getParamList() const;
	std::vector<std::pair<std::string, BaseType*>> getFullParamList() const;
	BaseType* getParamByName(const std::string& name) const;
	uint64_t getRetTypeID() const;
	BaseType* getRetType() const;

protected:
	uint64_t rettype;
	uint64_t address;

	// Map might also work, however I assume that a list is faster for <10 entries
	typedef std::vector<std::pair<std::string, uint64_t>> ParamList;
	ParamList paramList;
	bool paramsFinal;
};

#endif /* _FUNCTION_H_ */
