
#ifndef __HIRT_MAXIMUM_LENGTH_SEQUENCES__
#define __HIRT_MAXIMUM_LENGTH_SEQUENCES__

#include <cstdint>

// Feedback Mask

static const uint32_t feedback_mask_vals[] = {0x0u, 0x0u, 0x2u, 0x6u, 0xCu, 0x14u, 0x30u, 0x60u, 0xE1u, 0x100u, 0x240u, 0x500u, 0xE08u, 0x1C80u, 0x3802u, 0x6000u, 0xD008u, 0x12000u, 0x20400u, 0x72000u, 0x90000u, 0x500000u, 0xC00000u, 0x420000u, 0xE10000u};

// MLS Structure

struct t_mls
{
    uint32_t feedback_mask;
    uint32_t lfsr;

    uint32_t T;
    uint32_t order;

    double amp;
};

// Get Length

static inline uint32_t mls_get_length (t_mls *x)
{
    return x->T;
}

// Function Prototypes

void mls_params(t_mls *x, uint32_t log2_T, double amp);
void mls_reset(t_mls *x);

void mls_gen_float(t_mls *x, float *out, uintptr_t N);
void mls_gen_double(t_mls *x, double *out, uintptr_t N);

void mls_gen_block(t_mls *x, void *out, uintptr_t N, bool double_precision);
void mls_gen(t_mls *x, void *out, bool double_precision);

uint32_t get_next_lfsr_int(uint32_t lfsr, uint32_t feedback_mask) ;

#endif    /* __HIRT_MAXIMUM_LENGTH_SEQUENCES__ */
