# distutils: language = c++
# distutils: extra_compile_args = ["-std=c++14"]
# cython: profile=False
# needed to use the c++ print functions
# cython: language_level=3

# See corresponding C++ headers for reference
# Currently the Python wrapper is only meant to be a read only interface.
# Functions changing the current symbol state are only implemented in C.

from libcpp.string cimport string
from libcpp cimport bool
from libcpp.vector cimport vector
from libcpp.pair cimport pair

from libc.stdint cimport uintptr_t
from libc.stdint cimport int8_t
from libc.stdint cimport int16_t
from libc.stdint cimport int32_t
from libc.stdint cimport int64_t
from libc.stdint cimport uint8_t
from libc.stdint cimport uint16_t
from libc.stdint cimport uint32_t
from libc.stdint cimport uint64_t

ctypedef void* ptr_type

cdef extern from "symbolmanager.h":
    cdef cppclass symbol_source:
        pass

    cdef cppclass SymbolManager:
        SymbolManager()

        ptr_type findSymbolByName[T](const string &name)
        Symbol* findSymbolByID(uint64_t ID)
        ptr_type findBaseTypeByName[T](const string &name)
        BaseType* findBaseTypeByID(uint64_t ID)
        RefBaseType *findRefBaseTypeByName(const string &name);
        RefBaseType *findRefBaseTypeByID(uint64_t symId);
        Array *findArrayByID(uint64_t id);
        Array *findArrayByTypeID(uint64_t id, uint64_t length);
        Function* findFunctionByName(const string &name)
        Function* findFunctionByID(uint64_t ID)
        Variable* findVariableByName(const string &name)
        Variable* findVariableByID(uint64_t ID)

        uint64_t getSymbolAddress(const string &name)
        uint64_t numberOfSymbols();
        vector[string] getVarNames();


        uint64_t getSystemMapAddress(const string &name, bool priv);
        uint64_t getSymbolAddress(const string &name,
                                  symbol_source src);

        uint64_t getElfSymbolAddress(const string &name);
        string getElfSymbolName(uint64_t address);

        bool isSymbol(uint64_t address);
        bool isFunction(uint64_t address);

        uint64_t getFunctionAddress(const string &name);
        string getFunctionName(uint64_t address);

        uint64_t getContainingSymbol(uint64_t address);


cdef extern from "dwarfparser.h":
    cdef cppclass DwarfParser:
        @staticmethod
        void parseDwarfFromFilename(const string &filename, SymbolManager *mgr)

cdef extern from "symbol.h":
    cdef cppclass Symbol:
        uint32_t getByteSize() const
        uint64_t getID() const
        const string &getName() const
        void print() const
ctypedef Symbol* Symbol_ptr

cdef extern from "referencingtype.h":
    cdef cppclass ReferencingType:
        BaseType* getBaseType()
        void print() const
ctypedef ReferencingType* ReferincingType_ptr

cdef extern from "basetype.h":
    cdef cppclass BaseType(Symbol):
        uint64_t getEncoding()
        Instance getInstance(uint64_t)
        T getValue[T] (uint64_t, uint64_t)
        T getRawValue[T] (uint64_t, uint64_t)
        void print() const
ctypedef BaseType* BaseType_ptr

cdef extern from "refbasetype.h":
    cdef cppclass RefBaseType(BaseType):
        BaseType *getBaseType();
        uint32_t getByteSize();
        uint64_t getType();
        void print() const;
ctypedef RefBaseType* RefBaseType_ptr

cdef extern from "consttype.h":
    cdef cppclass ConstType(RefBaseType):
        void print() const;
ctypedef ConstType* ConstType_ptr

cdef extern from "typedef.h":
    cdef cppclass Typedef(RefBaseType):
        void print() const;
ctypedef Typedef* Typedef_ptr

cdef extern from "pointer.h":
    cdef cppclass Pointer(RefBaseType):
        void print() const;
ctypedef Pointer* Pointer_ptr

cdef extern from "array.h":
    cdef cppclass Array(Pointer):
        uint64_t getLength();
        uint32_t getByteSize();
        void updateTypes();
        void print() const;
ctypedef Array* Array_ptr

cdef extern from "funcpointer.h":
    cdef cppclass FuncPointer(RefBaseType):
        void print() const;
ctypedef FuncPointer* FuncPointer_ptr

cdef extern from "enum.h":
    cdef cppclass Enum(BaseType):
        string enumName(uint32_t);
        uint32_t enumValue(const string&);
        void print() const;
ctypedef Enum* Enum_ptr

cdef extern from "function.h":
    cdef cppclass Function(BaseType):
        uint64_t getAddress();
        void updateTypes();
        vector[pair[string, uint64_t]] getParamList() const;
        vector[pair[string, BaseType]] getFullParamList() const;
        BaseType* getParamByName(const string&) const;
        uint64_t getRetTypeID() const;
        BaseType* getRetType() const;
        bool operator <(const Function&) const;
        bool operator ==(const Function&) const;
        void print() const;
ctypedef Function* Funtion_ptr

cdef extern from "structuredmember.h":
    cdef cppclass StructuredMember(Symbol, ReferencingType):
        uint32_t getByteSize();
        uint32_t getBitSize();
        uint32_t getBitOffset();
        uint32_t getMemberLocation();
ctypedef StructuredMember* StructuredMember_ptr


cdef extern from "structured.h":
    cdef cppclass Structured(BaseType):
        StructuredMember *memberByName(const string&);
        void listMembers();
        StructuredMember *memberByOffset(uint32_t offset);
        string memberNameByOffset(uint32_t offset);
        uint32_t memberOffset(const string&) const;
        void print() const;
ctypedef Structured* Structured_ptr

cdef extern from "struct.h":
    cdef cppclass Struct(Structured):
        pass
ctypedef Struct* Struct_ptr

cdef extern from "union.h":
    cdef cppclass Union(Structured):
        pass
ctypedef Union* Union_ptr

cdef extern from "variable.h":
    cdef cppclass Variable(Symbol, ReferencingType):
        uint64_t getLocation();
        void setLocation(uint64_t location);
        Instance getInstance();
        void print() const;
ctypedef Variable* Variable_ptr

cdef extern from "instance.h":
    cdef cppclass Instance:
        Instance ()
        Instance (BaseType*, uint64_t, const Instance*)
        BaseType *getType()
        BaseType *getRealType()
        uint64_t getAddress()
        uint64_t getLength()
        bool isNULL()
        Instance changeBaseType(const string &newType, const string fieldname) const;
        Instance arrayElem(uint64_t) const;
        Instance memberByName(const string&, bool, bool) const;
        Instance memberByOffset(uint64_t, bool) const;
        string memberName(uint64_t) const;
        uint32_t size() const;
        uint32_t memberOffset(const string&) const;
        Instance dereference() const;
        T getValue[T]();
        T getRawValue[T](bool);
        bool operator ==(const Instance&) const;
        bool operator !=(const Instance&) const;
        void print() const;
ctypedef Instance* Instance_ptr
