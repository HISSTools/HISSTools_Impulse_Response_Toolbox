
#include <AH_Types.h>
#include "convolve_latency_modes.h"

#ifdef __cplusplus
extern "C" {
    struct t_multi_channel_convolve;

#include "convolve_errors.h"

#else

#include "output_channel_convolve.h"
#include "convolve_errors.h"

#include <AH_Generic_Memory_Swap.h>

#include <stdio.h>
#include <string.h>

#ifndef __MULTICHANCONVOLVE_STRUCT__
#define __MULTICHANCONVOLVE_STRUCT__

typedef struct _multi_channel_convolve
{
    t_output_channel_convolve *chan_convolvers[MAX_CHANS];

    AH_UIntPtr num_in_chans;
    AH_UIntPtr num_out_chans;
    long N2M;

    vFloat *in_temps[MAX_CHANS];
    vFloat *temp1;
    vFloat *temp2;

    t_memory_swap temporary_memory;

} t_multi_channel_convolve;

#endif // __MULTICHANCONVOLVE_STRUCT__

#endif // _cplusplus

// N.B. pass 0 or less for out_chans to create a mono-to-mono parallel convolver

void multi_channel_convolve_free(t_multi_channel_convolve *x);
t_multi_channel_convolve *multi_channel_convolve_new(AH_UIntPtr in_chans, AH_UIntPtr out_chans, t_convolve_latency_mode latency_mode, AH_UIntPtr max_length);

void multi_channel_convolve_clear(t_multi_channel_convolve *x, AH_Boolean resize);
t_convolve_error multi_channel_convolve_resize(t_multi_channel_convolve *x, AH_UIntPtr in_chan, AH_UIntPtr out_chan, AH_UIntPtr impulse_length);
t_convolve_error multi_channel_convolve_set(t_multi_channel_convolve *x, AH_UIntPtr in_chan, AH_UIntPtr out_chan, float *input, AH_UIntPtr impulse_length, AH_Boolean resize);

void multi_channel_convolve_process_float(t_multi_channel_convolve *x, float **ins, float **outs, float *dry_gain, float *wet_gain, AH_UIntPtr vec_size, AH_UIntPtr active_in_chans, AH_UIntPtr active_out_chans);
void multi_channel_convolve_process_double(t_multi_channel_convolve *x, double **ins, double **outs, double *dry_gain, double *wet_gain, AH_UIntPtr vec_size, AH_UIntPtr active_in_chans, AH_UIntPtr active_out_chans);

#ifdef __cplusplus
}  /* this brace matches the one on the extern "C" line */
#endif
