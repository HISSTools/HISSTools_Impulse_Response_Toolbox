
#include "FFT_Header.h"
#include "HISSTools_FFT_SIMD.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void FFT_FUNC_NAME(fft_fill_table) (FFT_SPLIT_TYPE *table, HstFFT_UInt length)
{	
	HstFFT_UInt i;
	
	FFT_TYPE *table_cos = table->realp;
	FFT_TYPE *table_sin = table->imagp;
	
	for (i = 0; i < length; i++)
	{
		double angle = -((double) i) * M_PI / (double) length;
		
		*table_cos++ = (float) cos(angle);
		*table_sin++ = (float) sin(angle);
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


FFT_SETUP_TYPE *FFT_FUNC_NAME(create_setup) (HstFFT_UInt max_fft_log2) 
{
	FFT_SETUP_TYPE *setup = ALIGNED_MALLOC(sizeof(FFT_SETUP_TYPE));
	HstFFT_UInt i;
		
	// Check for SSE here (this must be called anyway before doing an FFT)
	
	AHFFT_SSE_Exists = SSE2_check();
	
	// Create Tables
	
	for (i = FFTLOG2_TRIG_OFFSET; i <= max_fft_log2; i++)
	{
		HstFFT_UInt length = (HstFFT_UInt) 1 << (i - 1);
		
		setup->tables[i - FFTLOG2_TRIG_OFFSET].realp = ALIGNED_MALLOC(sizeof(double) * 2 * length);
		setup->tables[i - FFTLOG2_TRIG_OFFSET].imagp = setup->tables[i - FFTLOG2_TRIG_OFFSET].realp + length;
		
		FFT_FUNC_NAME(fft_fill_table) (&setup->tables[i - FFTLOG2_TRIG_OFFSET], length);
	}
	
	setup->max_fft_log2 = max_fft_log2;
	
	return setup;
}	


void FFT_FUNC_NAME(destroy_setup) (FFT_SETUP_TYPE *setup) 
{
	HstFFT_UInt max_fft_log2 = setup->max_fft_log2;
	HstFFT_UInt i;
	
	for (i = FFTLOG2_TRIG_OFFSET; i <= max_fft_log2; i++)
		ALIGNED_FREE(setup->tables[i - FFTLOG2_TRIG_OFFSET].realp);
	
	ALIGNED_FREE(setup);
}

