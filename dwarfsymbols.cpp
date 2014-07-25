#include <sys/types.h> /* For open() */
#include <sys/stat.h>  /* For open() */
#include <fcntl.h>     /* For open() */
#include <stdlib.h>     /* For exit() */
#include <unistd.h>     /* For close() */
#include <stdio.h>
#include <iostream>
#include <errno.h>
#include <string.h>

#include <elf.h>
#include "dwarf.h"
#include "libdwarf.h"

#include <cassert>

#include <vector>
#include <map>

#include <exception>

class DwarfException : std::exception {
	public:
		DwarfException() throw();
		DwarfException(const char* reason) throw();
		virtual ~DwarfException() throw();
		virtual const char* what() const throw();


	private:
		std::string reason;
};

DwarfException::DwarfException() throw() {}
DwarfException::DwarfException(const char* reason) throw() : reason(reason){}
DwarfException::~DwarfException() throw() {}
const char* DwarfException::what() const throw(){
	std::string result = std::string("DwarfException: ");
	return result.append(this->reason).c_str();
}


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

	Dwarf_Debug dbg;


private:
	DwarfParser(std::string filename);
	virtual ~DwarfParser();

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

class Symbol {

public:
	Symbol(Dwarf_Die object);
	virtual ~Symbol();

	uint32_t getByteSize();
	std::string getName();

	static Symbol* findSymbolByName(std::string name);

protected:
	uint64_t offset;
	uint32_t byteSize;
	std::string name;
	
	typedef std::map<std::string, Symbol*> SymbolMap;
	static SymbolMap symbolMap;
};

Symbol::SymbolMap Symbol::symbolMap;

Symbol::Symbol(Dwarf_Die object){
	DwarfParser* instance = DwarfParser::getInstance();
	this->byteSize = instance->getDieByteSize(object);
	this->offset = instance->getDieOffset(object);
	this->name = instance->getDieName(object);

	if(this->name.size() != 0){
		symbolMap[this->name] = this;
	}
	
}

Symbol::~Symbol(){}

Symbol* Symbol::findSymbolByName(std::string name){
	return symbolMap.find(name)->second;
}

uint32_t Symbol::getByteSize(){
	return this->byteSize;
}

std::string Symbol::getName(){
	return this->name;
}

class BaseType: public Symbol {
public:
	BaseType(Dwarf_Die object);
	virtual ~BaseType();

	static BaseType* findBaseTypeById(uint64_t id);
	static BaseType* findSymbolByName(std::string name);
	uint64_t getEncoding();

private:
	uint64_t encoding;
	
	typedef std::map<std::string, BaseType*> BaseTypeNameMap;
	static BaseTypeNameMap baseTypeNameMap;
	
};

BaseType::BaseTypeNameMap BaseType::baseTypeNameMap;

BaseType::BaseType(Dwarf_Die object):
	Symbol(object){
	DwarfParser *parser = DwarfParser::getInstance();
	if(parser->dieHasAttr(object, DW_AT_encoding)){
		this->encoding = parser->getDieAttributeNumber(object, DW_AT_encoding);
	}
	if(this->name.size() != 0){
		baseTypeNameMap[this->name] = this;
	}

}

BaseType::~BaseType(){

}

BaseType* BaseType::findBaseTypeById(uint64_t id){
	return 0;
}

BaseType* BaseType::findSymbolByName(std::string name){
	return baseTypeNameMap.find(name)->second;
}


uint64_t BaseType::getEncoding(){
	return this->encoding;
}

class ReferencingType {
public:
	ReferencingType(Dwarf_Die object);
	virtual ~ReferencingType();

private:
	uint64_t type;
	BaseType* base;
};

ReferencingType::ReferencingType(Dwarf_Die object){
	DwarfParser *parser = DwarfParser::getInstance();
	if(parser->dieHasAttr(object, DW_AT_type)){
		this->type = parser->getDieAttributeNumber(object, DW_AT_type);
	}
}

ReferencingType::~ReferencingType(){}


class RefBaseType: public BaseType {
public:
	RefBaseType(Dwarf_Die object);
	virtual ~RefBaseType();

	void resolveBaseType();
	BaseType* getBaseType();

protected:
	uint64_t type;
	BaseType* base;
};

RefBaseType::RefBaseType(Dwarf_Die object):
	BaseType(object){
	DwarfParser* parser = DwarfParser::getInstance();
	if(parser->dieHasAttr(object, DW_AT_type)){
		this->type = parser->getDieAttributeNumber(object, DW_AT_type);
	}
	this->base = NULL;
}

RefBaseType::~RefBaseType(){

}

void RefBaseType::resolveBaseType(){
	this->base = BaseType::findBaseTypeById(this->type);
}

class ConstType: public RefBaseType {
public:
	ConstType(Dwarf_Die object);
	virtual ~ConstType();
};

ConstType::ConstType(Dwarf_Die object):
	RefBaseType(object){

}

ConstType::~ConstType(){

}

class Typedef: public RefBaseType {
public:
	Typedef(Dwarf_Die object);
	virtual ~Typedef();
};

Typedef::Typedef(Dwarf_Die object):
	RefBaseType(object){

}

Typedef::~Typedef(){

}

class StructuredMember;

class Structured: public BaseType {
public:
	Structured(Dwarf_Die object);
	virtual ~Structured();

	virtual void addMember(StructuredMember *member);

private:
	std::vector<StructuredMember*> members;
};

Structured::Structured(Dwarf_Die object):
	BaseType(object){

}

Structured::~Structured(){

}

void Structured::addMember(StructuredMember *member){
	this->members.push_back(member);
}

class Struct: public Structured {
public:
	Struct(Dwarf_Die object);
	virtual ~Struct();
};

Struct::Struct(Dwarf_Die object):
	Structured(object){

}

Struct::~Struct(){

}

class Union: public Structured {
public:
	Union(Dwarf_Die object);
	virtual ~Union();
};

Union::Union(Dwarf_Die object):
	Structured(object){

}

Union::~Union(){

}

class StructuredMember: public Symbol, public ReferencingType {
public:
	StructuredMember(Dwarf_Die object, Structured *parent);
	virtual ~StructuredMember();

protected:
	uint32_t bitoffset;

};

StructuredMember::StructuredMember(Dwarf_Die object, Structured *parent):
	Symbol(object), ReferencingType(object){
	if(parent == NULL){
		std::cout << "parent not set" << std::endl;
		throw DwarfException("Parent not set");
	}
	parent->addMember(this);
	DwarfParser *parser = DwarfParser::getInstance();
	if(parser->dieHasAttr(object, DW_AT_data_member_location)){
		this->bitoffset = parser->getDieAttributeNumber(object, DW_AT_data_member_location);
	}
}

StructuredMember::~StructuredMember(){

}

class Enum: public BaseType {
public:
	Enum(Dwarf_Die object);
	virtual ~Enum();

	void addEnum(Dwarf_Die object);
	std::string enumValue(uint32_t value);
	uint32_t enumName(std::string name);
};

Enum::Enum(Dwarf_Die object):
	BaseType(object){

}

Enum::~Enum(){}

void Enum::addEnum(Dwarf_Die object){}

std::string Enum::enumValue(uint32_t value){}

uint32_t enumName(std::string name){}

class Pointer: public RefBaseType {
public:
	Pointer(Dwarf_Die object);
	virtual ~Pointer();
};

Pointer::Pointer(Dwarf_Die object):
	RefBaseType(object){

}

Pointer::~Pointer(){

}

class FuncPointer: public RefBaseType {
public:
	FuncPointer(Dwarf_Die object);
	virtual ~FuncPointer();
};

FuncPointer::FuncPointer(Dwarf_Die object):
	RefBaseType(object){

}

FuncPointer::~FuncPointer(){

}

class Function : public FuncPointer {
public:
	Function(Dwarf_Die object);
	virtual ~Function();
};

Function::Function(Dwarf_Die object):
	FuncPointer(object){

}

Function::~Function(){

}

class Variable : public Symbol, public ReferencingType {
public:
	Variable(Dwarf_Die object);
	virtual ~Variable();

	uint64_t getLocation();
	void setLocation(uint64_t location);

private:
	uint64_t location;
};

Variable::Variable(Dwarf_Die object):
	Symbol(object), ReferencingType(object){
	
	DwarfParser *parser = DwarfParser::getInstance();
	
	//Get location from elf
	this->location = 0;
	
}

Variable::~Variable(){}

uint64_t Variable::getLocation(){
	return this->location;
}

void Variable::setLocation(uint64_t location){
	this->location = location;
}

DwarfParser *DwarfParser::instance = NULL;

DwarfParser::DwarfParser(std::string filename) :
		dbg(), fd(0), res(DW_DLV_ERROR), error(), errhand(), errarg()
	{
		fd = open(filename.c_str(),O_RDONLY);
		res = dwarf_init(fd,DW_DLC_READ,errhand,errarg, &dbg,&error);
		if(res != DW_DLV_OK) {
		    throw DwarfException("Giving up, cannot do DWARF processing\n");
		}
	}

DwarfParser::~DwarfParser(){

	//Done reading DWARF symbols
	res = dwarf_finish(dbg,&error);
    if(res != DW_DLV_OK) {
        printf("dwarf_finish failed!\n");
    }
    close(fd);
}

DwarfParser::Srcfilesdata::Srcfilesdata():
	srcfilesres(DW_DLV_ERROR), srcfiles(0), srcfilescount(0)
{

}

DwarfParser::Srcfilesdata::~Srcfilesdata()
{
	Dwarf_Debug dbg;
    Dwarf_Signed sri = 0;
    for (sri = 0; sri < this->srcfilescount; ++sri) {
        dwarf_dealloc(dbg, this->srcfiles[sri], DW_DLA_STRING);
    }
    dwarf_dealloc(dbg, this->srcfiles, DW_DLA_LIST);
    this->srcfilesres = DW_DLV_ERROR;
    this->srcfiles = 0;
    this->srcfilescount = 0;
}

void DwarfParser::parseDwarf(std::string filename){
	DwarfParser::instance = new DwarfParser(filename);

	instance->read_cu_list();
}

DwarfParser* DwarfParser::getInstance(){
	return instance;
}


int
main(int argc, char **argv)
{

    if(argc < 2) {
		printf("Expecting filename as argument\n");
		return 0;
    } else {
        printf("Using %s as file\n", argv[1]);
		try{
			DwarfParser::parseDwarf(std::string(argv[1]));
		}catch(DwarfException &e){
			std::cout << e.what() << std::endl;
		}
    }

    return 0;
}

void DwarfParser::read_cu_list()
{
    Dwarf_Unsigned cu_header_length = 0;
    Dwarf_Half version_stamp = 0;
    Dwarf_Unsigned abbrev_offset = 0;
    Dwarf_Half address_size = 0;
    Dwarf_Unsigned next_cu_header = 0;
    Dwarf_Error error;
    int cu_number = 0;

    for(;;++cu_number) {
    	Srcfilesdata sf;
        Dwarf_Die no_die = 0;
        Dwarf_Die cu_die = 0;
        int res = DW_DLV_ERROR;
        res = dwarf_next_cu_header(dbg,&cu_header_length,
            &version_stamp, &abbrev_offset, &address_size,
            &next_cu_header, &error);
        if(res == DW_DLV_ERROR) {
            printf("Error in dwarf_next_cu_header\n");
            exit(1);
        }
        if(res == DW_DLV_NO_ENTRY) {
            /* Done. */
            return;
        }
        /* The CU will have a single sibling, a cu_die. */
        res = dwarf_siblingof(dbg,no_die,&cu_die,&error);
        if(res == DW_DLV_ERROR) {
            printf("Error in dwarf_siblingof on CU die \n");
            exit(1);
        }
        if(res == DW_DLV_NO_ENTRY) {
            /* Impossible case. */
            printf("no entry! in dwarf_siblingof on CU die \n");
            exit(1);
        }
        get_die_and_siblings(cu_die, 0, 0, sf);
        dwarf_dealloc(dbg, cu_die,DW_DLA_DIE);
    }
}

void DwarfParser::get_die_and_siblings(Dwarf_Die in_die, Symbol *parent, int in_level, Srcfilesdata sf)
{
    int res = DW_DLV_ERROR;
    Dwarf_Die cur_die=in_die;
    Dwarf_Die child = 0;
    Dwarf_Error error;

    Symbol *cursym = 0;

	cursym = initSymbolFromDie(in_die, parent, in_level,sf);

    for(;;) {
        Dwarf_Die sib_die = 0;
        res = dwarf_child(cur_die,&child,&error);
        if(res == DW_DLV_ERROR) {
            printf("Error in dwarf_child , level %d \n",in_level);
            exit(1);
        }
        if(res == DW_DLV_OK) {
            get_die_and_siblings(child, cursym, in_level+1,sf);
        }
        /* res == DW_DLV_NO_ENTRY */
        res = dwarf_siblingof(dbg,cur_die,&sib_die,&error);
        if(res == DW_DLV_ERROR) {
            printf("Error in dwarf_siblingof , level %d \n",in_level);
            exit(1);
        }
        if(res == DW_DLV_NO_ENTRY) {
            /* Done at this level. */
            break;
        }
        /* res == DW_DLV_OK */
        if(cur_die != in_die) {
            dwarf_dealloc(dbg,cur_die,DW_DLA_DIE);
        }
        cur_die = sib_die;
        cursym = initSymbolFromDie(cur_die, parent, in_level,sf);
    }
    return;
}

//static void
//get_addr(Dwarf_Attribute attr,Dwarf_Addr *val)
//{
//    Dwarf_Error error = 0;
//    int res;
//    Dwarf_Addr uval = 0;
//    res = dwarf_formaddr(attr,&uval,&error);
//    if(res == DW_DLV_OK) {
//        *val = uval;
//        return;
//    }
//    return;
//}

//static void
//print_subprog(Dwarf_Debug dbg,Dwarf_Die die, int level,
//    struct srcfilesdata *sf,
//    const char *name)
//{
//    int res;
//    Dwarf_Error error = 0;
//    Dwarf_Attribute *attrbuf = 0;
//    Dwarf_Addr lowpc = 0;
//    Dwarf_Addr highpc = 0;
//    Dwarf_Signed attrcount = 0;
//    Dwarf_Unsigned i;
//    Dwarf_Unsigned filenum = 0;
//    Dwarf_Unsigned linenum = 0;
//    char *filename = 0;
//    res = dwarf_attrlist(die,&attrbuf,&attrcount,&error);
//    if(res != DW_DLV_OK) {
//        return;
//    }
//    for(i = 0; i < attrcount ; ++i) {
//        Dwarf_Half aform;
//        res = dwarf_whatattr(attrbuf[i],&aform,&error);
//        if(res == DW_DLV_OK) {
//            if(aform == DW_AT_decl_file) {
//                get_number(attrbuf[i],&filenum);
//                if((filenum > 0) && (sf->srcfilescount > (filenum-1))) {
//                    filename = sf->srcfiles[filenum-1];
//                }
//            }
//            if(aform == DW_AT_decl_line) {
//                get_number(attrbuf[i],&linenum);
//            }
//            if(aform == DW_AT_low_pc) {
//                get_addr(attrbuf[i],&lowpc);
//            }
//            if(aform == DW_AT_high_pc) {
//                /*  This will FAIL with DWARF4 highpc form
//                    of 'class constant'.  */
//                get_addr(attrbuf[i],&highpc);
//            }
//        }
//        dwarf_dealloc(dbg,attrbuf[i],DW_DLA_ATTR);
//    }
//    /*  Here let's test some alternative interfaces for high and low pc.
//        We only do both dwarf_highpc and dwarf_highpcb_b as
//        an error check. Do not do both yourself. */
//    if(checkoptionon){
//        int hres = 0;
//        int hresb = 0;
//        int lres = 0;
//        Dwarf_Addr althipc = 0;
//        Dwarf_Addr hipcoffset = 0;
//        Dwarf_Addr althipcb = 0;
//        Dwarf_Addr altlopc = 0;
//        Dwarf_Half highform = 0;
//        enum Dwarf_Form_Class highclass = 0;
//
//        /*  Should work for DWARF 2/3 DW_AT_high_pc, and
//            all high_pc where the FORM is DW_FORM_addr
//            Avoid using this interface as of 2013. */
//        hres  = dwarf_highpc(die,&althipc,&error);
//
//        /* Should work for all DWARF DW_AT_high_pc.  */
//        hresb = dwarf_highpc_b(die,&althipcb,&highform,&highclass,&error);
//
//        lres = dwarf_lowpc(die,&altlopc,&error);
//        printf("high_pc checking %s ",name);
//
//        if (hres == DW_DLV_OK) {
//            /* present, FORM addr */
//            printf("highpc   0x%" DW_PR_XZEROS DW_PR_DUx " ",
//                althipc);
//        } else if (hres == DW_DLV_ERROR) {
//            printf("dwarf_highpc() error not class address ");
//        } else {
//            /* absent */
//        }
//        if(hresb == DW_DLV_OK) {
//            /* present, FORM addr or const. */
//            if(highform == DW_FORM_addr) {
//                printf("highpcb  0x%" DW_PR_XZEROS DW_PR_DUx " ",
//                    althipcb);
//            } else {
//                if(lres == DW_DLV_OK) {
//                    hipcoffset = althipcb;
//                    althipcb = altlopc + hipcoffset;
//                    printf("highpcb  0x%" DW_PR_XZEROS DW_PR_DUx " "
//                        "highoff  0x%" DW_PR_XZEROS DW_PR_DUx " ",
//                        althipcb,hipcoffset);
//                } else {
//                    printf("highoff  0x%" DW_PR_XZEROS DW_PR_DUx " ",
//                        althipcb);
//                }
//            }
//        } else if (hresb == DW_DLV_ERROR) {
//            printf("dwarf_highpc_b() error!");
//        } else {
//            /* absent */
//        }
//
//        /* Should work for all DWARF DW_AT_low_pc */
//        if (lres == DW_DLV_OK) {
//            /* present, FORM addr. */
//            printf("lowpc    0x%" DW_PR_XZEROS DW_PR_DUx " ",
//                altlopc);
//        } else if (lres == DW_DLV_ERROR) {
//            printf("dwarf_lowpc() error!");
//        } else {
//            /* absent. */
//        }
//        printf("\n");
//
//
//
//    }
//    if(namesoptionon && (filenum || linenum)) {
//        printf("<%3d> file: %" DW_PR_DUu " %s  line %"
//            DW_PR_DUu "\n",level,filenum,filename?filename:"",linenum);
//    }
//    if(namesoptionon && lowpc) {
//        printf("<%3d> low_pc : 0x%" DW_PR_DUx  "\n",
//            level, (Dwarf_Unsigned)lowpc);
//    }
//    if(namesoptionon && highpc) {
//        printf("<%3d> high_pc: 0x%" DW_PR_DUx  "\n",
//            level, (Dwarf_Unsigned)highpc);
//    }
//    dwarf_dealloc(dbg,attrbuf,DW_DLA_LIST);
//}
//
//static void
//print_comp_dir(Dwarf_Debug dbg,Dwarf_Die die,int level, struct srcfilesdata *sf)
//{
//    int res;
//    Dwarf_Error error = 0;
//    Dwarf_Attribute *attrbuf = 0;
//    Dwarf_Signed attrcount = 0;
//    Dwarf_Unsigned i;
//    res = dwarf_attrlist(die,&attrbuf,&attrcount,&error);
//    if(res != DW_DLV_OK) {
//        return;
//    }
//    sf->srcfilesres = dwarf_srcfiles(die,&sf->srcfiles,&sf->srcfilescount,
//        &error);
//    for(i = 0; i < attrcount ; ++i) {
//        Dwarf_Half aform;
//        res = dwarf_whatattr(attrbuf[i],&aform,&error);
//        if(res == DW_DLV_OK) {
//            if(aform == DW_AT_comp_dir) {
//                char *name = 0;
//                res = dwarf_formstring(attrbuf[i],&name,&error);
//                if(res == DW_DLV_OK) {
//                    printf(    "<%3d> compilation directory : \"%s\"\n",
//                        level,name);
//                }
//            }
//            if(aform == DW_AT_stmt_list) {
//                /* Offset of stmt list for this CU in .debug_line */
//            }
//        }
//        dwarf_dealloc(dbg,attrbuf[i],DW_DLA_ATTR);
//    }
//    dwarf_dealloc(dbg,attrbuf,DW_DLA_LIST);
//}

void DwarfParser::print_die_data(Dwarf_Die print_me,int level, Srcfilesdata sf)
{
    char *name = 0;
    Dwarf_Half tag = 0;
    const char *tagname = 0;
    int localname = 0;

    int res = dwarf_diename(print_me,&name,&error);

    if(res == DW_DLV_ERROR) {
        printf("Error in dwarf_diename , level %d \n",level);
        exit(1);
    }
    if(res == DW_DLV_NO_ENTRY) {
        name = (char *)"<no DW_AT_name attr>";
        localname = 1;
    }
    res = dwarf_tag(print_me,&tag,&error);
    if(res != DW_DLV_OK) {
        printf("Error in dwarf_tag , level %d \n",level);
        exit(1);
    }
    res = dwarf_get_TAG_name(tag,&tagname);
    if(res != DW_DLV_OK) {
        printf("Error in dwarf_get_TAG_name , level %d \n",level);
        exit(1);
    }
	printf("--\nDescription of tag: %s\n", tagname);
	
    Dwarf_Attribute *attrbuf = 0;
	Dwarf_Signed attrcount = 0;
	char *filename = 0;
	res = dwarf_attrlist(print_me,&attrbuf,&attrcount,&error);
	if(res == DW_DLV_ERROR) {
		throw DwarfException("Unable to get attribute list");
	}else if(res != DW_DLV_NO_ENTRY){
		for(Dwarf_Signed i = 0; i < attrcount ; ++i) {
			Dwarf_Half aform;
			res = dwarf_whatattr(attrbuf[i],&aform,&error);
			if(res == DW_DLV_OK) {
				const char * atname;
				dwarf_get_AT_name(aform, &atname);
				
				Dwarf_Attribute myattr;
				res = dwarf_attr(print_me, aform, &myattr, &error);
				if(res == DW_DLV_ERROR) {
					throw DwarfException("Error in dwarf_attr\n");
				}
				uint16_t formid = ((uint16_t*)myattr)[1];

				std::cout << atname << ": ";
				uint64_t number;
				std::string name;
				const char* formname;


				switch(formid){
					case DW_FORM_addr:
						number = getDieAttributeAddress(print_me, aform);
						std::cout << number;
						break;
					case DW_FORM_data1:
					case DW_FORM_data2:
					case DW_FORM_data4:
					case DW_FORM_data8:
						number = getDieAttributeNumber(print_me, aform);
						std::cout << number;
						break;
					case DW_FORM_ref4:
					case DW_FORM_sdata:
						number = getDieAttributeNumber(print_me, aform);
						std::cout << (int64_t) number;
						break;
					case DW_FORM_exprloc:
						std::cout << "<unavailable> - ";
						break;
					case DW_FORM_flag_present:
						number = getDieAttributeFlag(print_me, aform);
						std::cout << ((number) ? "True" : "False");
						break;
					case DW_FORM_strp:
						name = getDieAttributeString(print_me, aform);
						std::cout << name;
						break;
					default:
						dwarf_get_FORM_name(formid, &formname);
						std::cout << "<unavailable> - " << formname;
						break;
				}

				std::cout << std::endl;

			}
			dwarf_dealloc(dbg,attrbuf[i],DW_DLA_ATTR);
		}
		dwarf_dealloc(dbg,attrbuf,DW_DLA_LIST);
	}
//    if(namesoptionon ||checkoptionon) {
//        if( tag == DW_TAG_subprogram) {
//            if(namesoptionon) {
//                printf(    "<%3d> subprogram            : \"%s\"\n",level,name);
//            }
//            print_subprog(dbg,print_me,level,sf,name);
//        }
//        if( (namesoptionon) && (tag == DW_TAG_compile_unit ||
//            tag == DW_TAG_partial_unit ||
//            tag == DW_TAG_type_unit)) {
//
//            resetsrcfiles(dbg,sf);
//            printf(    "<%3d> source file           : \"%s\"\n",level,name);
//            print_comp_dir(dbg,print_me,level,sf);
//        }
//    } else {
        printf("<%d> tag: %d %s  name: \"%s\"\n",level,tag,tagname,name);
//    }
    if(!localname) {
        dwarf_dealloc(dbg,name,DW_DLA_STRING);
    }
}

Symbol *DwarfParser::initSymbolFromDie(Dwarf_Die cur_die, Symbol *parent, int level, Srcfilesdata sf){

	Dwarf_Half tag = 0;
    const char *tagname = 0;
	res = dwarf_tag(cur_die,&tag,&error);
	if(res != DW_DLV_OK){
		throw DwarfException("Error in dwarf_get_TAG_name");
	}

	std::string name = getDieName(cur_die);

	Symbol *cursym = 0;

	Structured* structured = 0;
	Enum* enumType = 0;

	switch(tag){

		case DW_TAG_typedef:
			cursym = new Typedef(cur_die);
			break;
		case DW_TAG_structure_type:
			cursym = new Struct(cur_die);
			break;
		case DW_TAG_union_type:
			cursym = new Union(cur_die);
			break;
		case DW_TAG_member:
			if(!parent){
				//std::cout << "Parent not set" << std::endl;
				//print_die_data(cur_die,level,sf);
				break;
			}
			structured = dynamic_cast<Structured*>(parent);
			if (structured){
				cursym = new StructuredMember(cur_die, structured);
				break;
			}else{
				//print_die_data(cur_die,level,sf);
				//class also contains members
				//throw DwarfException("Parent structured not set");
			}
			break;
		case DW_TAG_base_type:
			cursym = new BaseType(cur_die);
			break;
		case DW_TAG_pointer_type:
			cursym = new Pointer(cur_die);
			break;
		case DW_TAG_const_type:
			cursym = new ConstType(cur_die);
		case DW_TAG_enumeration_type:
			cursym = new Enum(cur_die);
			break;
		case DW_TAG_enumerator:
			assert(parent);
			enumType = dynamic_cast<Enum*>(parent);
			if(enumType){
				enumType->addEnum(cur_die);
			}else{
				print_die_data(cur_die, level, sf);
			}
			break;
		case DW_TAG_variable:
			if(this->dieHasAttr(cur_die, DW_AT_specification)){
				//This is an initializer for another object
				break;
			}
			cursym = new Variable(cur_die);
			break;
		//case:
		//	break;

		case DW_TAG_compile_unit:
		case DW_TAG_namespace:
		case DW_TAG_imported_declaration:
		case DW_TAG_class_type:
		case DW_TAG_lexical_block:
		case DW_TAG_formal_parameter:
		case DW_TAG_subprogram:
			/* This tag is currently not supported */
			break;

		default:
			res = dwarf_get_TAG_name(tag,&tagname);
			if(res != DW_DLV_OK) {
				throw DwarfException("Error in dwarf_get_TAG_name");
			}
			printf("We are currently not interested in the tag: %s\n", tagname);
			//print_die_data(cur_die,level,sf);

	}
	return cursym;
}

std::string DwarfParser::getDieName(Dwarf_Die die){
	char *name = 0;

	std::string result = std::string("");

	int res = dwarf_diename(die,&name,&error);

    if(res == DW_DLV_ERROR) {
    	throw DwarfException("Error in dwarf_diename\n");
    }
    if (name != NULL){
    	result = std::string(name);
    }
    dwarf_dealloc(dbg,name,DW_DLA_STRING);
	return result;
}

uint64_t DwarfParser::getDieOffset(Dwarf_Die die){
	Dwarf_Off offset;

	int res = dwarf_dieoffset(die,&offset,&error);

    if(res == DW_DLV_ERROR) {
    	throw DwarfException("Error in dwarf_dieoffset\n");
    }
	return (uint64_t) offset;
}

uint64_t DwarfParser::getDieByteSize(Dwarf_Die die){
	Dwarf_Unsigned size;

	int res = dwarf_bytesize(die,&size,&error);

    if(res == DW_DLV_ERROR) {
    	throw DwarfException("Error in dwarf_bytesize\n");
    }
	return (uint64_t) size;
}

uint64_t DwarfParser::getDieBitOffset(Dwarf_Die die){
	Dwarf_Unsigned size;

	int res = dwarf_bitoffset(die,&size,&error);

    if(res == DW_DLV_ERROR) {
    	throw DwarfException("Error in dwarf_bitoffset\n");
    }
	return (uint64_t) size;
}

bool DwarfParser::dieHasAttr(Dwarf_Die die, Dwarf_Half attr){
	Dwarf_Bool hasattr;
	int res = dwarf_hasattr(die, attr, &hasattr, &error);
    if(hasattr == 0) {
    	return false;
    }
	return true;
}

uint64_t DwarfParser::getDieAttributeNumber(Dwarf_Die die, Dwarf_Half attr){
	uint64_t result;
	Dwarf_Attribute myattr;
	Dwarf_Bool hasattr;

	int res = dwarf_hasattr(die, attr, &hasattr, &error);
    if(hasattr == 0) {
    	throw DwarfException("Attr not in Die\n");
    }
	
	res = dwarf_attr(die, attr, &myattr, &error);
    if(res == DW_DLV_ERROR) {
    	throw DwarfException("Error in dwarf_attr\n");
    }
	
    res = dwarf_formudata(myattr, (Dwarf_Unsigned*) &result, &error);
    if(res == DW_DLV_OK) {
    	return result;
    }
    res = dwarf_formsdata(myattr,(Dwarf_Signed*) &result,&error);
	if(res == DW_DLV_OK){
    	return result;
    }
	res = dwarf_formref(myattr,(Dwarf_Off*) &result,&error);
	if(res == DW_DLV_OK){
		return result;
	}
	throw DwarfException("Error in getDieAttributeNumber\n");
	return 0;
}

std::string DwarfParser::getDieAttributeString(Dwarf_Die die, Dwarf_Half attr){
	char* str;
	std::string result;
	Dwarf_Attribute myattr;
	Dwarf_Bool hasattr;

	int res = dwarf_hasattr(die, attr, &hasattr, &error);
	if(hasattr == 0) {
		throw DwarfException("Attr not in Die\n");
	}

	res = dwarf_attr(die, attr, &myattr, &error);
	if(res == DW_DLV_ERROR) {
		throw DwarfException("Error in dwarf_attr\n");
	}
    res = dwarf_formstring(myattr, &str,&error);
	if(res == DW_DLV_OK){
    	result = std::string(str);
        dwarf_dealloc(dbg,str,DW_DLA_STRING);
		return result;
	}
	
	throw DwarfException("Error in getDieAttributeString\n");
	return NULL;
}

uint64_t DwarfParser::getDieAttributeAddress(Dwarf_Die die, Dwarf_Half attr){
	uint64_t result;
	Dwarf_Attribute myattr;
	Dwarf_Bool hasattr;

	int res = dwarf_hasattr(die, attr, &hasattr, &error);
    if(hasattr == 0) {
    	throw DwarfException("Attr not in Die\n");
    }
	
	res = dwarf_attr(die, attr, &myattr, &error);
    if(res == DW_DLV_ERROR) {
    	throw DwarfException("Error in dwarf_attr\n");
    }
	
    res = dwarf_formaddr(myattr, (Dwarf_Addr*) &result, &error);
    if(res == DW_DLV_OK) {
    	return result;
    }

	throw DwarfException("Error in getDieAttributeAddress\n");
	return 0;
}

bool DwarfParser::getDieAttributeFlag(Dwarf_Die die, Dwarf_Half attr){
	bool result;
	Dwarf_Attribute myattr;
	Dwarf_Bool hasattr;

	int res = dwarf_hasattr(die, attr, &hasattr, &error);
    if(hasattr == 0) {
    	throw DwarfException("Attr not in Die\n");
    }
	
	res = dwarf_attr(die, attr, &myattr, &error);
    if(res == DW_DLV_ERROR) {
    	throw DwarfException("Error in dwarf_attr\n");
    }
	
    res = dwarf_formflag(myattr, &hasattr, &error);
    if(res == DW_DLV_OK) {
    	return hasattr;
    }

	throw DwarfException("Error in getDieAttributeAddress\n");
	return 0;
}
