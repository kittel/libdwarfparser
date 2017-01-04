#ifndef _ARRAY_H_
#define _ARRAY_H_

#include "pointer.h"

#include <vector>

class Array : public Pointer {
public:
	Array(SymbolManager *mgr,
	      DwarfParser *parser,
	      const Dwarf_Die &object,
	      const std::string &name);
	virtual ~Array();

	virtual uint32_t getByteSize() override;
	virtual void print() const override;

	/**
	 * @return Number of elements in Array
	 */
	uint64_t getLength();

	/**
	 * Update state of Array
	 */
	void update(DwarfParser *parser, const Dwarf_Die &object);

	bool operator <(const Array &array) const;
	bool operator ==(const Array &array) const;
	void updateTypes();


protected:
	uint64_t length;
	uint64_t lengthType;
	BaseType *lengthTypeBT;

};

#endif /* _ARRAY_H_ */
