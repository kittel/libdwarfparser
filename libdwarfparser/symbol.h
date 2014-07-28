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
	uint64_t getID();
	std::string getName();

	static Symbol* findSymbolByName(std::string name);
	static Symbol* findSymbolByID(uint64_t id);

protected:
	uint64_t id;
	uint32_t byteSize;
	std::string name;
	
	typedef std::map<std::string, Symbol*> SymbolNameMap;
	typedef std::map<uint64_t, Symbol*> SymbolIDMap;

	static SymbolNameMap symbolNameMap;
	static SymbolIDMap symbolIDMap;
};

#endif  /* _SYMBOL_H_ */

