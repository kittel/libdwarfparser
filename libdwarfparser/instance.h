#ifndef _INSTANCE_H_
#define _INSTANCE_H_

#include "basetype.h"
#include "refbasetype.h"

class Instance {

	public:
		Instance(BaseType* type = 0, uint64_t va = 0, Instance *parent = 0);
		virtual ~Instance();

		BaseType *getType();
		uint64_t getAddress();
		Instance changeBaseType(const std::string &newType, 
				const std::string &fieldname="list");

		uint64_t getLength();
		Instance arrayElem(uint64_t element);
		Instance memberByName(const std::string &name, bool ptr = false);
		Instance memberByOffset(uint64_t offset, bool ptr = false);

		std::string memberName(uint64_t offset);

		uint32_t size();

		uint32_t memberOffset(const std::string &name) const;
		Instance dereference();
		
		template<typename T>
		inline T getValue();
		
		template<typename T>
		inline T getRawValue(bool dereference = true);

		bool operator==(const Instance& instance) const;
		bool operator!=(const Instance& instance) const;

		void print();

	private:
		Instance *parent;
		BaseType *type;
		uint64_t address;
};

template<typename T>
inline T Instance::getValue(){
	assert(address);
	assert(type);
	BaseType* bt = this->type;
	RefBaseType* rbt;
	while((rbt = dynamic_cast<RefBaseType*>(bt))){
		assert(rbt->getBaseType());
		bt = rbt->getBaseType();
	}
	return bt->getValue<T>(this->address);
}

template<typename T>
inline T Instance::getRawValue(bool dereference){
	assert(address);
	assert(type);
	if(typeid(T) != typeid(std::string) && dereference && 
			dynamic_cast<RefBaseType*>(this->type)){
		Instance i = this->dereference();
	   	return i.getRawValue<T>(i.address);
	}
	return this->type->getRawValue<T>(this->address);
}

#endif /* _INSTANCE_H_ */
