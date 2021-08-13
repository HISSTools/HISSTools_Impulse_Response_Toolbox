
#ifndef __HIRT_COLOURED_NOISE__
#define __HIRT_COLOURED_NOISE__

#include <cstdint>

// Mode Enum

enum t_noise_mode
{
    NOISE_MODE_WHITE = 0,
    NOISE_MODE_BROWN = 1,
    NOISE_MODE_PINK = 2,
};

// Noise and XORShift Structures

struct t_xorshift
{
    uint32_t w;
    uint32_t x;
    uint32_t y;
    uint32_t z;
};

struct t_noise_params
{
    double prev_output;
    double alpha;

    double alpha0;
    double alpha1;
    double alpha2;
    double alpha3;
    double alpha4;

    double b0;
    double b1;
    double b2;
    double b3;
    double b4;
    double b5;
    double b6;

    double amp;
    double sample_rate;

    double fade_in;
    double fade_out;
    double RT;

    t_noise_mode mode;
    t_xorshift gen;
    uintptr_t T;
};

// Get Length

static inline uintptr_t coloured_noise_get_length (t_noise_params *x)
{
    return x->T;
}

// Function Prototypes

void coloured_noise_params(t_noise_params *x, t_noise_mode mode, double fade_in, double fade_out, double length, double sample_rate, double amp);
void coloured_noise_reset(t_noise_params *x);

void coloured_noise_gen_float(t_noise_params *x, float *out, uintptr_t startN, uintptr_t N);
void coloured_noise_gen_double(t_noise_params *x, double *out, uintptr_t startN, uintptr_t N);

void coloured_noise_gen_block(t_noise_params *x, void *out, uintptr_t startN, uintptr_t N, bool double_precision);
void coloured_noise_gen(t_noise_params *x, void *out, bool double_precision);

void coloured_noise_measure(t_noise_params *x, uintptr_t N, double *max_out_pink, double *max_out_brown);

#endif    /* __HIRT_COLOURED_NOISE__ */
