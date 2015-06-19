#ifndef _DWARFPARSER_H_
#define _DWARFPARSER_H_

#include "libdwarf/dwarf.h"
#include "libdwarf/libdwarf.h"


#include <string>

struct Dwarf_Die_s;
typedef struct Dwarf_Die_s* Dwarf_Die; 

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

	static void parseDwarfFromFilename(std::string filename);
	static void parseDwarfFromFD(int fd);
	static DwarfParser* getInstance();

	bool dieHasAttr(Dwarf_Die die, Dwarf_Half attr);
	std::string getDieName(Dwarf_Die die);
	uint64_t getDieOffset(Dwarf_Die die);
	uint64_t getDieByteSize(Dwarf_Die die);
	uint64_t getDieBitOffset(Dwarf_Die die);
	uint64_t getDieAttributeNumber(Dwarf_Die die, Dwarf_Half attr);
	std::string getDieAttributeString(Dwarf_Die die, Dwarf_Half attr);
	uint64_t getDieAttributeAddress(Dwarf_Die die, Dwarf_Half attr);
	bool isDieExternal(Dwarf_Die die);
	bool isDieDeclaration(Dwarf_Die die);
	bool getDieAttributeFlag(Dwarf_Die die, Dwarf_Half attr);

	template<class T>
	T* getTypeInstance(Dwarf_Die object, std::string dieName);
	template<class T>
	T* getRefTypeInstance(Dwarf_Die object, std::string dieName);

	uint32_t getFileID();
	
private:
	DwarfParser(int fd);
	virtual ~DwarfParser();

	Dwarf_Debug dbg;
	int fd;
	int res;
	uint32_t fileID;
	Dwarf_Error error;
	Dwarf_Handler errhand;
	Dwarf_Ptr errarg;

	uint64_t curCUOffset;
	uint64_t nextCUOffset;

	static uint32_t nextFileID;
	static DwarfParser *instance;

	void read_cu_list();
	void get_die_and_siblings(Dwarf_Die in_die, Symbol *parent, int in_level, Srcfilesdata sf);
	void print_die_data(Dwarf_Die print_me,int level, Srcfilesdata sf);
	Symbol *initSymbolFromDie(Dwarf_Die cur_die, Symbol *parent, int level, Srcfilesdata sf);

};

#endif  /* _DWARFPARSER_H_ */

