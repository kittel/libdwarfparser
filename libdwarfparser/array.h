#ifndef _ARRAY_H_
#define _ARRAY_H_

#include "pointer.h"

#include <vector>

class Array: public Pointer {
	public: 
		Array(DwarfParser *parser, const Dwarf_Die &object, const std::string &name);
		virtual ~Array();

		uint64_t getLength();
		virtual uint32_t getByteSize();
		void update(DwarfParser *parser, const Dwarf_Die &object);

		static void cleanArrays();
	
		static Array* findArrayByID(uint64_t id);
		static Array* findArrayByTypeID(uint64_t id, uint64_t length);

		bool operator< (const Array& array) const;
		bool operator==(const Array& array) const;

	private:
		uint64_t length;
		uint64_t lengthType;
		BaseType *lengthTypeBT;

		void updateTypes();
		void print();

		typedef std::multimap<uint64_t, Array*> ArrayTypeMap;
		static std::mutex arrayTypeMapMutex;
		static ArrayTypeMap arrayTypeMap;
		typedef std::vector<Array*> ArrayVector;
		static std::mutex arrayVectorMutex;
		static ArrayVector arrayVector;
};

#endif /* _ARRAY_H_ */
