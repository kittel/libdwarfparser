#ifndef _HELPERS_H_
#define _HELPERS_H_


#define UNUSED(expr) do { (void)(expr); } while (0)
#define DELETE(expr) do { if(expr){ delete expr; expr = 0; }; } while (0)

#define COLOR_RESET         "\033[0m"
#define COLOR_NORM          "\033[39m"
#define COLOR_GRAY          "\033[30m"
#define COLOR_RED           "\033[31m"
#define COLOR_GREEN         "\033[32m"
#define COLOR_YELLOW        "\033[33m"
#define COLOR_BLUE          "\033[34m"
#define COLOR_MARGENTA      "\033[35m"
#define COLOR_CYAN          "\033[36m"
#define COLOR_WHITE         "\033[37m"
#define COLOR_CRIMSON       "\033[38m"
#define COLOR_BOLD          "\033[1m"
#define COLOR_BOLD_OFF      "\033[22m"
#define COLOR_FAINT         "\033[2m"
#define COLOR_FAINT_OFF     "\033[22m"
#define COLOR_ITALIC        "\033[3m"
#define COLOR_ITALIC_OFF    "\033[23m"
#define COLOR_UNDERLINE     "\033[4m"
#define COLOR_UNDERLINE_OFF "\033[24m"

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
