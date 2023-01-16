#pragma once

#include <iostream>
#include <type_traits>
#include <cstdarg>
#include <cstdio>

// #define AML_NAMESPACE namespace aml {
// #define AML_NAMESPACE_END }

#ifdef AML_LIBRARY
	#error Something went wrong
#endif

// Macro that determines whether the AML library is connected
#define AML_LIBRARY

//#define AML_NO_CUSTOM_OPERATORS

#define AML_HAS_NO_VALUE(macro) ((~(~macro + 0) == 0) && (~(~macro + 1) == 1))

#if defined(__INTEL_COMPILER)
	#define AML_INTEL 1
#else
	#define AML_INTEL 0
#endif

#if defined(__clang__)
	#define AML_CLANG 1
#else
	#define AML_CLANG 0
#endif

#if defined(__GNUC__) || defined(__GNUG__)
	#define AML_GCC 1
#else
	#define AML_GCC 0
#endif

#if (AML_GCC && !AML_CLANG && !AML_INTEL)
	#define AML_ONLY_GCC 1
#else
	#define AML_ONLY_GCC 0
#endif

#if defined(_MSC_VER)
	#define AML_MSVC 1
#else
	#define AML_MSVC 0
#endif

#if AML_CLANG
	#define AML_ASSUME(expression) __builtin_assume(expression)
#elif AML_GCC 
	#define AML_ASSUME(expression) \
		do { if (expression) {} else { __builtin_unreachable(); } } while (0)
#elif AML_MSVC
	#define AML_ASSUME(expression) __assume(expression)
#else
	#define AML_ASSUME(expression) ((void)0)
#endif

#if AML_GCC || AML_CLANG
	#define AML_UNREACHABLE __builtin_unreachable()
#elif AML_MSVC
	#define AML_UNREACHABLE __assume(0)
#else
	#define AML_UNREACHABLE ((void)0)
#endif

#if AML_CLANG
	#define AML_FORCEINLINE [[gnu::always_inline]] [[gnu::gnu_inline]] extern inline
#elif AML_GCC
	#define AML_FORCEINLINE [[gnu::always_inline]] inline
#elif AML_MSVC
	#define AML_FORCEINLINE __forceinline
#else
	#AML_FORCEINLINE inline
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#define AML_WINDOWS 1
#else
	#define AML_WINDOWS 0
#endif

#if defined(__unix__)
	#define AML_UNIX 1
#else
	#define AML_UNIX 0
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

#if defined(_MSVC_LANG) && (_MSVC_LANG > __cplusplus)
	#define AML_CXX_STL _MSVC_LANG
#else // !(defined(_MSVC_LANG) && (_MSVC_LANG > __cplusplus))
	#define AML_CXX_STL __cplusplus
#endif

#if AML_CXX_STL >= 202002L
	#define AML_CXX20 1
	#define AML_CXX17 1
#elif AML_CXX_STL >= 201703L
	#define AML_CXX20 0
	#define AML_CXX17 1
#else // AML_CXX_STL < 201703L
	#define AML_CXX20 0
	#define AML_CXX17 0
#endif

#if !AML_CXX17
	#error C++17 or higher is required
#endif

#if AML_MSVC
	#define AML_PRETTY_FUNCTION __FUNCSIG__
	//#define AML_PRETTY_FUNCTION __FUNCTION__
	//#define AML_PRETTY_FUNCTION __func__
#elif AML_GCC
	#define AML_PRETTY_FUNCTION __PRETTY_FUNCTION__
#else
	#define AML_PRETTY_FUNCTION ""
#endif

#if AML_WINDOWS && AML_MSVC
	#include <intrin.h>
	#define AML_DEBUG_BREAK ::__debugbreak()
#elif AML_GCC
	#define AML_DEBUG_BREAK __builtin_trap()
#elif AML_UNIX
	#include <csignal>
	#define AML_DEBUG_BREAK ::raise(SIGTRAP)
#else
	#include <cstdlib>
	#define AML_DEBUG_BREAK ::std::abort()
#endif

namespace aml {

using error_line_type		= std::decay_t<decltype(__LINE__)>;
using error_file_str_type	= std::decay_t<decltype(__FILE__)>;
using error_func_str_type	= const char*;

using error_string_type		= const char*; // std::string?

[[noreturn]] inline
void logerror(
	error_string_type msg, 
	error_func_str_type func, 
	error_file_str_type file, 
	error_line_type line, 
	...
) noexcept {
	std::cerr
		<< "\n   ";

	va_list argptr;
	va_start(argptr, line);
	vfprintf(stderr, msg, argptr);
	va_end(argptr);

	std::cerr
		<< "\n\t"
		<< " >>> " << func << "\n\t"
		<< file << "(" << line << ")" << '\n';

	AML_DEBUG_BREAK;
}

#if AML_DEBUG
	#define AML_DEBUG_ERROR(errmsg, ...) \
		::aml::logerror(errmsg, AML_PRETTY_FUNCTION, __FILE__, __LINE__, __VA_ARGS__)

	//#define AML_DEBUG_ERROR_LOCATION(errmsg, file_, line_) ::aml::logerror(errmsg, file_, line_)
#else /// !AML_DEBUG
	#define AML_DEBUG_ERROR(errmsg) ((void)0)
#endif /// !AML_DEBUG

#if AML_DEBUG
	#define AML_DEBUG_VERIFY(expression, errmsg, ...)	\
		do { if (expression) {} else { AML_DEBUG_ERROR(errmsg, __VA_ARGS__); } } while (0)

	/*
	#define AML_DEBUG_VERIFY_LOCATION(expression, errmsg, file_, line_) \
		if (expression) {}												\
		else {															\
			AML_DEBUG_ERROR_LOCATION(errmsg, file_, line_);				\
		}
	*/
#else // !AML_DEBUG
	#define AML_DEBUG_VERIFY(expression, errmsg) ((void)0)
#endif // !AML_DEBUG

//#define AML_CREATE_CUSTOM_BINARY_OPERATOR(opname, returntype, firsttype, secondtype) \


}
