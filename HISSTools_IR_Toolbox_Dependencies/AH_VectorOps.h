
/*
 *  AH_VectorOps
 *
 *	This header file provides a platform independent interface for SIMD operations (or vector operations).
 *	Code using this interface should compile for SSE or Altivec processors (and is also open to future changes in processor architecture).
 *	The list of operations here is by no means exhasutive, and is updated as needed using a consistent naming scheme.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#ifndef _AH_CROSS_PLATFORM_VECTOR_OPS_
#define _AH_CROSS_PLATFORM_VECTOR_OPS_ 

#ifdef __APPLE__

#include <Accelerate/Accelerate.h>

#define FORCE_INLINE				__attribute__ ((always_inline))
#define FORCE_INLINE_DEFINITION

#define ALIGNED_MALLOC malloc
#define ALIGNED_FREE free

#else

// Windows

#include <emmintrin.h>
#include <malloc.h>

#define FORCE_INLINE				__forceinline
#define FORCE_INLINE_DEFINITION		__forceinline;

#define ALIGNED_MALLOC(x)  _aligned_malloc(x, 16)
#define ALIGNED_FREE  _aligned_free

typedef	__m128i	vUInt8;
typedef __m128i vSInt8;
typedef	__m128i vUInt16;
typedef __m128i vSInt16;
typedef __m128i vUInt32;
typedef	__m128i vSInt32;
typedef	__m128i vBool32;
typedef __m128i vUInt64;
typedef __m128i vSInt64;
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
//////////////////////////////////////////////////// Utility macros (non platform-specific)  //////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// The standard compare operations return all bits on, but for use in MSP we probably want values of one
// These routines can be used in this case

static const vFloat Vec_Ops_F32_One = {1.,1.,1.,1.};

#define F32_VEC_EQUAL_MSP_OP(a,b)		F32_VEC_AND_OP(Vec_Ops_F32_One, F32_VEC_EQUAL_OP(a,b)) 
#define F32_VEC_NOTEQUAL_MSP_OP(a,b)	F32_VEC_SUB_OP(Vec_Ops_F32_One, F32_VEC_AND_OP(F32_VEC_EQUAL_OP(a,b), Vec_Ops_F32_One))
#define F32_VEC_GT_MSP_OP(a,b)			F32_VEC_AND_OP(Vec_Ops_F32_One, F32_VEC_GT_OP(a,b))
#define F32_VEC_LT_MSP_OP(a,b)			F32_VEC_AND_OP(Vec_Ops_F32_One, F32_VEC_LT_OP(a,b)) 


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////// Macros for platform-specific vector ops ///////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef TARGET_INTEL

// Intel

// Comparisons that return one or zero

static const vDouble Vec_Ops_F64_One = {1.,1.};

#define F64_VEC_EQUAL_MSP_OP(a,b)		F64_VEC_AND_OP(Vec_Ops_F64_One, F64_VEC_EQUAL_OP(a,b)) 
#define F64_VEC_NOTEQUAL_MSP_OP(a,b)	F64_VEC_SUB_OP(Vec_Ops_F64_One, F64_VEC_AND_OP(F64_VEC_EQUAL_OP(a,b), Vec_Ops_F64_One))
#define F64_VEC_GT_MSP_OP(a,b)			F64_VEC_AND_OP(Vec_Ops_F64_One, F64_VEC_GT_OP(a,b))
#define F64_VEC_LT_MSP_OP(a,b)			F64_VEC_AND_OP(Vec_Ops_F64_One, F64_VEC_LT_OP(a,b)) 

// Floating point single precision (32 bit) intrinsics or local functions

#define float2vector					_mm_set1_ps

#define F32_VEC_MUL_OP					_mm_mul_ps
#define F32_VEC_DIV_OP					_mm_div_ps
#define F32_VEC_ADD_OP					_mm_add_ps
#define F32_VEC_SUB_OP					_mm_sub_ps

#define F32_VEC_AND_OP					_mm_and_ps
#define F32_VEC_ANDNOT_OP				_mm_andnot_ps
#define F32_VEC_OR_OP					_mm_or_ps
#define F32_VEC_XOR_OP					_mm_xor_ps

#define F32_VEC_SEL_OP					_mm_sel_ps

#define F32_VEC_MIN_OP					_mm_min_ps
#define F32_VEC_MAX_OP					_mm_max_ps

#define F32_VEC_EQUAL_OP(a,b)			_mm_cmpeq_ps(a,b)
#define F32_VEC_NEQUAL_OP(a,b)			_mm_cmpneq_ps(a,b)
#define F32_VEC_GT_OP(a,b)				_mm_cmpgt_ps(a,b)
#define F32_VEC_LT_OP(a,b)				_mm_cmplt_ps(a,b)

#define F32_VEC_SQRT_OP					_mm_sqrt_ps

#define F32_VEC_ULOAD					_mm_loadu_ps
#define F32_VEC_USTORE					_mm_storeu_ps
#define F32_VEC_MOVE_LO					_mm_movelh_ps
#define F32_VEC_MOVE_HI					_mm_movehl_ps
#define F32_VEC_SHUFFLE					_mm_shuffle_ps

// Conversions from and to 32 bit floating point vectors

#define F32_VEC_FROM_I32				_mm_cvtepi32_ps
#define I32_VEC_FROM_F32_ROUND			_mm_cvtps_epi32
#define I32_VEC_FROM_F32_TRUNC			_mm_cvttps_epi32

// Floating-point double precision (64 bit) intrinsics or local functions (only available under intel)

#define double2vector					_mm_set1_pd

#define F64_VEC_MUL_OP					_mm_mul_pd
#define F64_VEC_DIV_OP					_mm_div_pd
#define F64_VEC_ADD_OP					_mm_add_pd
#define F64_VEC_SUB_OP					_mm_sub_pd

#define F64_VEC_AND_OP					_mm_and_pd
#define F64_VEC_ANDNOT_OP				_mm_andnot_pd
#define F64_VEC_OR_OP					_mm_or_pd
#define F64_VEC_XOR_OP					_mm_xor_pd

#define F64_VEC_SEL_OP					_mm_sel_pd

#define F64_VEC_MIN_OP					_mm_min_pd
#define F64_VEC_MAX_OP					_mm_max_pd

#define F64_VEC_EQUAL_OP(a,b)			_mm_cmpeq_pd(a,b)
#define F64_VEC_NEQUAL_OP(a,b)			_mm_cmpneq_pd(a,b)
#define F64_VEC_GT_OP					_mm_cmpgt_pd
#define F64_VEC_LT_OP					_mm_cmplt_pd

#define F64_VEC_SQRT_OP					_mm_sqrt_pd

#define F64_VEC_ULOAD					_mm_loadu_pd
#define F64_VEC_USTORE					_mm_storeu_pd
#define F64_VEC_UNPACK_LO				_mm_unpacklo_pd
#define F64_VEC_UNPACK_HI				_mm_unpackhi_pd
#define F64_VEC_STORE_HI				_mm_storeh_pd
#define F64_VEC_STORE_LO				_mm_storel_pd
#define F64_VEC_SET_BOTH				_mm_set1_pd
#define F64_VEC_SHUFFLE					_mm_shuffle_pd

// Conversions from and to 64 bit floating point vectors

#define F64_VEC_FROM_F32				_mm_cvtps_pd
#define F32_VEC_FROM_F64				_mm_cvtpd_ps

#define F64_VEC_FROM_I32				_mm_cvtepi32_pd 
#define I32_VEC_FROM_F64_ROUND			_mm_cvtpd_epi32
#define I32_VEC_FROM_F64_TRUNC			_mm_cvttpd_epi32

// Integer 32 bit intrinsics

#define s32int2vector					_mm_set1_epi32

#define I32_VEC_ADD_OP					_mm_add_epi32
#define I32_VEC_SUB_OP					_mm_sub_epi32
#define I32_VEC_ADD_OP					_mm_add_epi32

#define I32_VEC_MIN_OP					_mm_min_epi32
#define I32_VEC_MAX_OP					_mm_max_epi32

#define I32_VEC_OR_OP					_mm_or_si128
#define I32_VEC_AND_OP					_mm_and_si128

#define I32_VEC_SHUFFLE_OP				_mm_shuffle_epi32

// Altivec has min / max intrinics for 32 bit signed integers, but on intel this must be done in software (although it is provided under windows)
// These routines are taken directly from the apple SSE migration guide
// The guide can be found at http://developer.apple.com/legacy/mac/library/documentation/Performance/Conceptual/Accelerate_sse_migration/Accelerate_sse_migration.pdf

#ifdef __APPLE__
static __inline vSInt32 _mm_min_epi32(vSInt32 a, vSInt32 b) FORCE_INLINE;
static __inline vSInt32 _mm_min_epi32(vSInt32 a, vSInt32 b) 
{ 
	vSInt32 t = _mm_cmpgt_epi32(a,b);
	return _mm_or_si128( _mm_and_si128(t,b),_mm_andnot_si128(t,a));
}

static __inline vSInt32 _mm_max_epi32(vSInt32 a, vSInt32 b) FORCE_INLINE;
static __inline vSInt32 _mm_max_epi32(vSInt32 a, vSInt32 b) 
{ 
	vSInt32 t = _mm_cmpgt_epi32(a,b);
	return _mm_or_si128( _mm_andnot_si128(t,b),_mm_and_si128(t,a));
}
#endif 

// Altivec has selection intrinics for 32 bit floating point vectors, but on intel this must be done in software
// These routines are taken directly from the apple SSE migration guide
// The guide can be found at http://developer.apple.com/legacy/mac/library/documentation/Performance/Conceptual/Accelerate_sse_migration/Accelerate_sse_migration.pdf

#ifdef __APPLE__
static __inline vFloat _mm_sel_ps(vFloat a, vFloat b, vFloat mask) FORCE_INLINE;
static __inline vUInt32 _mm_sel_epi32(vUInt32 a, vUInt32 b, vUInt32 mask) FORCE_INLINE;
#endif

static __inline vFloat _mm_sel_ps(vFloat a, vFloat b, vFloat mask) FORCE_INLINE_DEFINITION
{ 
    b = _mm_and_ps(b, mask); 
    a = _mm_andnot_ps(mask, a); 
    return _mm_or_ps(a, b); 
} 

static __inline vUInt32 _mm_sel_epi32(vUInt32 a, vUInt32 b, vUInt32 mask) FORCE_INLINE_DEFINITION
{ 
    b = _mm_and_si128(b, mask); 
    a = _mm_andnot_si128(mask, a); 
    return _mm_or_si128(a, b); 
} 

static __inline vDouble _mm_sel_pd(vDouble a, vDouble b, vDouble mask) FORCE_INLINE_DEFINITION
{ 
    b = _mm_and_pd(b, mask); 
    a = _mm_andnot_pd(mask, a); 
    return _mm_or_pd(a, b); 
} 

#else

// Altivec

static const vFloat Vec_Ops_F32_Zero = {0.f,0.f,0.f,0.f};

// Floating point 32 bit intrinsics or local functions

#define F32_VEC_MUL_OP(v1, v2)			vec_madd(v1,v2, Vec_Ops_F32_Zero)
#define F32_VEC_DIV_OP					vdivf
#define F32_VEC_ADD_OP					vec_add
#define F32_VEC_SUB_OP					vec_sub

#define F32_VEC_AND_OP					vec_and
#define F32_VEC_XOR_OP					vec_xor
#define F32_VEC_OR_OP					vec_or
#define F32_VEC_SEL_OP					vec_sel

#define F32_VEC_MIN_OP					vec_min
#define F32_VEC_MAX_OP					vec_max

#define F32_VEC_EQUAL_OP(a,b)			vec_cmpeq(a,b) 
#define F32_VEC_NEQUAL_OP(a,b)			vec_xor(vec_cmpeq(a,b),Vec_Ops_F32_One) 
#define F32_VEC_GT_OP(a,b)				vec_cmpgt(a,b) 
#define F32_VEC_LT_OP(a,b)				vec_cmplt(a,b)

#define F32_VEC_ULOAD(p)				vec_uload((unsigned char *)p)
#define F32_VEC_USTORE(p, v)			vec_ustore((unsigned char *)p, (vector unsigned char)v)
#define F32_VEC_SHUFFLE					vec_permute

// Conversions from and to 32 bit floating point vectors

#define F32_VEC_FROM_I32(a)				vec_ctf(a, 0)
#define I32_VEC_FROM_F32_ROUND(a)		vec_cts(vec_round(a), 0)
#define I32_VEC_FROM_F32_TRUNC(a)		vec_cts(a, 0)

// Integer 32 bit intrinsics

#define I32_VEC_ADD_OP					vec_add
#define I32_VEC_SUB_OP					vec_sub

#define I32_VEC_AND_OP					vec_and

#define I32_VEC_MIN_OP					vec_min
#define I32_VEC_MAX_OP					vec_max

// Return a vector filled with a single signed integer value

static __inline vSInt32 s32int2vector (int s32int_val) FORCE_INLINE;
static __inline vSInt32 s32int2vector (int s32int_val) FORCE_INLINE_DEFINITION
{
	vSInt32 TheVector = {s32int_val, s32int_val, s32int_val, s32int_val};
	return TheVector;
}

// Return a vector filled with a single float value

static __inline vFloat float2vector (float floatval) FORCE_INLINE;
static __inline vFloat float2vector (float floatval) FORCE_INLINE_DEFINITION
{
	vFloat TheVector;
	float *TheFloatArray = (float *) &TheVector;
	
	TheFloatArray[0] = floatval;
	TheFloatArray[1] = floatval;
	TheFloatArray[2] = floatval;
	TheFloatArray[3] = floatval;
	
	return TheVector;
}

// Provide altivec safe misaligned loads and stores (not sure how fast these are)
// These routines are taken directly from the apple SSE migration guide
// The guide can be found at http://developer.apple.com/legacy/mac/library/documentation/Performance/Conceptual/Accelerate_sse_migration/Accelerate_sse_migration.pdf


static inline vFloat vec_uload(unsigned char *target) FORCE_INLINE;
static inline vFloat vec_uload(unsigned char *target)										
{
    vector unsigned char MSQ, LSQ;
    vector unsigned char mask;

    MSQ = vec_ld(0, target);						// most significant quadword
    LSQ = vec_ld(15, target);						// least significant quadword
    mask = vec_lvsl(0, target);						// create the permute mask
    return (vFloat) vec_perm(MSQ, LSQ, mask);		// align the data
}


static __inline void vec_ustore(unsigned char *target, vector unsigned char src) FORCE_INLINE;
static __inline void vec_ustore(unsigned char *target, vector unsigned char src)				
{
    src = vec_perm( src, src, vec_lvsr( 0, target ) );
    vec_ste( (vector unsigned char) src, 0, (unsigned char*) target );
    vec_ste( (vector unsigned short)src,1,(unsigned short*) target );
    vec_ste( (vector unsigned int) src, 3, (unsigned int*) target );
    vec_ste( (vector unsigned int) src, 4, (unsigned int*) target );
    vec_ste( (vector unsigned int) src, 8, (unsigned int*) target );
    vec_ste( (vector unsigned int) src, 12, (unsigned int*) target );
    vec_ste( (vector unsigned short)src,14,(unsigned short*) target );
    vec_ste( (vector unsigned char) src,15,(unsigned char*) target );
}

#endif	/* TARGET_INTEL */

#endif	/* _AH_CROSS_PLATFORM_VECTOR_OPS_ */
