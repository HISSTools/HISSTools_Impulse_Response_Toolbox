

#include "output_channel_convolve.h"

#include <string.h>

void output_channel_convolve_free(t_output_channel_convolve *x)
{
    if (!x)
        return;
    
    for (AH_UIntPtr i = 0; i < x->num_in_chans; i++)
        zero_latency_convolve_free(x->convolvers[i]);
    
    free(x);
}


t_output_channel_convolve *output_channel_convolve_new(AH_UIntPtr input_chans, AH_UIntPtr max_length, t_convolve_latency_mode latency_mode)
{
    t_output_channel_convolve *x = malloc(sizeof(t_output_channel_convolve));
    
    if (!x)
        return 0;
    
    if (input_chans > MAX_CHANS)
        input_chans = MAX_CHANS;
    
    x->num_in_chans = 0;
    
    for (AH_UIntPtr i = 0; i < input_chans; i++)
    {
        x->convolvers[i] = zero_latency_convolve_new(max_length, latency_mode);
        
        if (!x->convolvers[i])
        {
            output_channel_convolve_free(x);
            return 0;
        }
        
        x->num_in_chans++;
    }
    
    return (x);
}


t_convolve_error output_channel_convolve_resize(t_output_channel_convolve *x, AH_UIntPtr in_chan, AH_UIntPtr impulse_length)
{
    if (in_chan < x->num_in_chans)
    {
        if (!zero_latency_convolve_resize(x->convolvers[in_chan], impulse_length, false))
            return CONVOLVE_ERR_MEM_UNAVAILABLE;
    }
    else
        return CONVOLVE_ERR_IN_CHAN_OUT_OF_RANGE;
    
    return CONVOLVE_ERR_NONE;
}


t_convolve_error output_channel_convolve_set(t_output_channel_convolve *x, AH_UIntPtr in_chan, float *input, AH_UIntPtr impulse_length, AH_Boolean resize)
{
    if (in_chan < x->num_in_chans)
        return zero_latency_convolve_set(x->convolvers[in_chan], input, impulse_length, resize);
    else
        return CONVOLVE_ERR_IN_CHAN_OUT_OF_RANGE;
}


t_convolve_error output_channel_convolve_reset(t_output_channel_convolve *x, AH_UIntPtr in_chan)
{
    if (in_chan < x->num_in_chans)
        return zero_latency_convolve_reset(x->convolvers[in_chan]);
    else
        return CONVOLVE_ERR_IN_CHAN_OUT_OF_RANGE;
}


void output_channel_convolve_process_float(t_output_channel_convolve *x, float **ins, float *out, float *temp1, AH_UIntPtr vec_size, AH_UIntPtr active_in_chans)
{
    // Zero output
    
    memset(out, 0, sizeof(float) * vec_size);
    
    // Convolve
    
    for (AH_UIntPtr i = 0; i < x->num_in_chans && i < active_in_chans ; i++)
        zero_latency_convolve_process(x->convolvers[i], ins[i], temp1, out, vec_size);
}


void output_channel_convolve_process_double(t_output_channel_convolve *x, float **ins, double *out, float *temp1, float *temp2, AH_UIntPtr vec_size, AH_UIntPtr active_in_chans)
{
    // Zero out temp
    
    memset(temp2, 0, sizeof(float) * vec_size);
    
    // Convolve
    
    for (AH_UIntPtr i = 0; i < x->num_in_chans && i < active_in_chans ; i++)
        zero_latency_convolve_process(x->convolvers[i], ins[i], temp1, temp2, vec_size);
    
    // Copy output
    
    for (AH_UIntPtr i = 0; i < vec_size; i++)
        out[i] = temp2[i];
}
