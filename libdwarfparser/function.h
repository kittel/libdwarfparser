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
		static void cleanFunctions();
		void update(Dwarf_Die object);
		void print();

		static Function* findFunctionByID(uint64_t id);
		static Function* findFunctionByName(std::string name);

		uint64_t getAddress();

	protected:
		uint64_t rettype;

		uint64_t address;

		typedef std::vector<uint64_t> ParamList;
		ParamList paramList;
		bool paramsFinal;

		void updateTypes();
		
		typedef std::map<std::string, Function*> FunctionNameMap;
		static FunctionNameMap functionNameMap;

		typedef std::vector<Function*> FuncList;
		static FuncList funcList;
		
};

#endif  /* _FUNCTION_H_ */

