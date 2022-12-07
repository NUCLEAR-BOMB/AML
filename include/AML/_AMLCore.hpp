#pragma once

#define AML_NAMESPACE namespace aml {
#define AML_NAMESPACE_END }

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

