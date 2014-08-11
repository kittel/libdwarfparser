#ifndef _SYMBOL_H_
#define _SYMBOL_H_

#include <map>
#include <string.h>
#include <cstdint>

struct Dwarf_Die_s;
typedef struct Dwarf_Die_s* Dwarf_Die;

class Symbol {


public:
	Symbol(Dwarf_Die object);
	virtual ~Symbol();

	uint32_t getByteSize();
	uint64_t getID();
	std::string getName();

	void addAlternativeID(uint64_t id);

	static Symbol* findSymbolByName(std::string name);
	template<class T>
	static inline T* findSymbolByName(std::string name){
		auto bt = symbolNameMap.find(name);
		for(auto i = bt; bt != symbolNameMap.end(); ++i){
			T* t = dynamic_cast<T*>(i->second);
			if(t) return t;
		}
		return NULL;
	}

	static Symbol* findSymbolByID(uint64_t id);
	static uint64_t numberOfSymbols();

protected:
	uint64_t id;
	uint32_t byteSize;
	std::string name;
	
	typedef std::multimap<std::string, Symbol*> SymbolNameMap;
	typedef std::map<uint64_t, Symbol*> SymbolIDMap;
	typedef std::map<uint64_t, uint64_t> SymbolIDAliasMap;

	static SymbolNameMap symbolNameMap;
	static SymbolIDMap symbolIDMap;
	static SymbolIDAliasMap symbolIDAliasMap;
};

#endif  /* _SYMBOL_H_ */

