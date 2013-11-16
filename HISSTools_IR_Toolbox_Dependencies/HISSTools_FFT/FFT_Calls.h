
#include "FFT_Header.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FFT_FUNC_NAME(do_small_fft) (FFT_SPLIT_TYPE *input, HstFFT_UInt fft_log2)
{
	FFT_TYPE r1, r2, r3, r4, r5, r6, r7, r8;
	FFT_TYPE i1, i2, i3, i4, i5, i6, i7, i8;
	FFT_TYPE t1, t2, t3, t4;
	
	FFT_TYPE *realp = input->realp;
	FFT_TYPE *imagp = input->imagp;
	
	if (fft_log2 < 1)
		return;

	switch (fft_log2)
	{
		case 1:
			
			r1 = realp[0];
			r2 = realp[1];
			i1 = imagp[0];
			i2 = imagp[1];
			
			realp[0] = r1 + r2;
			realp[1] = r1 - r2;
			imagp[0] = i1 + i2;
			imagp[1] = i1 - i2;
			
			break;
						
		case 2:
			
			r5 = realp[0];
			r6 = realp[1];
			r2 = realp[2];
			r4 = realp[3];
			i5 = imagp[0];
			i6 = imagp[1];
			i2 = imagp[2];
			i4 = imagp[3];
			
			// Pass One
			
			r1 = r5 + r2;
			r2 = r5 - r2;
			r3 = r6 + r4;
			r4 = r6 - r4;
			i1 = i5 + i2;
			i2 = i5 - i2;
			i3 = i6 + i4;
			i4 = i6 - i4;
			
			// Pass Two
			
			realp[0] = r1 + r3;
			realp[1] = r2 + i4;
			realp[2] = r1 - r3;
			realp[3] = r2 - i4;
			imagp[0] = i1 + i3;
			imagp[1] = i2 - r4;
			imagp[2] = i1 - i3;
			imagp[3] = i2 + r4;
			
			break;

		case 3:
			
			t1 = realp[0];
			t3 = realp[1];
			t2 = realp[2];
			t4 = realp[3];
			r2 = realp[4];
			r6 = realp[5];
			r4 = realp[6];
			r8 = realp[7];
			
			// Pass One
			
			r1 = t1 + r2;
			r2 = t1 - r2;
			r3 = t2 + r4;
			r4 = t2 - r4;
			r5 = t3 + r6;
			r6 = t3 - r6;
			r7 = t4 + r8;
			r8 = t4 - r8;
			
			t1 = imagp[0];
			t3 = imagp[1];
			t2 = imagp[2];
			t4 = imagp[3];
			i2 = imagp[4];
			i6 = imagp[5];
			i4 = imagp[6];
			i8 = imagp[7];
			
			i1 = t1 + i2;
			i2 = t1 - i2;
			i3 = t2 + i4;
			i4 = t2 - i4;
			i5 = t3 + i6;
			i6 = t3 - i6;
			i7 = t4 + i8;
			i8 = t4 - i8;
			
			// Pass Two
			
			realp[0] = r1 + r3;
			realp[1] = r2 + i4;
			realp[2] = r1 - r3;
			realp[3] = r2 - i4;
			realp[4] = r5 + r7;
			realp[5] = r6 + i8;
			realp[6] = r5 - r7;
			realp[7] = r6 - i8;
			
			imagp[0] = i1 + i3;
			imagp[1] = i2 - r4;
			imagp[2] = i1 - i3;
			imagp[3] = i2 + r4;
			imagp[4] = i5 + i7;
			imagp[5] = i6 - r8;
			imagp[6] = i5 - i7;
			imagp[7] = i6 + r8;
			
			// Pass Three
			
			FFT_FUNC_NAME (pass_3) (input, (HstFFT_UInt) 8, (HstFFT_UInt) 3);
			
			break;
			
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void FFT_FUNC_NAME (fft_internal) (FFT_SPLIT_TYPE *input, FFT_SETUP_TYPE *setup, HstFFT_UInt fft_log2)
{
	HstFFT_UInt length = (HstFFT_UInt) 1 << fft_log2;
	HstFFT_UInt i;
	
	if (fft_log2 < 4)
	{
		FFT_FUNC_NAME (do_small_fft) (input, fft_log2);
		return;
	}
	
#ifdef VECTOR_F64_128BIT
	if ((HstFFT_UInt) input->realp % 16 || (HstFFT_UInt) input->imagp % 16 || !AHFFT_SSE_Exists)
#endif
	{
		FFT_FUNC_NAME (pass_1_2_reorder) (input, length, fft_log2);
		
		if (fft_log2 > 5)
			FFT_FUNC_NAME (pass_3_reorder) (input, length, fft_log2);
		else
			FFT_FUNC_NAME (pass_3) (input, length, fft_log2);
		
		for (i = 3; i < (fft_log2 >> 1); i++)
			FFT_FUNC_NAME(pass_trig_table_reorder) (input, setup, length, i);
		
		for (; i < fft_log2; i++)
			FFT_FUNC_NAME(pass_trig_table) (input, setup, length, i);
	}
#ifdef VECTOR_F64_128BIT
	else 
	{
		FFT_FUNC_NAME (pass_1_2_reorder_simd) (input, length);

		if (fft_log2 > 5)
			FFT_FUNC_NAME (pass_3_reorder_simd) (input, length);
		else
			FFT_FUNC_NAME (pass_3) (input, length, fft_log2);
	
		for (i = 3; i < (fft_log2 >> 1); i++)
			FFT_FUNC_NAME(pass_trig_table_reorder_simd) (input, setup, length, i);
		
		for (; i < fft_log2; i++)
			FFT_FUNC_NAME(pass_trig_table_simd) (input, setup, length, i);
	}
#endif	
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void FFT_FUNC_NAME(do_fft) (FFT_SPLIT_TYPE *input, FFT_SETUP_TYPE *setup, HstFFT_UInt fft_log2)
{
	FFT_FUNC_NAME(fft_internal) (input, setup, fft_log2);
}



void FFT_FUNC_NAME(do_ifft) (FFT_SPLIT_TYPE *input, FFT_SETUP_TYPE *setup, HstFFT_UInt fft_log2)
{
	FFT_SPLIT_TYPE swap;
	
	swap.realp = input->imagp;
	swap.imagp = input->realp;
	
	FFT_FUNC_NAME(fft_internal) (&swap, setup, fft_log2);
}
