#ifndef _SYMBOL_H_
#define _SYMBOL_H_

#include <map>
#include <dwarf.h>
#include <libdwarf.h>
#include <string.h>
#include <cstdint>

class Symbol {

public:
	Symbol(Dwarf_Die object);
	virtual ~Symbol();

	uint32_t getByteSize();
	std::string getName();

	static Symbol* findSymbolByName(std::string name);

protected:
	uint64_t offset;
	uint32_t byteSize;
	std::string name;
	
	typedef std::map<std::string, Symbol*> SymbolMap;
	static SymbolMap symbolMap;
};

#endif  /* _SYMBOL_H_ */

