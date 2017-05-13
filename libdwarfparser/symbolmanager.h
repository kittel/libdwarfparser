#ifndef _SYMBOLMANAGER_H_
#define _SYMBOLMANAGER_H_

#include <cstdint>
#include <cstring>
#include <map>
#include <mutex>
#include <set>
#include <unordered_map>
#include <vector>

class Array;
class BaseType;
class Function;
class Symbol;
class RefBaseType;
class Variable;


template <int pos>
constexpr uint64_t bit() {
	return 1UL << pos;
}

/**
 * available sources for symbols
 */
enum class symbol_source : uint64_t {
	system_map = bit<0>(),
	modules = bit<1>(),
	functions = bit<2>(),
	dwarf_function = bit<3>(),
	dwarf_variable = bit<4>(),
	all = 0xffffffffffffffff,
};

inline constexpr symbol_source operator &(symbol_source __x,
                                          symbol_source __y) {
	return static_cast<symbol_source>(static_cast<uint64_t>(__x) &
	                                  static_cast<uint64_t>(__y));
}

inline constexpr symbol_source operator |(symbol_source __x,
                                          symbol_source __y) {
	return static_cast<symbol_source>(static_cast<uint64_t>(__x) |
	                                  static_cast<uint64_t>(__y));
}

inline constexpr symbol_source operator ~(symbol_source __x) {
	return static_cast<symbol_source>(~static_cast<uint64_t>(__x));
}


/**
 * Manages a symbol namespace.
 */
class SymbolManager {
public:
	SymbolManager();
	virtual ~SymbolManager();

	// disallow copies and moves.
	SymbolManager(SymbolManager &&other) = delete;
	SymbolManager &operator =(SymbolManager &&other) = delete;
	SymbolManager(const SymbolManager &other) = delete;
	SymbolManager &operator =(const SymbolManager &other) = delete;

	/**
	 * Create or return a id in this id namespace for a dwarfid/fileid pair.
	 */
	uint64_t getID(uint64_t dwarfID, uint32_t fileID);
	std::pair<uint64_t, uint32_t> getRevID(uint64_t id);

	void addSymbol(Symbol *sym);
	void addAlternativeID(uint64_t id, uint64_t new_id);
	void addBaseType(BaseType *bt);
	void addRefBaseType(RefBaseType *bt);
	void addFunction(Function *fun);
	void addArray(Array *ar);
	void addVariable(Variable *var);

	void removeSymbol(Symbol *sym);
	void removeSymbol(uint64_t id);

	/**
	 * Searching for a Symbol by Name is flawed by design.
	 * return the symbol by name and cast it to T.
	 */
	template <class T>
	inline T *findSymbolByName(const std::string &name) {
		T* t = nullptr;
		this->symbolNameMapMutex.lock();
		auto range = this->symbolNameMap.equal_range(name);
		for (auto it = range.first; it != range.second ; it++) {
			T *t = dynamic_cast<T *>(it->second);
			if (t) break;
		}
		this->symbolNameMapMutex.unlock();
		return t;
	}

	Symbol *findSymbolByID(uint64_t id);
	uint64_t numberOfSymbols();
	std::set<uint64_t> getAliases(uint64_t id);

	BaseType *findBaseTypeByID(uint64_t id);
	BaseType *findBaseTypeByName(const std::string &name);

	Function *findFunctionByID(uint64_t id);
	Function *findFunctionByName(const std::string &name);
	void cleanFunctions();

	template <class T>
	T *findBaseTypeByName(const std::string &name) {
		auto range = this->baseTypeNameMap.equal_range(name);
		for (auto i = range.first; i != range.second; ++i) {
			T *t = dynamic_cast<T *>(i->second);
			if (t)
				return t;
		}
		return nullptr;
	}

	RefBaseType *findRefBaseTypeByID(uint64_t id);
	RefBaseType *findRefBaseTypeByName(const std::string &name);

	Array *findArrayByID(uint64_t id);
	Array *findArrayByTypeID(uint64_t id, uint64_t length);
	void cleanArrays();

	Variable *findVariableByID(uint64_t id);
	Variable *findVariableByName(const std::string &name);
	std::vector<std::string> getVarNames();

	// migrated from kernel.h
	/** return the address of public system map symbol */
	uint64_t getSystemMapAddress(const std::string &name, bool priv=false);

	/**
	 * add an elf symbol.
	 * if replace is true, previous entries will be overwritten.
	 * the return value will then tell you if that was the case.
	 * @returns true if it was already known
	 */
	bool addSymbolAddress(const std::string &name, uint64_t address,
	                      bool replace=false);

	/** try all possible sources for finding the address for a symbol */
	uint64_t getSymbolAddress(const std::string &name,
	                          symbol_source src=symbol_source::all);

	/** return the address of a elf symbol */
	uint64_t getElfSymbolAddress(const std::string &name);

	/** return the symbol name for given address */
	std::string getElfSymbolName(uint64_t address);

	/** test if the address is known as symbol. */
	bool isSymbol(uint64_t address);

	/** test if address is known as function */
	bool isFunction(uint64_t address);

	/** add a known function address */
	void addFunctionAddress(const std::string &name, uint64_t address);

	/** return the address of a function with given name */
	uint64_t getFunctionAddress(const std::string &name);

	/** return the function name of function at address */
	std::string getFunctionName(uint64_t address);

	/** return the symbol that is the next one previous to address */
	uint64_t getContainingSymbol(uint64_t address);

	void updateRevMaps();
	// ------

	/** place a new entry in the sysmap symbol map */
	void addSysmapSymbol(const std::string &name, uint64_t address, bool priv);


protected:
	typedef std::unordered_map<std::string, uint64_t> SymbolMap;
	typedef std::map<uint64_t, std::string> SymbolRevMap;
	SymbolMap sysMapSymbols;            // sysmap symbols
	SymbolMap sysMapSymbolsPrivate;     // sysmap private symbols

	SymbolMap elfSymbolMap;  // module symbols from elf files
	SymbolMap functionSymbolMap; // functions
	SymbolRevMap elfSymbolRevMap; // addr -> symbol
	SymbolRevMap functionSymbolRevMap;  // addr -> funcname

	/**
	 * The last id issued.
	 */
	uint64_t currentID;

	typedef std::map<std::pair<uint64_t, uint32_t>, uint64_t> IDMap;
	typedef std::unordered_map<uint64_t, std::pair<uint64_t, uint32_t>> IDRevMap;
	typedef std::multimap<std::string, Symbol *> SymbolNameMap;
	typedef std::unordered_map<uint64_t, Symbol *> SymbolIDMap;
	typedef std::unordered_map<uint64_t, uint64_t> SymbolIDAliasMap;
	typedef std::unordered_map<uint64_t, std::set<uint64_t>> SymbolIDAliasReverseList;
	typedef std::multimap<std::string, BaseType *> BaseTypeNameMap;
	typedef std::unordered_map<std::string, Function *> FunctionNameMap;
	typedef std::vector<Function *> FuncList;
	typedef std::unordered_map<std::string, RefBaseType*> RefBaseTypeNameMap;
	typedef std::multimap<uint64_t, Array *> ArrayTypeMap;
	typedef std::vector<Array *> ArrayVector;
	typedef std::map<std::string, Variable *> VariableNameMap;

	IDRevMap                 idRevMap;
	IDMap                    idMap;
	std::mutex               mapMutex;

	SymbolNameMap            symbolNameMap;
	std::mutex               symbolNameMapMutex;

	SymbolIDMap              symbolIDMap;
	std::mutex               symbolIDMapMutex;

	SymbolIDAliasMap         symbolIDAliasMap;
	std::mutex               symbolIDAliasMapMutex;

	SymbolIDAliasReverseList symbolIDAliasReverseList;
	std::mutex               symbolIDAliasReverseListMutex;

	BaseTypeNameMap          baseTypeNameMap;

	FunctionNameMap          functionNameMap;
	std::mutex               functionNameMapMutex;

	FuncList                 funcList;
	std::mutex               funcListMutex;

	RefBaseTypeNameMap       refBaseTypeNameMap;

	ArrayVector              arrayVector;
	std::mutex               arrayVectorMutex;

	ArrayTypeMap             arrayTypeMap;
	std::mutex               arrayTypeMapMutex;

	VariableNameMap          variableNameMap;
};

#endif
