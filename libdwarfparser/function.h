#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include "basetype.h"

#include <vector>

class Function : public BaseType {
	public:
		Function(Dwarf_Die object);
		virtual ~Function();

		void addParam(Dwarf_Die object);

		bool operator< (const Function& func) const;
		bool operator==(const Function& func) const;
		bool operator()(const Function *f1, const Function *f2);
		static void cleanFunctions();

	protected:
		uint64_t rettype;

		typedef std::vector<uint64_t> ParamList;
		ParamList paramList;

		void updateTypes();
		
		typedef std::vector<Function*> FuncList;
		static FuncList funcList;
		
};

#endif  /* _FUNCTION_H_ */

