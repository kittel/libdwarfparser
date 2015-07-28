#ifndef _DWARFPARSER_H_
#define _DWARFPARSER_H_

#include "libdwarf/dwarf.h"
#include "libdwarf/libdwarf.h"


#include <string>

#include <mutex>

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

	static void parseDwarfFromFilename(const std::string &filename);
	static void parseDwarfFromFD(int fd);

	bool dieHasAttr(const Dwarf_Die &die, const Dwarf_Half &attr);
	std::string getDieName(const Dwarf_Die &die);
	uint64_t getDieOffset(const Dwarf_Die &die);
	uint64_t getDieByteSize(const Dwarf_Die &die);
	uint64_t getDieBitOffset(const Dwarf_Die &die);
	uint64_t getDieAttributeNumber(const Dwarf_Die &die, const Dwarf_Half &attr);
	std::string getDieAttributeString(const Dwarf_Die &die, const Dwarf_Half &attr);
	uint64_t getDieAttributeAddress(const Dwarf_Die &die, const Dwarf_Half &attr);
	bool isDieExternal(const Dwarf_Die &die);
	bool isDieDeclaration(const Dwarf_Die &die);
	bool getDieAttributeFlag(const Dwarf_Die &die, const Dwarf_Half &attr);

	template<class T>
	T* getTypeInstance(const Dwarf_Die &object, const std::string &dieName);
	template<class T>
	T* getRefTypeInstance(const Dwarf_Die &object, const std::string &dieName);

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
	void get_die_and_siblings(const Dwarf_Die &in_die, 
			Symbol *parent, int in_level, Srcfilesdata sf);
	void print_die_data(const Dwarf_Die &print_me, 
			int level, Srcfilesdata sf);
	Symbol *initSymbolFromDie(const Dwarf_Die &cur_die, 
			Symbol *parent, int level, Srcfilesdata sf);

};

#endif  /* _DWARFPARSER_H_ */

