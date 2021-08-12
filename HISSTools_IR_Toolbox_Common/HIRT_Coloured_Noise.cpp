
#include "HIRT_Coloured_Noise.h"


//////////////////////////////////////////////////////////////////////////
//////////////////////////// Defines / Helpers ///////////////////////////
//////////////////////////////////////////////////////////////////////////


#define UNSIGNED_INT32_TO_NORM_DOUBLE 2.32830643653869628906e-10


static inline double min_double(double v1, double v2)
{
    v1 = v1 < v2 ? v1 : v2;

    return v1;
}


//////////////////////////////////////////////////////////////////////////
////////////////////////////// Params / Reset ////////////////////////////
//////////////////////////////////////////////////////////////////////////


void coloured_noise_params(t_noise_params *x, t_noise_mode mode, double fade_in, double fade_out, double length, double sample_rate, double amp)
{
    // All Times in seconds
    // Browning filter coefficient - N.B cf is 16Hz

    x->alpha = sin(M_PI * 2.0 * 16.0 / sample_rate);

    // Pinking filter coefficients

    x->alpha0 = sin(M_PI * 2.0 * 8.00135734209627 / sample_rate);
    x->alpha1 = sin(M_PI * 2.0 * 46.88548507044182 / sample_rate);
    x->alpha2 = sin(M_PI * 2.0 * 217.61558695916962 / sample_rate);
    x->alpha3 = sin(M_PI * 2.0 * 939.80665948455472 / sample_rate);
    x->alpha4 = sin(M_PI * 2.0 * 3276.10128392439381 / sample_rate);

    // Zero filter memory

    x->prev_output = 0.0;

    x->b0 = 0.0;
    x->b1 = 0.0;
    x->b2 = 0.0;
    x->b3 = 0.0;
    x->b4 = 0.0;
    x->b5 = 0.0;
    x->b6 = 0.0;

    // Reset RNG parameters

    coloured_noise_reset(x);

    // Common Parameters

    x->amp = amp;
    x->sample_rate = sample_rate;
    x->T = (AH_UIntPtr) (length * sample_rate);
    x->RT = length;
    x->fade_in = fade_in;
    x->fade_out = fade_out;

    // Mode

    if (mode > 2)
        mode = NOISE_MODE_WHITE;

    x->mode = mode;
}


void coloured_noise_reset(t_noise_params *x)
{
    x->gen.w = 0;
    x->gen.x = 0;
    x->gen.y = 0;
    x->gen.z = 4294967295u;
}


//////////////////////////////////////////////////////////////////////////
///////////////////////////// Generate Noise /////////////////////////////
//////////////////////////////////////////////////////////////////////////


void coloured_noise_gen_float(t_noise_params *x, float *out, AH_UIntPtr startN, AH_UIntPtr N)
{
    double prev_output = x->prev_output;
    double alpha = x->alpha;

    double alpha0 = x->alpha0;
    double alpha1 = x->alpha1;
    double alpha2 = x->alpha2;
    double alpha3 = x->alpha3;
    double alpha4 = x->alpha4;

    double b0 = x->b0;
    double b1 = x->b1;
    double b2 = x->b2;
    double b3 = x->b3;
    double b4 = x->b4;
    double b5 = x->b5;
    double b6 = x->b6;

    double one_amp = x->amp;
    double two_amp = one_amp * 2.0;

    double sample_rate = x->sample_rate;
    double FiN = x->fade_in * sample_rate * 2.0;
    double FoN = x->fade_out * sample_rate * 2.0;
    double fade_in;
    double fade_out;
    double input_val;
    double result;

    AH_UInt32 xr = x->gen.x;
    AH_UInt32 yr = x->gen.y;
    AH_UInt32 zr = x->gen.z;
    AH_UInt32 wr =x->gen.w;
    AH_UInt32 r;

    AH_UIntPtr T = x->T;
    AH_UIntPtr i;

    FiN = (FiN < 1.0) ? 1.0 : FiN;
    FoN = (FoN < 1.0) ? 1.0 : FoN;

    switch (x->mode)
    {
        case NOISE_MODE_WHITE:

            for (i = startN; i < startN + N; i++)
            {
                fade_in = (1 - cos(M_PI * min_double(0.5, i / FiN)));
                fade_out = (1 - cos(M_PI * min_double(0.5, (T - i) / FoN)));

                r = (xr ^ (xr << 20)) ^ (yr ^ (yr >> 11)) ^ (zr ^ (zr << 27)) ^ (wr ^ (wr >> 6));
                xr = yr;
                yr = zr;
                zr = wr;
                wr = r;

                input_val = (r * UNSIGNED_INT32_TO_NORM_DOUBLE * two_amp) - one_amp;
                *out++ = (float) (fade_in * fade_out * input_val);
            }
            break;

        case NOISE_MODE_BROWN:

            for (i = startN; i < startN + N; i++)
            {
                fade_in = (1 - cos(M_PI * min_double(0.5, i / FiN)));
                fade_out = (1 - cos(M_PI * min_double(0.5, (T - i) / FoN)));

                r = (xr ^ (xr << 20)) ^ (yr ^ (yr >> 11)) ^ (zr ^ (zr << 27)) ^ (wr ^ (wr >> 6));
                xr = yr;
                yr = zr;
                zr = wr;
                wr = r;

                input_val = ((r * UNSIGNED_INT32_TO_NORM_DOUBLE * two_amp) - one_amp);
                result = prev_output + (alpha * (input_val - prev_output));
                *out++ = (float) (fade_in * fade_out * result);
                prev_output = result;
            }
            break;

        case NOISE_MODE_PINK:

            for (i = startN; i < startN + N; i++)
            {
                fade_in = (1 - cos(M_PI * min_double(0.5, i / FiN)));
                fade_out = (1 - cos(M_PI * min_double(0.5, (T - i) / FoN)));

                r = (xr ^ (xr << 20)) ^ (yr ^ (yr >> 11)) ^ (zr ^ (zr << 27)) ^ (wr ^ (wr >> 6));
                xr = yr;
                yr = zr;
                zr = wr;
                wr = r;

                input_val = (r * UNSIGNED_INT32_TO_NORM_DOUBLE * two_amp) - one_amp;

                b0 = b0 + (alpha0 * ((input_val * 48.69991228070175) - b0));
                b1 = b1 + (alpha1 * ((input_val * 11.23890718562874) - b1));
                b2 = b2 + (alpha2 * ((input_val * 4.96296774193548) - b2));
                b3 = b3 + (alpha3 * ((input_val * 2.32573483146067) - b3));
                b4 = b4 + (alpha4 * ((input_val * 1.18433822222222) - b4));
                b5 = -0.7616 * b5 - input_val * 0.0168980;
                result = (b0 + b1 + b2 + b3 + b4 + b5 + b6 + input_val * 0.5362);
                b6 = input_val * 0.115926;

                *out++ = (float) (fade_in * fade_out * result);
                prev_output = result;
            }
            break;
    }

    x->b0 = b0;
    x->b1 = b1;
    x->b2 = b2;
    x->b3 = b3;
    x->b4 = b4;
    x->b5 = b5;
    x->b6 = b6;

    x->gen.x = xr;
    x->gen.y = yr;
    x->gen.z = zr;
    x->gen.w = wr;

    x->prev_output = prev_output;
}


void coloured_noise_gen_double(t_noise_params *x, double *out, AH_UIntPtr startN, AH_UIntPtr N)
{
    double prev_output = x->prev_output;
    double alpha = x->alpha;

    double alpha0 = x->alpha0;
    double alpha1 = x->alpha1;
    double alpha2 = x->alpha2;
    double alpha3 = x->alpha3;
    double alpha4 = x->alpha4;

    double b0 = x->b0;
    double b1 = x->b1;
    double b2 = x->b2;
    double b3 = x->b3;
    double b4 = x->b4;
    double b5 = x->b5;
    double b6 = x->b6;

    double one_amp = x->amp;
    double two_amp = one_amp * 2.0;

    double sample_rate = x->sample_rate;
    double FiN = x->fade_in * sample_rate * 2.0;
    double FoN = x->fade_out * sample_rate * 2.0;
    double fade_in;
    double fade_out;
    double input_val;
    double result;

    AH_UInt32 xr = x->gen.x;
    AH_UInt32 yr = x->gen.y;
    AH_UInt32 zr = x->gen.z;
    AH_UInt32 wr =x->gen.w;
    AH_UInt32 r;

    AH_UIntPtr T = x->T;
    AH_UIntPtr i;

    FiN = (FiN < 1.0) ? 1.0 : FiN;
    FoN = (FoN < 1.0) ? 1.0 : FoN;

    switch (x->mode)
    {
        case NOISE_MODE_WHITE:

            for (i = startN; i < startN + N; i++)
            {
                fade_in = (1 - cos(M_PI * min_double(0.5, i / FiN)));
                fade_out = (1 - cos(M_PI * min_double(0.5, (T - i) / FoN)));

                r = (xr ^ (xr << 20)) ^ (yr ^ (yr >> 11)) ^ (zr ^ (zr << 27)) ^ (wr ^ (wr >> 6));
                xr = yr;
                yr = zr;
                zr = wr;
                wr = r;

                input_val = (r * UNSIGNED_INT32_TO_NORM_DOUBLE * two_amp) - one_amp;
                *out++ = fade_in * fade_out * input_val;
            }
            break;

        case NOISE_MODE_BROWN:

            for (i = startN; i < startN + N; i++)
            {
                fade_in = (1 - cos(M_PI * min_double(0.5, i / FiN)));
                fade_out = (1 - cos(M_PI * min_double(0.5, (T - i) / FoN)));

                r = (xr ^ (xr << 20)) ^ (yr ^ (yr >> 11)) ^ (zr ^ (zr << 27)) ^ (wr ^ (wr >> 6));
                xr = yr;
                yr = zr;
                zr = wr;
                wr = r;

                input_val = ((r * UNSIGNED_INT32_TO_NORM_DOUBLE * two_amp) - one_amp);
                result = prev_output + (alpha * (input_val - prev_output));
                *out++ = fade_in * fade_out * result;
                prev_output = result;
            }
            break;

        case NOISE_MODE_PINK:

            for (i = startN; i < startN + N; i++)
            {
                fade_in = (1 - cos(M_PI * min_double(0.5, i / FiN)));
                fade_out = (1 - cos(M_PI * min_double(0.5, (T - i) / FoN)));

                r = (xr ^ (xr << 20)) ^ (yr ^ (yr >> 11)) ^ (zr ^ (zr << 27)) ^ (wr ^ (wr >> 6));
                xr = yr;
                yr = zr;
                zr = wr;
                wr = r;

                input_val = (r * UNSIGNED_INT32_TO_NORM_DOUBLE * two_amp) - one_amp;

                b0 = b0 + (alpha0 * ((input_val * 48.69991228070175) - b0));
                b1 = b1 + (alpha1 * ((input_val * 11.23890718562874) - b1));
                b2 = b2 + (alpha2 * ((input_val * 4.96296774193548) - b2));
                b3 = b3 + (alpha3 * ((input_val * 2.32573483146067) - b3));
                b4 = b4 + (alpha4 * ((input_val * 1.18433822222222) - b4));
                b5 = -0.7616 * b5 - input_val * 0.0168980;
                result = (b0 + b1 + b2 + b3 + b4 + b5 + b6 + input_val * 0.5362);
                b6 = input_val * 0.115926;

                *out++ = fade_in * fade_out * result;
                prev_output = result;
            }
            break;
    }

    x->b0 = b0;
    x->b1 = b1;
    x->b2 = b2;
    x->b3 = b3;
    x->b4 = b4;
    x->b5 = b5;
    x->b6 = b6;

    x->gen.x = xr;
    x->gen.y = yr;
    x->gen.z = zr;
    x->gen.w = wr;

    x->prev_output = prev_output;
}


void coloured_noise_gen_block(t_noise_params *x, void *out, AH_UIntPtr startN, AH_UIntPtr N, AH_Boolean double_precision)
{
    if (double_precision)
        coloured_noise_gen_double(x, (double *) out, startN, N);
    else
        coloured_noise_gen_float(x, (float *) out, startN, N);
}


void coloured_noise_gen(t_noise_params *x, void *out, AH_Boolean double_precision)
{
    if (double_precision)
        coloured_noise_gen_double(x, (double *) out, 0, x->T);
    else
        coloured_noise_gen_float(x, (float *) out, 0, x->T);
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////// Measure Max Output ///////////////////////////
//////////////////////////////////////////////////////////////////////////


void coloured_noise_measure(t_noise_params *x, AH_UIntPtr N, double *max_out_pink, double *max_out_brown)
{
    double prev_output = x->prev_output;
    double alpha = x->alpha;

    double alpha0 = x->alpha0;
    double alpha1 = x->alpha1;
    double alpha2 = x->alpha2;
    double alpha3 = x->alpha3;
    double alpha4 = x->alpha4;

    double b0 = x->b0;
    double b1 = x->b1;
    double b2 = x->b2;
    double b3 = x->b3;
    double b4 = x->b4;
    double b5 = x->b5;
    double b6 = x->b6;

    double max_brown = 0.0;
    double max_pink = 0.0;

    double input_val;
    double result;

    AH_UInt32 xr = x->gen.x;
    AH_UInt32 yr = x->gen.y;
    AH_UInt32 zr = x->gen.z;
    AH_UInt32 wr =x->gen.w;
    AH_UInt32 r;

    AH_UIntPtr i;

    for (i = 0; i < N; i++)
    {
        r = (xr ^ (xr << 20)) ^ (yr ^ (yr >> 11)) ^ (zr ^ (zr << 27)) ^ (wr ^ (wr >> 6));
        xr = yr;
        yr = zr;
        zr = wr;
        wr = r;

        input_val = ((r * UNSIGNED_INT32_TO_NORM_DOUBLE * 2.0) - 1.0);
        result = prev_output + (alpha * (input_val - prev_output));
        prev_output = result;

        max_brown = fabs(result) >= max_brown ? fabs(result): max_brown;
    }

    coloured_noise_reset(x);

    for (i = 0; i < N; i++)
    {
        r = (xr ^ (xr << 20)) ^ (yr ^ (yr >> 11)) ^ (zr ^ (zr << 27)) ^ (wr ^ (wr >> 6));
        xr = yr;
        yr = zr;
        zr = wr;
        wr = r;

        input_val = (r * UNSIGNED_INT32_TO_NORM_DOUBLE * 2.0) - 1.0;

        b0 = b0 + (alpha0 * ((input_val * 48.69991228070175) - b0));
        b1 = b1 + (alpha1 * ((input_val * 11.23890718562874) - b1));
        b2 = b2 + (alpha2 * ((input_val * 4.96296774193548) - b2));
        b3 = b3 + (alpha3 * ((input_val * 2.32573483146067) - b3));
        b4 = b4 + (alpha4 * ((input_val * 1.18433822222222) - b4));
        b5 = -0.7616 * b5 - input_val * 0.0168980;
        result = (b0 + b1 + b2 + b3 + b4 + b5 + b6 + input_val * 0.5362);
        b6 = input_val * 0.115926;

        max_pink = fabs(result) >= max_pink ? fabs(result): max_pink;
    }

    *max_out_pink = max_pink;
    *max_out_brown = max_brown;
}


