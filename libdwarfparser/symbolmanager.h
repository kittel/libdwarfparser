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

	/** return the symbol associated with the name */
	Symbol *findSymbolByName(const std::string &name);

	/**
	 * return the symbol by name and cast it to T.
	 */
	template <class T>
	inline T *findSymbolByName(const std::string &name) {
		auto bt = this->symbolNameMap.find(name);
		for (auto &i : this->symbolNameMap) {
			T *t = dynamic_cast<T *>(i.second);
			if (t) {
				return t;
			}
		}
		return nullptr;
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
		auto bt = this->baseTypeNameMap.find(name);
		for (auto &i = bt; bt != this->baseTypeNameMap.end(); ++i) {
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

	void replaceBy(const SymbolManager &other);

protected:
	/**
	 * The last id issued.
	 */
	uint64_t currentID;

	typedef std::map<std::pair<uint64_t, uint32_t>, uint64_t> IDMap;
	typedef std::map<uint64_t, std::pair<uint64_t, uint32_t>> IDRevMap;
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
