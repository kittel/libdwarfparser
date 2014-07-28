#ifndef _DWARFPARSER_H_
#define _DWARFPARSER_H_

#include "dwarf.h"
#include "libdwarf.h"

#include <string>

class Symbol;

class DwarfParser {

	class Srcfilesdata {
	    char ** srcfiles;
	    Dwarf_Signed srcfilescount;
	    int srcfilesres;

	public:
	    Srcfilesdata();
	    virtual ~Srcfilesdata();
	};


public:

	static void parseDwarf(std::string filename);
	static DwarfParser* getInstance();

	bool dieHasAttr(Dwarf_Die die, Dwarf_Half attr);
	std::string getDieName(Dwarf_Die die);
	uint64_t getDieOffset(Dwarf_Die die);
	uint64_t getDieByteSize(Dwarf_Die die);
	uint64_t getDieBitOffset(Dwarf_Die die);
	uint64_t getDieAttributeNumber(Dwarf_Die die, Dwarf_Half attr);
	std::string getDieAttributeString(Dwarf_Die die, Dwarf_Half attr);
	uint64_t getDieAttributeAddress(Dwarf_Die die, Dwarf_Half attr);
	bool getDieAttributeFlag(Dwarf_Die die, Dwarf_Half attr);



private:
	DwarfParser(std::string filename);
	virtual ~DwarfParser();

	Dwarf_Debug dbg;
	int fd;
	int res;
	Dwarf_Error error;
	Dwarf_Handler errhand;
	Dwarf_Ptr errarg;

	static DwarfParser *instance;

	void read_cu_list();
	void get_die_and_siblings(Dwarf_Die in_die, Symbol *parent, int in_level, Srcfilesdata sf);
	void print_die_data(Dwarf_Die print_me,int level, Srcfilesdata sf);
	Symbol *initSymbolFromDie(Dwarf_Die cur_die, Symbol *parent, int level, Srcfilesdata sf);

};

#endif  /* _DWARFPARSER_H_ */

