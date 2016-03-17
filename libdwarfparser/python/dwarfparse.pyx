# distutils: language = c++
# distutils: extra_compile_args = ["-std=c++14"]
# cython: profile=False
# cython: language_level=3


cimport dwarfparse_c as sym

from libcpp.string cimport string
from libcpp cimport bool
from libcpp cimport vector
from libcpp cimport pair

from libc.stdint cimport uintptr_t
from libc.stdint cimport int8_t
from libc.stdint cimport int16_t
from libc.stdint cimport int32_t
from libc.stdint cimport int64_t
from libc.stdint cimport uint8_t
from libc.stdint cimport uint16_t
from libc.stdint cimport uint32_t
from libc.stdint cimport uint64_t


# Copy from "libdwarf/dwarf.h"
DW_ATE_address            = 0x01
DW_ATE_boolean            = 0x02
DW_ATE_complex_float      = 0x03
DW_ATE_float              = 0x04
DW_ATE_signed             = 0x05
DW_ATE_signed_char        = 0x06
DW_ATE_unsigned           = 0x07
DW_ATE_unsigned_char      = 0x08
DW_ATE_imaginary_float    = 0x09  #/* DWARF3 */
DW_ATE_packed_decimal     = 0x0a  #/* DWARF3f */
DW_ATE_numeric_string     = 0x0b  #/* DWARF3f */
DW_ATE_edited             = 0x0c  #/* DWARF3f */
DW_ATE_signed_fixed       = 0x0d  #/* DWARF3f */
DW_ATE_unsigned_fixed     = 0x0e  #/* DWARF3f */
DW_ATE_decimal_float      = 0x0f  #/* DWARF3f */
DW_ATE_UTF                = 0x10  #/* DWARF4 */
DW_ATE_UCS                = 0x11  #/* DWARF5 */
DW_ATE_ASCII              = 0x12  #/* DWARF5 */



class InstantiationNotAllowed(Exception):
    pass

cdef class SymbolManager:
    cdef public bool xownership
    cdef sym.SymbolManager* sm_ptr
    def __cinit__(self, uintptr_t ptr = 0):
        self.xownership = False
        if ptr != 0:
            sm_ptr = <sym.SymbolManager*> ptr
        elif type(self) is SymbolManager:
            self.xownership = True
            self.sm_ptr = new sym.SymbolManager()
    def __dealloc__(self):
        if type(self) is SymbolManager and self.xownership:
            del self.sm_ptr
    def findSymbolByName(self, string name):
        cdef sym.Symbol* ptr = <sym.Symbol*> self.sm_ptr.findSymbolByName[sym.Symbol](name)
        return Symbol(<uintptr_t> ptr)
    def findBaseTypeByName(self, string name):
        cdef sym.BaseType* ptr = <sym.BaseType*> self.sm_ptr.findBaseTypeByName[sym.BaseType](name)
        return BaseType(<uintptr_t> ptr)
    def findRefBaseTypeByName(self, string name):
        cdef sym.RefBaseType* ptr = self.sm_ptr.findRefBaseTypeByName(name)
        return RefBaseType(<uintptr_t> ptr)
    def findFunctionByName(self, string name):
        cdef sym.Function* ptr = self.sm_ptr.findFunctionByName(name)
        return Function(<uintptr_t> ptr)
    def findVariableByName(self, string name):
        cdef sym.Variable* ptr = self.sm_ptr.findVariableByName(name)
        return Variable(<uintptr_t> ptr)

    def findSymbolByID(self, uint64_t typeID):
        cdef sym.Symbol* ptr = self.sm_ptr.findSymbolByID(typeID)
        return Symbol(<uintptr_t> ptr)
    def findBaseTypeByID(self, uint64_t typeID):
        cdef sym.BaseType* ptr = self.sm_ptr.findBaseTypeByID(typeID)
        return BaseType(<uintptr_t> ptr)
    def findRefBaseTypeByID(self, uint64_t typeID):
        cdef sym.RefBaseType* ptr = self.sm_ptr.findRefBaseTypeByID(typeID)
        return RefBaseType(<uintptr_t> ptr)
    def findArrayByID(self, uint64_t typeID):
        cdef sym.Array* ptr = self.sm_ptr.findArrayByID(typeID)
        return Array(<uintptr_t> ptr)
    def findArrayByTypeID(self, uint64_t typeID, uint64_t length):
        cdef sym.Array* ptr = self.sm_ptr.findArrayByTypeID(typeID, length)
        return Array(<uintptr_t> ptr)
    def findFunctionByID(self, uint64_t typeID):
        cdef sym.Function* ptr = self.sm_ptr.findFunctionByID(typeID)
        return Function(<uintptr_t> ptr)
    def findVariableByID(self, uint64_t typeID):
        cdef sym.Variable* ptr = self.sm_ptr.findVariableByID(typeID)
        return Variable(<uintptr_t> ptr)

    def getSymbolAddress(self, const string &name):
        return self.sm_ptr.getSymbolAddress(name)
    def numberOfSymbols(self):
        return self.sm_ptr.numberOfSymbols()
    def getVarNames(self):
        return self.sm_ptr.getVarNames()

#       uint64_t getSystemMapAddress(const string &name, bool priv);
#       uint64_t getSymbolAddress(const string &name,
#                                 symbol_source src);

    def isSymbol(self, uint64_t address):
        return self.sm_ptr.isSymbol(address)
    def isFunction(self, uint64_t address):
        return self.sm_ptr.isFunction(address)

    def getModuleSymbolName(self, uint64_t address):
        return self.sm_ptr.getModuleSymbolName(address)
    def getModuleSymbolAddress(self, const string &name):
        return self.sm_ptr.getModuleSymbolAddress(name)
    def getFunctionName(self, uint64_t address):
        return self.sm_ptr.getFunctionName(address)
    def getFunctionAddress(self, const string &name):
        return self.sm_ptr.getFunctionAddress(name)

    def getContainingSymbol(self, uint64_t address):
        return self.sm_ptr.getContainingSymbol(address)

cdef class DwarfParser:
    @staticmethod
    def parseDwarfFromFilename(string filename, SymbolManager mgr):
        return sym.DwarfParser.parseDwarfFromFilename(filename, mgr.sm_ptr)


cdef class Symbol:
    cdef public bool xownership
    cdef sym.Symbol* Symbol_ptr
    def __cinit__(self, uintptr_t ptr = 0):
        self.xownership = False
        if ptr != 0:
            Symbol_ptr = <sym.Symbol*> ptr
        elif type(self) is Symbol:
            raise InstantiationNotAllowed()
            #self.xownership = True
            #self.Symbol_ptr = new sym.Symbol()
    def __dealloc__(self):
        if type(self) is Symbol and self.xownership:
            del self.Symbol_ptr
    def getByteSize(self):
        return self.Symbol_ptr.getByteSize()
    def getID(self):
        return self.Symbol_ptr.getID()
    def getName(self):
        return self.Symbol_ptr.getName()
    def print(self):
        return self.Symbol_ptr.print()


cdef class ReferencingType:
    cdef public bool xownership
    cdef sym.ReferencingType* ReferencingType_ptr
    def __cinit__(self, uintptr_t ptr = 0):
        self.xownership = False
        if ptr != 0:
            ReferencingType_ptr = <sym.ReferencingType*> ptr
        elif type(self) is ReferencingType:
            raise InstantiationNotAllowed()
            #self.xownership = True
            #self.ReferencingType_ptr = new sym.ReferencingType()
    def __dealloc__(self):
        if type(self) is ReferencingType and self.xownership:
            del self.ReferencingType_ptr
    def getBaseType(self):
        cdef sym.BaseType* ptr = self.ReferencingType_ptr.getBaseType()
        return BaseType(<uintptr_t> ptr)
    def print(self):
        return self.ReferencingType_ptr.print()

cdef class BaseType(Symbol):
    cdef sym.BaseType* BaseType_ptr
    def __cinit__(self, uintptr_t ptr = 0):
        self.xownership = False
        if ptr != 0:
            BaseType_ptr = <sym.BaseType*> ptr
        elif type(self) is BaseType:
            raise InstantiationNotAllowed()
            #self.xownership = True
            #self.Symbol_ptr = self.BaseType_ptr = new sym.BaseType()
    def __dealloc__(self):
        if type(self) is BaseType and self.xownership:
            del self.BaseType_ptr
    def getEncoding(self):
        return self.BaseType_ptr.getEncoding()
    def getInstance(self, uint64_t typeID):
        instance = self.BaseType_ptr.getInstance(typeID)
        p_instance = Instance()
        p_instance.setInstance(instance)
        return p_instance
    def getValue(self, uint64_t va, uint64_t pid = 0):
        encoding = self.BaseType_ptr.getEncoding()
        #if encoding == DW_ATE_address:
        if encoding == DW_ATE_boolean:
            return self.BaseType_ptr.getValue[bool](va, pid)
        #if encoding == DW_ATE_complex_float:
        #if encoding == DW_ATE_float:
        if encoding == DW_ATE_signed:
            return self.BaseType_ptr.getValue[int64_t](va, pid)
        if encoding == DW_ATE_signed_char:
            return self.BaseType_ptr.getValue[int8_t](va, pid)
        if encoding == DW_ATE_unsigned:
            return self.BaseType_ptr.getValue[uint64_t](va, pid)
        if encoding == DW_ATE_unsigned_char:
            return self.BaseType_ptr.getValue[uint8_t](va, pid)
        #if encoding == DW_ATE_imaginary_float:
        #if encoding == DW_ATE_packed_decimal:
        #if encoding == DW_ATE_numeric_string:
        #if encoding == DW_ATE_edited:
        #if encoding == DW_ATE_signed_fixed:
        #if encoding == DW_ATE_unsigned_fixed:
        #if encoding == DW_ATE_decimal_float:
        #if encoding == DW_ATE_UTF:
        #if encoding == DW_ATE_UCS:
        #if encoding == DW_ATE_ASCII:

        return None
        

    def getRawValueInt64_t(self, uint64_t va, uint64_t pid = 0):
        return self.BaseType_ptr.getRawValue[int64_t](va, pid);
    def getRawValueUint64_t(self, uint64_t va, uint64_t pid = 0):
        return self.BaseType_ptr.getRawValue[uint64_t](va, pid);
    def getRawValueInt32_t(self, uint64_t va, uint64_t pid = 0):
        return self.BaseType_ptr.getRawValue[int32_t](va, pid);
    def getRawValueUint32_t(self, uint64_t va, uint64_t pid = 0):
        return self.BaseType_ptr.getRawValue[uint32_t](va, pid);
    def getRawValueInt16_t(self, uint64_t va, uint64_t pid = 0):
        return self.BaseType_ptr.getRawValue[int16_t](va, pid);
    def getRawValueUint16_t(self, uint64_t va, uint64_t pid = 0):
        return self.BaseType_ptr.getRawValue[uint16_t](va, pid);
    def getRawValueInt8_t(self, uint64_t va, uint64_t pid = 0):
        return self.BaseType_ptr.getRawValue[int8_t](va, pid);
    def getRawValueUint8_t(self, uint64_t va, uint64_t pid = 0):
        return self.BaseType_ptr.getRawValue[uint8_t](va, pid);


cdef class RefBaseType(Symbol):
    cdef sym.RefBaseType* RefBaseType_ptr
    def __cinit__(self, uintptr_t ptr = 0):
        if ptr != 0:
            RefBaseType_ptr = <sym.RefBaseType*> ptr
        elif type(self) is RefBaseType:
            raise InstantiationNotAllowed()
            #self.__ownership = True
            #self.Symbol_ptr = self.RefBaseType_ptr = new sym.RefBaseType()
    def __dealloc__(self):
        if type(self) is RefBaseType and self.xownership:
            del self.RefBaseType_ptr
    def getBaseType(self):
        cdef sym.BaseType* ptr = self.RefBaseType_ptr.getBaseType()
        return BaseType(<uintptr_t> ptr)
    def getType(self):
        return self.RefBaseType_ptr.getType()

cdef class ConstType(RefBaseType):
    cdef sym.ConstType* ConstType_ptr
    def __cinit__(self, uintptr_t ptr = 0):
        if ptr != 0:
            ConstType_ptr = <sym.ConstType*> ptr
        elif type(self) is ConstType:
            raise InstantiationNotAllowed()
            #self.__ownership = True
            #self.Symbol_ptr = self.RefBaseType_ptr = self.ConstType_ptr = new sym.ConstType()
    def __dealloc__(self):
        if type(self) is ConstType and self.xownership:
            del self.ConstType_ptr

cdef class Typedef(RefBaseType):
    cdef sym.Typedef* Typedef_ptr
    def __cinit__(self, uintptr_t ptr = 0):
        if ptr != 0:
            Typedef_ptr = <sym.Typedef*> ptr
        elif type(self) is Typedef:
            raise InstantiationNotAllowed()
            #self.__ownership = True
            #self.Symbol_ptr = self.RefBaseType_ptr = self.Typedef_ptr = new sym.Typedef()
    def __dealloc__(self):
        if type(self) is Typedef and self.xownership:
            del self.Typedef_ptr

cdef class Pointer(RefBaseType):
    cdef sym.Pointer* Pointer_ptr
    def __cinit__(self, uintptr_t ptr = 0):
        if ptr != 0:
            Pointer_ptr = <sym.Pointer*> ptr
        elif type(self) is Pointer:
            raise InstantiationNotAllowed()
            #self.__ownership = True
            #self.Symbol_ptr = self.RefBaseType_ptr = self.Pointer_ptr = new sym.Pointer()
    def __dealloc__(self):
        if type(self) is Pointer and self.xownership:
            del self.Pointer_ptr

cdef class Array(Pointer):
    cdef sym.Array* Array_ptr
    def __cinit__(self, uintptr_t ptr = 0):
        if ptr != 0:
            Array_ptr = <sym.Array*> ptr
        elif type(self) is Array:
            raise InstantiationNotAllowed()
            #self.__ownership = True
            #self.Symbol_ptr = self.RefBaseType_ptr = self.Pointer_ptr = self.Array_ptr = new sym.Array()
    def __dealloc__(self):
        if type(self) is Array and self.xownership:
            del self.Array_ptr
    def getLength(self):
        return self.Array_ptr.getLength()

cdef class FuncPointer(RefBaseType):
    cdef sym.FuncPointer* FuncPointer_ptr
    def __cinit__(self, uintptr_t ptr = 0):
        if ptr != 0:
            FuncPointer_ptr = <sym.FuncPointer*> ptr
        elif type(self) is FuncPointer:
            raise InstantiationNotAllowed()
            #self.__ownership = True
            #self.Symbol_ptr = self.RefBaseType_ptr = self.FuncPointer_ptr = new sym.FuncPointer()
    def __dealloc__(self):
        if type(self) is FuncPointer and self.xownership:
            del self.FuncPointer_ptr

cdef class Enum(BaseType):
    cdef sym.Enum* Enum_ptr
    def __cinit__(self, uintptr_t ptr = 0):
        if ptr != 0:
            Enum_ptr = <sym.Enum*> ptr
        elif type(self) is Enum:
            raise InstantiationNotAllowed()
            #self.__ownership = True
            #self.Symbol_ptr = self.BaseType_ptr = self.Enum_ptr = new sym.Enum()
    def __dealloc__(self):
        if type(self) is Enum and self.xownership:
            del self.Enum_ptr
    def enumName(self, uint32_t enumID):
        return self.Enum_ptr.enumName(enumID)
    def enumValue(self, const string& enumName):
        return self.Enum_ptr.enumValue(enumName)

cdef class Function(BaseType):
    cdef sym.Function* Function_ptr
    def __cinit__(self, uintptr_t ptr = 0):
        if ptr != 0:
            Function_ptr = <sym.Function*> ptr
        elif type(self) is Function:
            raise InstantiationNotAllowed()
            #self.__ownership = True
            #self.Symbol_ptr = self.BaseType_ptr = self.Function_ptr = new sym.Function()
    def __dealloc__(self):
        if type(self) is Function and self.xownership:
            del self.Function_ptr

    def getAddress(self):
        return self.Function_ptr.getAddress()
#   vector[pair[string, uint64_t]] getParamList() const;
#   vector[pair[string, BaseType]] getFullParamList() const;
    def getParamByName(self, const string& name):
        cdef sym.BaseType* ptr = self.Function_ptr.getParamByName(name)
        return BaseType(<uintptr_t> ptr)
    def getRetTypeID(self):
        return self.Function_ptr.getRetTypeID()
    def getRetType(self):
        cdef sym.BaseType* ptr = self.Function_ptr.getRetType()
        return BaseType(<uintptr_t> ptr)


cdef class StructuredMember(Symbol):
#cdef class StructuredMember(Symbol, ReferencingType):
    cdef sym.StructuredMember* StructuredMember_ptr
    def __cinit__(self, uintptr_t ptr = 0):
        if ptr != 0:
            StructuredMember_ptr = <sym.StructuredMember*> ptr
        elif type(self) is StructuredMember:
            raise InstantiationNotAllowed()
            #self.__ownership = True
            #self.Symbol_ptr = self.StructuredMember_ptr = new sym.StructuredMember()
    def __dealloc__(self):
        if type(self) is StructuredMember and self.xownership:
            del self.StructuredMember_ptr
    def getBitSize(self):
        return self.StructuredMember_ptr.getBitSize()
    def getBitOffset(self):
        return self.StructuredMember_ptr.getBitOffset()
    def getMemberLocation(self):
        return self.StructuredMember_ptr.getMemberLocation()

cdef class Structured(BaseType):
    cdef sym.Structured* Structured_ptr
    def __cinit__(self, uintptr_t ptr = 0):
        if ptr != 0:
            Structured_ptr = <sym.Structured*> ptr
        elif type(self) is Structured:
            raise InstantiationNotAllowed()
            #self.__ownership = True
            #self.Symbol_ptr = self.BaseType_ptr = self.Structured_ptr = new sym.Structured()
    def __dealloc__(self):
        if type(self) is Structured and self.xownership:
            del self.Structured_ptr
    def memberByName(self, const string& name):
        cdef sym.StructuredMember* ptr = self.Structured_ptr.memberByName(name)
        return StructuredMember(<uintptr_t> ptr)
    def memberByOffset(self, uint32_t offset):
        cdef sym.StructuredMember* ptr = self.Structured_ptr.memberByOffset(offset)
        return StructuredMember(<uintptr_t> ptr)
    def memberNameByOffset(self, uint32_t offset):
        return self.Structured_ptr.memberNameByOffset(offset)
    def memberOffset(self, const string& name):
        return self.Structured_ptr.memberOffset(name)
    def listMembers(self):
        self.Structured_ptr.listMembers()

cdef class Struct(Structured):
    cdef sym.Struct* Struct_ptr
    def __cinit__(self, uintptr_t ptr = 0):
        if ptr != 0:
            Struct_ptr = <sym.Struct*> ptr
        elif type(self) is Struct:
            raise InstantiationNotAllowed()
            #self.__ownership = True
            #self.Symbol_ptr = self.BaseType_ptr = self.Structured_ptr = self.Struct_ptr = new sym.Struct()
    def __dealloc__(self):
        if type(self) is Struct and self.xownership:
            del self.Struct_ptr

cdef class Union(Structured):
    cdef sym.Union* Union_ptr
    def __cinit__(self, uintptr_t ptr = 0):
        if ptr != 0:
            Union_ptr = <sym.Union*> ptr
        elif type(self) is Union:
            raise InstantiationNotAllowed()
            #self.__ownership = True
            #self.Symbol_ptr = self.BaseType_ptr = self.Structured_ptr = self.Union_ptr = new sym.Union()
    def __dealloc__(self):
        if type(self) is Union and self.xownership:
            del self.Union_ptr

cdef class Variable(Symbol):
#cdef class Variable(Symbol, ReferencingType):
    cdef sym.Variable* Variable_ptr
    def __cinit__(self, uintptr_t ptr = 0):
        if ptr != 0:
            Variable_ptr = <sym.Variable*> ptr
        elif type(self) is Variable:
            raise InstantiationNotAllowed()
            #self.__ownership = True
            #self.Symbol_ptr = self.Variable_ptr = new sym.Variable()
    def __dealloc__(self):
        if type(self) is Variable and self.xownership:
            del self.Variable_ptr
    def getLocation(self):
        return self.Variable_ptr.getLocation()
    def setLocation(self, uint64_t location):
        self.Variable_ptr.setLocation(location)
    def getInstance(self):
        instance = self.Variable_ptr.getInstance()
        p_instance = Instance()
        p_instance.setInstance(instance)
        return p_instance

cdef class Instance:
    cdef sym.Instance instance
    def __cinit__(self):
        pass
    cdef setInstance(self, sym.Instance new_instance):
        instance = new_instance
    def getType(self):
        cdef sym.BaseType* ptr = self.instance.getType()
        return BaseType(<uintptr_t> ptr)
    def getRealType(self):
        cdef sym.BaseType* ptr = self.instance.getRealType()
        return BaseType(<uintptr_t> ptr)
    def getAddress(self):
        return self.instance.getAddress()
    def getLength(self):
        return self.instance.getLength()
    def isNULL(self):
        return self.instance.isNULL()
    def changeBaseType(self, const string& newType, const string fieldname):
        instance = self.instance.changeBaseType(newType, fieldname)
        p_instance = Instance()
        p_instance.setInstance(instance)
        return p_instance
    def arrayElem(self, uint64_t index):
        instance = self.instance.arrayElem(index)
        p_instance = Instance()
        p_instance.setInstance(instance)
        return p_instance
    def memberByName(self, const string& name, bool ptr = False, bool expectZero = False):
        instance = self.instance.memberByName(name, ptr, expectZero)
        p_instance = Instance()
        p_instance.setInstance(instance)
        return p_instance
    def memberByOffset(self, uint64_t offset, bool ptr = False):
        instance = self.instance.memberByOffset(offset, ptr)
        p_instance = Instance()
        p_instance.setInstance(instance)
        return p_instance
    def memberName(self, uint64_t offset):
        return self.instance.memberName(offset)
    def size(self):
        return self.instance.size()
    def memberOffset(self, const string& name):
        return self.instance.memberOffset(name)
    def dereference(self):
        instance = self.instance.dereference()
        p_instance = Instance()
        p_instance.setInstance(instance)
        return p_instance
    def getValue(self):
        address = self.instance.getAddress()
        baseType_ptr = self.instance.getRealType()
        baseType = BaseType(<uintptr_t> baseType_ptr)
        return baseType.getValue(address)
    def getRawValueInt64_t(self, bool dereference = True):
        return self.instance.getRawValue[int64_t](dereference);
    def getRawValueUint64_t(self, bool dereference = True):
        return self.instance.getRawValue[uint64_t](dereference);
    def getRawValueInt32_t(self, bool dereference = True):
        return self.instance.getRawValue[int32_t](dereference);
    def getRawValueUint32_t(self, bool dereference = True):
        return self.instance.getRawValue[uint32_t](dereference);
    def getRawValueInt16_t(self, bool dereference = True):
        return self.instance.getRawValue[int16_t](dereference);
    def getRawValueUint16_t(self, bool dereference = True):
        return self.instance.getRawValue[uint16_t](dereference);
    def getRawValueInt8_t(self, bool dereference = True):
        return self.instance.getRawValue[int8_t](dereference);
    def getRawValueUint8_t(self, bool dereference = True):
        return self.instance.getRawValue[uint8_t](dereference);

