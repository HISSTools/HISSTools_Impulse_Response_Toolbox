

#include <AH_VectorOps.h>

#include "zero_latency_convolve.h"
#include "convolve_errors.h"

#ifndef __OUTPUTCHANCONVOLVE_STRUCT__
#define __OUTPUTCHANCONVOLVE_STRUCT__

#define MAX_CHANS 96

typedef struct _output_channel_convolve
{
    t_zero_latency_convolve *convolvers[MAX_CHANS];

    AH_UIntPtr num_in_chans;

} t_output_channel_convolve;

#endif //__OUTPUTCHANCONVOLVE_STRUCT__

void output_channel_convolve_free(t_output_channel_convolve *x);
t_output_channel_convolve *output_channel_convolve_new(AH_UIntPtr input_chans, AH_UIntPtr max_length, t_convolve_latency_mode latency_mode);

t_convolve_error output_channel_convolve_resize(t_output_channel_convolve *x, AH_UIntPtr in_chan, AH_UIntPtr impulse_length);
t_convolve_error output_channel_convolve_set(t_output_channel_convolve *x, AH_UIntPtr in_chan, float *input, AH_UIntPtr impulse_length, AH_Boolean resize);

void output_channel_convolve_process_float(t_output_channel_convolve *x, vFloat **ins, float *out, vFloat *temp1, vFloat *temp2, AH_UIntPtr vec_size, AH_UIntPtr active_in_chans);
void output_channel_convolve_process_double(t_output_channel_convolve *x, vFloat **ins, double *out, vFloat *temp1, vFloat *temp2, AH_UIntPtr vec_size, AH_UIntPtr active_in_chans);
