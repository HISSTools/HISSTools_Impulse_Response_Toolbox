
#ifndef __HIRT_CORE_FUNCTIONS__
#define __HIRT_CORE_FUNCTIONS__

#include <AH_Types.h>
#include <HISSTools_FFT.h>

#ifdef __cplusplus
extern "C" {
#else
#include <AH_Win_Complex_Math.h>
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////// Definitions and Enums //////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#define HIRT_MAX_MEASURE_CHANS 32
#define HIRT_MAX_SPECIFIER_ITEMS 2048
#define HIRT_DB_MIN -500

typedef enum {
	
	SMOOTH_MODE_FULL = 0,
	SMOOTH_MODE_FAST = 1,
	SMOOTH_MODE_FAST_OCT = 2,

} t_smooth_mode;

typedef enum {
	
	SPECTRUM_REAL,
	SPECTRUM_FULL
	
} t_spectrum_format;

typedef enum {
	
	FILTER_REGULARISATION = 0,
	FILTER_CLIP = 1,
	FILTER_FILTER = 2

} t_filter_type;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////// Single Value Conversions /////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static __inline double db_to_a (double db)
{
	return pow (10.0, db / 20.); 
}


static __inline double db_to_pow (double db)
{
	return pow (10.0, db / 10.); 
}



static __inline double a_to_db (double a)
{
	double db;
	
	if (!a) 
		return HIRT_DB_MIN;
	
	db = 20. * log10(a);
	
	if (db < HIRT_DB_MIN) 
		db = HIRT_DB_MIN;
	
	return db;
}


static __inline double pow_to_db (double pow)
{
	double db;
	
	if (!pow) 
		return HIRT_DB_MIN;
	
	db = 10. * log10(pow);
	
	if (db < HIRT_DB_MIN) 
		db = HIRT_DB_MIN;
	
	return db;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////// Function Prototypes ///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// FFT Size Calculations

AH_UIntPtr int_log2 (AH_UIntPtr in, AH_UIntPtr *inexact);
AH_UIntPtr calculate_fft_size(AH_UIntPtr input_size, AH_UIntPtr *fft_size_log2);

// DB / Pow Array Conversions

void db_to_pow_array (double *in, AH_UIntPtr length);
void pow_to_db_array (double *in, AH_UIntPtr length);

// Time-Freq / Freq-Time Transforms

void time_to_spectrum_float(FFT_SETUP_D fft_setup, float *in_buf, AH_UIntPtr in_length, FFT_SPLIT_COMPLEX_D spectrum, AH_UIntPtr fft_size);
void time_to_halfspectrum_float(FFT_SETUP_D fft_setup, float *in_buf, AH_UIntPtr in_length, FFT_SPLIT_COMPLEX_D spectrum, AH_UIntPtr fft_size);
void time_to_spectrum_double (FFT_SETUP_D fft_setup, double *in_buf, AH_UIntPtr in_length, FFT_SPLIT_COMPLEX_D spectrum, AH_UIntPtr fft_size);
void time_to_halfspectrum_double(FFT_SETUP_D fft_setup, double *in_buf, AH_UIntPtr in_length, FFT_SPLIT_COMPLEX_D spectrum, AH_UIntPtr fft_size);
void spectrum_to_time(FFT_SETUP_D fft_setup, double *out_buf, FFT_SPLIT_COMPLEX_D spectrum, AH_UIntPtr fft_size, t_spectrum_format half_spectrum);

// Calculate Power Spectrum

void power_spectrum(FFT_SPLIT_COMPLEX_D spectrum, AH_UIntPtr fft_size, t_spectrum_format format);
void power_full_spectrum_from_half_spectrum(FFT_SPLIT_COMPLEX_D spectrum, AH_UIntPtr fft_size);

// Spectral Smoothing

void smooth_power_spectrum (FFT_SPLIT_COMPLEX_D spectrum, t_smooth_mode mode, AH_UIntPtr fft_size, double smooth_lo, double smooth_hi);

// Phase Routines

void zero_phase_from_power_spectrum (FFT_SPLIT_COMPLEX_D spectrum, AH_UIntPtr fft_size, t_spectrum_format format);
void linear_phase_from_power_spectrum (FFT_SPLIT_COMPLEX_D spectrum, AH_UIntPtr fft_size, t_spectrum_format format);
void minimum_phase_components_from_power_spectrum (FFT_SETUP_D fft_setup, FFT_SPLIT_COMPLEX_D spectrum, AH_UIntPtr fft_size);
void minimum_phase_from_power_spectrum (FFT_SETUP_D fft_setup, FFT_SPLIT_COMPLEX_D spectrum, AH_UIntPtr fft_size);	
void noncausal_maximum_phase_from_power_spectrum (FFT_SETUP_D fft_setup, FFT_SPLIT_COMPLEX_D spectrum, AH_UIntPtr fft_size);
void maximum_phase_from_power_spectrum (FFT_SETUP_D fft_setup, FFT_SPLIT_COMPLEX_D spectrum, AH_UIntPtr fft_size);
void mixed_phase_from_power_spectrum (FFT_SETUP_D fft_setup, FFT_SPLIT_COMPLEX_D spectrum, AH_UIntPtr fft_size, double phase, AH_Boolean zero_center);
void variable_phase_from_power_spectrum (FFT_SETUP_D fft_setup, FFT_SPLIT_COMPLEX_D spectrum, AH_UIntPtr fft_size, double phase, AH_Boolean zero_center);

// Freq Specified Power Array

void make_freq_dependent_power_array (double *power_array, double *specifier_array, AH_UIntPtr fft_size, double sample_rate, double db_offset);

// Convolution

void convolve (FFT_SPLIT_COMPLEX_D fft_data_1, FFT_SPLIT_COMPLEX_D fft_data_2, AH_UIntPtr fft_size, t_spectrum_format format);

// Zero Phase Deconvolution

void deconvolve_with_amp_filter (FFT_SPLIT_COMPLEX_D spectrum_1, FFT_SPLIT_COMPLEX_D spectrum_2, double *filter_amps, AH_UIntPtr fft_size, t_spectrum_format format);
void deconvolve_regularised_zero_phase (FFT_SPLIT_COMPLEX_D spectrum_1, FFT_SPLIT_COMPLEX_D spectrum_2, double *beta_in, AH_UIntPtr fft_size, t_spectrum_format format);
void deconvolve_clip_zero_phase (FFT_SPLIT_COMPLEX_D spectrum_1, FFT_SPLIT_COMPLEX_D spectrum_2, double *clip_min, double *clip_max, AH_UIntPtr fft_size, t_spectrum_format format);

// Variable Phase Deconvolution

void deconvolve_with_filter (FFT_SPLIT_COMPLEX_D spectrum_1, FFT_SPLIT_COMPLEX_D spectrum_2, FFT_SPLIT_COMPLEX_D filter_spectrum, AH_UIntPtr fft_size, t_spectrum_format format);
void make_regularisation_filter (FFT_SETUP_D fft_setup, FFT_SPLIT_COMPLEX_D denominator_spectrum, FFT_SPLIT_COMPLEX_D filter_spectrum, double *beta_in, AH_UIntPtr fft_size, t_spectrum_format format, double phase);
void make_clip_filter (FFT_SETUP_D fft_setup, FFT_SPLIT_COMPLEX_D denominator_spectrum, FFT_SPLIT_COMPLEX_D filter_spectrum, double *clip_min, double *clip_max, AH_UIntPtr fft_size, t_spectrum_format format, double phase);
void deconvolve_regularised_variable_phase (FFT_SETUP_D fft_setup, FFT_SPLIT_COMPLEX_D spectrum_1, FFT_SPLIT_COMPLEX_D spectrum_2, FFT_SPLIT_COMPLEX_D temp_full_spectrum, double *beta_in, AH_UIntPtr fft_size, t_spectrum_format format, double phase);
void deconvolve_clip_variable_phase (FFT_SETUP_D fft_setup, FFT_SPLIT_COMPLEX_D spectrum_1, FFT_SPLIT_COMPLEX_D spectrum_2, FFT_SPLIT_COMPLEX_D temp_full_spectrum, double *clip_min, double *clip_max, AH_UIntPtr fft_size, t_spectrum_format format, double phase);

// High Level Deconvolution Routines

void deconvolve_zero_phase (FFT_SPLIT_COMPLEX_D spectrum_1, FFT_SPLIT_COMPLEX_D spectrum_2, FFT_SPLIT_COMPLEX_D filter_spectrum, double *filter_specifier, double *range_specifier, double filter_db_offset, AH_UIntPtr fft_size, t_spectrum_format format, t_filter_type mode, double sample_rate);
void make_deconvolution_filter (FFT_SETUP_D fft_setup, FFT_SPLIT_COMPLEX_D denominator_spectrum, FFT_SPLIT_COMPLEX_D filter_spectrum, double *filter_specifier, double *range_specifier, double filter_db_offset, float *filter_in, AH_UIntPtr filter_length, AH_UIntPtr fft_size, t_spectrum_format format, t_filter_type mode, double phase, double sample_rate);
void deconvolve_variable_phase (FFT_SETUP_D fft_setup, FFT_SPLIT_COMPLEX_D spectrum_1, FFT_SPLIT_COMPLEX_D spectrum_2, FFT_SPLIT_COMPLEX_D filter_spectrum, double *filter_specifier, double *range_specifier, double filter_db_offset, float *filter_in, AH_UIntPtr filter_length, AH_UIntPtr fft_size, t_spectrum_format format, t_filter_type mode, double phase, double sample_rate);
void deconvolve (FFT_SETUP_D fft_setup, FFT_SPLIT_COMPLEX_D spectrum_1, FFT_SPLIT_COMPLEX_D spectrum_2, FFT_SPLIT_COMPLEX_D filter_spectrum, double *filter_specifier, double *range_specifier, double filter_db_offset, float *filter_in, AH_UIntPtr filter_length, AH_UIntPtr fft_size, t_spectrum_format format, t_filter_type type, double phase, double delay, double sample_rate);

// Analytic Spikes / Delay for Modelling Delays

void spike_spectrum (FFT_SPLIT_COMPLEX_D spectrum, AH_UIntPtr fft_size, t_spectrum_format format, double spike);
void delay_spectrum (FFT_SPLIT_COMPLEX_D spectrum, AH_UIntPtr fft_size, t_spectrum_format format, double delay);

#ifdef __cplusplus
}
#endif

#endif
