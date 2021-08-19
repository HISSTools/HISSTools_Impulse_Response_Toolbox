
#ifndef __HIRT_CORE_FUNCTIONS__
#define __HIRT_CORE_FUNCTIONS__

#include <HISSTools_FFT/HISSTools_FFT.h>
#include "HIRT_FFT_Setup.hpp"
#include "HIRT_Memory.hpp"

#include <cmath>
#include <complex>
#include <cstdint>

// Definitions and Enums

static constexpr int HIRT_MAX_MEASURE_CHANS = 128;
static constexpr int HIRT_MAX_SPECIFIER_ITEMS = 2048;
static constexpr double HIRT_DB_MIN = -500;

enum t_smooth_mode
{
    SMOOTH_MODE_FULL = 0,
    SMOOTH_MODE_FAST = 1,
    SMOOTH_MODE_FAST_OCT = 2,
};

enum t_spectrum_format
{
    SPECTRUM_REAL,
    SPECTRUM_FULL
};

enum t_filter_type
{
    FILTER_REGULARISATION = 0,
    FILTER_CLIP = 1,
    FILTER_FILTER = 2
};

// Single Value Conversions

static inline double db_to_a(double db)
{
    return std::pow(10.0, db / 20.0);
}

static inline double db_to_pow(double db)
{
    return std::pow(10.0, db / 10.0);
}

static inline double a_to_db(double a)
{
    double db;

    if (!a)
        return HIRT_DB_MIN;

    db = 20.0 * std::log10(a);

    if (db < HIRT_DB_MIN)
        db = HIRT_DB_MIN;

    return db;
}

static inline double pow_to_db(double pow)
{
    double db;

    if (!pow)
        return HIRT_DB_MIN;

    db = 10.0 * std::log10(pow);

    if (db < HIRT_DB_MIN)
        db = HIRT_DB_MIN;

    return db;
}

// Function Prototypes

// FFT Size Calculations

uintptr_t int_log2(uintptr_t in, bool& inexact);
uintptr_t calculate_fft_size(uintptr_t input_size, uintptr_t& fft_size_log2);

// DB / Pow Array Conversions

void db_to_pow_array(double *in, uintptr_t length);
void pow_to_db_array(double *in, uintptr_t length);

// Time-Freq / Freq-Time Transforms

void time_to_spectrum_float(FFT_SETUP_D fft_setup, float *in_buf, uintptr_t in_length, FFT_SPLIT_COMPLEX_D spectrum, uintptr_t fft_size);
void time_to_halfspectrum_float(FFT_SETUP_D fft_setup, float *in_buf, uintptr_t in_length, FFT_SPLIT_COMPLEX_D spectrum, uintptr_t fft_size);
void time_to_spectrum_double(FFT_SETUP_D fft_setup, double *in_buf, uintptr_t in_length, FFT_SPLIT_COMPLEX_D spectrum, uintptr_t fft_size);
void time_to_halfspectrum_double(FFT_SETUP_D fft_setup, double *in_buf, uintptr_t in_length, FFT_SPLIT_COMPLEX_D spectrum, uintptr_t fft_size);
void spectrum_to_time(FFT_SETUP_D fft_setup, double *out_buf, FFT_SPLIT_COMPLEX_D spectrum, uintptr_t fft_size, t_spectrum_format half_spectrum);

// Calculate Power Spectrum

void power_spectrum(FFT_SPLIT_COMPLEX_D spectrum, uintptr_t fft_size, t_spectrum_format format);
void power_full_spectrum_from_half_spectrum(FFT_SPLIT_COMPLEX_D spectrum, uintptr_t fft_size);

// Spectral Smoothing

void smooth_power_spectrum(FFT_SPLIT_COMPLEX_D spectrum, t_smooth_mode mode, uintptr_t fft_size, double smooth_lo, double smooth_hi);

// Phase Routines

void zero_phase_from_power_spectrum(FFT_SPLIT_COMPLEX_D spectrum, uintptr_t fft_size, t_spectrum_format format);
void linear_phase_from_power_spectrum(FFT_SPLIT_COMPLEX_D spectrum, uintptr_t fft_size, t_spectrum_format format);
void minimum_phase_components_from_power_spectrum(FFT_SETUP_D fft_setup, FFT_SPLIT_COMPLEX_D spectrum, uintptr_t fft_size);
void minimum_phase_from_power_spectrum(FFT_SETUP_D fft_setup, FFT_SPLIT_COMPLEX_D spectrum, uintptr_t fft_size);
void noncausal_maximum_phase_from_power_spectrum(FFT_SETUP_D fft_setup, FFT_SPLIT_COMPLEX_D spectrum, uintptr_t fft_size);
void maximum_phase_from_power_spectrum(FFT_SETUP_D fft_setup, FFT_SPLIT_COMPLEX_D spectrum, uintptr_t fft_size);
void mixed_phase_from_power_spectrum(FFT_SETUP_D fft_setup, FFT_SPLIT_COMPLEX_D spectrum, uintptr_t fft_size, double phase, bool zero_center);
void variable_phase_from_power_spectrum(FFT_SETUP_D fft_setup, FFT_SPLIT_COMPLEX_D spectrum, uintptr_t fft_size, double phase, bool zero_center);

// Freq Specified Power Array

void make_freq_dependent_power_array(double *power_array, double *specifier_array, uintptr_t fft_size, double sample_rate, double db_offset);

// Convolution

void convolve(FFT_SPLIT_COMPLEX_D fft_data_1, FFT_SPLIT_COMPLEX_D fft_data_2, uintptr_t fft_size, t_spectrum_format format);

// Zero Phase Deconvolution

void deconvolve_with_amp_filter(FFT_SPLIT_COMPLEX_D spectrum_1, FFT_SPLIT_COMPLEX_D spectrum_2, double *filter_amps, uintptr_t fft_size, t_spectrum_format format);
void deconvolve_regularised_zero_phase(FFT_SPLIT_COMPLEX_D spectrum_1, FFT_SPLIT_COMPLEX_D spectrum_2, double *beta_in, uintptr_t fft_size, t_spectrum_format format);
void deconvolve_clip_zero_phase(FFT_SPLIT_COMPLEX_D spectrum_1, FFT_SPLIT_COMPLEX_D spectrum_2, double *clip_min, double *clip_max, uintptr_t fft_size, t_spectrum_format format);

// Variable Phase Deconvolution

void deconvolve_with_filter(FFT_SPLIT_COMPLEX_D spectrum_1, FFT_SPLIT_COMPLEX_D spectrum_2, FFT_SPLIT_COMPLEX_D filter_spectrum, uintptr_t fft_size, t_spectrum_format format);
void make_regularisation_filter(FFT_SETUP_D fft_setup, FFT_SPLIT_COMPLEX_D denominator_spectrum, FFT_SPLIT_COMPLEX_D filter_spectrum, double *beta_in, uintptr_t fft_size, t_spectrum_format format, double phase);
void make_clip_filter(FFT_SETUP_D fft_setup, FFT_SPLIT_COMPLEX_D denominator_spectrum, FFT_SPLIT_COMPLEX_D filter_spectrum, double *clip_min, double *clip_max, uintptr_t fft_size, t_spectrum_format format, double phase);
void deconvolve_regularised_variable_phase(FFT_SETUP_D fft_setup, FFT_SPLIT_COMPLEX_D spectrum_1, FFT_SPLIT_COMPLEX_D spectrum_2, FFT_SPLIT_COMPLEX_D temp_full_spectrum, double *beta_in, uintptr_t fft_size, t_spectrum_format format, double phase);
void deconvolve_clip_variable_phase(FFT_SETUP_D fft_setup, FFT_SPLIT_COMPLEX_D spectrum_1, FFT_SPLIT_COMPLEX_D spectrum_2, FFT_SPLIT_COMPLEX_D temp_full_spectrum, double *clip_min, double *clip_max, uintptr_t fft_size, t_spectrum_format format, double phase);

// High Level Deconvolution Routines

void deconvolve_zero_phase(FFT_SPLIT_COMPLEX_D spectrum_1, FFT_SPLIT_COMPLEX_D spectrum_2, FFT_SPLIT_COMPLEX_D filter_spectrum, double *filter_specifier, double *range_specifier, double filter_db_offset, uintptr_t fft_size, t_spectrum_format format, t_filter_type mode, double sample_rate);
void make_deconvolution_filter(FFT_SETUP_D fft_setup, FFT_SPLIT_COMPLEX_D denominator_spectrum, FFT_SPLIT_COMPLEX_D filter_spectrum, double *filter_specifier, double *range_specifier, double filter_db_offset, float *filter_in, uintptr_t filter_length, uintptr_t fft_size, t_spectrum_format format, t_filter_type mode, double phase, double sample_rate);
void deconvolve_variable_phase(FFT_SETUP_D fft_setup, FFT_SPLIT_COMPLEX_D spectrum_1, FFT_SPLIT_COMPLEX_D spectrum_2, FFT_SPLIT_COMPLEX_D filter_spectrum, double *filter_specifier, double *range_specifier, double filter_db_offset, float *filter_in, uintptr_t filter_length, uintptr_t fft_size, t_spectrum_format format, t_filter_type mode, double phase, double sample_rate);
void deconvolve(FFT_SETUP_D fft_setup, FFT_SPLIT_COMPLEX_D spectrum_1, FFT_SPLIT_COMPLEX_D spectrum_2, FFT_SPLIT_COMPLEX_D filter_spectrum, double *filter_specifier, double *range_specifier, double filter_db_offset, float *filter_in, uintptr_t filter_length, uintptr_t fft_size, t_spectrum_format format, t_filter_type type, double phase, double delay, double sample_rate);

// Analytic Spikes / Delay for Modelling Delays

void spike_spectrum(FFT_SPLIT_COMPLEX_D spectrum, uintptr_t fft_size, t_spectrum_format format, double spike);
void delay_spectrum(FFT_SPLIT_COMPLEX_D spectrum, uintptr_t fft_size, t_spectrum_format format, double delay);

#endif
