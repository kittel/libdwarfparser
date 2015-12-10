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

	uint64_t getLength();
	virtual uint32_t getByteSize();
	void update(DwarfParser *parser, const Dwarf_Die &object);

	bool operator <(const Array &array) const;
	bool operator ==(const Array &array) const;

	void updateTypes();
	virtual void print() const;

protected:
	uint64_t length;
	uint64_t lengthType;
	BaseType *lengthTypeBT;
};

#endif /* _ARRAY_H_ */
