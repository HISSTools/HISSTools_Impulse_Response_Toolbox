
#include "convolve_errors.h"

#include <AH_VectorOps.h>
#include <HISSTools_FFT.h>
#include <AH_Types.h>

#ifndef __PARTCONVOLVE_STRUCT__
#define __PARTCONVOLVE_STRUCT__

typedef struct _partition_convolve
{		
	// FFT variables
	
	FFT_SETUP_F fft_setup_real;
	
	AH_UIntPtr max_fft_size;
	AH_UIntPtr max_fft_size_log2; 
	AH_UIntPtr fft_size; 
	AH_UIntPtr fft_size_log2;
	
	AH_UIntPtr till_next_fft; 
	AH_UIntPtr rw_pointer1;
	AH_UIntPtr rw_pointer2;
	
	// Scheduling variables
	
	AH_UIntPtr num_partitions;
	AH_UIntPtr valid_partitions;
	AH_UIntPtr partitions_done;
	AH_UIntPtr last_partition;
	
	AH_UIntPtr input_position;
	AH_UIntPtr schedule_counter;
	
	// Internal buffers 
	
	vFloat *fft_buffers[4];
	
	FFT_SPLIT_COMPLEX_F impulse_buffer;
	FFT_SPLIT_COMPLEX_F	input_buffer;
	FFT_SPLIT_COMPLEX_F	accum_buffer;
	FFT_SPLIT_COMPLEX_F	partition_temp;
	
	AH_UIntPtr max_impulse_length;
	
	// Attributes
	
	AH_UIntPtr offset;
	AH_UIntPtr length;
	
	// Flags
	
	char reset_flag;				// reset fft data on next perform call
	
} t_partition_convolve;

#endif // __PARTCONVOLVE_STRUCT__


// N.B. MIN_FFT_SIZE_LOG2 should never be smaller than 4, as below code assumes loop unroll of vectors (4 vals) by 4 (== 16 or 2^4)
//		MAX_FFT_SIZE_LOG2 is perhaps conservative right now, assuming realtime usage, but it is easy to increase this if necessary

#define MIN_FFT_SIZE_LOG2					5
#define MAX_FFT_SIZE_LOG2					20


void partition_convolve_free(t_partition_convolve *x);
t_partition_convolve *partition_convolve_new(AH_UIntPtr max_fft_size, AH_UIntPtr max_impulse_length, AH_UIntPtr offset, AH_UIntPtr length);
void init_partition_convolve();

t_convolve_error partition_convolve_fft_size_set(t_partition_convolve *x, AH_UIntPtr fft_size);
t_convolve_error partition_convolve_length_set(t_partition_convolve *x, AH_UIntPtr length);
void partition_convolve_offset_set(t_partition_convolve *x, AH_UIntPtr offset);

t_convolve_error partition_convolve_set(t_partition_convolve *x, float *input, AH_UIntPtr impulse_length);

AH_Boolean partition_convolve_process(t_partition_convolve *x, vFloat *in, vFloat *out, AH_UIntPtr vec_size);
void partition_convolve_process_partition(FFT_SPLIT_COMPLEX_F in1, FFT_SPLIT_COMPLEX_F in2, FFT_SPLIT_COMPLEX_F out, AH_UIntPtr num_vecs);
