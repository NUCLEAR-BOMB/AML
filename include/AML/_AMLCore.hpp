#pragma once

#include <iostream>
#include <type_traits>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>

// #define AML_NAMESPACE namespace aml {
// #define AML_NAMESPACE_END }

#if defined(AML_LIBRARY) && !defined(__INTELLISENSE__)
	#error Something went wrong
#endif

// Macro that determines whether the AML library is connected
#define AML_LIBRARY

//#define AML_NO_CUSTOM_OPERATORS
//#define AML_ALWAYS_NOEXCEPT

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
	#define AML_FORCEINLINE [[gnu::always_inline]] inline
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

#if defined(_MSVC_LANG) && defined(__cplusplus) && (_MSVC_LANG > __cplusplus)
	#define AML_CXX_STL _MSVC_LANG
#elif defined(__cplusplus)
	#define AML_CXX_STL __cplusplus
#else
	#define AML_CXX_STL 0
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

#if !AML_CXX17 && !defined(__INTELLISENSE__)
	#error C++17 or higher is required
#endif

#if AML_MSVC || defined(__FUNCSIG__)
	#define AML_CURRENT_FUNCTION __FUNCSIG__
	//#define AML_CURRENT_FUNCTION __FUNCTION__
	//#define AML_CURRENT_FUNCTION __func__
#elif AML_GCC || defined(__PRETTY_FUNCTION__)
	#define AML_CURRENT_FUNCTION __PRETTY_FUNCTION__
#elif AML_CXX_STL >= 201103
	#define AML_CURRENT_FUNCTION __func__
#else
	#define AML_CURRENT_FUNCTION ""
#endif

#if AML_WINDOWS && AML_MSVC
	#include <intrin.h>
	#define AML_DEBUG_BREAK ::__debugbreak(); ::std::abort()
#elif AML_GCC
	#define AML_DEBUG_BREAK __builtin_trap(); ::std::abort()
#elif AML_UNIX
	#include <csignal>
	#define AML_DEBUG_BREAK ::raise(SIGTRAP); ::std::abort()
#else
	#define AML_DEBUG_BREAK ::std::abort()
#endif

#ifndef AML_ALWAYS_NOEXCEPT
	#define AML_NOEXCEPT_EXPR(...) noexcept(__VA_ARGS__)
#else
	#define AML_NOEXCEPT_EXPR(...) noexcept
#endif

namespace aml {

using error_line_type		= std::decay_t<decltype(__LINE__)>;
using error_file_str_type	= std::decay_t<decltype(__FILE__)>;
using error_func_str_type	= const char*;

using error_string_type		= const char*; // std::string?

[[noreturn]] inline
void logerror(
	error_line_type line,
	error_func_str_type func, 
	error_file_str_type file,
	error_string_type msg,
	...
) noexcept 
{
	std::fprintf(stderr, "\n   ");

	va_list argptr;
	va_start(argptr, msg);
	std::vfprintf(stderr, msg, argptr);
	va_end(argptr);
	
	std::fprintf(stderr, "\n\t >>> %s \n\t %s(%d) \n", func, file, line);

	AML_DEBUG_BREAK;
}

#if AML_DEBUG
	#define AML_DEBUG_ERROR(...) \
		::aml::logerror(__LINE__, AML_CURRENT_FUNCTION, __FILE__, __VA_ARGS__)
#else /// !AML_DEBUG
	#define AML_DEBUG_ERROR(errmsg) ((void)0)
#endif /// !AML_DEBUG

#if AML_DEBUG
	#define AML_DEBUG_VERIFY(expression, ...)	\
		do { if (expression) {} else { AML_DEBUG_ERROR(__VA_ARGS__); } } while (0)
#else // !AML_DEBUG
	#define AML_DEBUG_VERIFY(expression, errmsg) ((void)0)
#endif // !AML_DEBUG

}
