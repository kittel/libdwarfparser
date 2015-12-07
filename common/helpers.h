#ifndef _HELPERS_H_
#define _HELPERS_H_


#define UNUSED(expr) do { (void)(expr); } while (0)
#define DELETE(expr) do { if(expr){ delete expr; expr = 0; }; } while (0)

#ifdef __cplusplus
#include <type_traits>

template <typename T>
typename T::mapped_type returnPtrInMap(const T &map, const typename T::key_type &query) {
	static_assert(std::is_pointer<typename T::mapped_type>::value,
		"map needs to return a pointer type");

	auto it = map.find(query);
	return (it == map.end()) ? nullptr : it->second;
}

#endif /* __cplusplus */

#endif /* _HELPERS_H_ */
