
#ifndef __TRIM_NORMALISE__
#define __TRIM_NORMALISE__

#include <cstdint>

// Fade Type Enum

enum t_fade_type
{
    FADE_LIN = 0,
    FADE_SQUARE = 1,
    FADE_SQUARE_ROOT = 2,
    FADE_COS = 3,
    FADE_GOMPERTZ = 4,
};

// RMS Result Enum

enum t_rms_result
{
    RMS_RESULT_SUCCESS = 0,
    RMS_RESULT_IN_LEVEL_NOT_FOUND = 1,
    RMS_RESULT_OUT_LEVEL_NOT_FOUND = 2,
};

// Function Prototypes

double norm_find_max(double *in, uintptr_t length, double start_max);

void fade_calc_fade_in(double *in_buf, uintptr_t fade_length, uintptr_t length, t_fade_type fade_type);
void fade_calc_fade_out(double *in_buf, uintptr_t fade_length, uintptr_t length, t_fade_type fade_type);

t_rms_result trim_find_crossings_rms(double *in_buf, uintptr_t length, uintptr_t window_in, uintptr_t window_out, double in_db, double out_db, double mul, uintptr_t *current_start, uintptr_t *current_end);
void trim_copy_part(double *out_buf, double *in_buf, uintptr_t offset, uintptr_t length);

#endif /* __TRIM_NORMALISE__ */
