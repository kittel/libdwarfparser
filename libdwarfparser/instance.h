#ifndef _INSTANCE_H_
#define _INSTANCE_H_

#include "basetype.h"
#include "refbasetype.h"

class SymbolManager;

class Instance {
public:
	Instance();
	Instance(BaseType *type,
	         uint64_t va=0,
	         const Instance *parent=nullptr);
	virtual ~Instance();

	BaseType *getType() const;
	BaseType *getRealType() const;
	uint64_t getAddress() const;
	uint64_t getLength() const;
	bool isNULL() const;

	Instance changeBaseType(const std::string &newType,
	                        const std::string &fieldname="list") const;

	Instance arrayElem(uint64_t element) const;
	Instance memberByName(const std::string &name,
	                      bool ptr=false,
	                      bool expectZeroPtr=false) const;
	Instance memberByOffset(uint64_t offset, bool ptr=false) const;

	std::string memberName(uint64_t offset) const;

	uint32_t size() const;

	uint32_t memberOffset(const std::string &name) const;
	Instance dereference() const;

	template <typename T>
	inline T getValue();

	template <typename T>
	inline T getRawValue(bool dereference=true);

	bool operator ==(const Instance &instance) const;
	bool operator !=(const Instance &instance) const;
	Instance operator[](uint64_t element) const;

	virtual void print() const;

private:
	const Instance *parent;
	BaseType *type;
	uint64_t address;
};

template <typename T>
inline T Instance::getValue() {
	assert(address);
	return this->getRealType()->getValue<T>(this->address);
}

template <typename T>
inline T Instance::getRawValue(bool dereference) {
	assert(address);
	assert(type);
	if (typeid(T) != typeid(std::string) && dereference &&
	    dynamic_cast<RefBaseType *>(this->type)) {
		Instance i = this->dereference();
		return i.getRawValue<T>(i.address);
	}
	return this->type->getRawValue<T>(this->address);
}

#endif /* _INSTANCE_H_ */
