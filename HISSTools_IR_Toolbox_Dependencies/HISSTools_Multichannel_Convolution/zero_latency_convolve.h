

#include "partition_convolve.h"
#include "time_domain_convolve.h"
#include "convolve_latency_modes.h"
#include "convolve_errors.h"

#include <AH_Generic_Memory_Swap.h>

#ifndef __ZEROLATENCYCONVOLVE_STRUCT__
#define __ZEROLATENCYCONVOLVE_STRUCT__

typedef struct _zero_latency_convolve
{
    t_time_domain_convolve *time1;
    t_partition_convolve *part1;
    t_partition_convolve *part2;
    t_partition_convolve *part3;

    t_memory_swap part4;

    AH_UIntPtr impulse_length;
    t_convolve_latency_mode latency_mode;

} t_zero_latency_convolve;

#endif // __ZEROLATENCYCONVOLVE_STRUCT__

void zero_latency_convolve_free(t_zero_latency_convolve *x);
t_zero_latency_convolve *zero_latency_convolve_new(AH_UIntPtr max_length, t_convolve_latency_mode latency_mode);

t_partition_convolve *zero_latency_convolve_resize(t_zero_latency_convolve *x, AH_UIntPtr impulse_length, AH_Boolean keep_lock);
t_convolve_error zero_latency_convolve_set(t_zero_latency_convolve *x, float *input, AH_UIntPtr impulse_length, AH_Boolean resize);
t_convolve_error zero_latency_convolve_reset(t_zero_latency_convolve *x);

void zero_latency_convolve_process_sum(float *out, float *add, AH_UIntPtr vec_size);
void zero_latency_convolve_process(t_zero_latency_convolve *x, float *in, float *temp, float *out, AH_UIntPtr vec_size);

