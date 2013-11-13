
#include "FFT_Header.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FFT_FUNC_NAME(pass_1_2_reorder) (FFT_SPLIT_TYPE *input, HstFFT_UInt length, HstFFT_UInt fft_log2)
{	
	HstFFT_UInt i, j, loop;
	HstFFT_UInt offset = length >> 1;
	
	FFT_TYPE *r1_ptr = input->realp;
	FFT_TYPE *i1_ptr = input->imagp;
	FFT_TYPE *r2_ptr = r1_ptr + offset;
	FFT_TYPE *i2_ptr = i1_ptr + offset;
	
	FFT_TYPE r1, r2, r3, r4, r5, r6, r7, r8;
	FFT_TYPE i1, i2, i3, i4, i5, i6, i7, i8;
	
	for (i = 0; i < (length >> 2); i++)
	{
		// Pass One
		
		// Real
		
		r1 = *r1_ptr;
		r2 = *r2_ptr;
		r3 = *(r1_ptr + 1);
		r4 = *(r2_ptr + 1);
		
		*r1_ptr++ = r1 + r2;
		*r1_ptr++ = r1 - r2;
		*r2_ptr++ = r3 + r4;
		*r2_ptr++ = r3 - r4;
		
		// Imaginary
		
		i1 = *i1_ptr;
		i2 = *i2_ptr;
		i3 = *(i1_ptr + 1);
		i4 = *(i2_ptr + 1);
		
		*i1_ptr++ = i1 + i2;
		*i1_ptr++ = i1 - i2;
		*i2_ptr++ = i3 + i4;
		*i2_ptr++ = i3 - i4;
	}
	
	// Pass Two
	
	offset >>= 1;
	
	r1_ptr = input->realp;
	i1_ptr = input->imagp;
	r2_ptr = r1_ptr + offset;
	i2_ptr = i1_ptr + offset;
	
	for (j = 0, loop = (length >> 4); j < 2; j++) 
	{
		for (i = 0; i < loop; i++)
		{
			// Get Real
			
			r1 = *(r1_ptr + 0);
			r2 = *(r1_ptr + 1);
			r3 = *(r2_ptr + 0);
			r4 = *(r2_ptr + 1);
			r5 = *(r1_ptr + 2);
			r6 = *(r1_ptr + 3);
			r7 = *(r2_ptr + 2);
			r8 = *(r2_ptr + 3);
			
			// Get Imaginary
			
			i1 = *(i1_ptr + 0);
			i2 = *(i1_ptr + 1);
			i3 = *(i2_ptr + 0);
			i4 = *(i2_ptr + 1);
			i5 = *(i1_ptr + 2);
			i6 = *(i1_ptr + 3);
			i7 = *(i2_ptr + 2);
			i8 = *(i2_ptr + 3);
			
			// Store Real
			
			*r1_ptr++ = r1 + r3;
			*r1_ptr++ = r2 + i4;
			*r1_ptr++ = r1 - r3;
			*r1_ptr++ = r2 - i4;
			*r2_ptr++ = r5 + r7;
			*r2_ptr++ = r6 + i8;
			*r2_ptr++ = r5 - r7;
			*r2_ptr++ = r6 - i8;
			
			// Store Imaginary
			
			*i1_ptr++ = i1 + i3;
			*i1_ptr++ = i2 - r4;
			*i1_ptr++ = i1 - i3;
			*i1_ptr++ = i2 + r4;
			*i2_ptr++ = i5 + i7;
			*i2_ptr++ = i6 - r8;
			*i2_ptr++ = i5 - i7;
			*i2_ptr++ = i6 + r8;
		}
		
		r1_ptr += offset;
		i1_ptr += offset;
		r2_ptr += offset;
		i2_ptr += offset;
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void FFT_FUNC_NAME (pass_3_reorder) (FFT_SPLIT_TYPE *input, HstFFT_UInt length, HstFFT_UInt fft_log2)
{		
	HstFFT_UInt offset = (HstFFT_UInt) 1 << (fft_log2 - 3);
	HstFFT_UInt i, j, loop;
	
	FFT_TYPE sqrt_2_2 = (FFT_TYPE) SQRT_2_2;
	FFT_TYPE r1, r2, r3, r4, r5, r6, r7, r8;
	FFT_TYPE r9, r10, r11, r12, r13, r14, r15, r16;
	FFT_TYPE i1, i2, i3, i4, i5, i6, i7, i8;
	FFT_TYPE i9, i10, i11, i12, i13, i14, i15, i16;
	FFT_TYPE t1, t2, t3, t4;
	
	FFT_TYPE *r1_ptr = input->realp;
	FFT_TYPE *i1_ptr = input->imagp;
	
	// Pass Three
	
	FFT_TYPE *r2_ptr = r1_ptr + offset;
	FFT_TYPE *i2_ptr = i1_ptr + offset;
	
	for (j = 0, loop = length >> 6; j < 4; j++)
	{		
		for (i = 0; i < loop; i++)
		{
			// Get Real
			
			r1 = *(r1_ptr + 0);
			r2 = *(r1_ptr + 1);
			r3 = *(r1_ptr + 2);
			r4 = *(r1_ptr + 3);
			
			r9 = *(r1_ptr + 4);
			r10 = *(r1_ptr + 5);
			r11 = *(r1_ptr + 6);
			r12 = *(r1_ptr + 7);
			
			r5 = *(r2_ptr + 0);
			r6 = *(r2_ptr + 1);
			r7 = *(r2_ptr + 2);
			r8 = *(r2_ptr + 3);
			
			r13 = *(r2_ptr + 4);
			r14 = *(r2_ptr + 5);
			r15 = *(r2_ptr + 6);
			r16 = *(r2_ptr + 7);
			
			// Get Imaginary
			
			i1 = *(i1_ptr + 0);
			i2 = *(i1_ptr + 1);
			i3 = *(i1_ptr + 2);
			i4 = *(i1_ptr + 3);
			
			i9 = *(i1_ptr + 4);
			i10 = *(i1_ptr + 5);
			i11 = *(i1_ptr + 6);
			i12 = *(i1_ptr + 7);
	
			i5 = *(i2_ptr + 0);
			i6 = *(i2_ptr + 1);
			i7 = *(i2_ptr + 2);
			i8 = *(i2_ptr + 3);
			
			i13 = *(i2_ptr + 4);
			i14 = *(i2_ptr + 5);
			i15 = *(i2_ptr + 6);
			i16 = *(i2_ptr + 7);
			
			t1 = sqrt_2_2 * (r6 + i6);
			t2 = sqrt_2_2 * (i8 - r8);
			t3 = sqrt_2_2 * (r14 + i14);
			t4 = sqrt_2_2 * (i16 - r16);
			
			*r1_ptr++ = r1 + r5;
			*r1_ptr++ = r2 + t1;
			*r1_ptr++ = r3 + i7;
			*r1_ptr++ = r4 + t2;
			*r1_ptr++ = r1 - r5;
			*r1_ptr++ = r2 - t1;
			*r1_ptr++ = r3 - i7;
			*r1_ptr++ = r4 - t2;
			
			*r2_ptr++ = r9 + r13;
			*r2_ptr++ = r10 + t3;
			*r2_ptr++ = r11 + i15;
			*r2_ptr++ = r12 + t4;
			*r2_ptr++ = r9 - r13;
			*r2_ptr++ = r10 - t3;
			*r2_ptr++ = r11 - i15;
			*r2_ptr++ = r12 - t4;
			
			// Imaginary
			
			t1 = sqrt_2_2 * (i6 - r6);
			t2 = -sqrt_2_2 * (r8 + i8);
			t3 = sqrt_2_2 * (i14 - r14);
			t4 = -sqrt_2_2 * (r16 + i16);
			
			*i1_ptr++ = i1 + i5;
			*i1_ptr++ = i2 + t1;
			*i1_ptr++ = i3 - r7;
			*i1_ptr++ = i4 + t2;
			*i1_ptr++ = i1 - i5;
			*i1_ptr++ = i2 - t1;
			*i1_ptr++ = i3 + r7;
			*i1_ptr++ = i4 - t2;
			
			*i2_ptr++ = i9 + i13;
			*i2_ptr++ = i10 + t3;
			*i2_ptr++ = i11 - r15;
			*i2_ptr++ = i12 + t4;
			*i2_ptr++ = i9 - i13;
			*i2_ptr++ = i10 - t3;
			*i2_ptr++ = i11 + r15;
			*i2_ptr++ = i12 - t4;
		}
		
		r1_ptr += offset;
		i1_ptr += offset;
		r2_ptr += offset;
		i2_ptr += offset;		
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void FFT_FUNC_NAME (pass_3) (FFT_SPLIT_TYPE *input, HstFFT_UInt length, HstFFT_UInt fft_log2)
{	
	HstFFT_UInt i;

	FFT_TYPE sqrt_2_2 = (FFT_TYPE) SQRT_2_2;
	FFT_TYPE r1, r2, r3, r4, r5, r6, r7, r8;
	FFT_TYPE i1, i2, i3, i4, i5, i6, i7, i8;
	FFT_TYPE t1, t2;
	
	FFT_TYPE *r1_ptr = input->realp;
	FFT_TYPE *i1_ptr = input->imagp;
	
	for (i = 0; i < length >> 3; i++)
	{
		// Pass Three
		
		r1 = *(r1_ptr + 0);
		r2 = *(r1_ptr + 1);
		r3 = *(r1_ptr + 2);
		r4 = *(r1_ptr + 3);
		r5 = *(r1_ptr + 4);
		r6 = *(r1_ptr + 5);
		r7 = *(r1_ptr + 6);
		r8 = *(r1_ptr + 7);
		
		i1 = *(i1_ptr + 0);
		i2 = *(i1_ptr + 1);
		i3 = *(i1_ptr + 2);
		i4 = *(i1_ptr + 3);
		i5 = *(i1_ptr + 4);
		i6 = *(i1_ptr + 5);
		i7 = *(i1_ptr + 6);
		i8 = *(i1_ptr + 7);
		
		// Real
		
		t1 = sqrt_2_2 * (r6 + i6);
		t2 = sqrt_2_2 * (i8 - r8);
		
		*r1_ptr++ = r1 + r5;
		*r1_ptr++ = r2 + t1;
		*r1_ptr++ = r3 + i7;
		*r1_ptr++ = r4 + t2;
		*r1_ptr++ = r1 - r5;
		*r1_ptr++ = r2 - t1;
		*r1_ptr++ = r3 - i7;
		*r1_ptr++ = r4 - t2;
		
		// Imaginary
		
		t1 = sqrt_2_2 * (i6 - r6);
		t2 = -sqrt_2_2 * (r8 + i8);
		
		*i1_ptr++ = i1 + i5;
		*i1_ptr++ = i2 + t1;
		*i1_ptr++ = i3 - r7;
		*i1_ptr++ = i4 + t2;
		*i1_ptr++ = i1 - i5;
		*i1_ptr++ = i2 - t1;
		*i1_ptr++ = i3 + r7;
		*i1_ptr++ = i4 - t2;
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void FFT_FUNC_NAME (pass_trig_table_reorder) (FFT_SPLIT_TYPE *input, FFT_SETUP_TYPE *setup, HstFFT_UInt length, HstFFT_UInt pass)
{	
	HstFFT_UInt size = 2 << pass;
	HstFFT_UInt incr = size >> 1;
	HstFFT_UInt loop = size;
	HstFFT_UInt offset = length >> (pass + 1);
	HstFFT_UInt outer_loop = ((length >> 1) / size) / ((HstFFT_UInt) 1 << pass);
	HstFFT_UInt i, j;
	
	FFT_TYPE r1, r2, r3, r4, r5; 
	FFT_TYPE i1, i2, i3, i4, i5;
	FFT_TYPE twiddle_c, twiddle_s;
	
	FFT_TYPE *r1_ptr = input->realp;
	FFT_TYPE *i1_ptr = input->imagp;
	FFT_TYPE *r2_ptr = r1_ptr + incr;
	FFT_TYPE *i2_ptr = i1_ptr + incr;
	FFT_TYPE *r3_ptr = r1_ptr + offset;
	FFT_TYPE *i3_ptr = i1_ptr + offset;
	FFT_TYPE *r4_ptr = r3_ptr + incr;   
	FFT_TYPE *i4_ptr = i3_ptr + incr;
	
	for (j = 0, i = 0; i < length >> 1; loop += size)
	{		
		FFT_TYPE *t_ptr_r = setup->tables[pass - PASS_TRIG_OFFSET].realp; 
		FFT_TYPE *t_ptr_i = setup->tables[pass - PASS_TRIG_OFFSET].imagp;
		
		for (; i < loop; i += 2)
		{
			// Get input
			
			r1 = *r1_ptr;
			i1 = *i1_ptr;
			r2 = *r3_ptr;
			i2 = *i3_ptr;
			
			// Multiply by twiddle
			
			twiddle_c = *t_ptr_r++;
			twiddle_s = *t_ptr_i++;
			
			r5 = (r2 * twiddle_c) - (i2 * twiddle_s);
			i5 = (r2 * twiddle_s) + (i2 * twiddle_c);
			
			// Get input
			
			r3 = *r2_ptr;
			i3 = *i2_ptr;
			r4 = *r4_ptr;
			i4 = *i4_ptr;
			
			// Store output (same pos as inputs)
			
			*r1_ptr++ = r1 + r5;
			*i1_ptr++ = i1 + i5;
		
			*r2_ptr++ = r1 - r5;
			*i2_ptr++ = i1 - i5;
			
			// Multiply by twiddle
			
			r5 = (r4 * twiddle_c) - (i4 * twiddle_s);
			i5 = (r4 * twiddle_s) + (i4 * twiddle_c);
			
			// Store output (same pos as inputs)
			
			*r3_ptr++ = r3 + r5;
			*i3_ptr++ = i3 + i5;
			
			*r4_ptr++ = r3 - r5;
			*i4_ptr++ = i3 - i5;
		}
		
		r1_ptr += incr;
		r2_ptr += incr;
		r3_ptr += incr;
		r4_ptr += incr;
		i1_ptr += incr;
		i2_ptr += incr;
		i3_ptr += incr;
		i4_ptr += incr;
		
		if (!(++j % outer_loop))
		{
			r1_ptr += offset;
			r2_ptr += offset;
			r3_ptr += offset;
			r4_ptr += offset;
			i1_ptr += offset;
			i2_ptr += offset;
			i3_ptr += offset;
			i4_ptr += offset;
			
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void FFT_FUNC_NAME (pass_trig_table) (FFT_SPLIT_TYPE *input, FFT_SETUP_TYPE *setup, HstFFT_UInt length, HstFFT_UInt pass)
{	
	HstFFT_UInt size = 2 << pass;
	HstFFT_UInt incr = size >> 1;
	HstFFT_UInt loop = size;
	
	double r1, r2, i1, i2, r_0, i_0;
	double twiddle_c, twiddle_s;
	
	FFT_TYPE *r1_ptr = input->realp;
	FFT_TYPE *i1_ptr = input->imagp;
	FFT_TYPE *r2_ptr = r1_ptr + (size >> 1);
	FFT_TYPE *i2_ptr = i1_ptr + (size >> 1);
	
	HstFFT_UInt i;
	
	for (i = 0; i < length; loop += size)
	{		
		FFT_TYPE *tr_ptr = setup->tables[pass - PASS_TRIG_OFFSET].realp;
		FFT_TYPE *ti_ptr = setup->tables[pass - PASS_TRIG_OFFSET].imagp;
		
		for (; i < loop; i += 2)
		{
			twiddle_c = *tr_ptr++;
			twiddle_s = *ti_ptr++;
			
			// Get input
			
			r1 = *r1_ptr;
			i1 = *i1_ptr;
			r2 = *r2_ptr;
			i2 = *i2_ptr;
			
			// Multiply by twiddle
			
			r_0 = (r2 * twiddle_c) - (i2 * twiddle_s);
			i_0 = (r2 * twiddle_s) + (i2 * twiddle_c);
			
			// Store output (same pos as inputs)
			
			*r1_ptr++ = (FFT_TYPE) (r1 + r_0);
			*i1_ptr++ = (FFT_TYPE) (i1 + i_0);
			
			*r2_ptr++ = (FFT_TYPE) (r1 - r_0);
			*i2_ptr++ = (FFT_TYPE) (i1 - i_0);
		}
		
		
		r1_ptr += incr;
		r2_ptr += incr;
		i1_ptr += incr;
		i2_ptr += incr;
	}
}
