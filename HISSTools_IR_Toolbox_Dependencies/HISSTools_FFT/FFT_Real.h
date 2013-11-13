
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void FFT_FUNC_NAME(unzip_complex) (FFT_TYPE *input, FFT_SPLIT_TYPE *output, HstFFT_UInt half_length)
{
	HstFFT_UInt i;
	
	FFT_TYPE *realp = output->realp;
	FFT_TYPE *imagp = output->imagp;
	
	for (i = 0; i < half_length; i++)
	{
		*realp++ = *input++;
		*imagp++ = *input++;
	}
}


void FFT_FUNC_NAME(zip_complex) (FFT_SPLIT_TYPE *input, FFT_TYPE *output, HstFFT_UInt half_length)
{
	HstFFT_UInt i;
	
	FFT_TYPE *realp = input->realp;
	FFT_TYPE *imagp = input->imagp;
	
	for (i = 0; i < half_length; i++)
	{
		*output++ = *realp++;
		*output++ = *imagp++;
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void FFT_FUNC_NAME(pass_real_trig_table) (FFT_SPLIT_TYPE *input, FFT_SETUP_TYPE *setup, HstFFT_UInt fft_log2, long ifft)
{
	HstFFT_UInt length = (HstFFT_UInt) 1 << (fft_log2 - 1);
	HstFFT_UInt length_m1 = length - 1;
	
	FFT_TYPE r1, r2, r3, r4, i1, i2, i3, i4, t1, t2;
	FFT_TYPE twiddle_c1, twiddle_s1;
	
	FFT_TYPE *r1_ptr = input->realp;
	FFT_TYPE *i1_ptr = input->imagp;
	
	FFT_TYPE *r2_ptr = r1_ptr + length_m1;
	FFT_TYPE *i2_ptr = i1_ptr + length_m1;
	FFT_TYPE *tr1_ptr = setup->tables[fft_log2 - FFTLOG2_TRIG_OFFSET].realp;
	FFT_TYPE *ti1_ptr = setup->tables[fft_log2 - FFTLOG2_TRIG_OFFSET].imagp;
	
	FFT_TYPE flip = 1.;
	
	HstFFT_UInt i;
	
	if (ifft)
		flip = -1.;
	
	// Do DC and Nyquist (note the the complex values can be considerd periodic)
	
	tr1_ptr++;
	ti1_ptr++;
	
	r1 = *r1_ptr;
	i1 = *i1_ptr;
	
	t1 = r1 + i1;
	t2 = r1 - i1;
	
	if (!ifft)
	{
		t1 *= 2.;
		t2 *= 2.;
	}
	
	*r1_ptr++ = t1;
	*i1_ptr++ = t2;
	
	// N.B. - The last time through this loop will write the same values twice to the same places
	// N.B. - In this case: t1 == 0, i4 == 0, r1_ptr == r2_ptr, i1_ptr == i2_ptr
	
	for (i = 0; i < (length >> 1); i++)
	{
		twiddle_c1 = flip * *tr1_ptr++;
		twiddle_s1 = *ti1_ptr++;
		
		// Get input
		
		r1 = *r1_ptr;
		i1 = *i1_ptr;
		r2 = *r2_ptr; 
		i2 = *i2_ptr;
		
		r3 = r1 + r2;
		i3 = i1 + i2;
		r4 = r1 - r2;
		i4 = i1 - i2;
		
		t1 = (twiddle_c1 * i3) + (twiddle_s1 * r4);
		t2 = (twiddle_c1 * -r4) + (twiddle_s1 * i3);
		
		// Store output (same pos as inputs)
		
		*r1_ptr++ = r3 + t1;		
		*i1_ptr++ = t2 + i4;
		
		// Store output (same pos as inputs)
		
		*r2_ptr-- = r3 - t1;
		*i2_ptr-- = t2 - i4;
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void FFT_FUNC_NAME(do_small_real_fft) (FFT_SPLIT_TYPE *input, HstFFT_UInt fft_log2, long ifft)
{
	FFT_TYPE r1, r2, r3, r4, i1, i2;
	FFT_TYPE scale = 2;
	
	FFT_TYPE *realp = input->realp;
	FFT_TYPE *imagp = input->imagp;
	
	if (fft_log2 < 1)
		return;

	if (ifft)
		scale = 1.;
		
	switch (fft_log2)
	{
		case 1:
			
			r1 = realp[0];
			r2 = imagp[0];
			
			realp[0] = (r1 + r2) * scale;
			imagp[0] = (r1 - r2) * scale;
			
			break;
			
		case 2:
		
			if (!ifft)
			{
				r3 = realp[0];
				r4 = realp[1];
				i1 = imagp[0];
				i2 = imagp[1];
				
				// Pass One
				
				r1 = r3 + r4;
				r2 = r3 - r4;
				r3 = i1 + i2;
				r4 = i1 - i2;
				
				// Pass Two
				
				realp[0] = (r1 + r3) * 2;
				realp[1] = r2 * 2;
				imagp[0] = (r1 - r3) * 2;
				imagp[1] = -r4 * 2;
			}
			else 
			{
				i1 = realp[0];
				r2 =  realp[1] + realp[1];
				i2 = imagp[0];
				r4 = -imagp[1] - imagp[1];
				
				// Pass One
				
				r1 = i1 + i2;
				r3 = i1 - i2;
				
				// Pass Two
				
				realp[0] = r1 + r2;
				realp[1] = r1 - r2;
				imagp[0] = r3 + r4;
				imagp[1] = r3 - r4;
			}

			break;
	}	
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void FFT_FUNC_NAME(do_real_fft) (FFT_SPLIT_TYPE *input, FFT_SETUP_TYPE *setup, HstFFT_UInt fft_log2)
{	
	if (fft_log2 < 3)
	{
		FFT_FUNC_NAME(do_small_real_fft) (input, fft_log2, 0L);
		return;
	}
	
	FFT_FUNC_NAME(do_fft) (input, setup, fft_log2 - 1);
	FFT_FUNC_NAME(pass_real_trig_table) (input, setup, fft_log2, 0L);
}


void FFT_FUNC_NAME(do_real_ifft) (FFT_SPLIT_TYPE *input, FFT_SETUP_TYPE *setup, HstFFT_UInt fft_log2)
{	
	if (fft_log2 < 3)
	{
		FFT_FUNC_NAME(do_small_real_fft) (input, fft_log2, 1L);
		return;
	}
	
	FFT_FUNC_NAME(pass_real_trig_table) (input, setup, fft_log2, 1L);
	FFT_FUNC_NAME(do_ifft) (input, setup, fft_log2 - 1);
}

