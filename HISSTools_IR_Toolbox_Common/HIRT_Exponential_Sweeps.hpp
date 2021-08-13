
#ifndef __HIRT_EXPONENTIAL_SWEEPS__
#define __HIRT_EXPONENTIAL_SWEEPS__

#include <cstdint>

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

    uintptr_t T;
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

    uintptr_t num_amp_specifiers;

    double amp_specifier[36];
};

// Get Length / Harm Offsets

static inline uintptr_t ess_get_length (t_ess *x)
{
    return x->T;
}

static inline double ess_harm_offset(t_ess *x, uintptr_t harm)
{
    return x->T / log(x->hi_f_act/x->lo_f_act) * log((double) harm);
}

// Function Prototypes

uintptr_t ess_params(t_ess *x, double f1, double f2, double fade_in, double fade_out, double T, double sample_rate, double amp, double *amp_curve);

uintptr_t ess_gen_float(t_ess *x, float *out, uintptr_t startN, uintptr_t N);
uintptr_t ess_igen_float(t_ess *x, float *out, uintptr_t startN, uintptr_t N, t_invert_mode inv_amp);
uintptr_t ess_gen_double(t_ess *x, double *out, uintptr_t startN, uintptr_t N);
uintptr_t ess_igen_double(t_ess *x, double *out, uintptr_t startN, uintptr_t N, t_invert_mode inv_amp);

uintptr_t ess_gen_block(t_ess *x, void *out, uintptr_t startN, uintptr_t N, bool double_precision);
uintptr_t ess_igen_block(t_ess *x, void *out, uintptr_t startN, uintptr_t N, t_invert_mode inv_amp, bool double_precision);
uintptr_t ess_gen(t_ess *x, void *out, bool double_precision);
uintptr_t ess_igen(t_ess *x, void *out, t_invert_mode inv_amp, bool double_precision);

#endif /* __HIRT_EXPONENTIAL_SWEEPS__ */
