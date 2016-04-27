#ifndef _BASETYPE_H_
#define _BASETYPE_H_

#include "symbol.h"
#include <libdwarf/dwarf.h>

#include <cassert>
#include <typeinfo>

#include <iostream>

#include "libvmiwrapper/vmiinstance.h"
#include "dwarfparser.h"

#include "dwarfexception.h"

class Instance;
class SymbolManager;

class BaseType : public Symbol {
public:
	BaseType(SymbolManager *manager,
	         DwarfParser *parser,
	         const Dwarf_Die &object,
	         const std::string &name);
	virtual ~BaseType();

	/**
	 * The encoding of a type is the corresponding base type (char, int, ...) if available.
	 * @return Encoding of this type.
	 */
	uint64_t getEncoding();

	/**
	 * Return an Instance for this BaseType for easier navigation.
	 * @param va Address of the BaseType in Memory
	 * @return Instance for this BaseType
	 */
	Instance getInstance(uint64_t va);

	/**
	 * Get value of this BaseType in memory. This function does an additional
	 * check for the correct encoding.
	 * @param va  Address of the BaseType in memory.
	 * @param pid Process ID to select the corresponding address space. Defaults to 0 (Kernelspace).
	 * @return Value of BaseType in memory.
	 */
	template <typename T>
	inline T getValue(uint64_t va, uint64_t pid=0);

	/**
	 * Get value of this BaseType in memory. This function returns the value
	 * independent of the actual encoding of the BaseType.
	 * @param va  Address of the BaseType in memory.
	 * @param pid Process ID to select the corresponding address space. Defaults to 0 (Kernelspace).
	 * @return Value of BaseType in memory.
	 */
	template <typename T>
	inline T getRawValue(uint64_t va, uint64_t pid=0);

	void print() const override;

private:
	uint64_t encoding; ///< Encoding of this BaseType.
};

template <>
inline std::string BaseType::getValue(uint64_t va, uint64_t pid) {
	assert(va);
	VMIInstance *vmi = VMIInstance::getInstance();
	return vmi->readStrFromVA(va, pid);
}

template <typename T>
inline T BaseType::getValue(uint64_t va, uint64_t pid) {
	assert(va);
	if (!encoding) {
		std::cout << std::hex << this->getID() << std::dec << " "
		          << this->getName() << std::endl;
		this->print();
	}
	assert(encoding);
	VMIInstance *vmi = VMIInstance::getInstance();

	switch (this->encoding) {
	case DW_ATE_address:
		// Type currenty not expected
		assert(false);
	case DW_ATE_boolean:
		if (typeid(T) == typeid(bool)) {
			return (T)vmi->read64FromVA(va, pid);
		} else {
			throw DwarfTypeException("Type not bool");
		}
	case DW_ATE_complex_float:
		// Type currenty not expected
		assert(false);
	case DW_ATE_float:
		if (typeid(T) == typeid(float)) {
			return (T)vmi->read64FromVA(va, pid);
		} else {
			throw DwarfTypeException("Type not float");
		}
	case DW_ATE_signed:
		if (typeid(T) == typeid(int64_t)) {
			return (T)vmi->read64FromVA(va, pid);
		} else {
			throw DwarfTypeException("Type not int64_t");
		}
	case DW_ATE_signed_char:
		if (typeid(T) == typeid(int8_t)) {
			return (T)vmi->read8FromVA(va, pid);
		} else {
			throw DwarfTypeException("Type not int8_t");
		}
	case DW_ATE_unsigned:
		if (typeid(T) == typeid(uint64_t)) {
			return (T)vmi->read64FromVA(va, pid);
		} else {
			throw DwarfTypeException("Type not uint64_t");
		}
	case DW_ATE_unsigned_char:
		if (typeid(T) == typeid(int8_t)) {
			return (T)vmi->read8FromVA(va, pid);
		} else {
			throw DwarfTypeException("Type not int8_t");
		}
	case DW_ATE_imaginary_float:
		assert(false);
	case DW_ATE_packed_decimal:
		assert(false);
	case DW_ATE_numeric_string:
		assert(false);
	case DW_ATE_edited:
		assert(false);
	case DW_ATE_signed_fixed:
		assert(false);
	case DW_ATE_unsigned_fixed:
		assert(false);
	case DW_ATE_decimal_float:
		assert(false);
	default:
		std::cout << "Type " << this->encoding << " not expected" << std::endl;
		assert(false);
		return 0;
	}
}

template <>
inline int8_t BaseType::getRawValue(uint64_t va, uint64_t pid) {
	assert(va);
	VMIInstance *vmi = VMIInstance::getInstance();
	return vmi->read8FromVA(va, pid);
}

template <>
inline uint8_t BaseType::getRawValue(uint64_t va, uint64_t pid) {
	assert(va);
	VMIInstance *vmi = VMIInstance::getInstance();
	return vmi->read8FromVA(va, pid);
}

template <>
inline int16_t BaseType::getRawValue(uint64_t va, uint64_t pid) {
	assert(va);
	VMIInstance *vmi = VMIInstance::getInstance();
	return vmi->read16FromVA(va, pid);
}

template <>
inline uint16_t BaseType::getRawValue(uint64_t va, uint64_t pid) {
	assert(va);
	VMIInstance *vmi = VMIInstance::getInstance();
	return vmi->read16FromVA(va, pid);
}

template <>
inline int32_t BaseType::getRawValue(uint64_t va, uint64_t pid) {
	assert(va);
	VMIInstance *vmi = VMIInstance::getInstance();
	return vmi->read32FromVA(va, pid);
}

template <>
inline uint32_t BaseType::getRawValue(uint64_t va, uint64_t pid) {
	assert(va);
	VMIInstance *vmi = VMIInstance::getInstance();
	return vmi->read32FromVA(va, pid);
}

template <>
inline int64_t BaseType::getRawValue(uint64_t va, uint64_t pid) {
	assert(va);
	VMIInstance *vmi = VMIInstance::getInstance();
	return vmi->read64FromVA(va, pid);
}

template <>
inline uint64_t BaseType::getRawValue(uint64_t va, uint64_t pid) {
	assert(va);
	VMIInstance *vmi = VMIInstance::getInstance();
	return vmi->read64FromVA(va, pid);
}

template <>
inline std::string BaseType::getRawValue(uint64_t va, uint64_t pid) {
	assert(va);
	VMIInstance *vmi = VMIInstance::getInstance();
	return vmi->readStrFromVA(va, pid);
}

template <typename T>
inline T BaseType::getRawValue(uint64_t va, uint64_t pid) {
	assert(va);
	VMIInstance *vmi = VMIInstance::getInstance();
	return 0;
}

#endif /* _BASETYPE_H_ */
