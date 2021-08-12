

#ifndef __HISSTOOLS_FFT__
#define __HISSTOOLS_FFT__

#include <stdint.h>

/** @file HISSTools_FFT.h @brief The main interface for the HISSTools FFT.
 
    The FFT is compatiable with the FFT routines provided by Apple's vDSP library and can be configured to use this fast FFT when available. */

/**
 The NO_NATIVE_FFT preprocessor command instructs the HISSTools FFT to use its own code even if the Apple FFT is available. You must link against the Accelerate framework if this is not defined under Mac OS. It is not a default setting
 */

// Platform check for Apple FFT selection

#if defined __APPLE__ && !defined NO_NATIVE_FFT
#define USE_APPLE_FFT
#endif

/**
    Split is a Split for a double-precision FFT.
 */

template <class T> struct Split
{
    Split() {}
    Split(T *real, T *imag) : realp(real), imagp(imag) {}
    /** A pointer to the real portion of the data */
    T *realp;
    /** A pointer to the imaginary portion of the data */
    T *imagp;
};

// Type defintions for Apple / HISSTools FFT

#if defined(USE_APPLE_FFT)

#include <Accelerate/Accelerate.h>

typedef FFTSetupD FFT_SETUP_D;
typedef FFTSetup FFT_SETUP_F;

typedef struct DSPDoubleSplitComplex FFT_SPLIT_COMPLEX_D;
typedef struct DSPSplitComplex FFT_SPLIT_COMPLEX_F;

#else

typedef Split<double> DoubleSplit;
typedef Split<float> FloatSplit;

/**
    FFT_SETUP_D is an opaque setup structure for a double-precision FFT.
 */

typedef struct DoubleSetup *FFT_SETUP_D;

/**
    FFT_SETUP_F is an opaque setup structure for a single-precision FFT.
 */

typedef struct FloatSetup *FFT_SETUP_F;

/**
    FFT_SPLIT_COMPLEX_D is a Structure for storing a double-precision complex array in split form.
 */

typedef DoubleSplit FFT_SPLIT_COMPLEX_D;

/**
    FFT_SPLIT_COMPLEX_F is a Structure for storing a single-precision complex array in split form.
 */
typedef FloatSplit FFT_SPLIT_COMPLEX_F;

#endif

/**
    hisstools_create_setup() creates an FFT setup suitable for double-precision FFTs and iFFTs up to a maximum specified size.
 
	@param	setup           A pointer to an uninitialised FFT_SETUP_D.
	@param	max_fft_log_2   The log base 2 of the FFT size of the maimum FFT size you wish to support..
	
	@remark             On return the object pointed to by setup will be intialsed,
 */

void hisstools_create_setup(FFT_SETUP_D *setup, uintptr_t max_fft_log_2);

/**
 hisstools_create_setup() creates an FFT setup suitable for single-precision FFTs and iFFTs up to a maximum specified size.
 
	@param	setup           A pointer to an uninitialised FFT_SETUP_F.
	@param	max_fft_log_2   The log base 2 of the FFT size of the maimum FFT size you wish to support..
	
	@remark             On return the object pointed to by setup will be intialsed,
 */

void hisstools_create_setup(FFT_SETUP_F *setup, uintptr_t max_fft_log_2);

/**
    hisstools_destroy_setup() destroys a double-precision FFT setup.
 
	@param	setup		A FFT_SETUP_D (double-precision setup).
 
	@remark             After calling this routine the setup is destroyed.
 */

void hisstools_destroy_setup(FFT_SETUP_D setup);

/**
    hisstools_destroy_setup() destroys a single-precision FFT setup.
 
	@param	setup		A FFT_SETUP_F (single-precision setup).
 
	@remark             After calling this routine the setup is destroyed.
 */

void hisstools_destroy_setup(FFT_SETUP_F setup);

/**
    hisstools_fft() performs an in-place complex Fast Fourier Transform.
 
	@param	setup		A FFT_SETUP_D that has been created to deal with an appropriate maximum size of FFT.
	@param	input		A pointer to a FFT_SPLIT_COMPLEX_D structure containing the complex input.
	@param	log2n		The log base 2 of the FFT size.
	
	@remark             The FFT may be performed with either scalar or SIMD instructions. SIMD instuctions will be used when the pointers within the FFT_SPLIT_COMPLEX_D are sixteen byte aligned
 */

void hisstools_fft(FFT_SETUP_D setup, FFT_SPLIT_COMPLEX_D *input, uintptr_t log2n);

/**
    hisstools_fft() performs an in-place complex Fast Fourier Transform.
 
	@param	setup		A FFT_SETUP_F that has been created to deal with an appropriate maximum size of FFT.
	@param	input		A pointer to a FFT_SPLIT_COMPLEX_F structure containing the complex input.
	@param	log2n		The log base 2 of the FFT size.
	
	@remark             The FFT may be performed with scalar or SIMD instructions. SIMD instuctions will be used when the pointers within the FFT_SPLIT_COMPLEX_D are sixteen byte aligned.
 */

void hisstools_fft(FFT_SETUP_F setup, FFT_SPLIT_COMPLEX_F *input, uintptr_t log2n);

/**
    hisstools_rfft() performs an in-place real Fast Fourier Transform.
 
	@param	setup		A FFT_SETUP_D that has been created to deal with an appropriate maximum size of FFT.
	@param	input		A pointer to a FFT_SPLIT_COMPLEX_D structure containing a complex input.
	@param	log2n		The log base 2 of the FFT size.
	
	@remark             The FFT may be performed with either scalar or SIMD instructions. SIMD instuctions will be used when the pointers within the FFT_SPLIT_COMPLEX_D are sixteen byte aligned. Note that the input should first be unzipped into the complex input structure using hisstools_unzip() or hisstools_unzip_zero).
 */

void hisstools_rfft(FFT_SETUP_D setup, FFT_SPLIT_COMPLEX_D *input, uintptr_t log2n);

/**
    hisstools_rfft() performs an in-place real Fast Fourier Transform.
 
	@param	setup		A FFT_SETUP_F that has been created to deal with an appropriate maximum size of FFT.
	@param	input		A pointer to a FFT_SPLIT_COMPLEX_F structure containing a complex input.
	@param	log2n		The log base 2 of the FFT size.
	
	@remark             The FFT may be performed with either scalar or SIMD instructions. SIMD instuctions will be used when the pointers within the FFT_SPLIT_COMPLEX_D are sixteen byte aligned. Note that the input should first be unzipped into the complex input structure using hisstools_unzip() or hisstools_unzip_zero).
 */

void hisstools_rfft(FFT_SETUP_F setup, FFT_SPLIT_COMPLEX_F *input, uintptr_t log2n);

/**
 hisstools_rfft() performs an out-of-place real Fast Fourier Transform.
 
	@param	setup		A FFT_SETUP_F that has been created to deal with an appropriate maximum size of FFT.
	@param	input		A pointer to a real input
 .	@param	output		A pointer to a a FFT_SPLIT_COMPLEX_D structure which will hold the complex output.
	@param	in_length   The length of the input real array.
	@param	log2n		The log base 2 of the FFT size.
	
	@remark             The FFT may be performed with either scalar or SIMD instructions. SIMD instuctions will be used when the pointers within the FFT_SPLIT_COMPLEX_D are sixteen byte aligned.
 */

void hisstools_rfft(FFT_SETUP_D setup, const double *input, FFT_SPLIT_COMPLEX_D *output, uintptr_t in_length, uintptr_t log2n);

/**
 hisstools_rfft() performs an out-of-place real Fast Fourier Transform.
 
	@param	setup		A FFT_SETUP_F that has been created to deal with an appropriate maximum size of FFT.
	@param	input		A pointer to a real input
 .	@param	output		A pointer to a a FFT_SPLIT_COMPLEX_D structure which will hold the complex output.
	@param	in_length   The length of the input real array.
	@param	log2n		The log base 2 of the FFT size.
	
	@remark             The FFT may be performed with either scalar or SIMD instructions. SIMD instuctions will be used when the pointers within the FFT_SPLIT_COMPLEX_D are sixteen byte aligned.
 */

void hisstools_rfft(FFT_SETUP_F setup, const float *input, FFT_SPLIT_COMPLEX_F *output, uintptr_t in_length, uintptr_t log2n);

/**
 hisstools_rfft() performs an out-of-place real Fast Fourier Transform.
 
	@param	setup		A FFT_SETUP_F that has been created to deal with an appropriate maximum size of FFT.
	@param	input		A pointer to a real input
 .	@param	output		A pointer to a a FFT_SPLIT_COMPLEX_D structure which will hold the complex output.
	@param	in_length   The length of the input real array.
	@param	log2n		The log base 2 of the FFT size.
	
	@remark             The FFT may be performed with either scalar or SIMD instructions. SIMD instuctions will be used when the pointers within the FFT_SPLIT_COMPLEX_D are sixteen byte aligned.
 */

void hisstools_rfft(FFT_SETUP_D setup, const float *input, FFT_SPLIT_COMPLEX_D *output, uintptr_t in_length, uintptr_t log2n);

/**
 hisstools_ifft() performs an in-place inverse complex Fast Fourier Transform.
 
	@param	setup		A FFT_SETUP_D that has been created to deal with an appropriate maximum size of FFT.
	@param	input		A pointer to a FFT_SPLIT_COMPLEX_D structure containing a complex input.
	@param	log2n		The log base 2 of the FFT size.
	
	@remark             The inverse FFT may be performed with either scalar or SIMD instructions. SIMD instuctions will be used when the pointers within the FFT_SPLIT_COMPLEX_D are sixteen byte aligned.
 */

void hisstools_ifft(FFT_SETUP_D setup, FFT_SPLIT_COMPLEX_D *input, uintptr_t log2n);

/**
    hisstools_ifft() performs an in-place inverse complex Fast Fourier Transform.
 
	@param	setup		A FFT_SETUP_D that has been created to deal with an appropriate maximum size of FFT.
	@param	input		A pointer to a FFT_SPLIT_COMPLEX_F structure containing a complex input.
	@param	log2n		The log base 2 of the FFT size.
	
	@remark             The inverse FFT may be performed with either scalar or SIMD instructions. SIMD instuctions will be used when the pointers within the FFT_SPLIT_COMPLEX_F are sixteen byte aligned.
 */

void hisstools_ifft(FFT_SETUP_F setup, FFT_SPLIT_COMPLEX_F *input, uintptr_t log2n);

/**
    hisstools_rifft() performs an in-place inverse real Fast Fourier Transform.
 
	@param	setup		A FFT_SETUP_D that has been created to deal with an appropriate maximum size of FFT.
	@param	input		A pointer to a FFT_SPLIT_COMPLEX_D structure containing a complex input.
	@param	log2n		The log base 2 of the FFT size.
	
	@remark             The inverse FFT may be performed with either scalar or SIMD instructions. SIMD instuctions will be used when the pointers within the FFT_SPLIT_COMPLEX_D are sixteen byte aligned. Note that the output will need to be zipped from the complex output structure using hisstools_zip().
 */

void hisstools_rifft(FFT_SETUP_D setup, FFT_SPLIT_COMPLEX_D *input, uintptr_t log2n);

/**
    hisstools_rifft() performs an in-place inverse real Fast Fourier Transform.
 
	@param	setup		A FFT_SETUP_F that has been created to deal with an appropriate maximum size of FFT.
	@param	input		A pointer to a FFT_SPLIT_COMPLEX_F structure containing a complex input.
	@param	log2n		The log base 2 of the FFT size.
	
	@remark             The inverse FFT may be performed with either scalar or SIMD instructions. SIMD instuctions will be used when the pointers within the FFT_SPLIT_COMPLEX_D are sixteen byte aligned. Note that the output will need to be zipped from the complex output structure using hisstools_zip().
 */
 
void hisstools_rifft(FFT_SETUP_F setup, FFT_SPLIT_COMPLEX_F *input, uintptr_t log2n);

/**
    hisstools_rifft() performs an out-out-place inverse real Fast Fourier Transform.
 
	@param	setup		A FFT_SETUP_F that has been created to deal with an appropriate maximum size of FFT.
	@param	input		A pointer to a FFT_SPLIT_COMPLEX_F structure containing a complex input.
	@param	output		A pointer to a real array to hold the output.
	@param	log2n		The log base 2 of the FFT size.
	
	@remark             The inverse FFT may be performed with either scalar or SIMD instructions. SIMD instuctions will be used when the pointers within the FFT_SPLIT_COMPLEX_D are sixteen byte aligned. .
 */

void hisstools_rifft(FFT_SETUP_D setup, FFT_SPLIT_COMPLEX_D *input, double *output, uintptr_t log2n);

/**
 hisstools_rifft() performs an out-out-place inverse real Fast Fourier Transform.
 
	@param	setup		A FFT_SETUP_F that has been created to deal with an appropriate maximum size of FFT.
	@param	input		A pointer to a FFT_SPLIT_COMPLEX_F structure containing a complex input.
	@param	output		A pointer to a real array to hold the output.
	@param	log2n		The log base 2 of the FFT size.
	
	@remark             The inverse FFT may be performed with either scalar or SIMD instructions. SIMD instuctions will be used when the pointers within the FFT_SPLIT_COMPLEX_D are sixteen byte aligned. .
 */

void hisstools_rifft(FFT_SETUP_F setup, FFT_SPLIT_COMPLEX_F *input, float *output, uintptr_t log2n);

/**
 hisstools_unzip_zero() performs unzipping and zero-padding prior to an in-place real FFT.
 
	@param	input		A pointer to the real double-precision input.
	@param	output		A pointer to a FFT_SPLIT_COMPLEX_D structure to unzip to.
	@param	in_length   The actual length of the input
	@param	log2n		The log base 2 of the FFT size.
	
	@remark             Prior to running a real FFT the data must be unzipped from a contiguous memory location into a complex split structure. This function performs unzipping, and zero-pads any remaining input for inputs that may not match the length of the FFT.
 */

void hisstools_unzip_zero(const double *input, FFT_SPLIT_COMPLEX_D *output, uintptr_t in_length, uintptr_t log2n);

/**
    hisstools_unzip_zero() performs unzipping and zero-padding prior to an in-place real FFT.
 
	@param	input		A pointer to the real single-precision input.
	@param	output		A pointer to a FFT_SPLIT_COMPLEX_F structure to unzip to.
	@param	in_length   The actual length of the input
	@param	log2n		The log base 2 of the FFT size.
	
	@remark             Prior to running a real FFT the data must be unzipped from a contiguous memory location into a complex split structure. This function performs unzipping, and zero-pads any remaining input for inputs that may not match the length of the FFT.
 */

void hisstools_unzip_zero(const float *input, FFT_SPLIT_COMPLEX_F *output, uintptr_t in_length, uintptr_t log2n);

/**
    hisstools_unzip_zero() performs unzipping and zero-padding prior to an in-place real FFT.
 
	@param	input		A pointer to the real single-precision input.
	@param	output		A pointer to a FFT_SPLIT_COMPLEX_D structure to unzip to.
	@param	in_length   The actual length of the input
	@param	log2n		The log base 2 of the FFT size.
	
	@remark             Prior to running a real FFT the data must be unzipped from a contiguous memory location into a complex split structure. This function performs unzipping, and zero-pads any remaining input for inputs that may not match the length of the FFT. This version allows a floating point input to be unzipped directly to a double-precision complex split structure.
 */

void hisstools_unzip_zero(const float *input, FFT_SPLIT_COMPLEX_D *output, uintptr_t in_length, uintptr_t log2n);

/**
    hisstools_unzip() performs unzipping prior to an in-place real FFT.
 
	@param	input		A pointer to the real double-precision input.
	@param	output		A pointer to a FFT_SPLIT_COMPLEX_D structure to unzip to.
	@param	log2n		The log base 2 of the FFT size.
	
	@remark             Prior to running a real FFT the data must be unzipped from a contiguous memory location into a complex split structure. This function performs the unzipping. 
 */

void hisstools_unzip(const double *input, FFT_SPLIT_COMPLEX_D *output, uintptr_t log2n);

/**
    hisstools_unzip() performs unzipping prior to an in-place real FFT.
 
	@param	input		A pointer to the real single-precision input.
	@param	output		A pointer to a FFT_SPLIT_COMPLEX_F structure to unzip to.
	@param	log2n		The log base 2 of the FFT size.
	
	@remark             Prior to running a real FFT the data must be unzipped from a contiguous memory location into a complex split structure. This function performs the unzipping.
 */

void hisstools_unzip(const float *input, FFT_SPLIT_COMPLEX_F *output, uintptr_t log2n);

/**
    hisstools_zip() performs zipping subsequent to an in-place real FFT.
 
	@param	input		A pointer to a FFT_SPLIT_COMPLEX_D structure to zip from.
	@param	output		A pointer to the real double-precision output.
	@param	log2n		The log base 2 of the FFT size.
	
	@remark             Subsequent to running a real FFT the data must be zipped from a complex split structue into a contiguous memory location for final output. This function performs the zipping.
 */

void hisstools_zip(const FFT_SPLIT_COMPLEX_D *input, double *output, uintptr_t log2n);

/**
    hisstools_zip() performs zipping subsequent to an in-place real FFT.
 
	@param	input		A pointer to a FFT_SPLIT_COMPLEX_F structure to zip from.
	@param	output		A pointer to the real single-precision output.
	@param	log2n		The log base 2 of the FFT size.
	
	@remark             Subsequent to running a real FFT the data must be zipped from a complex split structue into a contiguous memory location for final output. This function performs the zipping.
 */

void hisstools_zip(const FFT_SPLIT_COMPLEX_F *input, float *output, uintptr_t log2n);

#endif

