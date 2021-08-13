

#include "HIRT_Max_Length_Sequences.hpp"


//////////////////////////////////////////////////////////////////////////
////////////////////////////// Params / Reset ////////////////////////////
//////////////////////////////////////////////////////////////////////////


void mls_params(t_mls *x, uint32_t log2_T, double amp)
{
    log2_T = (log2_T < 1) ? 1 : log2_T;
    log2_T = (log2_T > 24) ? 24 : log2_T;
    x->amp = amp;

    x->T = (1u << log2_T) - 1u;
    x->order = log2_T;
    x->feedback_mask = feedback_mask_vals[log2_T];
    x->lfsr = 0x1u;
}


void mls_reset(t_mls *x)
{
    x->lfsr = 0x1u;
}


//////////////////////////////////////////////////////////////////////////
////////////////////////////// Generate MLS //////////////////////////////
//////////////////////////////////////////////////////////////////////////


uint32_t get_next_lfsr_int(uint32_t lfsr, uint32_t feedback_mask)
{
    return (lfsr >> 1) ^ (uint32_t)((0 - (lfsr & 0x1u)) & feedback_mask);
}


void mls_gen_float(t_mls *x, float *out, uintptr_t N)
{
    uint32_t lfsr = x->lfsr;
    uint32_t feedback_mask = x->feedback_mask;
    uint32_t i;

    float amp = (float) x->amp;
    float two_amp = amp * 2;

    for (i = 0; i < N; i++)
    {
        *out++ = ((lfsr & 0x1u) * two_amp) - amp;
        lfsr = get_next_lfsr_int(lfsr, feedback_mask);
    }

    x->lfsr = lfsr;
}


void mls_gen_double(t_mls *x, double *out, uintptr_t N)
{
    uint32_t lfsr = x->lfsr;
    uint32_t feedback_mask = x->feedback_mask;
    uint32_t i;

    double amp = x->amp;
    double two_amp = amp * 2;

    for (i = 0; i < N; i++)
    {
        *out++ = ((lfsr & 0x1u) * two_amp) - amp;
        lfsr = get_next_lfsr_int(lfsr, feedback_mask);
    }

    x->lfsr = lfsr;
}


void mls_gen_block(t_mls *x, void *out, uintptr_t N, bool double_precision)
{
    if (double_precision)
        mls_gen_double(x, (double *) out, N);
    else
        mls_gen_float(x, (float *) out, N);
}


void mls_gen(t_mls *x, void *out, bool double_precision)
{
    if (double_precision)
        mls_gen_double(x, (double *) out, x->T);
    else
        mls_gen_float(x, (float *) out, x->T);
}
