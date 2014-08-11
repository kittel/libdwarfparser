#ifndef _ARRAY_H_
#define _ARRAY_H_

#include "pointer.h"

class Array: public Pointer {
	public: 
		Array(Dwarf_Die object);
		virtual ~Array();

		uint64_t getLength();
		uint64_t getByteSize();
		void update(Dwarf_Die object);

		static void cleanArrays();
	
		static Array* findArrayByID(uint64_t id);
		static Array* findArrayByTypeID(uint64_t id, uint64_t length);

	private:
		uint64_t length;
		uint64_t lengthType;
		BaseType *lengthTypeBT;

		typedef std::multimap<uint64_t, Array*> ArrayTypeMap;
		static ArrayTypeMap arrayTypeMap;
};

#endif /* _ARRAY_H_ */
