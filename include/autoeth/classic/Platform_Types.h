/* AUTOETH Classic C compatibility: Platform_Types.h
 * Fixed-width host types (R0.2.1, R0.2.2). C99 only; C++ includes it through
 * an extern "C" block. Names, widths and order follow the supplied tables. */
#ifndef AUTOETH_CLASSIC_PLATFORM_TYPES_H
#define AUTOETH_CLASSIC_PLATFORM_TYPES_H

#include <stdint.h>

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t sint8;
typedef int16_t sint16;
typedef int32_t sint32;
typedef int64_t sint64;

typedef int8_t uint8_least;
typedef int16_t uint16_least;
typedef int32_t uint32_least;

typedef int8_t sint8_least;
typedef int16_t sint16_least;
typedef int32_t sint32_least;

typedef float float32;
typedef double float64;

#ifdef __cplusplus
typedef bool boolean;
#define NULL_PTR nullptr
#else
typedef _Bool boolean;
#define NULL_PTR ((void*)0)
#endif

#ifndef TRUE
#define TRUE ((boolean)1)
#endif
#ifndef FALSE
#define FALSE ((boolean)0)
#endif

typedef void* VoidPtr;
typedef const void* ConstVoidPtr;

#define CPU_TYPE_8 8
#define CPU_TYPE_16 16
#define CPU_TYPE_32 32
#define CPU_TYPE_64 64

#if UINTPTR_MAX == 0xFFFFFFFFFFFFFFFFu
#define CPU_TYPE CPU_TYPE_64
#elif UINTPTR_MAX == 0xFFFFFFFFu
#define CPU_TYPE CPU_TYPE_32
#else
#define CPU_TYPE CPU_TYPE_16
#endif

#endif