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

	virtual uint32_t getByteSize();
	uint64_t getID();
	const std::string &getName();

	void addAlternativeDwarfID(uint64_t id, uint32_t fileID);
	virtual void print();

	const std::string name;

	SymbolManager *const manager;

protected:
	uint32_t byteSize;
	uint64_t id;
};

#endif  /* _SYMBOL_H_ */
