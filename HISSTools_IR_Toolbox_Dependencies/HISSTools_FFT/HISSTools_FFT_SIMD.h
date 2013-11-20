
#ifndef _HISSTOOLS_FFT_SIMD_
#define _HISSTOOLS_FFT_SIMD_ 

#ifdef __APPLE__
#define ALIGNED_MALLOC malloc
#define ALIGNED_FREE free
#else
#include <malloc.h>
#define ALIGNED_MALLOC(x)  _aligned_malloc(x, 16)
#define ALIGNED_FREE(x)  _aligned_free(x)
#endif

#ifdef USE_APPLE_FFT
#include <Accelerate/Accelerate.h>
#else
#include <emmintrin.h>
typedef	__m128  vFloat;
typedef	__m128d vDouble;
#endif


// Test for intel compilation

#ifndef TARGET_INTEL
#if defined( __i386__ ) || defined( __x86_64__ ) || defined(WIN_VERSION)
#define TARGET_INTEL
#endif
#endif


// Define for 64 bit float vector in 128bits (2 doubles)

#ifdef TARGET_INTEL
#ifndef VECTOR_F64_128BIT
#define VECTOR_F64_128BIT
#endif
#endif


// Runtime test for SSE2

static __inline int SSE2_check()
{
#ifdef __APPLE__
	return 1;
#else
	int SSE2_flag = 0;
	int CPUInfo[4] = {-1, 0, 0, 0};
	int nIds;
	
	__cpuid(CPUInfo, 0);
	nIds = CPUInfo[0];
	
	if (nIds > 0)
	{
		__cpuid(CPUInfo, 1);
		SSE2_flag = (CPUInfo[3] >> 26) & 0x1;
	}
	
	return SSE2_flag;
#endif
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// Macros for platform-specific vector /////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef TARGET_INTEL

// Floating point 32 bit intrinsics or functions defined here

#define F32_VEC_MUL_OP					_mm_mul_ps
#define F32_VEC_ADD_OP					_mm_add_ps
#define F32_VEC_SUB_OP					_mm_sub_ps
#define F32_VEC_SHUFFLE					_mm_shuffle_ps

#define F32_SHUFFLE_CONST(z, y, x, w)	((z<<6)|(y<<4)|(x<<2)|w)

// Double precision (64 bit) floating point vector ops (intel only - test for intel compile before using)

#define F64_VEC_MUL_OP					_mm_mul_pd
#define F64_VEC_ADD_OP					_mm_add_pd
#define F64_VEC_SUB_OP					_mm_sub_pd
#define F64_VEC_SHUFFLE					_mm_shuffle_pd

#define F64_SHUFFLE_CONST(y, x)			((y<<1)|x)

#endif	/* TARGET_INTEL */

#endif	/* _HISSTOOLS_FFT_SIMD_ */
