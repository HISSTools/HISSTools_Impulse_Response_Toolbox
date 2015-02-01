
#include "multi_channel_convolve.h"


void multi_channel_convolve_free(t_multi_channel_convolve *x)
{
	AH_UIntPtr i;
	
	if (!x)
		return;
	
	for (i = 0; i < x->num_out_chans; i++)
		output_channel_convolve_free(x->chan_convolvers[i]);
	
	ALIGNED_FREE(x->in_temps[0]);

	free(x);
}


void multi_channel_convolve_temp_setup(t_multi_channel_convolve *x, void *mem_pointer, AH_UIntPtr max_frame_size)
{
	AH_UIntPtr num_in_chans = x->num_in_chans;
	AH_UIntPtr i;
	
	x->in_temps[0] = mem_pointer;
	
	for (i = 1; i < num_in_chans; i++)
		x->in_temps[i] = x->in_temps[0] + (i * (max_frame_size >> 2));
	
	x->temp1 = x->in_temps[num_in_chans - 1] + (max_frame_size >> 2);
	x->temp2 = x->temp1 + (max_frame_size >> 2);
}


t_multi_channel_convolve *multi_channel_convolve_new(AH_UIntPtr num_in_chans, AH_UIntPtr num_out_chans, t_convolve_latency_mode latency_mode, AH_UIntPtr max_length)
{
	t_multi_channel_convolve *x = malloc(sizeof(t_multi_channel_convolve));
	long N2M;
	AH_UIntPtr i;
	
	if (!x)
		return 0;

	num_in_chans = num_in_chans < 1 ? 1 : num_in_chans;
	num_in_chans = num_in_chans > MAX_CHANS ? MAX_CHANS : num_in_chans;
	num_out_chans = num_out_chans > MAX_CHANS ? MAX_CHANS : num_out_chans;

	x->N2M = N2M = num_out_chans > 0;
	x->num_in_chans = 0;
	x->num_out_chans = 0;
	x->in_temps[0] = 0;
	
	num_out_chans = !N2M ? num_in_chans : num_out_chans;
	
	for (i = 0; i < num_out_chans; i++)
	{
		x->chan_convolvers[i] = output_channel_convolve_new(N2M ? num_in_chans : 1, max_length, latency_mode);
		
		if (!x->chan_convolvers[i])
		{
			multi_channel_convolve_free(x);
			return 0;
		}
		
		x->num_out_chans++;
	}
	
	x->num_in_chans = num_in_chans;
	alloc_memory_swap(&x->temporary_memory, (AH_UIntPtr) 0, (AH_UIntPtr) 0);

	return (x);
}

void multi_channel_convolve_clear(t_multi_channel_convolve *x, AH_Boolean resize)
{
	AH_UIntPtr i, j;
	
	if (x->N2M)
	{
		for (i = 0; i < x->num_out_chans; i++)
			for (j = 0; j < x->num_in_chans; j++)
				multi_channel_convolve_set(x, j, i, (AH_UIntPtr) 0, (AH_UIntPtr) 0, resize);
	}
	else 
	{
		for (i = 0; i < x->num_out_chans; i++)
			multi_channel_convolve_set(x, i, i, (AH_UIntPtr) 0, (AH_UIntPtr) 0, resize);
	}
}


t_convolve_error multi_channel_convolve_resize(t_multi_channel_convolve *x, AH_UIntPtr in_chan, AH_UIntPtr out_chan, AH_UIntPtr impulse_length)
{
	// For Parallel operation you must pass the same in/out channel
	
	if (!x->N2M)
		in_chan -= out_chan;
	
	if (out_chan < x->num_out_chans)
		return output_channel_convolve_resize(x->chan_convolvers[out_chan], in_chan, impulse_length);
	else 
		return CONVOLVE_ERR_IN_CHAN_OUT_OF_RANGE;
}


t_convolve_error multi_channel_convolve_set(t_multi_channel_convolve *x, AH_UIntPtr in_chan, AH_UIntPtr out_chan, float *input, AH_UIntPtr impulse_length, AH_Boolean resize)
{
	// For Parallel operation you must pass the same in/out channel
	
	if (!x->N2M)
		in_chan -= out_chan;
	
	if (out_chan < x->num_out_chans)
		return output_channel_convolve_set(x->chan_convolvers[out_chan], in_chan, input, impulse_length, resize);
	else 
		return CONVOLVE_ERR_OUT_CHAN_OUT_OF_RANGE;
}


void multi_channel_convolve_process_float(t_multi_channel_convolve *x, float **ins, float **outs, float *dry_gain, float *wet_gain, AH_UIntPtr vec_size, AH_UIntPtr active_in_chans, AH_UIntPtr active_out_chans)
{
	void *mem_pointer;
	vFloat **in_temps = x->in_temps;
	vFloat *temp1;
	vFloat *temp2;
	AH_UIntPtr num_in_chans = x->num_in_chans;
	AH_UIntPtr num_out_chans = x->num_out_chans;
	long N2M = x->N2M;
	AH_UIntPtr i, j;
	
#if defined( __i386__ ) || defined( __x86_64__ )
	unsigned int oldMXCSR = _mm_getcsr();				// read the old MXCSR setting 
	unsigned int newMXCSR = oldMXCSR | 0x8040;			// set DAZ and FZ bits 
	_mm_setcsr(newMXCSR);								// write the new MXCSR setting to the MXCSR 
#endif
	
	mem_pointer = grow_memory_swap(&x->temporary_memory, (num_in_chans + 2) * vec_size * sizeof(float), vec_size);
	multi_channel_convolve_temp_setup(x, mem_pointer, x->temporary_memory.current_size);
	
	temp1 = x->temp1;
	temp2 = x->temp2;
	
	if (!x->temporary_memory.current_ptr)
		active_in_chans = active_out_chans = 0;
	
	active_in_chans = active_in_chans > num_in_chans ? num_in_chans : active_in_chans;
	active_out_chans = active_out_chans > num_out_chans ? num_out_chans : active_out_chans;
	
	for (i = 0; i < active_in_chans; i++)
	{
		float *current_in = (float *) in_temps[i];
		
		for (j = 0; j < vec_size; j++)
			current_in[j] = ins[i][j];
	}
	
	for (i = 0; i < active_out_chans; i++)
		output_channel_convolve_process_float(x->chan_convolvers[i], N2M ? in_temps : in_temps + i, outs[i], temp1, temp2, vec_size, active_in_chans);
	
	if (wet_gain)
	{
		for (i = 0; i < active_out_chans; i++)
			for (j = 0; j < vec_size; j++)
				outs[i][j] *= wet_gain[j];
	}
	
	if (dry_gain)
	{
		float *current_in;
		
		for (i = 0; i < active_in_chans && i < active_out_chans; i++)
			for (j = 0, current_in  = (float *) in_temps[i]; j < vec_size; j++)
				outs[i][j] += current_in[j] * dry_gain[j];
	}

	unlock_memory_swap(&x->temporary_memory);
	
#if defined( __i386__ ) || defined( __x86_64__ )	
	_mm_setcsr(oldMXCSR);	
#endif
}


void multi_channel_convolve_process_double(t_multi_channel_convolve *x, double **ins, double **outs, double *dry_gain, double *wet_gain, AH_UIntPtr vec_size, AH_UIntPtr active_in_chans, AH_UIntPtr active_out_chans)
{
	void *mem_pointer;
	vFloat **in_temps = x->in_temps;
	vFloat *temp1;
	vFloat *temp2;
	AH_UIntPtr num_in_chans = x->num_in_chans;
	AH_UIntPtr num_out_chans = x->num_out_chans;
	long N2M = x->N2M;
	AH_UIntPtr i, j;
	
#if defined( __i386__ ) || defined( __x86_64__ )
	unsigned int oldMXCSR = _mm_getcsr();				// read the old MXCSR setting 
	unsigned int newMXCSR = oldMXCSR | 0x8040;			// set DAZ and FZ bits 
	_mm_setcsr(newMXCSR);								// write the new MXCSR setting to the MXCSR 
#endif
	
	mem_pointer = grow_memory_swap(&x->temporary_memory, (num_in_chans + 2) * vec_size * sizeof(float), vec_size);
	multi_channel_convolve_temp_setup(x, mem_pointer, x->temporary_memory.current_size);
	
	temp1 = x->temp1;
	temp2 = x->temp2;
	
	if (!x->temporary_memory.current_ptr)
		active_in_chans = active_out_chans = 0;
	
	active_in_chans = active_in_chans > num_in_chans ? num_in_chans : active_in_chans;
	active_out_chans = active_out_chans > num_out_chans ? num_out_chans : active_out_chans;

	for (i = 0; i < active_in_chans; i++)
	{
		float *current_in = (float *) in_temps[i];
		
		for (j = 0; j < vec_size; j++)
			current_in[j] = (float) ins[i][j];
	}
	
	for (i = 0; i < active_out_chans; i++)
		output_channel_convolve_process_double(x->chan_convolvers[i], N2M ? in_temps : in_temps + i, outs[i], temp1, temp2, vec_size, active_in_chans);
	
	if (wet_gain)
	{
		for (i = 0; i < active_out_chans; i++)
			for (j = 0; j < vec_size; j++)
				outs[i][j] *= wet_gain[j];
	}
	
	if (dry_gain)
	{
		float *current_in;
		
		for (i = 0; i < active_in_chans && i < active_out_chans; i++)
			for (j = 0, current_in  = (float *) in_temps[i]; j < vec_size; j++)
				outs[i][j] += current_in[j] * dry_gain[j];
	}
	
	unlock_memory_swap(&x->temporary_memory);

#if defined( __i386__ ) || defined( __x86_64__ )	
	_mm_setcsr(oldMXCSR);	
#endif
}
