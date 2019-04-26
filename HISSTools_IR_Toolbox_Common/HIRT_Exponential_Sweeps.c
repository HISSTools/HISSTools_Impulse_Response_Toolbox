
#include "HIRT_Exponential_Sweeps.h"
#include "ext.h"

//////////////////////////////////////////////////////////////////////////
////////////////////////////////// Helper ////////////////////////////////
//////////////////////////////////////////////////////////////////////////


static __inline double min_double(double v1, double v2)
{
    v1 = v1 < v2 ? v1 : v2;

    return v1;
}


//////////////////////////////////////////////////////////////////////////
////////////////////////////////// Params ////////////////////////////////
//////////////////////////////////////////////////////////////////////////


AH_UIntPtr ess_params(t_ess *x, double f1, double f2, double fade_in, double fade_out, double T, double sample_rate, double amp, double *amp_curve)
{
    // All times in seconds

    double L;

    double K1;
    double K2;

    double NT;
    double final_phase;
    double NNT;

    double last_db_val;

    long num_items;
    long num_invalid = 0;
    long i;

    x->RT = T;
    x->rf1 = f1;
    x->rf2 = f2;

    f1 /= sample_rate;
    f2 /= sample_rate;

    T *= sample_rate;

    L = round(f1 * T / (log(f2 / f1))) / f1;

    K1 = 2 * M_PI * f1 * L;
    K2 = 1 / L;

    NT = round(f1 * T / (log(f2 / f1))) * log(f2 / f1) / f1;
    final_phase = floor(L * f1 * (exp(NT * K2) - 1));
    NNT = ceil(log((final_phase / L / f1 + 1)) / K2);

    if (L == 0)
        return 0;

    x->K1 = K1;
    x->K2 = K2;
    x->T = (AH_UIntPtr) NNT;
    x->lo_f_act = f1;
    x->hi_f_act = f1 * exp(NNT / L);
    x->f1 = f1;
    x->f2 = f2;
    x->fade_in = fade_in;
    x->fade_out = fade_out;
    x->sample_rate = sample_rate;
    x->amp = amp;

    // Design amplitude specifier

    for (i = 0; amp_curve && (i < 32); i++)
    {
        if (isinf(amp_curve[i]))
            break;
    }

    num_items = i >> 1;

    // Start of amp curve

    x->amp_specifier[0] = num_items ? log(amp_curve[0] / (f1 * sample_rate)) : 0.0;
    x->amp_specifier[1] = last_db_val = num_items ? amp_curve[1] : 0.0;

    if (x->amp_specifier[0] > 0.0)
        x->amp_specifier[0] = 0.0;

    // Intermediate points

    for (i = 0; i < num_items; i++)
    {
        x->amp_specifier[2 * (i - num_invalid) + 2] = log(amp_curve[2 * i] / (f1 * sample_rate));
        x->amp_specifier[2 * (i - num_invalid) + 3] = amp_curve[2 * i + 1];

        // Sanitize values - If frequencies do not increase then ignore this pair

        if (x->amp_specifier[2 * i + 2] < x->amp_specifier[2 * i])
            num_invalid++;
        else
            last_db_val = x->amp_specifier[2 * (i - num_invalid) + 3];
    }

    num_items -= num_invalid;

    // Endstop

    x->amp_specifier[2 * num_items + 2] = HUGE_VAL;
    x->amp_specifier[2 * num_items + 3] = last_db_val;

    x->num_amp_specifiers = num_items;

    return (AH_UIntPtr) NNT;
}


//////////////////////////////////////////////////////////////////////////
////////////////////////////// Generate ESS //////////////////////////////
//////////////////////////////////////////////////////////////////////////


AH_UIntPtr ess_gen_float(t_ess *x, float *out, AH_UIntPtr startN, AH_UIntPtr N)
{
    double *amp_specifier = x->amp_specifier;

    double K1 = x->K1;
    double K2 = x->K2;
    double amp = x->amp;
    double sample_rate = x->sample_rate;
    double FiN = x->fade_in * sample_rate * 2.0;
    double FoN = x->fade_out * sample_rate * 2.0;
    double val, fade_in, fade_out, time_val, interp, curve_db, curve_amp;

    AH_UIntPtr T = x->T;
    AH_UIntPtr i;
    AH_UIntPtr j = 0;

    // Sanity Checks

    if (startN > T)
        return 0;

    N = (startN + N > T) ? T - startN : N;
    FiN = (FiN < 1.0) ? 1.0 : FiN;
    FoN = (FoN < 1.0) ? 1.0 : FoN;

    for (i = startN; i < startN + N; i++)
    {
        // Fades and time value

        fade_in = (1 - cos(M_PI * min_double(0.5, i / FiN)));
        fade_out = (1 - cos(M_PI * min_double(0.5, (T - i) / FoN)));
        time_val = i * K2;

        // Amplitude curve

        for ( ; time_val > amp_specifier[j + 2]; j += 2);

        interp = (amp_specifier[j + 2] - amp_specifier[j]);
        interp = interp ? ((time_val - amp_specifier[j]) / interp) : 0.0;
        curve_db = amp_specifier[j + 1] + interp * (amp_specifier[j + 3] - amp_specifier[j + 1]);
        curve_amp = pow(10.0, curve_db / 20.0);

        // Final value

        val = curve_amp * amp * fade_in * fade_out * sin (K1 * (exp(time_val) - 1));
        *out++ = (float) val;
    }

    return N;
}


AH_UIntPtr ess_igen_float(t_ess *x, float *out, AH_UIntPtr startN, AH_UIntPtr N, t_invert_mode inv_amp)
{
    double *amp_specifier = x->amp_specifier;

    double K1 = x->K1;
    double K2 = x->K2;
    double amp = (inv_amp == INVERT_ALL) ? x->amp : 1.0;
    double sample_rate = x->sample_rate;
    double FiN = x->fade_in * sample_rate * 2.0;
    double FoN = x->fade_out * sample_rate * 2.0;
    double amp_const = (inv_amp == INVERT_USER_CURVE_TO_FIXED_REFERENCE) ? x->amp : (4.0 * x->lo_f_act * K2) / amp;
    double val, fade_in, fade_out, time_val, interp, curve_db, curve_amp, exp_val;

    AH_UIntPtr T = x->T;
    AH_UIntPtr i;
    AH_UIntPtr j = 2 * x->num_amp_specifiers;

    // Sanity Checks

    if (startN > T)
        return 0;

    N = (startN + N > T) ? T - startN : N;
    FiN = (FiN < 1.0) ? 1.0 : FiN;
    FoN = (FoN < 1.0) ? 1.0 : FoN;

    for (i = startN; i < startN + N; i++)
    {
        // Fades and time value

        fade_in = (1 - cos(M_PI * min_double(0.5, (T - i - 1) / FiN)));
        fade_out = (1 - cos(M_PI * min_double(0.5, (i + 1) / FoN)));
        time_val = (T - i - 1) * K2;

        // Amplitude curve

        for ( ; time_val < amp_specifier[j]; j -= 2);

        interp = (amp_specifier[j + 2] - amp_specifier[j]);
        interp = interp ? ((time_val - amp_specifier[j]) / interp) : 0.0;
        curve_db = amp_specifier[j + 1] + interp * (amp_specifier[j + 3] - amp_specifier[j + 1]);
        curve_amp = pow(10.0, -curve_db / 20.0);

        // Final value

        exp_val = exp(time_val);
        val = curve_amp * amp_const * fade_in * fade_out * exp_val * sin(K1 * (exp_val - 1.0));
        *out++ = (float) val;
    }

    return N;
}


AH_UIntPtr ess_gen_double(t_ess *x, double *out, AH_UIntPtr startN, AH_UIntPtr N)
{
    double *amp_specifier = x->amp_specifier;

    double K1 = x->K1;
    double K2 = x->K2;
    double amp = x->amp;
    double sample_rate = x->sample_rate;
    double FiN = x->fade_in * sample_rate * 2.0;
    double FoN = x->fade_out * sample_rate * 2.0;
    double val, fade_in, fade_out, time_val, interp, curve_db, curve_amp;

    AH_UIntPtr T = x->T;
    AH_UIntPtr i;
    AH_UIntPtr j = 0;

    // Sanity Checks

    if (startN > T)
        return 0;

    N = (startN + N > T) ? T - startN : N;
    FiN = (FiN < 1.0) ? 1.0 : FiN;
    FoN = (FoN < 1.0) ? 1.0 : FoN;

    for (i = startN; i < startN + N; i++)
    {
        // Fades and time value

        fade_in = (1 - cos(M_PI * min_double(0.5, i / FiN)));
        fade_out = (1 - cos(M_PI * min_double(0.5, (T - i) / FoN)));
        time_val = i * K2;

        // Amplitude curve

        for ( ; time_val > amp_specifier[j + 2]; j += 2);

        interp = (amp_specifier[j + 2] - amp_specifier[j]);
        interp = interp ? ((time_val - amp_specifier[j]) / interp) : 0.0;
        curve_db = amp_specifier[j + 1] + interp * (amp_specifier[j + 3] - amp_specifier[j + 1]);
        curve_amp = pow(10.0, curve_db / 20.0);

        // Final value

        val = curve_amp * amp * fade_in * fade_out * sin(K1 * (exp(time_val) - 1));
        *out++ = val;
    }

    return N;
}


AH_UIntPtr ess_igen_double(t_ess *x, double *out, AH_UIntPtr startN, AH_UIntPtr N, t_invert_mode inv_amp)
{
    double *amp_specifier = x->amp_specifier;

    double K1 = x->K1;
    double K2 = x->K2;
    double amp = (inv_amp == INVERT_ALL) ? x->amp : 1.0;
    double sample_rate = x->sample_rate;
    double FiN = x->fade_in * sample_rate * 2.0;
    double FoN = x->fade_out * sample_rate * 2.0;
    double amp_const = (inv_amp == INVERT_USER_CURVE_TO_FIXED_REFERENCE) ? x->amp : (4.0 * x->lo_f_act * K2) / amp;
    double val, fade_in, fade_out, time_val, interp, curve_db, curve_amp, exp_val;

    AH_UIntPtr T = x->T;
    AH_UIntPtr i;
    AH_UIntPtr j = 2 * x->num_amp_specifiers;

    // Sanity Checks

    if (startN > T)
        return 0;

    N = (startN + N > T) ? T - startN : N;
    FiN = (FiN < 1.0) ? 1.0 : FiN;
    FoN = (FoN < 1.0) ? 1.0 : FoN;

    for (i = startN; i < startN + N; i++)
    {
        // Fades and time value

        fade_in = (1 - cos(M_PI * min_double(0.5, (T - i - 1) / FiN)));
        fade_out = (1 - cos(M_PI * min_double(0.5, (i + 1) / FoN)));
        time_val = (T - i - 1) * K2;

        // Amplitude curve

        for ( ; time_val < amp_specifier[j]; j -= 2);

        interp = (amp_specifier[j + 2] - amp_specifier[j]);
        interp = interp ? ((time_val - amp_specifier[j]) / interp) : 0.0;
        curve_db = amp_specifier[j + 1] + interp * (amp_specifier[j + 3] - amp_specifier[j + 1]);
        curve_amp = pow(10.0, -curve_db / 20.0);

        // Final value

        exp_val = exp(time_val);
        val = curve_amp * amp_const * fade_in * fade_out * exp_val * sin(K1 * (exp_val - 1.0));
        *out++ = val;
    }

    return N;
}


AH_UIntPtr ess_gen_block(t_ess *x, void *out, AH_UIntPtr startN, AH_UIntPtr N, AH_Boolean double_precision)
{
    if (double_precision)
        return ess_gen_double(x, out, startN, N);
    else
        return ess_gen_float(x, out, startN, N);
}


AH_UIntPtr ess_igen_block(t_ess *x, void *out, AH_UIntPtr startN, AH_UIntPtr N, t_invert_mode inv_amp, AH_Boolean double_precision)
{
    if (double_precision)
        return ess_igen_double(x, out, startN, N, inv_amp);
    else
        return ess_igen_float(x, out, startN, N, inv_amp);
}


AH_UIntPtr ess_gen(t_ess *x, void *out, AH_Boolean double_precision)
{
    if (double_precision)
        return ess_gen_double(x, out, 0, x->T);
    else
        return ess_gen_float(x, out, 0, x->T);
}


AH_UIntPtr ess_igen(t_ess *x, void *out, t_invert_mode inv_amp, AH_Boolean double_precision)
{
    if (double_precision)
        return ess_igen_double(x, out, 0, x->T, inv_amp);
    else
        return ess_igen_float(x, out, 0, x->T, inv_amp);
}


