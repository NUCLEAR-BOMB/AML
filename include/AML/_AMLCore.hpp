#pragma once

/** @file */

#include <iostream>
#include <type_traits>

// #define AML_NAMESPACE namespace aml {
// #define AML_NAMESPACE_END }

#if defined(_MSC_VER)
	#define AML_MSVC 1
#else
	#define AML_MSVC 0
#endif

#if AML_MSVC
	#define AML_ASSUME(expression) __assume(expression)
#else
	#define AML_ASSUME(expression)
#endif

#if AML_MSVC
	#define AML_UNREACHABLE __assume(0)
#else
	#define AML_UNREACHABLE __builtin_unreachable()
#endif

#if AML_MSVC
	#define AML_FORCEINLINE __forceinline
#else
	#AML_FORCEINLINE inline
#endif

#ifdef __cpp_constexpr_dynamic_alloc
	#define AML_CONSTEXPR_DYNAMIC_ALLOC constexpr
#else
	#define AML_CONSTEXPR_DYNAMIC_ALLOC
#endif

#ifdef __cpp_consteval
	#define AML_CONSTEVAL consteval
#else
	#define AML_CONSTEVAL constexpr
#endif

#ifdef NDEBUG
	#define AML_DEBUG 0
#else
	#define AML_DEBUG 1
#endif

namespace aml {

using error_line_type		= std::decay_t<decltype(__LINE__)>;
using error_file_str_type	= std::decay_t<decltype(__FILE__)>;

using error_string_type		= const char*; // std::string?

inline
void logerror(error_string_type msg, error_file_str_type file, error_line_type line) noexcept {
	std::cerr << "\n\tAML RUNTIME ERROR | " << file << "(" << line << "): " << msg << '\n';
	std::terminate();
}

#if AML_DEBUG
	#define AML_DEBUG_ERROR(errmsg) ::aml::logerror(errmsg, __FILE__, __LINE__)
	#define AML_DEBUG_ERROR_LOCATION(errmsg, file_, line_) ::aml::logerror(errmsg, file_, line_)
#else
	#define AML_DEBUG_ERROR(errmsg)
#endif

#if AML_DEBUG
	#define AML_DEBUG_VERIFY(expression, errmsg)	\
		if (expression) {}							\
		else {										\
			AML_DEBUG_ERROR(errmsg);				\
		}

	#define AML_DEBUG_VERIFY_LOCATION(expression, errmsg, file_, line_) \
		if (expression) {}												\
		else {															\
			AML_DEBUG_ERROR_LOCATION(errmsg, file_, line_);				\
		}
#else
	#define AML_DEBUG_VERIFY(expression, errmsg)
#endif

}
