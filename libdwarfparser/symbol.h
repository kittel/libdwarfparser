#ifndef _SYMBOL_H_
#define _SYMBOL_H_

#include <list>
#include <set>
#include <map>
#include <string.h>
#include <cstdint>
#include <mutex>

class DwarfParser;

struct Dwarf_Die_s;
typedef struct Dwarf_Die_s* Dwarf_Die;

class IDManager {

public:
	static uint64_t getID(uint64_t dwarfID, uint32_t fileID);
	static std::pair<uint64_t, uint32_t> getRevID(uint64_t id);

protected:

	static uint64_t nextID;
	typedef std::map<std::pair<uint64_t, uint32_t>, uint64_t> IDMap;
	typedef std::map<uint64_t, std::pair<uint64_t, uint32_t>> IDRevMap;
	static IDMap idMap;
	static IDRevMap idRevMap;

private:
	IDManager();
	virtual ~IDManager();

};

class Symbol {


public:
	Symbol(DwarfParser *parser, const Dwarf_Die &object, const std::string &name);
	virtual ~Symbol();

	virtual uint32_t getByteSize();
	uint64_t getID();
	std::string getName();

	void addAlternativeID(uint64_t id);
	void addAlternativeDwarfID(uint64_t id, uint32_t fileID);
	virtual void print();


	//Static Functions
	static Symbol* findSymbolByName(const std::string &name);
	template<class T>
	static inline T* findSymbolByName(const std::string &name){
		auto bt = symbolNameMap.find(name);
		for(auto i : symbolNameMap){
			T* t = dynamic_cast<T*>(i.second);
			if(t) return t;
		}
		return NULL;
	}

	static Symbol* findSymbolByID(uint64_t id);
	static uint64_t numberOfSymbols();
	static std::set<uint64_t> getAliases(uint64_t id);

protected:
	uint64_t id;
	uint32_t byteSize;
	std::string name;
	
	typedef std::multimap<std::string, Symbol*> SymbolNameMap;
	typedef std::map<uint64_t, Symbol*> SymbolIDMap;
	typedef std::map<uint64_t, uint64_t> SymbolIDAliasMap;
	typedef std::map<uint64_t, std::set<uint64_t>> SymbolIDAliasReverseList;

	static SymbolNameMap            symbolNameMap;
	static std::mutex               symbolNameMapMutex;
	static SymbolIDMap              symbolIDMap;
	static std::mutex               symbolIDMapMutex;
	static SymbolIDAliasMap         symbolIDAliasMap;
	static std::mutex               symbolIDAliasMapMutex;
	static SymbolIDAliasReverseList symbolIDAliasReverseList;
	static std::mutex               symbolIDAliasReverseListMutex;

};

#endif  /* _SYMBOL_H_ */

