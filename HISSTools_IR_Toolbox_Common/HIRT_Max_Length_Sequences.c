

#include "HIRT_Max_Length_Sequences.h"


//////////////////////////////////////////////////////////////////////////
////////////////////////////// Params / Reset ////////////////////////////
//////////////////////////////////////////////////////////////////////////


void mls_params(t_mls *x, AH_UInt32 log2_T, double amp)
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


AH_UInt32 get_next_lfsr_int(AH_UInt32 lfsr, AH_UInt32 feedback_mask)
{
    return (lfsr >> 1) ^ (AH_UInt32)((0 - (lfsr & 0x1u)) & feedback_mask);
}


void mls_gen_float(t_mls *x, float *out, AH_UIntPtr N)
{
    AH_UInt32 lfsr = x->lfsr;
    AH_UInt32 feedback_mask = x->feedback_mask;
    AH_UInt32 i;

    float amp = (float) x->amp;
    float two_amp = amp * 2;

    for (i = 0; i < N; i++)
    {
        *out++ = ((lfsr & 0x1u) * two_amp) - amp;
        lfsr = get_next_lfsr_int(lfsr, feedback_mask);
    }

    x->lfsr = lfsr;
}


void mls_gen_double(t_mls *x, double *out, AH_UIntPtr N)
{
    AH_UInt32 lfsr = x->lfsr;
    AH_UInt32 feedback_mask = x->feedback_mask;
    AH_UInt32 i;

    double amp = x->amp;
    double two_amp = amp * 2;

    for (i = 0; i < N; i++)
    {
        *out++ = ((lfsr & 0x1u) * two_amp) - amp;
        lfsr = get_next_lfsr_int(lfsr, feedback_mask);
    }

    x->lfsr = lfsr;
}


void mls_gen_block(t_mls *x, void *out, AH_UIntPtr N, AH_Boolean double_precision)
{
    if (double_precision)
        mls_gen_double(x, out, N);
    else
        mls_gen_float(x, out, N);
}


void mls_gen(t_mls *x, void *out, AH_Boolean double_precision)
{
    if (double_precision)
        mls_gen_double(x, out, x->T);
    else
        mls_gen_float(x, out, x->T);
}
