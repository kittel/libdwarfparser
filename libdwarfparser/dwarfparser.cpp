#include "dwarfparser.h"

#include "dwarfexception.h"

#include "libdwarfparser.h"

#include <string>
#include <iostream>
#include <fstream>
#include <cassert>

#include <fcntl.h>
#include <unistd.h>

#include "helpers.h"

#include "dwarfparser.h"

#include <typeinfo>

DwarfParser *DwarfParser::instance = NULL;
uint32_t DwarfParser::nextFileID = 0;

DwarfParser::DwarfParser(int fd) :
	dbg(), fd(fd), res(DW_DLV_ERROR), 
	fileID(++nextFileID),
	error(), errhand(), errarg(),
	curCUOffset(0), nextCUOffset(0)
{
	std::cout << "Loaded parser with id: " << fileID << std::endl;
	res = dwarf_init(this->fd,DW_DLC_READ,errhand,errarg, &dbg,&error);
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
	 srcfiles(0), srcfilescount(0), srcfilesres(DW_DLV_ERROR)
{

}

DwarfParser::Srcfilesdata::~Srcfilesdata()
{
	Dwarf_Debug dbg = 0;
    Dwarf_Signed sri = 0;
    for (sri = 0; sri < this->srcfilescount; ++sri) {
        dwarf_dealloc(dbg, this->srcfiles[sri], DW_DLA_STRING);
    }
    dwarf_dealloc(dbg, this->srcfiles, DW_DLA_LIST);
    this->srcfilesres = DW_DLV_ERROR;
    this->srcfiles = 0;
    this->srcfilescount = 0;
}

void DwarfParser::parseDwarfFromFilename(std::string filename){
	int fd = open(filename.c_str(),O_RDONLY);
	DwarfParser::instance = new DwarfParser(fd);

	instance->read_cu_list();

	DELETE(instance);
}

void DwarfParser::parseDwarfFromFD(int fd){
	DwarfParser::instance = new DwarfParser(fd);

	instance->read_cu_list();

	DELETE(instance);
}

DwarfParser* DwarfParser::getInstance(){
	return instance;
}
uint32_t DwarfParser::getFileID(){
	return this->fileID;
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
		this->curCUOffset = this->nextCUOffset;
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
            break;
        }
		this->nextCUOffset = next_cu_header;
	//	std::cout << std::hex << 
	//		"cu_header_length " <<  cu_header_length <<
	//		"\n version_stamp " << version_stamp << 
	//		"\n abbrev_offset " << abbrev_offset <<
	//		"\n address_size " << address_size <<
	//		"\n next_cu_header " << next_cu_header << 
	//		std::dec << std::endl;

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
	Array::cleanArrays();
	Function::cleanFunctions();
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

void DwarfParser::print_die_data(Dwarf_Die print_me,int level, Srcfilesdata sf)
{
    UNUSED(sf);
	char *name = 0;
    Dwarf_Half tag = 0;
    const char *tagname = 0;
    int localname = 0;

    int res = dwarf_diename(print_me,&name,&error);

	std::cout << getDieName(print_me) << " @ " << std::hex <<
		getDieOffset(print_me) << std::dec << std::endl;

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

				uint64_t number;
				std::string name;
				const char* formname;
				dwarf_get_FORM_name(formid, &formname);
				std::cout << atname << " with type: " << formname << ": ";


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
						std::cout << std::hex << (int64_t) number << std::dec;
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
    
	printf("<%d> tag: %d %s  name: \"%s\"\n",level,tag,tagname,name);
    
	if(!localname) {
        dwarf_dealloc(dbg,name,DW_DLA_STRING);
    }
}

template<class T>
T* DwarfParser::getRefTypeInstance(Dwarf_Die object){
	T* cursym;

	RefBaseType* rbt = RefBaseType::findRefBaseTypeByName(getDieName(object));
	cursym = dynamic_cast<T*>(rbt);
	if(!rbt){
		return new T(object);
	}else if (!cursym){
		std::cout << "RefBaseType with same name but different type!" << 
			typeid(T).name() << " vs " << typeid(rbt).name() <<
			" " << rbt->getName() << std::endl;
		std::cout << "Previous id: " << std::hex << rbt->getID() <<  
			" current ID: " << getDieOffset(object) << std::dec << std::endl;
		return NULL;
	}
	cursym->addAlternativeDwarfID(getDieOffset(object), fileID);
	return cursym;
}

template<>
Variable* DwarfParser::getTypeInstance(Dwarf_Die object){
	Variable* cursym;

	cursym = Variable::findVariableByName(getDieName(object));
	if(!cursym){
		return new Variable(object);
	}
	cursym->update(object);
	cursym->addAlternativeDwarfID(getDieOffset(object), fileID);
	return cursym;

}

template<class T>
T* DwarfParser::getTypeInstance(Dwarf_Die object){
	T* cursym = BaseType::findBaseTypeByName<T>(getDieName(object));
	if(!cursym){
		return new T(object);
	}
	//TODO Include
	//We need to update the representation of the object that we
	//already know about
	//cursym->update(object);
	cursym->addAlternativeDwarfID(getDieOffset(object), fileID);
	return cursym;
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
	Array* array = 0;
	Function * function = 0;

	switch(tag){

		case DW_TAG_typedef:
			cursym = getRefTypeInstance<Typedef>(cur_die);
			break;
		case DW_TAG_structure_type:
			cursym = getTypeInstance<Struct>(cur_die);
			break;
		case DW_TAG_union_type:
			cursym = getTypeInstance<Union>(cur_die);
			break;
		case DW_TAG_member:
			if(!parent){
				//std::cout << "Parent not set" << std::endl;
				//print_die_data(cur_die,level,sf);
				break;
			}
			structured = dynamic_cast<Structured*>(parent);
			if (structured){
				cursym = structured->addMember(cur_die);
				//cursym = getTypeInstance<Variable>(cur_die);
				break;
			}else{
				//print_die_data(cur_die,level,sf);
				//class also contains members
				//throw DwarfException("Parent structured not set");
			}
			break;
		case DW_TAG_base_type:
			cursym = getTypeInstance<BaseType>(cur_die);
			break;
		case DW_TAG_pointer_type:
			cursym = getRefTypeInstance<Pointer>(cur_die);
			break;
		case DW_TAG_const_type:
			cursym = getTypeInstance<ConstType>(cur_die);
		case DW_TAG_enumeration_type:
			cursym = getTypeInstance<Enum>(cur_die);
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
			cursym = getTypeInstance<Variable>(cur_die);
			break;
		case DW_TAG_array_type: 
			cursym = new Array(cur_die);
			break;
		case DW_TAG_subrange_type:
			assert(parent);
			array = dynamic_cast<Array*>(parent);
			if(array){
				array->update(cur_die);
			}else{
				print_die_data(cur_die, level, sf);
			}
			break;
		case DW_TAG_subroutine_type:
			cursym = new Function(cur_die);
			break;
		case DW_TAG_formal_parameter:
			function = dynamic_cast<Function*>(parent);
			if(function){
				function->addParam(cur_die);
			}
			break;
		case DW_TAG_compile_unit:
		case DW_TAG_namespace:
		case DW_TAG_imported_declaration:
		case DW_TAG_class_type:
		case DW_TAG_lexical_block:
		case DW_TAG_subprogram:
			/* This tag is currently not supported */
			break;

		default:
			res = dwarf_get_TAG_name(tag,&tagname);
			if(res != DW_DLV_OK) {
				throw DwarfException("Error in dwarf_get_TAG_name");
			}
			//printf("We are currently not interested in the tag: %s\n", tagname);
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
	dwarf_hasattr(die, attr, &hasattr, &error);
    if(hasattr == 0) {
    	return false;
    }
	return true;
}

uint64_t DwarfParser::getDieAttributeNumber(Dwarf_Die die, Dwarf_Half attr){
	uint64_t result;
	Dwarf_Attribute myattr;
	Dwarf_Bool hasattr;
	Dwarf_Block* block;

	int res = dwarf_hasattr(die, attr, &hasattr, &error);
    if(hasattr == 0) {
    	throw DwarfException("Attr not in Die\n");
    }
	
	res = dwarf_attr(die, attr, &myattr, &error);
    if(res == DW_DLV_ERROR) {
    	throw DwarfException("Error in dwarf_attr\n");
    }
	
	uint16_t formid = ((uint16_t*)myattr)[1];
	switch(formid){
		case DW_FORM_block1:
			res = dwarf_formblock(myattr, &block, &error);
			if(res == DW_DLV_OK){
				assert(block->bl_len > 0);
				switch(((uint8_t*) block->bl_data)[0]){
					case DW_OP_addr:
						result = 0;
						if(block->bl_len > 10)
							std::cout << "Error with " << std::hex << 
							   	getDieOffset(die) << std::dec << 
								" Block length mismatch" << std::endl;
						//assert(block->bl_len <= 10);
						for(Dwarf_Unsigned i = 1; i < 9; i++){
							result = result << 8;
							result += ((uint8_t*) block->bl_data)[9-i];
							//TODO Handle the last byte DW_OP_stack_value
						}
						break;
					case DW_OP_plus_uconst:
						result = 0;
						assert(block->bl_len <= 9);
						for(Dwarf_Unsigned i = 1; i < block->bl_len; i++){
							result = result << 8;
							result += ((uint8_t*) block->bl_data)[i];
						}
						break;
					default:
						result = 0;
						break;
						//TODO add other operators
						const char * atname;
						dwarf_get_AT_name(attr, &atname);
						const char* formname;
						dwarf_get_FORM_name(formid, &formname);

						std::cout << std::hex << getDieOffset(die) << std::dec << std::endl;
						std::cout << atname << ": ";
						std::cout << formname << std::endl;
						std::cout << "Value: " << std::endl;
						for(Dwarf_Unsigned i = 0; i < block->bl_len; i++){
							std::cout << std::hex << 
								(uint32_t) ((uint8_t*) block->bl_data)[i] << 
								std::dec << " ";
						}
						std::cout << std::endl;
						result = 0;
				}
				dwarf_dealloc(dbg,block,DW_DLA_BLOCK);
				return result;
			}
			break;
		case DW_FORM_data1:
		case DW_FORM_data2:
		case DW_FORM_data4:
			res = dwarf_formudata(myattr, (Dwarf_Unsigned*) &result, &error);
			if(res == DW_DLV_OK) {
				return result;
			}
			break;
		case DW_FORM_ref4:
			res = dwarf_formref(myattr,(Dwarf_Off*) &result,&error);
			if(res == DW_DLV_OK){
				return result + this->curCUOffset;
			}
			break;
		case DW_FORM_sdata:
			res = dwarf_formsdata(myattr,(Dwarf_Signed*) &result,&error);
			if(res == DW_DLV_OK){
				return result;
			}
			break;
		default:
			const char* formname;
			dwarf_get_FORM_name(formid, &formname);
			std::cout << formname << " currently not supported" << std::endl;
	}	

	const char * atname;
	dwarf_get_AT_name(attr, &atname);
	const char* formname;
	dwarf_get_FORM_name(formid, &formname);
	
	std::cout << getDieName(die) << std::endl;
	std::cout << atname << ": ";
	std::cout << formname << std::endl;

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

bool DwarfParser::isDieExternal(Dwarf_Die die){
	return this->getDieAttributeFlag(die, DW_AT_external);
}

bool DwarfParser::isDieDeclaration(Dwarf_Die die){
	return this->getDieAttributeFlag(die, DW_AT_declaration);
}

bool DwarfParser::getDieAttributeFlag(Dwarf_Die die, Dwarf_Half attr){
	Dwarf_Attribute myattr;
	Dwarf_Bool hasattr;

	int res = dwarf_hasattr(die, attr, &hasattr, &error);
    if(hasattr == 0) {
		return false;
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
