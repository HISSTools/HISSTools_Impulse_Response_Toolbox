
#include "HIRT_Trim_Normalise.hpp"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////// Normalise ///////////////////////////////
//////////////////////////////////////////////////////////////////////////

double norm_find_max(double *in, uintptr_t length, double start_max)
{
    double norm_factor = start_max;

    // Find peak

    for (uintptr_t i = 0; i < length; i++)
        norm_factor = std::max(std::fabs(in[i]), norm_factor);

    return norm_factor;
}

//////////////////////////////////////////////////////////////////////////
////////////////////////////////// Fades /////////////////////////////////
//////////////////////////////////////////////////////////////////////////

inline double fade_linear(uintptr_t i, double mul)
{
    return i * mul;
}

inline double fade_square(uintptr_t i, double mul)
{
    const double val = i * mul;
    return val * val;
}

inline double fade_square_root(uintptr_t i, double mul)
{
    const double val = i * mul;
    return sqrt(val);
}

inline double fade_cos(uintptr_t i, double mul)
{
    const double val = i * mul;
    return std::sin(val);
}

inline double fade_gompertz(uintptr_t i, double mul)
{
    const double val = (i * mul) - 1.2;
    return std::exp(-0.1 * std::exp(-5.0 * val));
}

void fade_calc_fade_in(double *in_buf, uintptr_t fade_length, uintptr_t length, t_fade_type fade_type)
{
    auto apply = [](double *io, uintptr_t length, double mul, double FadeOp(uintptr_t, double))
    {
        for (uintptr_t i = 0; i < length; i++)
            *io++ *= FadeOp(i, mul);
    };
    
    double mul = 1.0 / fade_length;
    length = std::min(fade_length, length);
    
    switch (fade_type)
    {
        case FADE_LIN:          apply(in_buf, length, mul, &fade_linear);              break;
        case FADE_SQUARE:       apply(in_buf, length, mul, &fade_square);              break;
        case FADE_SQUARE_ROOT:  apply(in_buf, length, mul, &fade_square_root);         break;
        case FADE_COS:          apply(in_buf, length, mul * M_PI * 0.5, &fade_cos);    break;
        case FADE_GOMPERTZ:     apply(in_buf, length, mul * 2.9, &fade_linear);        break;
    }
}

void fade_calc_fade_out(double *in_buf, uintptr_t fade_length, uintptr_t length, t_fade_type fade_type)
{
    auto apply = [](double *io, uintptr_t length, double mul, double FadeOp(uintptr_t, double))
    {
        for (uintptr_t i = 0; i < length; i++)
            *io-- *= FadeOp(i, mul);
    };
    
    double mul = 1.0 / fade_length;
    length = std::min(fade_length, length);

    in_buf += length - 1;

    switch (fade_type)
    {
        case FADE_LIN:          apply(in_buf, length, mul, &fade_linear);              break;
        case FADE_SQUARE:       apply(in_buf, length, mul, &fade_square);              break;
        case FADE_SQUARE_ROOT:  apply(in_buf, length, mul, &fade_square_root);         break;
        case FADE_COS:          apply(in_buf, length, mul * M_PI * 0.5, &fade_cos);    break;
        case FADE_GOMPERTZ:     apply(in_buf, length, mul * 2.9, &fade_linear);        break;
    }
}

//////////////////////////////////////////////////////////////////////////
////////////////////////////// Trim Routines /////////////////////////////
//////////////////////////////////////////////////////////////////////////

struct t_rms_measure
{
    // Reset for running calculation

    void reset(uintptr_t index_reset)
    {
        accum = 0.0;
        last_index = index_reset;
    }
    
    double accum = 0.0;
    uintptr_t last_index = 0;
};

double calculate_rms_run(t_rms_measure *rms, double *in, uintptr_t length, uintptr_t width, uintptr_t index)
{
    // Calculate the RMS value using a running average method (rather than looping each time)

    double accum = rms->accum;
    double in_val;

    // Width is effectively always odd

    uintptr_t last_index = rms->last_index;
    uintptr_t half_length = width >> 1;
    uintptr_t pre_length = half_length;
    uintptr_t post_length = half_length + 1;

    width = (half_length << 1) + 1;

    // If we have moved one index backward

    if (last_index == index + 1)
    {
        if (index >= half_length)
        {
            in_val = in[index - half_length];
            accum += in_val * in_val;
        }

        if (index + half_length + 1 < length)
        {
            in_val = in[index + half_length + 1];
            accum -= in_val * in_val;
        }

        rms->accum = accum;
        rms->last_index = index;

        return sqrt(accum / width);
    }

    // If we have moved one index forward

    if (last_index == index - 1)
    {
        if (index >= half_length + 1)
        {
            in_val = in[index - (half_length + 1)];
            accum -= in_val * in_val;
        }

        if (index + half_length < length)
        {
            in_val = in[index + half_length];
            accum += in_val * in_val;
        }

        rms->accum = accum;
        rms->last_index = index;

        return sqrt(accum / width);
    }

    // Calculate the rms directly

    pre_length = pre_length > index ? index : pre_length;
    post_length = (index + post_length >= length) ? (length - 1) - index: post_length;
	accum = 0.0;

    for (uintptr_t i = index - pre_length; i <= post_length; i++)
    {
        in_val = in[i];
        accum += in_val * in_val;
    }

    rms->accum = accum;
    rms->last_index = index;

    return sqrt(accum / width);
}

t_rms_result trim_find_crossings_rms(double *in_buf, uintptr_t length, uintptr_t window_in, uintptr_t window_out, double in_db, double out_db, double mul, uintptr_t *current_start, uintptr_t *current_end)
{
    uintptr_t start_search = *current_start;
    uintptr_t end_search = *current_end;
    uintptr_t i, j;

    double in_lin = std::pow(10.0, in_db / 20.0);
    double out_lin = std::pow(10.0, out_db / 20.0);

    t_rms_measure rms;

    // Don't search at either end

    in_lin = (in_db == -HUGE_VAL) ? -1.0 : in_lin;
    out_lin = (out_db == -HUGE_VAL) ? -1.0 : out_lin;

    // Search for in level

    rms.reset(length + 2);

    for (i = 0; i < length && i < start_search; i++)
        if (mul * calculate_rms_run(&rms, in_buf, length, window_in, i) > in_lin)
            break;

    if (i == length)
        return RMS_RESULT_IN_LEVEL_NOT_FOUND;

    // Search for out level

    rms.reset(length + 2);

    for (j = length; j > i && j > end_search; j--)
        if (mul * calculate_rms_run(&rms, in_buf, length, window_out, j - 1) > out_lin)
            break;

    if (j == i)
        return RMS_RESULT_OUT_LEVEL_NOT_FOUND;

    // Store current positions

    *current_start = i;
    *current_end = j + 1;

    return RMS_RESULT_SUCCESS;
}

void trim_copy_part(double *out_buf, double *in_buf, uintptr_t offset, uintptr_t length)
{
    // N.B. It is safe to destructively copy part of a buffer onto itself, as long as you are copying to the start of the buffer

    in_buf += offset;

    for (uintptr_t i = 0; i < length; i++)
        *out_buf++ = *in_buf++;
}
