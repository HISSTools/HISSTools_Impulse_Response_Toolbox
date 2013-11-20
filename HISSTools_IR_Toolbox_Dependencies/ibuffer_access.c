
/*
 *  ibuffer_access.c
 *
 *	This file provides code for accessing and interpolating samplesfrom an ibuffer (or standard MSP buffer).
 *	Various kinds of interpolation are supported.
 *	All pointers used should be 16-byte aligned.
 *
 *	See the accompanying header file for more details.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include "ibuffer.h"
#include "ibuffer_access.h"

long IBuffer_SSE_Exists = 0;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////// Symbols and init (for symbols) /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void ibuffer_init ()
{
	ps_buffer = gensym("buffer~");
	ps_ibuffer = gensym("ibuffer~");
	ps_none = gensym("none");
	ps_linear = gensym("linear");
	ps_bspline = gensym("bspline");
	ps_hermite = gensym("hermite");
	ps_lagrange = gensym("lagrange");
	
	IBuffer_SSE_Exists = SSE2_check();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// Convert from 32 bit integer format to 32 bit float ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifdef __APPLE__

__inline void convert_and_scale_int32_to_float (float *out, AH_SIntPtr n_samps)
{
	vFloat scale = {TWO_POW_31_RECIP, TWO_POW_31_RECIP, TWO_POW_31_RECIP, TWO_POW_31_RECIP};
	
	vSInt32	*ivec_ptr = (vSInt32 *) out;
	vFloat *fvec_ptr = (vFloat *) out;
	AH_SInt32 *temp_ptr;
		
	AH_SIntPtr i;
	
	// Do vectors
	
	for (i = 0; i < n_samps >> 2; i++)
		*fvec_ptr++ = F32_VEC_MUL_OP (scale, F32_VEC_FROM_I32 (*ivec_ptr++));
	
	// Clean up with scalars
	
	for (temp_ptr = (AH_SInt32 *) ivec_ptr, out = (float *) fvec_ptr, i <<= 2; i < n_samps; i++)
		*out++ = (float) *temp_ptr++ * TWO_POW_31_RECIP;	
}

#else

__inline void convert_and_scale_int32_to_float (float *out, AH_SIntPtr n_samps)
{
	vFloat scale = {TWO_POW_31_RECIP, TWO_POW_31_RECIP, TWO_POW_31_RECIP, TWO_POW_31_RECIP};
	
	vSInt32	*ivec_ptr;
	vFloat *fvec_ptr ;
	AH_SInt32 *temp_ptr = (AH_SInt32 *) out;
	
	AH_SIntPtr start_offset = (16 - ((AH_SIntPtr) out % 16)) >> 2;
	AH_SIntPtr i;
		
	if (start_offset == 4)
		start_offset = 0;
	
	// This should avoid executing any SSE instructions - but it may not be safe - check later...
	
	if (!IBuffer_SSE_Exists)
		start_offset = n_samps; 
	
	// Start with scalars
	
	for (i = 0; i < start_offset && i < n_samps; i++)
		*out++ = (float) *temp_ptr++ * TWO_POW_31_RECIP;	
	
	// Do vectors
	
	for (ivec_ptr = (vSInt32 *) temp_ptr, fvec_ptr = (vFloat *) out; i < n_samps - 3; i += 4)
		*fvec_ptr++ = F32_VEC_MUL_OP (scale, F32_VEC_FROM_I32 (*ivec_ptr++));
	
	// Clean up with scalars
	
	for (temp_ptr = (AH_SInt32 *) ivec_ptr, out = (float *) fvec_ptr; i < n_samps; i++)
		*out++ = (float) *temp_ptr++ * TWO_POW_31_RECIP;	
}

#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// Get multiple consecutive samples ////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void ibuffer_get_samps_16 (void *samps, float *out, AH_SIntPtr offset, AH_SIntPtr n_samps, long n_chans, long chan)
{
	AH_SInt16 *sampschan = ((AH_SInt16 *) samps) + chan + (offset * n_chans);
	AH_SInt32 *temp_ptr = (AH_SInt32 *) out;
	AH_SIntPtr i;
	
	for (i = 0; i < n_samps; i++)
		*temp_ptr++ = (*(sampschan + (i * n_chans)) << 16) & MASK_16_BIT;
		
	convert_and_scale_int32_to_float (out, n_samps);
}


void ibuffer_get_samps_24 (void *samps, float *out, AH_SIntPtr offset, AH_SIntPtr n_samps, long n_chans, long chan)
{	
	AH_SInt8 *sampschan = ((AH_SInt8 *) samps) + (3 * (chan + (offset * n_chans))) - 1;
	AH_SInt32 *temp_ptr = (AH_SInt32 *) out;
	AH_SIntPtr i;
	
	for (i = 0; i < n_samps; i++)
		*temp_ptr++ = *((AH_SInt32 *)(sampschan + (i * 3 * n_chans))) & MASK_24_BIT;
		
	convert_and_scale_int32_to_float (out, n_samps);
}


void ibuffer_get_samps_32 (void *samps, float *out, AH_SIntPtr offset, AH_SIntPtr n_samps, long n_chans, long chan)
{
	AH_SInt32 *sampschan = ((AH_SInt32 *) samps) + chan + (offset * n_chans);
	AH_SInt32	*temp_ptr = (AH_SInt32 *) out;
	AH_SIntPtr i;
		
	for (i = 0; i < n_samps; i++)
		*temp_ptr++ = *(sampschan + (i * n_chans));
	
	convert_and_scale_int32_to_float (out, n_samps);
}


void ibuffer_get_samps_float (void *samps, float *out, AH_SIntPtr offset, AH_SIntPtr n_samps, long n_chans, long chan)
{
	float *sampschan = ((float *) samps) + chan + (offset * n_chans);
	AH_SIntPtr i;
	
	for (i = 0; i < n_samps; i++)
		*out++ = *(sampschan + (i * n_chans));
}


void ibuffer_get_samps (void *samps, float *out, AH_SIntPtr offset, AH_SIntPtr n_samps, long n_chans, long chan, long format)
{
	switch (format)
	{
		case PCM_INT_16:
			ibuffer_get_samps_16 (samps, out, offset, n_samps, n_chans, chan);
			break;
			
		case PCM_INT_24:
			ibuffer_get_samps_24 (samps, out, offset, n_samps, n_chans, chan);
			break;
			
		case PCM_INT_32:
			ibuffer_get_samps_32 (samps, out, offset, n_samps, n_chans, chan);
			break;
		
		case PCM_FLOAT:
			ibuffer_get_samps_float (samps, out, offset, n_samps, n_chans, chan);
			break;			
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// Get multiple consecutive samples in reverse ///////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void ibuffer_get_samps_rev_16 (void *samps, float *out, AH_SIntPtr offset, AH_SIntPtr n_samps, long n_chans, long chan)
{
	AH_SInt16 *sampschan = ((AH_SInt16 *) samps) + chan + (offset * n_chans);
	AH_SInt32 *temp_ptr = (AH_SInt32 *) out;
	AH_SIntPtr i;
	
	for (i = n_samps - 1; i >= 0; i--)
		*temp_ptr++ = (*(sampschan + (i * n_chans)) << 16) & MASK_16_BIT;
	
	convert_and_scale_int32_to_float (out, n_samps);
}


void ibuffer_get_samps_rev_24 (void *samps, float *out, AH_SIntPtr offset, AH_SIntPtr n_samps, long n_chans, long chan)
{
	AH_SInt8 *sampschan = ((AH_SInt8 *) samps) + (3 * (chan + (offset * n_chans))) - 1;
	AH_SInt32 *temp_ptr = (AH_SInt32 *) out;
	AH_SIntPtr i;
	
	for (i = n_samps - 1; i >= 0; i--)
		*temp_ptr++ = *((AH_SInt32 *)(sampschan + (i * 3 * n_chans)))  & MASK_24_BIT;;
		
	convert_and_scale_int32_to_float (out, n_samps);
}


void ibuffer_get_samps_rev_32 (void *samps, float *out, AH_SIntPtr offset, AH_SIntPtr n_samps, long n_chans, long chan)
{
	AH_SInt32 *sampschan = ((AH_SInt32 *) samps) + chan + + (offset * n_chans);
	AH_SInt32 *temp_ptr = (AH_SInt32 *) out;
	AH_SIntPtr i;
	
	for (i = 0; i < n_samps; i++)
		*temp_ptr++ = *(sampschan + (i * n_chans));
	
	convert_and_scale_int32_to_float (out, n_samps);
}


void ibuffer_get_samps_rev_float (void *samps, float *out, AH_SIntPtr offset, AH_SIntPtr n_samps, long n_chans, long chan)
{
	float *sampschan = ((float *) samps) + chan + (offset * n_chans);
	AH_SIntPtr i;
	
	for (i = n_samps - 1; i >= 0; i--)
		*out++ = *(sampschan + (i * n_chans));
}


void ibuffer_get_samps_rev (void *samps, float *out, AH_SIntPtr offset, AH_SIntPtr n_samps, long n_chans, long chan, long format)
{
	switch (format)
	{
		case PCM_INT_16:
			ibuffer_get_samps_rev_16 (samps, out, offset, n_samps, n_chans, chan);
			break;
			
		case PCM_INT_24:
			ibuffer_get_samps_rev_24 (samps, out, offset, n_samps, n_chans, chan);
			break;
			
		case PCM_INT_32:
			ibuffer_get_samps_rev_32 (samps, out, offset, n_samps, n_chans, chan);
			break;
		
		case PCM_FLOAT:
			ibuffer_get_samps_rev_float (samps, out, offset, n_samps, n_chans, chan);
			break;
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// Fetch samples /////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define IBUFFER_FETCH_LOOP_UNROLL(x)			\
AH_SIntPtr i;									\
for (i = 0; i < n_samps >> 3; i++)				\
{x; x; x; x; x; x; x; x;}						\
for (i <<= 3; i < n_samps; i++)					\
{x;}											\
for (; i < ((n_samps + 3) >> 2) << 2; i++)		\
*out++ = 0;

__inline void ibuffer_fetch_samps_float (float *out, float *samps, AH_SIntPtr *offsets, AH_SIntPtr n_samps)
{	
	IBUFFER_FETCH_LOOP_UNROLL (*out++ = *(samps + *offsets++))
}

__inline void ibuffer_fetch_samps_16 (AH_SInt32 *out, AH_SInt16 *samps, AH_SIntPtr *offsets, AH_SIntPtr n_samps)
{
	IBUFFER_FETCH_LOOP_UNROLL (*out++ = (*(samps + *offsets++) << 16))
}

__inline void ibuffer_fetch_samps_24 (AH_SInt32 *out, AH_SInt8 *samps, AH_SIntPtr *offsets, AH_SIntPtr n_samps)
{	
#if (TARGET_RT_LITTLE_ENDIAN)
	IBUFFER_FETCH_LOOP_UNROLL (*out++ = (*((AH_SInt32 *) ((samps - 1) + *offsets++)) & MASK_24_BIT))
#else
	IBUFFER_FETCH_LOOP_UNROLL (*out++ = (*((AH_SInt32 *) (samps + *offsets++)) & MASK_24_BIT))
#endif
}

__inline void ibuffer_fetch_samps_32 (AH_SInt32 *out, AH_SInt32 *samps, AH_SIntPtr *offsets, AH_SIntPtr n_samps)
{
	IBUFFER_FETCH_LOOP_UNROLL (*out++ = (*(samps + *offsets++)))
}

void ibuffer_fetch_samps (void *out, void *samps, AH_SIntPtr *offsets, AH_SIntPtr n_samps, long n_chans, long chan, long format)
{
	switch (format)
	{
		case PCM_INT_16:
			ibuffer_fetch_samps_16 (out, ((AH_SInt16 *) samps) + chan, offsets, n_samps);
			break;
		case PCM_INT_24:
			ibuffer_fetch_samps_24 (out, (AH_SInt8 *) samps + (3 * chan), offsets, n_samps);
			break;
		case PCM_INT_32:
			ibuffer_fetch_samps_32 (out, ((AH_SInt32 *) samps) + chan, offsets, n_samps);
			break;
		case PCM_FLOAT:
			ibuffer_fetch_samps_float (out, (float *) samps + chan, offsets, n_samps);
			break;
	}
}

void ibuffer_fetch_samps_2 (void **temp, void *samps, AH_SIntPtr *offsets, AH_SIntPtr n_samps, long n_chans, long chan, long format)
{
	switch (format)
	{
		case PCM_INT_16:
			ibuffer_fetch_samps_16 (temp[0], ((AH_SInt16 *) samps) + chan, offsets, n_samps);
			ibuffer_fetch_samps_16 (temp[1], ((AH_SInt16 *) samps) + chan + n_chans, offsets, n_samps);
			break;
		case PCM_INT_24:
			ibuffer_fetch_samps_24 (temp[0], ((AH_SInt8 *) samps) + (3 * chan), offsets, n_samps);
			ibuffer_fetch_samps_24 (temp[1], ((AH_SInt8 *) samps) + (3 * (chan + n_chans)), offsets, n_samps);
			break;
		case PCM_INT_32:
			ibuffer_fetch_samps_32 (temp[0], ((AH_SInt32 *) samps) + chan, offsets, n_samps);
			ibuffer_fetch_samps_32 (temp[1], ((AH_SInt32 *) samps) + chan + n_chans, offsets, n_samps);
			break;
		case PCM_FLOAT:
			ibuffer_fetch_samps_float (temp[0], ((float *) samps) + chan, offsets, n_samps);
			ibuffer_fetch_samps_float (temp[1], ((float *) samps) + chan + n_chans, offsets, n_samps);
			break;
	}
}

void ibuffer_fetch_samps_4 (void **temp, void *samps, AH_SIntPtr *offsets, AH_SIntPtr n_samps, long n_chans, long chan, long format)
{
	switch (format)
	{
		case PCM_INT_16:
			ibuffer_fetch_samps_16 (temp[0], ((AH_SInt16 *) samps) + chan - n_chans, offsets, n_samps);
			ibuffer_fetch_samps_16 (temp[1], ((AH_SInt16 *) samps) + chan, offsets, n_samps);
			ibuffer_fetch_samps_16 (temp[2], ((AH_SInt16 *) samps) + chan + n_chans, offsets, n_samps);
			ibuffer_fetch_samps_16 (temp[3], ((AH_SInt16 *) samps) + chan + (n_chans << 1), offsets, n_samps);
			break;
		case PCM_INT_24:
			ibuffer_fetch_samps_24 (temp[0], ((AH_SInt8 *) samps) + (3 * (chan - n_chans)), offsets, n_samps);
			ibuffer_fetch_samps_24 (temp[1], ((AH_SInt8 *) samps) + (3 * chan), offsets, n_samps);
			ibuffer_fetch_samps_24 (temp[2], ((AH_SInt8 *) samps) + (3 * (chan + n_chans)), offsets, n_samps);
			ibuffer_fetch_samps_24 (temp[3], ((AH_SInt8 *) samps) + (3 * (chan + (n_chans << 1))), offsets, n_samps);
			break;
		case PCM_INT_32:
			ibuffer_fetch_samps_32 (temp[0], ((AH_SInt32 *) samps) + chan - n_chans, offsets, n_samps);
			ibuffer_fetch_samps_32 (temp[1], ((AH_SInt32 *) samps) + chan, offsets, n_samps);
			ibuffer_fetch_samps_32 (temp[2], ((AH_SInt32 *) samps) + chan + n_chans, offsets, n_samps);
			ibuffer_fetch_samps_32 (temp[3], ((AH_SInt32 *) samps) + chan + (n_chans << 1), offsets, n_samps);
			break;
		case PCM_FLOAT:
			ibuffer_fetch_samps_float (temp[0], ((float *) samps) + chan - n_chans, offsets, n_samps);
			ibuffer_fetch_samps_float (temp[1], ((float *) samps) + chan, offsets, n_samps);
			ibuffer_fetch_samps_float (temp[2], ((float *) samps) + chan + n_chans, offsets, n_samps);
			ibuffer_fetch_samps_float (temp[3], ((float *) samps) + chan + (n_chans << 1), offsets, n_samps);
			break;
	}
}
	

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// Get samples with no interpolation (32 bit) //////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void ibuffer_float_samps_simd_nointerp (void *samps, vFloat *out, AH_SIntPtr *offsets, AH_SIntPtr n_samps, long n_chans, long chan, long format, float mul)
{
	vFloat scale = float2vector ((format == PCM_FLOAT) ? mul : (mul * TWO_POW_31_RECIP));
	vSInt32 *int_vec = (vSInt32 *) out;
	AH_SIntPtr i;	

	ibuffer_fetch_samps ((void *) out, samps, offsets, n_samps, n_chans, chan, format);
	
	if (format == PCM_FLOAT)
	{
		for (i = 0; i < ((n_samps + 3) >> 2); i++, out++)
			*out = F32_VEC_MUL_OP (scale, *out);
	}
	else 
	{
		for (i = 0; i < (n_samps + 3) >> 2; i++)
			*out++ = F32_VEC_MUL_OP (scale, F32_VEC_FROM_I32 (*int_vec++));
	}
}

void ibuffer_float_samps_scalar_nointerp (void *samps, float *out, AH_SIntPtr *offsets, AH_SIntPtr n_samps, long n_chans, long chan, long format, float mul)
{
	AH_SIntPtr i;	
		
	for (i = 0; i < n_samps; i++)
		*out++ = mul * ibuffer_float_get_samp (samps, offsets[i], n_chans, chan, format);
}					


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// Get samples with linear interpolation (32 bit) ////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void ibuffer_lin_interp_float (vFloat *out,  void **inbuffers, vFloat *fracts, AH_SIntPtr n_samps_over_4, float mul)
{		
    vFloat *in1 = (vFloat *) inbuffers[0];
	vFloat *in2 = (vFloat *) inbuffers[1];
	
	vFloat scale = float2vector (mul);
	vFloat y0, y1;	
		
	while (n_samps_over_4--)
	{
		y0 = *in1++;
		y1 = *in2++; 
		
		*out++ = F32_VEC_MUL_OP (scale, F32_VEC_ADD_OP (y0, F32_VEC_MUL_OP (*fracts++, F32_VEC_SUB_OP (y1, y0))));  
	}
}

void ibuffer_lin_interp_int_to_float(vFloat *out, void **inbuffers, vFloat *fracts, AH_SIntPtr n_samps_over_4, float mul)
{		
    vSInt32 *in1 = inbuffers[0];
	vSInt32 *in2 = inbuffers[1];
	
	vFloat scale = float2vector (TWO_POW_31_RECIP * mul);
	vFloat y0, y1;	
		
	while(n_samps_over_4--)
	{
		y0 = F32_VEC_FROM_I32 (*in1++);
		y1 = F32_VEC_FROM_I32 (*in2++);
		
		*out++ = F32_VEC_MUL_OP (scale, F32_VEC_ADD_OP (y0, F32_VEC_MUL_OP (*fracts++, F32_VEC_SUB_OP (y1, y0)))); 
	}
}

void ibuffer_float_samps_simd_linear (void *samps, vFloat *out, AH_SIntPtr *offsets, vFloat *fracts, void **temp, AH_SIntPtr n_samps, long n_chans, long chan, long format, float mul)
{
	ibuffer_fetch_samps_2 (temp, samps, offsets, n_samps, n_chans, chan, format);
	
	if (format == PCM_FLOAT)
		ibuffer_lin_interp_float (out, temp, fracts, (n_samps + 3) >> 2, mul);
	else 
		ibuffer_lin_interp_int_to_float(out, temp, fracts, (n_samps + 3) >> 2, mul);
}

void ibuffer_float_samps_scalar_linear (void *samps, float *out, AH_SIntPtr *offsets, float *fracts, AH_SIntPtr n_samps, long n_chans, long chan, long format, float mul)
{
	float fract, y0, y1;
	AH_SIntPtr offset, i;
	
	for (i = 0; i < n_samps; i++)
	{
		offset = offsets[i];
		fract = fracts[i];
		
		y0 = ibuffer_float_get_samp (samps, offset, n_chans, chan, format);
		y1 = ibuffer_float_get_samp (samps, offset + 1, n_chans, chan, format);
		
		*out++ = mul * (y0 + (fract * (y1 - y0)));  
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// Get samples with cubic bspline interpolation (32 bit) /////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void ibuffer_cubic_bspline_float (vFloat *out, void **inbuffers, vFloat *fracts, AH_SIntPtr n_samps_over_4, float mul)
{		
    vFloat *in1 = fracts;
    vFloat *in2 = (vFloat *) inbuffers[0];
	vFloat *in3 = (vFloat *) inbuffers[1];
	vFloat *in4 = (vFloat *) inbuffers[2];
	vFloat *in5 = (vFloat *) inbuffers[3];
	
	vFloat c0, c1, c2, c3, y0py2, y0, y1, y2, y3, xval;
	
	vFloat Half = {0.5f, 0.5f, 0.5f, 0.5f};
	vFloat Sixth = {1.f/6.f, 1.f/6.f, 1.f/6.f, 1.f/6.f};
	vFloat TwoThirds = {2.f/3.f, 2.f/3.f, 2.f/3.f, 2.f/3.f};
	
	vFloat scale = float2vector (mul);
		
	while (n_samps_over_4--)
	{
		xval = *in1++;
		y0 = *in2++;
		y1 = *in3++; 
		y2 = *in4++; 
		y3 = *in5++;
		
		y0py2 = F32_VEC_ADD_OP (y0 , y2);								
		c0 = F32_VEC_ADD_OP(F32_VEC_MUL_OP (Sixth, y0py2), F32_VEC_MUL_OP (TwoThirds, y1));					
		c1 = F32_VEC_MUL_OP (Half, F32_VEC_SUB_OP (y2, y0));							
		c2 = F32_VEC_SUB_OP (F32_VEC_MUL_OP (Half, y0py2), y1);								
		c3 = F32_VEC_ADD_OP (F32_VEC_MUL_OP (Half, F32_VEC_SUB_OP (y1, y2)), F32_VEC_MUL_OP (Sixth, F32_VEC_SUB_OP (y3, y0)));	
				
		*out++ = F32_VEC_MUL_OP (scale, F32_VEC_ADD_OP (c0, F32_VEC_MUL_OP (xval, F32_VEC_ADD_OP (c1, F32_VEC_MUL_OP (xval, F32_VEC_ADD_OP (c2, F32_VEC_MUL_OP (xval, c3))))))); 
	}
}

void ibuffer_cubic_bspline_int_to_float(vFloat *out, void **inbuffers, vFloat *fracts, AH_SIntPtr n_samps_over_4, float mul)
{		
    vFloat *in1 = fracts;
	vSInt32 *in2 = (vSInt32 *) inbuffers[0];
	vSInt32 *in3 = (vSInt32 *) inbuffers[1];
	vSInt32 *in4 = (vSInt32 *) inbuffers[2];
	vSInt32 *in5 = (vSInt32 *) inbuffers[3];
	
	vFloat c0, c1, c2, c3, y0py2, y0, y1, y2, y3, xval;
	
	vFloat Half = {0.5f, 0.5f, 0.5f, 0.5f};
	vFloat Sixth = {1.f/6.f, 1.f/6.f, 1.f/6.f, 1.f/6.f};
	vFloat TwoThirds = {2.f/3.f, 2.f/3.f, 2.f/3.f, 2.f/3.f};
	
	vFloat scale = float2vector (TWO_POW_31_RECIP * mul);
		
	while (n_samps_over_4--)
	{
		xval = *in1++;
		y0 = F32_VEC_FROM_I32(*in2++);
		y1 = F32_VEC_FROM_I32(*in3++); 
		y2 = F32_VEC_FROM_I32(*in4++); 
		y3 = F32_VEC_FROM_I32(*in5++);
		
		y0py2 = F32_VEC_ADD_OP (y0 , y2);								
		c0 = F32_VEC_ADD_OP(F32_VEC_MUL_OP (Sixth, y0py2), F32_VEC_MUL_OP (TwoThirds, y1));					
		c1 = F32_VEC_MUL_OP (Half, F32_VEC_SUB_OP (y2, y0));							
		c2 = F32_VEC_SUB_OP (F32_VEC_MUL_OP (Half, y0py2), y1);								
		c3 = F32_VEC_ADD_OP (F32_VEC_MUL_OP (Half, F32_VEC_SUB_OP (y1, y2)), F32_VEC_MUL_OP (Sixth, F32_VEC_SUB_OP (y3, y0)));	
		
		*out++ = F32_VEC_MUL_OP (scale, F32_VEC_ADD_OP (c0, F32_VEC_MUL_OP (xval, F32_VEC_ADD_OP (c1, F32_VEC_MUL_OP (xval, F32_VEC_ADD_OP (c2, F32_VEC_MUL_OP (xval, c3))))))); 
	}
}


void ibuffer_float_samps_simd_cubic_bspline (void *samps, vFloat *out, AH_SIntPtr *offsets, vFloat *fracts, void **temp, AH_SIntPtr n_samps, long n_chans, long chan, long format, float mul)
{
	ibuffer_fetch_samps_4 (temp, samps, offsets, n_samps, n_chans, chan, format);
	
	if (format == PCM_FLOAT)
		ibuffer_cubic_bspline_float (out, temp, fracts, (n_samps + 3) >> 2, mul);
	else 
		ibuffer_cubic_bspline_int_to_float(out, temp, fracts, (n_samps + 3) >> 2, mul);
}

void ibuffer_float_samps_scalar_cubic_bspline (void *samps, float *out, AH_SIntPtr *offsets, float *fracts, AH_SIntPtr n_samps, long n_chans, long chan, long format, float mul)
{
	float fract, y0, y1, y2, y3, y0py2, c0, c1, c2, c3;
	AH_SIntPtr offset, i;
	
	for (i = 0; i < n_samps; i++)
	{
		offset = offsets[i];
		fract = fracts[i];
		
		y0 = ibuffer_float_get_samp (samps, offset - 1, n_chans, chan, format);
		y1 = ibuffer_float_get_samp (samps, offset, n_chans, chan, format);
		y2 = ibuffer_float_get_samp (samps, offset + 1, n_chans, chan, format);
		y3 = ibuffer_float_get_samp (samps, offset + 2, n_chans, chan, format);
		
		y0py2 = y0 + y2;
		c0 = 1.f/6.f * y0py2 + 2.f/3.f * y1;
		c1 = 1.f/2.f * (y2 - y0);
		c2 = 1.f/2.f * y0py2 - y1;
		c3 = 1.f/2.f * (y1 - y2) + 1.f/6.f * (y3 - y0);
		
		*out++ = mul * (((c3 * fract + c2) * fract + c1) * fract + c0); 
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// Get samples with cubic hermite interpolation (32 bit) /////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void ibuffer_cubic_hermite_float (vFloat *out, void **inbuffers, vFloat *fracts, AH_SIntPtr n_samps_over_4, float mul)
{		
    vFloat *in1 = fracts;
    vFloat *in2 = (vFloat *) inbuffers[0];
	vFloat *in3 = (vFloat *) inbuffers[1];
	vFloat *in4 = (vFloat *) inbuffers[2];
	vFloat *in5 = (vFloat *) inbuffers[3];
	
	vFloat c1, c2, c3, y0, y1, y2, y3, xval;
	
	vFloat Half = {0.5f, 0.5f, 0.5f, 0.5f};
	vFloat OneAndHalf = {1.5f, 1.5f, 1.5f, 1.5f};
	vFloat TwoAndHalf = {2.5f, 2.5f, 2.5f, 2.5f};
	
	vFloat scale = float2vector (mul);
		
	while (n_samps_over_4--)
	{
		xval = *in1++;
		y0 = *in2++;
		y1 = *in3++; 
		y2 = *in4++; 
		y3 = *in5++;
		
		c1 = F32_VEC_MUL_OP (Half, F32_VEC_SUB_OP (y2, y0));
		c2 = F32_VEC_ADD_OP (F32_VEC_SUB_OP (y0, F32_VEC_MUL_OP ( TwoAndHalf, y1)), F32_VEC_SUB_OP (F32_VEC_ADD_OP (y2, y2), F32_VEC_MUL_OP (Half, y3)));     		
		c3 = F32_VEC_ADD_OP (F32_VEC_MUL_OP (Half, F32_VEC_SUB_OP (y3, y0)), F32_VEC_MUL_OP (OneAndHalf, F32_VEC_SUB_OP (y1, y2)));
		
		*out++ = F32_VEC_MUL_OP (scale, F32_VEC_ADD_OP (y1, F32_VEC_MUL_OP (xval, F32_VEC_ADD_OP (c1, F32_VEC_MUL_OP (xval, F32_VEC_ADD_OP (c2, F32_VEC_MUL_OP (xval, c3))))))); 
	}
}

void ibuffer_cubic_hermite_int_to_float(vFloat *out,void **inbuffers, vFloat *fracts, AH_SIntPtr n_samps_over_4, float mul)
{		
    vFloat *in1 = fracts;
    vSInt32 *in2 = (vSInt32 *) inbuffers[0];
	vSInt32 *in3 = (vSInt32 *) inbuffers[1];
	vSInt32 *in4 = (vSInt32 *) inbuffers[2];
	vSInt32 *in5 = (vSInt32 *) inbuffers[3];
	
	vFloat c1, c2, c3, y0, y1, y2, y3, xval;
	
	vFloat Half = {0.5f, 0.5f, 0.5f, 0.5f};
	vFloat OneAndHalf = {1.5f, 1.5f, 1.5f, 1.5f};
	vFloat TwoAndHalf = {2.5f, 2.5f, 2.5f, 2.5f};
	
	vFloat scale = float2vector (TWO_POW_31_RECIP * mul);
		
	while (n_samps_over_4--)
	{
		xval = *in1++;
		y0 = F32_VEC_FROM_I32(*in2++);
		y1 = F32_VEC_FROM_I32(*in3++); 
		y2 = F32_VEC_FROM_I32(*in4++); 
		y3 = F32_VEC_FROM_I32(*in5++);
		
		c1 = F32_VEC_MUL_OP (Half, F32_VEC_SUB_OP (y2, y0));
		c2 = F32_VEC_ADD_OP (F32_VEC_SUB_OP (y0, F32_VEC_MUL_OP ( TwoAndHalf, y1)), F32_VEC_SUB_OP (F32_VEC_ADD_OP (y2, y2), F32_VEC_MUL_OP (Half, y3)));     		
		c3 = F32_VEC_ADD_OP (F32_VEC_MUL_OP (Half, F32_VEC_SUB_OP (y3, y0)), F32_VEC_MUL_OP (OneAndHalf, F32_VEC_SUB_OP (y1, y2)));
		
		*out++ = F32_VEC_MUL_OP (scale, F32_VEC_ADD_OP (y1, F32_VEC_MUL_OP (xval, F32_VEC_ADD_OP (c1, F32_VEC_MUL_OP (xval, F32_VEC_ADD_OP (c2, F32_VEC_MUL_OP (xval, c3))))))); 
	}
}

void ibuffer_float_samps_simd_cubic_hermite (void *samps, vFloat *out, AH_SIntPtr *offsets, vFloat *fracts, void **temp, AH_SIntPtr n_samps, long n_chans, long chan, long format, float mul)
{
	ibuffer_fetch_samps_4 (temp, samps, offsets, n_samps, n_chans, chan, format);
	
	if (format == PCM_FLOAT)
		ibuffer_cubic_hermite_float (out, temp, fracts, (n_samps + 3) >> 2, mul);
	else 
		ibuffer_cubic_hermite_int_to_float(out, temp, fracts, (n_samps + 3) >> 2, mul);
}

void ibuffer_float_samps_scalar_cubic_hermite (void *samps, float *out, AH_SIntPtr *offsets, float *fracts, AH_SIntPtr n_samps, long n_chans, long chan, long format, float mul)
{
	float fract, y0, y1, y2, y3, c0, c1, c2, c3;
	AH_SIntPtr offset, i;
	
	for (i = 0; i < n_samps; i++)
	{
		offset = offsets[i];
		fract = fracts[i];
		
		y0 = ibuffer_float_get_samp (samps, offset - 1, n_chans, chan, format);
		y1 = ibuffer_float_get_samp (samps, offset, n_chans, chan, format);
		y2 = ibuffer_float_get_samp (samps, offset + 1, n_chans, chan, format);
		y3 = ibuffer_float_get_samp (samps, offset + 2, n_chans, chan, format);
		
		c0 = y1;												
		c1 = 0.5f * (y2 - y0);								
		c2 = y0 - 2.5f * y1 + y2 + y2 - 0.5f * y3;	
		c3 = 0.5f * (y3 - y0) + 1.5f * (y1 - y2);	
		
		*out++ = mul * (((c3 * fract + c2) * fract + c1) * fract + c0);
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// Get samples with cubic lagrange interpolation (32 bit) ////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void ibuffer_cubic_lagrange_float (vFloat *out, void **inbuffers, vFloat *fracts, AH_SIntPtr n_samps_over_4, float mul)
{		
	vFloat *in1 = fracts;
	vFloat *in2 = (vFloat *) inbuffers[0];
	vFloat *in3 = (vFloat *) inbuffers[1];
	vFloat *in4 = (vFloat *) inbuffers[2];
	vFloat *in5 = (vFloat *) inbuffers[3];
 
	vFloat c1, c2, c3, y0, y1, y2, y3, xval;
	
	vFloat Half = {0.5f, 0.5f, 0.5f, 0.5f};
	vFloat Third = {1.f/3.f, 1.f/3.f, 1.f/3.f, 1.f/3.f};
	vFloat Sixth = {1.f/6.f, 1.f/6.f, 1.f/6.f, 1.f/6.f};
 
	vFloat scale = float2vector (mul);
  
	while (n_samps_over_4--)
	{
		xval = *in1++;
		y0 = *in2++;
		y1 = *in3++; 
		y2 = *in4++; 
		y3 = *in5++;

		c1 = F32_VEC_SUB_OP (F32_VEC_SUB_OP (y2, F32_VEC_MUL_OP (Third, y0)) , F32_VEC_ADD_OP (F32_VEC_MUL_OP (Half, y1), F32_VEC_MUL_OP (Sixth, y3)));
		c2 = F32_VEC_SUB_OP (F32_VEC_MUL_OP (Half, F32_VEC_ADD_OP (y0, y2)), y1);     		
		c3 = F32_VEC_ADD_OP (F32_VEC_MUL_OP (Sixth, F32_VEC_SUB_OP (y3, y0)), F32_VEC_MUL_OP (Half, F32_VEC_SUB_OP (y1, y2)));
 
		*out++ = F32_VEC_MUL_OP (scale, F32_VEC_ADD_OP (y1, F32_VEC_MUL_OP (xval, F32_VEC_ADD_OP (c1, F32_VEC_MUL_OP (xval, F32_VEC_ADD_OP (c2, F32_VEC_MUL_OP (xval, c3))))))); 
	}
}

void ibuffer_cubic_lagrange_int_to_float(vFloat *out,void **inbuffers, vFloat *fracts, AH_SIntPtr n_samps_over_4, float mul)
{		
	vFloat *in1 = fracts;
	vSInt32 *in2 = (vSInt32 *) inbuffers[0];
	vSInt32 *in3 = (vSInt32 *) inbuffers[1];
	vSInt32 *in4 = (vSInt32 *) inbuffers[2];
	vSInt32 *in5 = (vSInt32 *) inbuffers[3];
	
	vFloat c1, c2, c3, y0, y1, y2, y3, xval;
	
	vFloat Half = {0.5f, 0.5f, 0.5f, 0.5f};
	vFloat Third = {1.f/3.f, 1.f/3.f, 1.f/3.f, 1.f/3.f};
	vFloat Sixth = {1.f/6.f, 1.f/6.f, 1.f/6.f, 1.f/6.f};
	
	vFloat scale = float2vector (TWO_POW_31_RECIP * mul);
		
	while (n_samps_over_4--)
	{
		xval = *in1++;
		y0 = F32_VEC_FROM_I32(*in2++);
		y1 = F32_VEC_FROM_I32(*in3++); 
		y2 = F32_VEC_FROM_I32(*in4++); 
		y3 = F32_VEC_FROM_I32(*in5++);
		
		c1 = F32_VEC_SUB_OP (F32_VEC_SUB_OP (y2, F32_VEC_MUL_OP (Third, y0)) , F32_VEC_ADD_OP (F32_VEC_MUL_OP (Half, y1), F32_VEC_MUL_OP (Sixth, y3)));
		c2 = F32_VEC_SUB_OP (F32_VEC_MUL_OP (Half, F32_VEC_ADD_OP (y0, y2)), y1);     		
		c3 = F32_VEC_ADD_OP (F32_VEC_MUL_OP (Sixth, F32_VEC_SUB_OP (y3, y0)), F32_VEC_MUL_OP (Half, F32_VEC_SUB_OP (y1, y2)));
		
		*out++ = F32_VEC_MUL_OP (scale, F32_VEC_ADD_OP (y1, F32_VEC_MUL_OP (xval, F32_VEC_ADD_OP (c1, F32_VEC_MUL_OP (xval, F32_VEC_ADD_OP (c2, F32_VEC_MUL_OP (xval, c3))))))); 
	}
}

void ibuffer_float_samps_simd_cubic_lagrange (void *samps, vFloat *out, AH_SIntPtr *offsets, vFloat *fracts, void **temp, AH_SIntPtr n_samps, long n_chans, long chan, long format, float mul)
{
	ibuffer_fetch_samps_4 (temp, samps, offsets, n_samps, n_chans, chan, format);
	
	if (format == PCM_FLOAT)
		ibuffer_cubic_lagrange_float (out, temp, fracts, (n_samps + 3) >> 2, mul);
	else 
		ibuffer_cubic_lagrange_int_to_float(out, temp, fracts, (n_samps + 3) >> 2, mul);
}

void ibuffer_float_samps_scalar_cubic_lagrange (void *samps, float *out, AH_SIntPtr *offsets, float *fracts, AH_SIntPtr n_samps, long n_chans, long chan, long format, float mul)
{
	float fract, y0, y1, y2, y3, c0, c1, c2, c3;
	AH_SIntPtr offset, i;
	
	for (i = 0; i < n_samps; i++)
	{
		offset = offsets[i];
		fract = fracts[i];
		
		y0 = ibuffer_float_get_samp (samps, offset - 1, n_chans, chan, format);
		y1 = ibuffer_float_get_samp (samps, offset, n_chans, chan, format);
		y2 = ibuffer_float_get_samp (samps, offset + 1, n_chans, chan, format);
		y3 = ibuffer_float_get_samp (samps, offset + 2, n_chans, chan, format);
		
		c0 = y1;													
		c1 = y2 - 1.f/3.f * y0 - 1.f/2.f * y1 - 1.f/6.f * y3;	
		c2 = 1.f/2.f * (y0 + y2) - y1;							
		c3 = 1.f/6.f * (y3 - y0) + 1.f/2.f * (y1 - y2);	
		
		*out++ = mul * (((c3 * fract + c2) * fract + c1) * fract + c0);
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// Get samples with no interpolation (64 bit) //////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifdef VECTOR_F64_128BIT

void ibuffer_double_samps_simd_nointerp (void *samps, vDouble *out, AH_SIntPtr *offsets, AH_SIntPtr n_samps, long n_chans, long chan, long format, double mul)
{
	vDouble scale = double2vector ((format == PCM_FLOAT) ? mul : (mul * TWO_POW_31_RECIP));
	vFloat *float_vec = (vFloat *) ((double *) out + (((n_samps + 3) >> 2) << 1));
	vFloat float_temp;
	vSInt32 *int_vec = (vSInt32 *) float_vec;
	vSInt32 int_temp;
	AH_SIntPtr i;	
	
	ibuffer_fetch_samps ((void *) float_vec, samps, offsets, n_samps, n_chans, chan, format);
	
	if (format == PCM_FLOAT)
	{
		for (i = 0; i < ((n_samps + 3) >> 2); i++)
		{
			float_temp = *float_vec++;
			*out++ = F64_VEC_MUL_OP (scale, F64_VEC_FROM_F32(float_temp));
			*out++ = F64_VEC_MUL_OP (scale, F64_VEC_FROM_F32(F32_VEC_MOVE_HI(float_temp, float_temp)));
		}
	}
	else 
	{
		for (i = 0; i < ((n_samps + 3) >> 2); i++)
		{
			int_temp = *int_vec++;
			*out++ = F64_VEC_MUL_OP(scale, F64_VEC_FROM_I32(int_temp));
			*out++ = F64_VEC_MUL_OP(scale, F64_VEC_FROM_I32(I32_VEC_SHUFFLE_OP(int_temp, 0xE)));
		}
	}
}

#endif


void ibuffer_double_samps_scalar_nointerp (void *samps, double *out, AH_SIntPtr *offsets, AH_SIntPtr n_samps, long n_chans, long chan, long format, double mul)
{
	AH_SIntPtr i;	
	
	for (i = 0; i < n_samps; i++)
		*out++ = mul * ibuffer_double_get_samp (samps, offsets[i], n_chans, chan, format);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// Get samples with linear interpolation (64 bit) ////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifdef VECTOR_F64_128BIT

void ibuffer_lin_interp_double (vDouble *out, void **inbuffers, vDouble *fracts, AH_SIntPtr n_samps_over_4, double mul)
{		
    vFloat *in1 = (vFloat *) inbuffers[0];
	vFloat *in2 = (vFloat *) inbuffers[1];
	
	vFloat ya, yb;
	
	vDouble scale = double2vector (mul);
	vDouble y0, y1;	
		
	while (n_samps_over_4--)
	{
		ya = *in1++;
		yb = *in2++;
		
		y0 = F64_VEC_FROM_F32(ya);
		y1 = F64_VEC_FROM_F32(yb); 
		
		*out++ = F64_VEC_MUL_OP (scale, F64_VEC_ADD_OP (y0, F64_VEC_MUL_OP (*fracts++, F64_VEC_SUB_OP (y1, y0))));  
		
		y0 = F64_VEC_FROM_F32(F32_VEC_MOVE_HI(ya, ya));
		y1 = F64_VEC_FROM_F32(F32_VEC_MOVE_HI(yb, yb)); 
		
		*out++ = F64_VEC_MUL_OP (scale, F64_VEC_ADD_OP (y0, F64_VEC_MUL_OP (*fracts++, F64_VEC_SUB_OP (y1, y0))));  
	}
}

void ibuffer_lin_interp_int_to_double (vDouble *out, void **inbuffers, vDouble *fracts, AH_SIntPtr n_samps_over_4, double mul)
{		
    vSInt32 *in1 = (vSInt32 *) inbuffers[0];
	vSInt32 *in2 = (vSInt32 *) inbuffers[1];
	
	vSInt32 ya, yb;
	
	vDouble scale = double2vector (mul * TWO_POW_31_RECIP);
	vDouble y0, y1;	
	
	while (n_samps_over_4--)
	{
		ya = *in1++; 
		yb = *in2++;
		
		y0 = F64_VEC_FROM_I32(ya);
		y1 = F64_VEC_FROM_I32(yb);
			
		*out++ = F64_VEC_MUL_OP (scale, F64_VEC_ADD_OP (y0, F64_VEC_MUL_OP (*fracts++, F64_VEC_SUB_OP (y1, y0))));  
		
		y0 = F64_VEC_FROM_I32(I32_VEC_SHUFFLE_OP(ya, 0xE));
		y1 = F64_VEC_FROM_I32(I32_VEC_SHUFFLE_OP(yb, 0xE)); 
		
		*out++ = F64_VEC_MUL_OP (scale, F64_VEC_ADD_OP (y0, F64_VEC_MUL_OP (*fracts++, F64_VEC_SUB_OP (y1, y0))));  
	}
}

void ibuffer_double_samps_simd_linear (void *samps, vDouble *out, AH_SIntPtr *offsets, vDouble *fracts, void **temp, AH_SIntPtr n_samps, long n_chans, long chan, long format, double mul)
{
	ibuffer_fetch_samps_2 (temp, samps, offsets, n_samps, n_chans, chan, format);
	
	if (format == PCM_FLOAT)
		ibuffer_lin_interp_double (out, temp, fracts, (n_samps + 3) >> 2, mul);
	else 
		ibuffer_lin_interp_int_to_double (out, temp, fracts, (n_samps + 3) >> 2, mul);
}

#endif

void ibuffer_double_samps_scalar_linear (void *samps, double *out, AH_SIntPtr *offsets, double *fracts, AH_SIntPtr n_samps, long n_chans, long chan, long format, double mul)
{
	double fract, y0, y1;
	AH_SIntPtr offset, i;
	
	for (i = 0; i < n_samps; i++)
	{
		offset = offsets[i];
		fract = fracts[i];
		
		y0 = ibuffer_double_get_samp (samps, offset, n_chans, chan, format);
		y1 = ibuffer_double_get_samp (samps, offset + 1, n_chans, chan, format);
		
		*out++ = mul * (y0 + (fract * (y1 - y0)));  
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// Get samples with cubic bspline interpolation (64 bit) /////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifdef VECTOR_F64_128BIT

void ibuffer_cubic_bspline_double (vDouble *out, void **inbuffers, vDouble *fracts, AH_SIntPtr n_samps_over_4, double mul)
{		
    vDouble *in1 = fracts;
    vFloat *in2 = (vFloat *) inbuffers[0];
	vFloat *in3 = (vFloat *) inbuffers[1];
	vFloat *in4 = (vFloat *) inbuffers[2];
	vFloat *in5 = (vFloat *) inbuffers[3];
	
	vFloat ya, yb, yc, yd;
	
	vDouble c0, c1, c2, c3, y0py2, y0, y1, y2, y3, xval;
	
	vDouble Half = {0.5, 0.5};
	vDouble Sixth = {1.0/6.0, 1.0/6.0};
	vDouble TwoThirds = {2.0/3.0, 2.0/3.0};
	
	vDouble scale = double2vector(mul);
	
	while (n_samps_over_4--)
	{
		ya = *in2++;
		yb = *in3++; 
		yc = *in4++; 
		yd = *in5++;
		
		y0 = F64_VEC_FROM_F32(ya);
		y1 = F64_VEC_FROM_F32(yb); 
		y2 = F64_VEC_FROM_F32(yc); 
		y3 = F64_VEC_FROM_F32(yd);
		
		xval = *in1++;
		y0py2 = F64_VEC_ADD_OP (y0 , y2);								
		c0 = F64_VEC_ADD_OP(F64_VEC_MUL_OP (Sixth, y0py2), F64_VEC_MUL_OP (TwoThirds, y1));					
		c1 = F64_VEC_MUL_OP (Half, F64_VEC_SUB_OP (y2, y0));							
		c2 = F64_VEC_SUB_OP (F64_VEC_MUL_OP (Half, y0py2), y1);								
		c3 = F64_VEC_ADD_OP (F64_VEC_MUL_OP (Half, F64_VEC_SUB_OP (y1, y2)), F64_VEC_MUL_OP (Sixth, F64_VEC_SUB_OP (y3, y0)));	
		
		*out++ = F64_VEC_MUL_OP (scale, F64_VEC_ADD_OP (c0, F64_VEC_MUL_OP (xval, F64_VEC_ADD_OP (c1, F64_VEC_MUL_OP (xval, F64_VEC_ADD_OP (c2, F64_VEC_MUL_OP (xval, c3))))))); 
		
		y0 = F64_VEC_FROM_F32(F32_VEC_MOVE_HI(ya, ya));
		y1 = F64_VEC_FROM_F32(F32_VEC_MOVE_HI(yb, yb)); 
		y2 = F64_VEC_FROM_F32(F32_VEC_MOVE_HI(yc, yc));
		y3 = F64_VEC_FROM_F32(F32_VEC_MOVE_HI(yd, yd));
		
		xval = *in1++;
		y0py2 = F64_VEC_ADD_OP (y0 , y2);								
		c0 = F64_VEC_ADD_OP(F64_VEC_MUL_OP (Sixth, y0py2), F64_VEC_MUL_OP (TwoThirds, y1));					
		c1 = F64_VEC_MUL_OP (Half, F64_VEC_SUB_OP (y2, y0));							
		c2 = F64_VEC_SUB_OP (F64_VEC_MUL_OP (Half, y0py2), y1);								
		c3 = F64_VEC_ADD_OP (F64_VEC_MUL_OP (Half, F64_VEC_SUB_OP (y1, y2)), F64_VEC_MUL_OP (Sixth, F64_VEC_SUB_OP (y3, y0)));	
		
		*out++ = F64_VEC_MUL_OP (scale, F64_VEC_ADD_OP (c0, F64_VEC_MUL_OP (xval, F64_VEC_ADD_OP (c1, F64_VEC_MUL_OP (xval, F64_VEC_ADD_OP (c2, F64_VEC_MUL_OP (xval, c3))))))); 
	}
}

void ibuffer_cubic_bspline_int_to_double (vDouble *out, void **inbuffers, vDouble *fracts, AH_SIntPtr n_samps_over_4, double mul)
{		
    vDouble *in1 = fracts;
	vSInt32 *in2 = (vSInt32 *) inbuffers[0];
	vSInt32 *in3 = (vSInt32 *) inbuffers[1];
	vSInt32 *in4 = (vSInt32 *) inbuffers[2];
	vSInt32 *in5 = (vSInt32 *) inbuffers[3];
	
	vSInt32 ya, yb, yc, yd;
	
	vDouble c0, c1, c2, c3, y0py2, y0, y1, y2, y3, xval;
	
	vDouble Half = {0.5, 0.5};
	vDouble Sixth = {1.0/6.0, 1.0/6.0};
	vDouble TwoThirds = {2.0/3.0, 2.0/3.0};	
	
	vDouble scale = double2vector (mul * TWO_POW_31_RECIP);
	
	while (n_samps_over_4--)
	{
		ya = *in2++;
		yb = *in3++; 
		yc = *in4++; 
		yd = *in5++;
		
		y0 = F64_VEC_FROM_I32(ya);
		y1 = F64_VEC_FROM_I32(yb);
		y2 = F64_VEC_FROM_I32(yc);
		y3 = F64_VEC_FROM_I32(yd);
		
		xval = *in1++;
		y0py2 = F64_VEC_ADD_OP (y0 , y2);								
		c0 = F64_VEC_ADD_OP(F64_VEC_MUL_OP (Sixth, y0py2), F64_VEC_MUL_OP (TwoThirds, y1));					
		c1 = F64_VEC_MUL_OP (Half, F64_VEC_SUB_OP (y2, y0));							
		c2 = F64_VEC_SUB_OP (F64_VEC_MUL_OP (Half, y0py2), y1);								
		c3 = F64_VEC_ADD_OP (F64_VEC_MUL_OP (Half, F64_VEC_SUB_OP (y1, y2)), F64_VEC_MUL_OP (Sixth, F64_VEC_SUB_OP (y3, y0)));	
		
		*out++ = F64_VEC_MUL_OP (scale, F64_VEC_ADD_OP (c0, F64_VEC_MUL_OP (xval, F64_VEC_ADD_OP (c1, F64_VEC_MUL_OP (xval, F64_VEC_ADD_OP (c2, F64_VEC_MUL_OP (xval, c3))))))); 
		
		y0 = F64_VEC_FROM_I32(I32_VEC_SHUFFLE_OP(ya, 0xE));
		y1 = F64_VEC_FROM_I32(I32_VEC_SHUFFLE_OP(yb, 0xE)); 
		y2 = F64_VEC_FROM_I32(I32_VEC_SHUFFLE_OP(yc, 0xE));
		y3 = F64_VEC_FROM_I32(I32_VEC_SHUFFLE_OP(yd, 0xE));
		
		xval = *in1++;
		y0py2 = F64_VEC_ADD_OP (y0 , y2);								
		c0 = F64_VEC_ADD_OP(F64_VEC_MUL_OP (Sixth, y0py2), F64_VEC_MUL_OP (TwoThirds, y1));					
		c1 = F64_VEC_MUL_OP (Half, F64_VEC_SUB_OP (y2, y0));							
		c2 = F64_VEC_SUB_OP (F64_VEC_MUL_OP (Half, y0py2), y1);								
		c3 = F64_VEC_ADD_OP (F64_VEC_MUL_OP (Half, F64_VEC_SUB_OP (y1, y2)), F64_VEC_MUL_OP (Sixth, F64_VEC_SUB_OP (y3, y0)));	
		
		*out++ = F64_VEC_MUL_OP (scale, F64_VEC_ADD_OP (c0, F64_VEC_MUL_OP (xval, F64_VEC_ADD_OP (c1, F64_VEC_MUL_OP (xval, F64_VEC_ADD_OP (c2, F64_VEC_MUL_OP (xval, c3))))))); 
	}
}

void ibuffer_double_samps_simd_cubic_bspline (void *samps, vDouble *out, AH_SIntPtr *offsets, vDouble *fracts, void **temp, AH_SIntPtr n_samps, long n_chans, long chan, long format, double mul)
{
	ibuffer_fetch_samps_4 (temp, samps, offsets, n_samps, n_chans, chan, format);
	
	if (format == PCM_FLOAT)
		ibuffer_cubic_bspline_double (out, temp, fracts, (n_samps + 3) >> 2, mul);
	else 
		ibuffer_cubic_bspline_int_to_double (out, temp, fracts, (n_samps + 3) >> 2, mul);
}

#endif

void ibuffer_double_samps_scalar_cubic_bspline (void *samps, double *out, AH_SIntPtr *offsets, double *fracts, AH_SIntPtr n_samps, long n_chans, long chan, long format, double mul)
{
	double fract, y0, y1, y2, y3, y0py2, c0, c1, c2, c3;
	AH_SIntPtr offset, i;
	
	for (i = 0; i < n_samps; i++)
	{
		offset = offsets[i];
		fract = fracts[i];
		
		y0 = ibuffer_double_get_samp (samps, offset - 1, n_chans, chan, format);
		y1 = ibuffer_double_get_samp (samps, offset, n_chans, chan, format);
		y2 = ibuffer_double_get_samp (samps, offset + 1, n_chans, chan, format);
		y3 = ibuffer_double_get_samp (samps, offset + 2, n_chans, chan, format);
		
		y0py2 = y0 + y2;
		c0 = 1.0/6.0 * y0py2 + 2.0/3.0 * y1;
		c1 = 1.0/2.0 * (y2 - y0);
		c2 = 1.0/2.0 * y0py2 - y1;
		c3 = 1.0/2.0 * (y1 - y2) + 1.0/6.0 * (y3 - y0);
		
		*out++ = mul * (((c3 * fract + c2) * fract + c1) * fract + c0); 
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// Get samples with cubic hermite interpolation (64 bit) /////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifdef VECTOR_F64_128BIT

void ibuffer_cubic_hermite_double (vDouble *out, void **inbuffers, vDouble *fracts, AH_SIntPtr n_samps_over_4, double mul)
{		
    vDouble *in1 = fracts;
	vFloat *in2 = (vFloat *) inbuffers[0];
	vFloat *in3 = (vFloat *) inbuffers[1];
	vFloat *in4 = (vFloat *) inbuffers[2];
	vFloat *in5 = (vFloat *) inbuffers[3];
	
	vFloat ya, yb, yc, yd;
	
	vDouble c1, c2, c3, y0, y1, y2, y3, xval;
	
	vDouble Half = {0.5, 0.5};
	vDouble OneAndHalf = {1.5, 1.5};
	vDouble TwoAndHalf = {2.5, 2.5};
	
	vDouble scale = double2vector (mul);
		
	while (n_samps_over_4--)
	{
		ya = *in2++;
		yb = *in3++; 
		yc = *in4++; 
		yd = *in5++;
		
		y0 = F64_VEC_FROM_F32(ya);
		y1 = F64_VEC_FROM_F32(yb); 
		y2 = F64_VEC_FROM_F32(yc);
		y3 = F64_VEC_FROM_F32(yd);
		
		xval = *in1++;
		c1 = F64_VEC_MUL_OP (Half, F64_VEC_SUB_OP (y2, y0));
		c2 = F64_VEC_ADD_OP (F64_VEC_SUB_OP (y0, F64_VEC_MUL_OP ( TwoAndHalf, y1)), F64_VEC_SUB_OP (F64_VEC_ADD_OP (y2, y2), F64_VEC_MUL_OP (Half, y3)));     		
		c3 = F64_VEC_ADD_OP (F64_VEC_MUL_OP (Half, F64_VEC_SUB_OP (y3, y0)), F64_VEC_MUL_OP (OneAndHalf, F64_VEC_SUB_OP (y1, y2)));
		
		*out++ = F64_VEC_MUL_OP (scale, F64_VEC_ADD_OP (y1, F64_VEC_MUL_OP (xval, F64_VEC_ADD_OP (c1, F64_VEC_MUL_OP (xval, F64_VEC_ADD_OP (c2, F64_VEC_MUL_OP (xval, c3))))))); 
		
		y0 = F64_VEC_FROM_F32(F32_VEC_MOVE_HI(ya, ya));
		y1 = F64_VEC_FROM_F32(F32_VEC_MOVE_HI(yb, yb)); 
		y2 = F64_VEC_FROM_F32(F32_VEC_MOVE_HI(yc, yc));
		y3 = F64_VEC_FROM_F32(F32_VEC_MOVE_HI(yd, yd));
		
		xval = *in1++;
		c1 = F64_VEC_MUL_OP (Half, F64_VEC_SUB_OP (y2, y0));
		c2 = F64_VEC_ADD_OP (F64_VEC_SUB_OP (y0, F64_VEC_MUL_OP ( TwoAndHalf, y1)), F64_VEC_SUB_OP (F64_VEC_ADD_OP (y2, y2), F64_VEC_MUL_OP (Half, y3)));     		
		c3 = F64_VEC_ADD_OP (F64_VEC_MUL_OP (Half, F64_VEC_SUB_OP (y3, y0)), F64_VEC_MUL_OP (OneAndHalf, F64_VEC_SUB_OP (y1, y2)));
		
		*out++ = F64_VEC_MUL_OP (scale, F64_VEC_ADD_OP (y1, F64_VEC_MUL_OP (xval, F64_VEC_ADD_OP (c1, F64_VEC_MUL_OP (xval, F64_VEC_ADD_OP (c2, F64_VEC_MUL_OP (xval, c3))))))); 
	}
}

void ibuffer_cubic_hermite_int_to_double (vDouble *out, void **inbuffers, vDouble *fracts, AH_SIntPtr n_samps_over_4, double mul)
{		
    vDouble *in1 = fracts;
    vSInt32 *in2 = (vSInt32 *) inbuffers[0];
	vSInt32 *in3 = (vSInt32 *) inbuffers[1];
	vSInt32 *in4 = (vSInt32 *) inbuffers[2];
	vSInt32 *in5 = (vSInt32 *) inbuffers[3];
	
	vSInt32 ya, yb, yc, yd;
	
	vDouble c1, c2, c3, y0, y1, y2, y3, xval;
	
	vDouble Half = {0.5, 0.5};
	vDouble OneAndHalf = {1.5, 1.5};
	vDouble TwoAndHalf = {2.5, 2.5};
	
	vDouble scale = double2vector (mul * TWO_POW_31_RECIP);
	
	while (n_samps_over_4--)
	{
		ya = *in2++;
		yb = *in3++; 
		yc = *in4++; 
		yd = *in5++;
		
		y0 = F64_VEC_FROM_I32(ya);
		y1 = F64_VEC_FROM_I32(yb);
		y2 = F64_VEC_FROM_I32(yc);
		y3 = F64_VEC_FROM_I32(yd);
		
		xval = *in1++;
		c1 = F64_VEC_MUL_OP (Half, F64_VEC_SUB_OP (y2, y0));
		c2 = F64_VEC_ADD_OP (F64_VEC_SUB_OP (y0, F64_VEC_MUL_OP ( TwoAndHalf, y1)), F64_VEC_SUB_OP (F64_VEC_ADD_OP (y2, y2), F64_VEC_MUL_OP (Half, y3)));     		
		c3 = F64_VEC_ADD_OP (F64_VEC_MUL_OP (Half, F64_VEC_SUB_OP (y3, y0)), F64_VEC_MUL_OP (OneAndHalf, F64_VEC_SUB_OP (y1, y2)));
		
		*out++ = F64_VEC_MUL_OP (scale, F64_VEC_ADD_OP (y1, F64_VEC_MUL_OP (xval, F64_VEC_ADD_OP (c1, F64_VEC_MUL_OP (xval, F64_VEC_ADD_OP (c2, F64_VEC_MUL_OP (xval, c3))))))); 
		
		y0 = F64_VEC_FROM_I32(I32_VEC_SHUFFLE_OP(ya, 0xE));
		y1 = F64_VEC_FROM_I32(I32_VEC_SHUFFLE_OP(yb, 0xE)); 
		y2 = F64_VEC_FROM_I32(I32_VEC_SHUFFLE_OP(yc, 0xE));
		y3 = F64_VEC_FROM_I32(I32_VEC_SHUFFLE_OP(yd, 0xE));
		
		xval = *in1++;
		c1 = F64_VEC_MUL_OP (Half, F64_VEC_SUB_OP (y2, y0));
		c2 = F64_VEC_ADD_OP (F64_VEC_SUB_OP (y0, F64_VEC_MUL_OP ( TwoAndHalf, y1)), F64_VEC_SUB_OP (F64_VEC_ADD_OP (y2, y2), F64_VEC_MUL_OP (Half, y3)));     		
		c3 = F64_VEC_ADD_OP (F64_VEC_MUL_OP (Half, F64_VEC_SUB_OP (y3, y0)), F64_VEC_MUL_OP (OneAndHalf, F64_VEC_SUB_OP (y1, y2)));
		
		*out++ = F64_VEC_MUL_OP (scale, F64_VEC_ADD_OP (y1, F64_VEC_MUL_OP (xval, F64_VEC_ADD_OP (c1, F64_VEC_MUL_OP (xval, F64_VEC_ADD_OP (c2, F64_VEC_MUL_OP (xval, c3))))))); 
	}
}

void ibuffer_double_samps_simd_cubic_hermite (void *samps, vDouble *out, AH_SIntPtr *offsets, vDouble *fracts, void **temp, AH_SIntPtr n_samps, long n_chans, long chan, long format, double mul)
{
	ibuffer_fetch_samps_4 ((void **)temp, samps, offsets, n_samps, n_chans, chan, format);
	
	if (format == PCM_FLOAT)
		ibuffer_cubic_hermite_double (out, temp, fracts, (n_samps + 3) >> 2, mul);
	else 
		ibuffer_cubic_hermite_int_to_double (out, temp, fracts, (n_samps + 3) >> 2, mul);
}

#endif

void ibuffer_double_samps_scalar_cubic_hermite (void *samps, double *out, AH_SIntPtr *offsets, double *fracts, AH_SIntPtr n_samps, long n_chans, long chan, long format, double mul)
{
	double fract, y0, y1, y2, y3, c0, c1, c2, c3;
	AH_SIntPtr offset, i;
	
	for (i = 0; i < n_samps; i++)
	{
		offset = offsets[i];
		fract = fracts[i];
		
		y0 = ibuffer_double_get_samp (samps, offset - 1, n_chans, chan, format);
		y1 = ibuffer_double_get_samp (samps, offset, n_chans, chan, format);
		y2 = ibuffer_double_get_samp (samps, offset + 1, n_chans, chan, format);
		y3 = ibuffer_double_get_samp (samps, offset + 2, n_chans, chan, format);
		
		c0 = y1;												
		c1 = 0.5 * (y2 - y0);								
		c2 = y0 - 2.5 * y1 + y2 + y2 - 0.5 * y3;	
		c3 = 0.5 * (y3 - y0) + 1.5 * (y1 - y2);	
		
		*out++ = mul * (((c3 * fract + c2) * fract + c1) * fract + c0);
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// Get samples with cubic lagrange interpolation (64 bit) ////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifdef VECTOR_F64_128BIT

void ibuffer_cubic_lagrange_double (vDouble *out, void **inbuffers, vDouble *fracts, AH_SIntPtr n_samps_over_4, double mul)
{		
	vDouble *in1 = fracts;
	vFloat *in2 = (vFloat *) inbuffers[0];
	vFloat *in3 = (vFloat *) inbuffers[1];
	vFloat *in4 = (vFloat *) inbuffers[2];
	vFloat *in5 = (vFloat *) inbuffers[3];
	
	vFloat ya, yb, yc, yd;
	
	vDouble c1, c2, c3, y0, y1, y2, y3, xval;
	
	vDouble Half = {0.5, 0.5};
	vDouble Third = {1.0/3.0, 1.0/3.0};
	vDouble Sixth = {1.0/6.0, 1.0/6.0};
	
	vDouble scale = double2vector (mul);
		
	while (n_samps_over_4--)
	{
		ya = *in2++;
		yb = *in3++; 
		yc = *in4++; 
		yd = *in5++;
		
		y0 = F64_VEC_FROM_F32(ya);
		y1 = F64_VEC_FROM_F32(yb); 
		y2 = F64_VEC_FROM_F32(yc);
		y3 = F64_VEC_FROM_F32(yd);
		
		xval = *in1++;
		c1 = F64_VEC_SUB_OP (F64_VEC_SUB_OP (y2, F64_VEC_MUL_OP (Third, y0)) , F64_VEC_ADD_OP (F64_VEC_MUL_OP (Half, y1), F64_VEC_MUL_OP (Sixth, y3)));
		c2 = F64_VEC_SUB_OP (F64_VEC_MUL_OP (Half, F64_VEC_ADD_OP (y0, y2)), y1);     		
		c3 = F64_VEC_ADD_OP (F64_VEC_MUL_OP (Sixth, F64_VEC_SUB_OP (y3, y0)), F64_VEC_MUL_OP (Half, F64_VEC_SUB_OP (y1, y2)));
		
		*out++ = F64_VEC_MUL_OP (scale, F64_VEC_ADD_OP (y1, F64_VEC_MUL_OP (xval, F64_VEC_ADD_OP (c1, F64_VEC_MUL_OP (xval, F64_VEC_ADD_OP (c2, F64_VEC_MUL_OP (xval, c3))))))); 
		
		y0 = F64_VEC_FROM_F32(F32_VEC_MOVE_HI(ya, ya));
		y1 = F64_VEC_FROM_F32(F32_VEC_MOVE_HI(yb, yb)); 
		y2 = F64_VEC_FROM_F32(F32_VEC_MOVE_HI(yc, yc));
		y3 = F64_VEC_FROM_F32(F32_VEC_MOVE_HI(yd, yd));

		xval = *in1++;
		c1 = F64_VEC_SUB_OP (F64_VEC_SUB_OP (y2, F64_VEC_MUL_OP (Third, y0)) , F64_VEC_ADD_OP (F64_VEC_MUL_OP (Half, y1), F64_VEC_MUL_OP (Sixth, y3)));
		c2 = F64_VEC_SUB_OP (F64_VEC_MUL_OP (Half, F64_VEC_ADD_OP (y0, y2)), y1);     		
		c3 = F64_VEC_ADD_OP (F64_VEC_MUL_OP (Sixth, F64_VEC_SUB_OP (y3, y0)), F64_VEC_MUL_OP (Half, F64_VEC_SUB_OP (y1, y2)));
		
		*out++ = F64_VEC_MUL_OP (scale, F64_VEC_ADD_OP (y1, F64_VEC_MUL_OP (xval, F64_VEC_ADD_OP (c1, F64_VEC_MUL_OP (xval, F64_VEC_ADD_OP (c2, F64_VEC_MUL_OP (xval, c3))))))); 
	}
}

void ibuffer_cubic_lagrange_int_to_double (vDouble *out, void **inbuffers, vDouble *fracts, AH_SIntPtr n_samps_over_4, double mul)
{		
	vDouble *in1 = fracts;
	vSInt32 *in2 = (vSInt32 *) inbuffers[0];
	vSInt32 *in3 = (vSInt32 *) inbuffers[1];
	vSInt32 *in4 = (vSInt32 *) inbuffers[2];
	vSInt32 *in5 = (vSInt32 *) inbuffers[3];
	
	vSInt32 ya, yb, yc, yd;
	
	vDouble c1, c2, c3, y0, y1, y2, y3, xval;
	
	vDouble Half = {0.5, 0.5};
	vDouble Third = {1.0/3.0, 1.0/3.0};
	vDouble Sixth = {1.0/6.0, 1.0/6.0};
	
	vDouble scale = double2vector (mul * TWO_POW_31_RECIP);
	
	while (n_samps_over_4--)
	{
		ya = *in2++;
		yb = *in3++; 
		yc = *in4++; 
		yd = *in5++;
		
		y0 = F64_VEC_FROM_I32(ya);
		y1 = F64_VEC_FROM_I32(yb);
		y2 = F64_VEC_FROM_I32(yc);
		y3 = F64_VEC_FROM_I32(yd);
		
		xval = *in1++;
		c1 = F64_VEC_SUB_OP (F64_VEC_SUB_OP (y2, F64_VEC_MUL_OP (Third, y0)) , F64_VEC_ADD_OP (F64_VEC_MUL_OP (Half, y1), F64_VEC_MUL_OP (Sixth, y3)));
		c2 = F64_VEC_SUB_OP (F64_VEC_MUL_OP (Half, F64_VEC_ADD_OP (y0, y2)), y1);     		
		c3 = F64_VEC_ADD_OP (F64_VEC_MUL_OP (Sixth, F64_VEC_SUB_OP (y3, y0)), F64_VEC_MUL_OP (Half, F64_VEC_SUB_OP (y1, y2)));
		
		*out++ = F64_VEC_MUL_OP (scale, F64_VEC_ADD_OP (y1, F64_VEC_MUL_OP (xval, F64_VEC_ADD_OP (c1, F64_VEC_MUL_OP (xval, F64_VEC_ADD_OP (c2, F64_VEC_MUL_OP (xval, c3))))))); 
		
		y0 = F64_VEC_FROM_I32(I32_VEC_SHUFFLE_OP(ya, 0xE));
		y1 = F64_VEC_FROM_I32(I32_VEC_SHUFFLE_OP(yb, 0xE)); 
		y2 = F64_VEC_FROM_I32(I32_VEC_SHUFFLE_OP(yc, 0xE));
		y3 = F64_VEC_FROM_I32(I32_VEC_SHUFFLE_OP(yd, 0xE));
		
		xval = *in1++;
		c1 = F64_VEC_SUB_OP (F64_VEC_SUB_OP (y2, F64_VEC_MUL_OP (Third, y0)) , F64_VEC_ADD_OP (F64_VEC_MUL_OP (Half, y1), F64_VEC_MUL_OP (Sixth, y3)));
		c2 = F64_VEC_SUB_OP (F64_VEC_MUL_OP (Half, F64_VEC_ADD_OP (y0, y2)), y1);     		
		c3 = F64_VEC_ADD_OP (F64_VEC_MUL_OP (Sixth, F64_VEC_SUB_OP (y3, y0)), F64_VEC_MUL_OP (Half, F64_VEC_SUB_OP (y1, y2)));
		
		*out++ = F64_VEC_MUL_OP (scale, F64_VEC_ADD_OP (y1, F64_VEC_MUL_OP (xval, F64_VEC_ADD_OP (c1, F64_VEC_MUL_OP (xval, F64_VEC_ADD_OP (c2, F64_VEC_MUL_OP (xval, c3))))))); 
	}
}

void ibuffer_double_samps_simd_cubic_lagrange (void *samps, vDouble *out, AH_SIntPtr *offsets, vDouble *fracts, void **temp, AH_SIntPtr n_samps, long n_chans, long chan, long format, double mul)
{
	ibuffer_fetch_samps_4 ((void **)temp, samps, offsets, n_samps, n_chans, chan, format);

	if (format == PCM_FLOAT)
		ibuffer_cubic_lagrange_double (out, temp, fracts, (n_samps + 3) >> 2, mul);
	else 
		ibuffer_cubic_lagrange_int_to_double (out, temp, fracts, (n_samps + 3) >> 2, mul);
}

#endif

void ibuffer_double_samps_scalar_cubic_lagrange (void *samps, double *out, AH_SIntPtr *offsets, double *fracts, AH_SIntPtr n_samps, long n_chans, long chan, long format, double mul)
{
	double fract, y0, y1, y2, y3, c0, c1, c2, c3;
	AH_SIntPtr offset, i;
	
	for (i = 0; i < n_samps; i++)
	{
		offset = offsets[i];
		fract = fracts[i];
		
		y0 = ibuffer_double_get_samp (samps, offset - 1, n_chans, chan, format);
		y1 = ibuffer_double_get_samp (samps, offset, n_chans, chan, format);
		y2 = ibuffer_double_get_samp (samps, offset + 1, n_chans, chan, format);
		y3 = ibuffer_double_get_samp (samps, offset + 2, n_chans, chan, format);
		
		c0 = y1;													
		c1 = y2 - 1.0/3.0 * y0 - 1.0/2.f * y1 - 1.0/6.0 * y3;	
		c2 = 1.0/2.0 * (y0 + y2) - y1;							
		c3 = 1.0/6.0 * (y3 - y0) + 1.0/2.0 * (y1 - y2);	
		
		*out++ = mul * (((c3 * fract + c2) * fract + c1) * fract + c0);
	}
}

