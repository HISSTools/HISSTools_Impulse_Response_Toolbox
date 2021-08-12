
#ifndef __HIRT_EXPONENTIAL_SWEEPS__
#define __HIRT_EXPONENTIAL_SWEEPS__

#include <AH_Types.h>

// ESS Structure

enum t_invert_mode
{
    
    INVERT_USER_CURVE_TO_FIXED_REFERENCE = 0,   // Output at fixed level, inverting only the user amp curve
    INVERT_USER_CURVE_AND_SWEEP = 1,            // Output inverting the user amp curve and sweep but not the overall amplitude
    INVERT_ALL = 2,                             // Output inverting the user amp curve, sweep and the overall amplitude
};
    
struct t_ess
{
    // Internal only

    AH_UIntPtr T;
    double K1, K2;

    double lo_f_act;
    double hi_f_act;

    double f1;
    double f2;

    // Parameters requested

    double RT;
    double rf1, rf2;

    double fade_in;
    double fade_out;

    double sample_rate;
    double amp;

    // Amplitude curve

    AH_UIntPtr num_amp_specifiers;

    double amp_specifier[36];
};

// Get Length / Harm Offsets

static inline AH_UIntPtr ess_get_length (t_ess *x)
{
    return x->T;
}

static inline double ess_harm_offset(t_ess *x, AH_UIntPtr harm)
{
    return x->T / log(x->hi_f_act/x->lo_f_act) * log((double) harm);
}

// Function Prototypes

AH_UIntPtr ess_params(t_ess *x, double f1, double f2, double fade_in, double fade_out, double T, double sample_rate, double amp, double *amp_curve);

AH_UIntPtr ess_gen_float(t_ess *x, float *out, AH_UIntPtr startN, AH_UIntPtr N);
AH_UIntPtr ess_igen_float(t_ess *x, float *out, AH_UIntPtr startN, AH_UIntPtr N, t_invert_mode inv_amp);
AH_UIntPtr ess_gen_double(t_ess *x, double *out, AH_UIntPtr startN, AH_UIntPtr N);
AH_UIntPtr ess_igen_double(t_ess *x, double *out, AH_UIntPtr startN, AH_UIntPtr N, t_invert_mode inv_amp);

AH_UIntPtr ess_gen_block(t_ess *x, void *out, AH_UIntPtr startN, AH_UIntPtr N, AH_Boolean double_precision);
AH_UIntPtr ess_igen_block(t_ess *x, void *out, AH_UIntPtr startN, AH_UIntPtr N, t_invert_mode inv_amp, AH_Boolean double_precision);
AH_UIntPtr ess_gen(t_ess *x, void *out, AH_Boolean double_precision);
AH_UIntPtr ess_igen(t_ess *x, void *out, t_invert_mode inv_amp, AH_Boolean double_precision);

#endif /* __HIRT_EXPONENTIAL_SWEEPS__ */
