#ifndef _SYMBOL_H_
#define _SYMBOL_H_

#include <list>
#include <map>
#include <string.h>
#include <cstdint>

struct Dwarf_Die_s;
typedef struct Dwarf_Die_s* Dwarf_Die;

class IDManager {

public:
	static uint64_t getID(uint64_t dwarfID, uint64_t fileID);

protected:

	static uint64_t nextID;
	typedef std::map<std::pair<uint64_t, uint64_t>, uint64_t> IDMap;
	typedef std::map<uint64_t, std::pair<uint64_t, uint64_t>> IDRevMap;
	static IDMap idMap;
	static IDRevMap idRevMap;

private:
	IDManager();
	virtual ~IDManager();

};

class Symbol {


public:
	Symbol(Dwarf_Die object);
	virtual ~Symbol();

	virtual uint32_t getByteSize();
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
	static std::list<uint64_t> getAliases(uint64_t id);

protected:
	uint64_t id;
	uint32_t byteSize;
	std::string name;
	
	typedef std::multimap<std::string, Symbol*> SymbolNameMap;
	typedef std::map<uint64_t, Symbol*> SymbolIDMap;
	typedef std::map<uint64_t, uint64_t> SymbolIDAliasMap;
	typedef std::map<uint64_t, std::list<uint64_t>> SymbolIDAliasReverseList;

	static SymbolNameMap symbolNameMap;
	static SymbolIDMap symbolIDMap;
	static SymbolIDAliasMap symbolIDAliasMap;
	static SymbolIDAliasReverseList symbolIDAliasReverseList;

};

#endif  /* _SYMBOL_H_ */

