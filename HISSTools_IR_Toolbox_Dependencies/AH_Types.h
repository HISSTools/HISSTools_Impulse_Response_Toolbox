
#ifndef _AH_TYPES_
#define _AH_TYPES_

#include "stdint.h"

// This needs to be altered to cope with platforms other than windows/mac and compilers other than visual studio and GCC

// Pointer-sized Integer definitions

typedef uintptr_t AH_UIntPtr;
typedef intptr_t AH_SIntPtr;

// Integer definitions for given bit sizes

typedef uint64_t AH_UInt64;
typedef int64_t AH_SInt64;
typedef uint32_t AH_UInt32;
typedef int32_t AH_SInt32;
typedef uint16_t AH_UInt16;
typedef int16_t AH_SInt16;
typedef uint8_t AH_UInt8;
typedef int8_t AH_SInt8;

// Boolean

typedef int AH_Boolean;

#undef true
#define true 1
#undef false
#define false 0

#endif  /* _AH_TYPES_ */
