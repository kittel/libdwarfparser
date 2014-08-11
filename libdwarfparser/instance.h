#ifndef _INSTANCE_H_
#define _INSTANCE_H_

#include "basetype.h"
#include "refbasetype.h"

class Instance {

	public:
		Instance(BaseType* type, uint64_t va, Instance *parent = 0);
		virtual ~Instance();

		BaseType *getType();
		Instance getMember(std::string name);

		uint64_t getLength();
		Instance arrayElem(uint64_t element);
		Instance memberByOffset(uint64_t offset);

		uint32_t size();

		template<typename T>
		inline T getValue(){
			assert(address);
			assert(type);
			BaseType* bt = this->type;
			RefBaseType* rbt;
			while((rbt = dynamic_cast<RefBaseType*>(bt))){
				bt = rbt->getBaseType();
			}
			return bt->getValue<T>(this->address);
		}
		
		template<typename T>
		inline T getRawValue(){
			assert(address);
			assert(type);
			BaseType* bt = this->type;
			RefBaseType* rbt;
			while((rbt = dynamic_cast<RefBaseType*>(bt))){
				bt = rbt->getBaseType();
			}
			return bt->getRawValue<T>(this->address);
		}

		uint32_t memberOffset(std::string name) const;
		

	private:
		Instance *parent;
		BaseType *type;
		uint64_t address;
};


#endif /* _INSTANCE_H_ */
