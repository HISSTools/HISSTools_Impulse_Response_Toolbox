
#ifndef __TIMECONVOLVE_STRUCT__
#define __TIMECONVOLVE_STRUCT__

#include "convolve_errors.h"

#include <AH_Types.h>


typedef struct _time_domain_convolve
{
      // Internal buffers

    float *impulse_buffer;
    float *input_buffer;
    float *output_buffer;

    AH_UIntPtr input_position;
    AH_UIntPtr impulse_length;

    AH_UIntPtr offset;
    AH_UIntPtr length;

} t_time_domain_convolve;

#endif // __TIMECONVOLVE_STRUCT__


void time_domain_convolve_free(t_time_domain_convolve *x);
t_time_domain_convolve *time_domain_convolve_new(AH_UIntPtr offset, AH_UIntPtr length);

t_convolve_error time_domain_convolve_length_set(t_time_domain_convolve *x, AH_UIntPtr length);
void time_domain_convolve_offset_set(t_time_domain_convolve *x, AH_UIntPtr offset);

t_convolve_error time_domain_convolve_set(t_time_domain_convolve *x, float *input, AH_UIntPtr impulse_length);

void time_domain_convolve_process_scalar(t_time_domain_convolve *x, float *in, float *out, AH_UIntPtr vec_size);
void time_domain_convolve_process(t_time_domain_convolve *x, float *in, float *out, AH_UIntPtr vec_size);
