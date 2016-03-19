#ifndef _REFBASETYPE_H_
#define _REFBASETYPE_H_

#include "basetype.h"

class RefBaseType : public BaseType {
public:
	RefBaseType(SymbolManager *mgr,
	            DwarfParser *parser,
	            const Dwarf_Die &object,
	            const std::string &name);
	virtual ~RefBaseType();

	/* overloaded class functions */
	virtual uint32_t getByteSize() override;
	virtual void print() const override;

	/**
	 * @return Pointer to the referenced BaseType.
	 */
	BaseType *getBaseType();

	/**
	 * @return ID of the referenced BaseType.
	 */
	uint64_t getType();


protected:
	uint64_t type; ///< ID of the referenced BaseType
	BaseType *base; ///< Pointer to the referenced BaseType (if already resolved)

	/**
	 * Internal function to resolve the BaseType that is referenced by this Type
	 */
	void resolveBaseType();
};

#endif /* _REFBASETYPE_H_ */
