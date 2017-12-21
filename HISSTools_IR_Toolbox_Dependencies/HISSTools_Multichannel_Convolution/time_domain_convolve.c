
/*
 *  time_domain_convolve
 *
 *	time_domain_convolve performs real-time zero latency time-based convolution.
 *	
 *	Typically time_domain_convolve is suitable for use in conjunction with partition_convolve for zero-latency convolution with longer impulses (time_domain_convolve use apple's vDSP and the IR length is limited to 2044 samples).
 *	Note that in fact the algorithms process correlation with reversed impulse response coeffients - which is equivalent to convolution.
 *
 *  Copyright 2012 Alex Harker. All rights reserved.
 *
 */


#include "time_domain_convolve.h"

#ifdef __APPLE__
#include <Accelerate/Accelerate.h>
#endif

#include <AH_VectorOps.h>
#include <stdio.h>
#include <string.h>

AH_UIntPtr pad_length(AH_UIntPtr length)
{
	return ((length + 15) >> 4) << 4;
}


void time_domain_convolve_free(t_time_domain_convolve *x)
{
	if (!x)
		return;
	
	ALIGNED_FREE (x->impulse_buffer);
	ALIGNED_FREE (x->input_buffer);
	free(x);
}


t_time_domain_convolve *time_domain_convolve_new(AH_UIntPtr offset, AH_UIntPtr length)
{
	t_time_domain_convolve *x = malloc(sizeof(t_time_domain_convolve));
	AH_UIntPtr i;

	if (!x)
		return 0;
	
	// Set default initial variables
	
	x->input_position = 0;
	x->impulse_length = 0;
	
	time_domain_convolve_offset_set(x, offset);
	time_domain_convolve_length_set(x, length);
	
	// Allocate impulse buffer and input bufferr
	
	x->impulse_buffer = ALIGNED_MALLOC(sizeof(float) * 2048);
	x->input_buffer = ALIGNED_MALLOC(sizeof(float) *  8192);
	
	if (!x->impulse_buffer || !x->input_buffer)
	{
		time_domain_convolve_free(x);
		return 0;
	}
	
	for (i = 0; i < 2048; i++)
		x->impulse_buffer[i] = 0.f;

	for (i = 0; i < 8192; i++)
		x->input_buffer[i] = 0.f;

	
	return (x);
}


void time_domain_convolve_offset_set(t_time_domain_convolve *x, AH_UIntPtr offset)
{
	x->offset = offset;
}


t_convolve_error time_domain_convolve_length_set(t_time_domain_convolve *x, AH_UIntPtr length)
{	
	t_convolve_error error = CONVOLVE_ERR_NONE;

	if (length > 2044)
	{
		error = CONVOLVE_ERR_TIME_LENGTH_OUT_OF_RANGE;
		length = 2044;
	}
	
	x->length = length;
	
	return error;
}


t_convolve_error time_domain_convolve_set(t_time_domain_convolve *x, float *input, AH_UIntPtr impulse_length)
{	
	AH_UIntPtr offset = x->offset;
	AH_UIntPtr length = x->length;
	
	float *impulse_buffer = x->impulse_buffer;

	t_convolve_error error = CONVOLVE_ERR_NONE;
	
#ifndef __APPLE__
	AH_UIntPtr impulse_offset;
#endif
	AH_UIntPtr i, j;
	
	x->impulse_length = 0;
	
	// Calculate impulse length
	
	if (!input || impulse_length < offset)
		impulse_length = 0;
	
	impulse_length -= offset;
	if (length && length < impulse_length)
		impulse_length = length;
	
	if (impulse_length > 2044) 
	{
		error = CONVOLVE_ERR_TIME_IMPULSE_TOO_LONG;
		impulse_length = 2044;
	}
		
#ifdef __APPLE__
	if (impulse_length)
	{
		for (i = impulse_length, j = 0; i > 0; i--, j++)
			impulse_buffer[j] = input[i + offset - 1];
	}
#else
	if (impulse_length)
	{
		impulse_offset = pad_length(impulse_length) - impulse_length; 

		for (i = 0; i < impulse_offset; i++)
			impulse_buffer[i] = 0.f;

		for (i = impulse_length, j = 0; i > 0; i--, j++)
			impulse_buffer[j + impulse_offset] = input[i + offset - 1];
	}
#endif
		
	x->impulse_length = impulse_length;
	
	return error;
}


#ifndef __APPLE__

void ah_conv(float *in, vFloat *impulse, float *output, AH_UIntPtr N, AH_UIntPtr L)
{
	vFloat output_accum;
	float *input;
	float results[4];

	AH_UIntPtr i, j;
		
	L = pad_length(L);
				   
	for (i = 0; i < N; i++)
	{
		output_accum = float2vector(0.f);
		input = in - L + 1 + i;
		
		for (j = 0; j < L >> 2; j += 4)
		{
			// Load vals
			
			output_accum = F32_VEC_ADD_OP(output_accum, F32_VEC_MUL_OP(impulse[j], F32_VEC_ULOAD(input)));
			input += 4;
			output_accum = F32_VEC_ADD_OP(output_accum, F32_VEC_MUL_OP(impulse[j + 1], F32_VEC_ULOAD(input)));
			input += 4;
			output_accum = F32_VEC_ADD_OP(output_accum, F32_VEC_MUL_OP(impulse[j + 2], F32_VEC_ULOAD(input)));
			input += 4;
			output_accum = F32_VEC_ADD_OP(output_accum, F32_VEC_MUL_OP(impulse[j + 3], F32_VEC_ULOAD(input)));
			input += 4;
		}
		
		F32_VEC_USTORE(results, output_accum);
		
		*output++ = results[0] + results[1] + results[2] + results[3];
	}
}


void ah_conv_scalar(float *in, float *impulse, float *output, AH_UIntPtr N, AH_UIntPtr L)
{
	float output_accum;
	float *input;
	
	AH_UIntPtr i, j;
	
	L = pad_length(L);
	
	for (i = 0; i < N; i++)
	{
		output_accum = 0.f;
		input = in - L + 1 + i;
		
		for (j = 0; j < L; j += 8)
		{
			// Load vals
			
			output_accum += impulse[j+0] * *input++;
			output_accum += impulse[j+1] * *input++;
			output_accum += impulse[j+2] * *input++;
			output_accum += impulse[j+3] * *input++;
			output_accum += impulse[j+4] * *input++;
			output_accum += impulse[j+5] * *input++;
			output_accum += impulse[j+6] * *input++;
			output_accum += impulse[j+7] * *input++;
		}
		
		*output++ = output_accum;
	}
}


void time_domain_convolve_process_scalar(t_time_domain_convolve *x, float *in, float *out, AH_UIntPtr vec_size)
{
	float *impulse_buffer = x->impulse_buffer;
	float *input_buffer = x->input_buffer;
	AH_UIntPtr input_position = x->input_position;
	AH_UIntPtr impulse_length = x->impulse_length;
	AH_UIntPtr current_loop;

	for (current_loop = vec_size > 4096 ? 4096 : vec_size; current_loop; vec_size -= current_loop, current_loop = vec_size > 4096 ? 4096 : vec_size)  
	{
		// Copy input twice (allows us to read input out in one go)
	
		memcpy(input_buffer + input_position, in, sizeof(float) * vec_size);
		memcpy(input_buffer + 4096 + input_position, in, sizeof(float) * vec_size);
	
		// Advance pointer 
	
		input_position += vec_size;
		if (input_position >= 4096) 
			input_position -= 4096;
		x->input_position = input_position;
	
		// Do convolution
	
		ah_conv_scalar(input_buffer + 4096 + (input_position - vec_size), impulse_buffer, out, vec_size, impulse_length);
	}
}

#endif


void time_domain_convolve_process(t_time_domain_convolve *x, float *in, float *out, AH_UIntPtr vec_size)
{	
	float *impulse_buffer = x->impulse_buffer;
	float *input_buffer = x->input_buffer;
	AH_UIntPtr input_position = x->input_position;
	AH_UIntPtr impulse_length = x->impulse_length;
	AH_UIntPtr current_loop;
	
	for (current_loop = vec_size > 4096 ? 4096 : vec_size; current_loop; vec_size -= current_loop, current_loop = vec_size > 4096 ? 4096 : vec_size)  
	{
		// Copy input twice (allows us to read input out in one go)
		
		memcpy(input_buffer + input_position, in, sizeof(float) * vec_size);
		memcpy(input_buffer + 4096 + input_position, in, sizeof(float) * vec_size);
		
		// Advance pointer 
		
		input_position += vec_size;
		if (input_position >= 4096) 
			input_position -= 4096;
		x->input_position = input_position;
		
		// Do convolution
		
	#ifdef __APPLE__
		vDSP_conv(input_buffer + 4096 + input_position - (impulse_length + vec_size) + 1, (vDSP_Stride) 1, impulse_buffer, (vDSP_Stride) 1, out, (vDSP_Stride) 1, (vDSP_Length) vec_size, impulse_length);
	#else
		ah_conv(input_buffer + 4096 + (input_position - vec_size), (vFloat *) impulse_buffer, out, vec_size, impulse_length);
	#endif
	}
}

