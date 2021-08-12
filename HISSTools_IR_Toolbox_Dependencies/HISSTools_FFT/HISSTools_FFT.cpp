
#include "HISSTools_FFT.h"
#include "HISSTools_FFT_Core.h"

#if defined(USE_APPLE_FFT)

// This file provides bindings to the relevant Apple or HISSTools template routines.

// User FFT Routines

void hisstools_fft(FFT_SETUP_D setup, FFT_SPLIT_COMPLEX_D *input, uintptr_t log2n)
{
	vDSP_fft_zipD(setup, input, (vDSP_Stride) 1, log2n, FFT_FORWARD);
}

void hisstools_fft(FFT_SETUP_F setup, FFT_SPLIT_COMPLEX_F *input, uintptr_t log2n)
{
	vDSP_fft_zip(setup, input, (vDSP_Stride) 1, log2n, FFT_FORWARD);
}

void hisstools_rfft(FFT_SETUP_D setup, FFT_SPLIT_COMPLEX_D *input, uintptr_t log2n)
{
	vDSP_fft_zripD(setup, input, (vDSP_Stride) 1, log2n, FFT_FORWARD);
}

void hisstools_rfft(FFT_SETUP_F setup, FFT_SPLIT_COMPLEX_F *input, uintptr_t log2n)
{
	vDSP_fft_zrip(setup, input, (vDSP_Stride) 1, log2n, FFT_FORWARD);
}

void hisstools_ifft(FFT_SETUP_D setup, FFT_SPLIT_COMPLEX_D *input, uintptr_t log2n)
{
	vDSP_fft_zipD(setup, input, (vDSP_Stride) 1, log2n, FFT_INVERSE);
}

void hisstools_ifft(FFT_SETUP_F setup, FFT_SPLIT_COMPLEX_F *input, uintptr_t log2n)
{
	vDSP_fft_zip(setup, input, (vDSP_Stride) 1, log2n, FFT_INVERSE);
}

void hisstools_rifft(FFT_SETUP_D setup, FFT_SPLIT_COMPLEX_D *input, uintptr_t log2n)
{
	vDSP_fft_zripD(setup, input, (vDSP_Stride) 1, log2n, FFT_INVERSE);
}	

void hisstools_rifft(FFT_SETUP_F setup, FFT_SPLIT_COMPLEX_F *input, uintptr_t log2n)
{
	vDSP_fft_zrip(setup, input, (vDSP_Stride) 1, log2n, FFT_INVERSE);
}

// Zip and Unzip

void hisstools_unzip(const double *input, FFT_SPLIT_COMPLEX_D *output, uintptr_t log2n)
{
    vDSP_ctozD((DOUBLE_COMPLEX *) input, (vDSP_Stride) 2, output, (vDSP_Stride) 1, (vDSP_Length) (1 << (log2n - 1)));
}

void hisstools_unzip(const float *input, FFT_SPLIT_COMPLEX_F *output, uintptr_t log2n)
{
    vDSP_ctoz((COMPLEX *) input, (vDSP_Stride) 2, output, (vDSP_Stride) 1, (vDSP_Length) (1 << (log2n - 1)));
}

void hisstools_zip(const FFT_SPLIT_COMPLEX_D *input, double *output, uintptr_t log2n)
{
    vDSP_ztocD(input, (vDSP_Stride) 1, (DOUBLE_COMPLEX *) output, (vDSP_Stride) 2, (vDSP_Length) (1 << (log2n - 1)));
}

void hisstools_zip(const FFT_SPLIT_COMPLEX_F *input, float *output, uintptr_t log2n)
{
    vDSP_ztoc(input, (vDSP_Stride) 1, (COMPLEX *) output, (vDSP_Stride) 2, (vDSP_Length) (1 << (log2n - 1)));
}

// Setup Create / Destroy

void hisstools_create_setup(FFT_SETUP_D *setup, uintptr_t max_fft_log_2)
{
    *setup = vDSP_create_fftsetupD(max_fft_log_2, FFT_RADIX2);
}

void hisstools_create_setup(FFT_SETUP_F *setup, uintptr_t max_fft_log_2)
{
    *setup = vDSP_create_fftsetup(max_fft_log_2, FFT_RADIX2);
}

void hisstools_destroy_setup(FFT_SETUP_D setup)
{
    if (setup)
        vDSP_destroy_fftsetupD(setup);
}

void hisstools_destroy_setup(FFT_SETUP_F setup)
{
    if (setup)
        vDSP_destroy_fftsetup(setup);
}

// Zip and Unzip

template <class V> void unzipComplex(const float *input, FFT_SPLIT_COMPLEX_F *output, uintptr_t half_length)
{
    vDSP_ctoz((COMPLEX *) input, (vDSP_Stride) 2, output, (vDSP_Stride) 1, half_length);
}

template <class V> void unzipComplex(const double *input, FFT_SPLIT_COMPLEX_D *output, uintptr_t half_length)
{
    vDSP_ctozD((DOUBLE_COMPLEX *) input, (vDSP_Stride) 2, output, (vDSP_Stride) 1, half_length);
}

#else

// User FFT Routines

void hisstools_fft(FFT_SETUP_D setup, FFT_SPLIT_COMPLEX_D *input, uintptr_t log2n)
{
    hisstools_fft_impl::hisstools_fft(input, setup, log2n);
}

void hisstools_fft(FFT_SETUP_F setup, FFT_SPLIT_COMPLEX_F *input, uintptr_t log2n)
{
    hisstools_fft_impl::hisstools_fft(input, setup, log2n);
}

void hisstools_rfft(FFT_SETUP_D setup, FFT_SPLIT_COMPLEX_D *input, uintptr_t log2n)
{
    hisstools_fft_impl::hisstools_rfft(input, setup, log2n);
}

void hisstools_rfft(FFT_SETUP_F setup, FFT_SPLIT_COMPLEX_F *input, uintptr_t log2n)
{
    hisstools_fft_impl::hisstools_rfft(input, setup, log2n);
}

void hisstools_ifft(FFT_SETUP_D setup, FFT_SPLIT_COMPLEX_D *input, uintptr_t log2n)
{
    hisstools_fft_impl::hisstools_ifft(input, setup, log2n);
}

void hisstools_ifft(FFT_SETUP_F setup, FFT_SPLIT_COMPLEX_F *input, uintptr_t log2n)
{
    hisstools_fft_impl::hisstools_ifft(input, setup, log2n);
}

void hisstools_rifft(FFT_SETUP_D setup, FFT_SPLIT_COMPLEX_D *input, uintptr_t log2n)
{
    hisstools_fft_impl::hisstools_rifft(input, setup, log2n);
}

void hisstools_rifft(FFT_SETUP_F setup, FFT_SPLIT_COMPLEX_F *input, uintptr_t log2n)
{
    hisstools_fft_impl::hisstools_rifft(input, setup, log2n);
}

// Zip and Unzip

void hisstools_unzip(const double *input, FFT_SPLIT_COMPLEX_D *output, uintptr_t log2n)
{
    hisstools_fft_impl::unzip_complex<double>(input, output, (uintptr_t) 1 << (log2n - (uintptr_t) 1));
}

void hisstools_unzip(const float *input, FFT_SPLIT_COMPLEX_F *output, uintptr_t log2n)
{
    hisstools_fft_impl::unzip_complex<float>(input, output, (uintptr_t) 1 << (log2n - (uintptr_t) 1));
}

void hisstools_zip(const FFT_SPLIT_COMPLEX_D *input, double *output, uintptr_t log2n)
{
    hisstools_fft_impl::zip_complex(input, output, (uintptr_t) 1 << (log2n - (uintptr_t) 1));
}

void hisstools_zip(const FFT_SPLIT_COMPLEX_F *input, float *output, uintptr_t log2n)
{
    hisstools_fft_impl::zip_complex(input, output, (uintptr_t) 1 << (log2n - (uintptr_t) 1));
}

// Setup Create / Destroy

void hisstools_create_setup(FFT_SETUP_D *setup, uintptr_t max_fft_log_2)
{
    *setup = static_cast<FFT_SETUP_D>(hisstools_fft_impl::create_setup<double>(max_fft_log_2));
}

void hisstools_create_setup(FFT_SETUP_F *setup, uintptr_t max_fft_log_2)
{
    *setup = static_cast<FFT_SETUP_F>(hisstools_fft_impl::create_setup<float>(max_fft_log_2));
}

void hisstools_destroy_setup(FFT_SETUP_D setup)
{
    hisstools_fft_impl::destroy_setup(setup);
}

void hisstools_destroy_setup(FFT_SETUP_F setup)
{
    hisstools_fft_impl::destroy_setup(setup);
}

#endif

// Unzip incorporating zero padding

void hisstools_unzip_zero(const double *input, FFT_SPLIT_COMPLEX_D *output, uintptr_t in_length, uintptr_t log2n)
{
    hisstools_fft_impl::unzip_zero<double>(input, output, in_length, log2n);
}

void hisstools_unzip_zero(const float *input, FFT_SPLIT_COMPLEX_F *output, uintptr_t in_length, uintptr_t log2n)
{
    hisstools_fft_impl::unzip_zero<float>(input, output, in_length, log2n);
}

// N.B This routine specifically deals with unzipping float data into a double precision complex split format

void hisstools_unzip_zero(const float *input, FFT_SPLIT_COMPLEX_D *output, uintptr_t in_length, uintptr_t log2n)
{
    hisstools_fft_impl::unzip_zero<double>(input, output, in_length, log2n);
}

// Convenience Real FFT Functions

void hisstools_rfft(FFT_SETUP_D setup, const double *input, FFT_SPLIT_COMPLEX_D *output, uintptr_t in_length, uintptr_t log2n)
{
    hisstools_unzip_zero(input, output, in_length, log2n);
    hisstools_rfft(setup, output, log2n);
}

void hisstools_rfft(FFT_SETUP_F setup, const float *input, FFT_SPLIT_COMPLEX_F *output, uintptr_t in_length, uintptr_t log2n)
{
    hisstools_unzip_zero(input, output, in_length, log2n);
    hisstools_rfft(setup, output, log2n);
}

void hisstools_rfft(FFT_SETUP_D setup, const float *input, FFT_SPLIT_COMPLEX_D *output, uintptr_t in_length, uintptr_t log2n)
{
    hisstools_unzip_zero(input, output, in_length, log2n);
    hisstools_rfft(setup, output, log2n);
}

void hisstools_rifft(FFT_SETUP_D setup, FFT_SPLIT_COMPLEX_D *input, double *output, uintptr_t log2n)
{
    hisstools_rifft(setup, input, log2n);
    hisstools_zip(input, output, log2n);
}

void hisstools_rifft(FFT_SETUP_F setup, FFT_SPLIT_COMPLEX_F *input, float *output, uintptr_t log2n)
{
    hisstools_rifft(setup, input, log2n);
    hisstools_zip(input, output, log2n);
}
