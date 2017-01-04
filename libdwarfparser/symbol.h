#ifndef _SYMBOL_H_
#define _SYMBOL_H_

#include <cstdint>
#include <string>

struct Dwarf_Die_s;
typedef struct Dwarf_Die_s *Dwarf_Die;

class DwarfParser;
class SymbolManager;

/**
 * A symbol in one symbol namespace.
 */
class Symbol {
public:
	/**
	 * Constructs a symbol and registers it at its manager.
	 */
	Symbol(SymbolManager *manager, DwarfParser *parser,
	       const Dwarf_Die &object, const std::string &name);
	virtual ~Symbol();

	/**
	 * @return SymbolManager in charge of the Symbol
	 */
	virtual SymbolManager* getManager() const;

	/**
	 * @return Number of Bytes occupied by the current symbol.
	 */
	virtual uint32_t getByteSize();

	/**
	 * @return Internal ID of the current Symbol. This is NOT the dwarf ID of the symbol.
	 */
	uint64_t getID() const;

	/**
	 * @return Name of the Symbol (BaseType or Variable name)
	 */
	const std::string &getName() const;

	/**
	 * Internal function to register an alternative DwarfID. 
	 * Required to handle duplicate entries within the dwarf database.
	 */
	void addAlternativeDwarfID(uint64_t dwarfid, uint32_t fileID);

	/**
	 * Print the contents of this symbol to stdout. This function will soon be
	 * replaced by overloading the << operator.
	 */
	virtual void print() const;

protected:
	SymbolManager *const manager; ///< Reference to the corresponding SymbolManager.
	const std::string name; ///< Name of the Symbol.
	uint32_t byteSize; ///< Size of the Symbol in Bytes.
	uint64_t id; ///< Internal ID of the Symbol.
};

#endif  /* _SYMBOL_H_ */
