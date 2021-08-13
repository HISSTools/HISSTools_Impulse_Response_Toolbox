
#include "HIRT_Trim_Normalise.hpp"


//////////////////////////////////////////////////////////////////////////
//////////////////////////////// Normalise ///////////////////////////////
//////////////////////////////////////////////////////////////////////////


double norm_find_max(double *in, uintptr_t length, double start_max)
{
    double norm_factor = start_max;
    double norm_test;

    uintptr_t i;

    // Find peak

    for (i = 0; i < length; i++)
    {
        norm_test = fabs(in[i]);

        if (norm_test > norm_factor)
            norm_factor = norm_test;
    }

    return norm_factor;
}


//////////////////////////////////////////////////////////////////////////
////////////////////////////////// Fades /////////////////////////////////
//////////////////////////////////////////////////////////////////////////


void fade_calc_fade_in(double *in_buf, uintptr_t fade_length, uintptr_t length, t_fade_type fade_type)
{
    double mult = 1.0 / fade_length;
    double fade_val;

    uintptr_t i;

    if (fade_length > length)
        fade_length = length;

    switch (fade_type)
    {
        case FADE_LIN:
            for (i = 0; i < fade_length; i++)
            {
                fade_val = i * mult;
                *in_buf++ *= fade_val;
            }
            break;

        case FADE_SQUARE:
            for (i = 0; i < fade_length; i++)
            {
                fade_val = i * mult;
                fade_val *= fade_val;
                *in_buf++ *= fade_val;
            }
            break;

        case FADE_SQUARE_ROOT:
            for (i = 0; i < fade_length; i++)
            {
                fade_val = i * mult;
                fade_val = sqrt(fade_val);
                *in_buf++ *= fade_val;
            }
            break;

        case FADE_COS:
            mult *= M_PI * 0.5;
            for (i = 0; i < fade_length; i++)
            {
                fade_val = i * mult;
                fade_val = sin (fade_val);
                *in_buf++ *= fade_val;
            }
            break;

        case FADE_GOMPERTZ:
            mult *= 2.9;
            for (i = 0; i < fade_length; i++)
            {
                fade_val = (i * mult) - 1.2;
                fade_val = exp(-0.1 * exp(-5.0 * fade_val));
                *in_buf++ *= fade_val;
            }
            break;
    }
}


void fade_calc_fade_out(double *in_buf, uintptr_t fade_length, uintptr_t length, t_fade_type fade_type)
{
    double mult = 1.0 / fade_length;
    double fade_val;

    uintptr_t i;

    if (fade_length > length)
        fade_length = length;

    in_buf += length - 1;

    switch (fade_type)
    {
        case FADE_LIN:
            for (i = 0; i < fade_length; i++)
            {
                fade_val = i * mult;
                *in_buf-- *= fade_val;
            }
            break;

        case FADE_SQUARE:
            for (i = 0; i < fade_length; i++)
            {
                fade_val = i * mult;
                fade_val *= fade_val;
                *in_buf-- *= fade_val;
            }
            break;

        case FADE_SQUARE_ROOT:
            for (i = 0; i < fade_length; i++)
            {
                fade_val = i * mult;
                fade_val = sqrt(fade_val);
                *in_buf-- *= fade_val;
            }
            break;

        case FADE_COS:
            mult *= M_PI * 0.5;
            for (i = 0; i < fade_length; i++)
            {
                fade_val = i * mult;
                fade_val = sin (fade_val);
                *in_buf-- *= fade_val;
            }
            break;

        case FADE_GOMPERTZ:
            mult *= 2.9;
            for (i = 0; i < fade_length; i++)
            {
                fade_val = (i * mult) - 1.2;
                fade_val = exp(-0.1 * exp(-5.0 * fade_val));
                *in_buf-- *= fade_val;
            }
            break;
    }

}


//////////////////////////////////////////////////////////////////////////
////////////////////////////// Trim Routines /////////////////////////////
//////////////////////////////////////////////////////////////////////////


struct t_rms_measure
{
    double accum;

    uintptr_t last_index;
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
    uintptr_t i;

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

    for (i = index - pre_length, accum = 0.0; i <= post_length; i++)
    {
        in_val = in[i];
        accum += in_val * in_val;
    }

    rms->accum = accum;
    rms->last_index = index;

    return sqrt(accum / width);
}


void reset_rms(t_rms_measure *rms, uintptr_t index_reset)
{
    // Reset for running calculation

    rms->accum = 0.0;
    rms->last_index = index_reset;
}


t_rms_result trim_find_crossings_rms (double *in_buf, uintptr_t length, uintptr_t window_in, uintptr_t window_out, double in_db, double out_db, double mul, uintptr_t *current_start, uintptr_t *current_end)
{
    uintptr_t start_search = *current_start;
    uintptr_t end_search = *current_end;
    uintptr_t i, j;

    double in_lin = pow(10.0, in_db / 20.0);
    double out_lin = pow(10.0, out_db / 20.0);

    t_rms_measure rms;

    // Don't search at either end

    in_lin = (in_db == -HUGE_VAL) ? -1.0 : in_lin;
    out_lin = (out_db == -HUGE_VAL) ? -1.0 : out_lin;

    // Search for in level

    reset_rms(&rms, length + 2);

    for (i = 0; i < length && i < start_search; i++)
        if (mul * calculate_rms_run(&rms, in_buf, length, window_in, i) > in_lin)
            break;

    if (i == length)
        return RMS_RESULT_IN_LEVEL_NOT_FOUND;

    // Search for out level

    reset_rms(&rms, length + 2);

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
    // N.B. It is safe to destructiovely copy part of a buffer onto itself, as long as you are copying to the start of the buffer

    uintptr_t i;

    in_buf += offset;

    for (i = 0; i < length; i++)
        *out_buf++ = *in_buf++;
}
