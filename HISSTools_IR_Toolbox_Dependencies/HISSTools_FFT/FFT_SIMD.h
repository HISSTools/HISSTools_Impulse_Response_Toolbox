
#include "FFT_Header.h"
#include "HISSTools_FFT_SIMD.h"

#ifdef VECTOR_F64_128BIT

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void pass_1_2_reorder_simd (SplitDouble *input, HstFFT_UInt length)
{
	HstFFT_UInt i;
	
	vDouble *r1_ptr = (vDouble *) input->realp;
	vDouble *r2_ptr = r1_ptr + (length >> 3);
	vDouble *r3_ptr = r2_ptr + (length >> 3);
	vDouble *r4_ptr = r3_ptr + (length >> 3);
	
	vDouble *i1_ptr = (vDouble *) input->imagp;
	vDouble *i2_ptr = i1_ptr + (length >> 3);
	vDouble *i3_ptr = i2_ptr + (length >> 3);
	vDouble *i4_ptr = i3_ptr + (length >> 3);
	
	vDouble r1, r2, r3, r4, r5, r6, r7, r8;
	vDouble i1, i2, i3, i4, i5, i6, i7, i8;
	vDouble t1, t2, t3, t4;
	
	for (i = 0; i < length >> 4; i++)
	{		
		t1 = *r1_ptr;
		t2 = *(r1_ptr + 1);
		t3 = *r2_ptr;
		t4 = *(r2_ptr + 1);
		r5 = *r3_ptr;
		r6 = *(r3_ptr + 1);
		r7 = *r4_ptr;
		r8 = *(r4_ptr + 1);
		
		r1 = F64_VEC_ADD_OP(t1, r5);
		r2 = F64_VEC_ADD_OP(t2, r6);
		r3 = F64_VEC_ADD_OP(t3, r7);
		r4 = F64_VEC_ADD_OP(t4, r8);
		r5 = F64_VEC_SUB_OP(t1, r5);
		r6 = F64_VEC_SUB_OP(t2, r6);
		r7 = F64_VEC_SUB_OP(t3, r7);
		r8 = F64_VEC_SUB_OP(t4, r8);
		
		t1 = *i1_ptr;
		t2 = *(i1_ptr + 1);
		t3 = *i2_ptr;
		t4 = *(i2_ptr + 1);
		i5 = *i3_ptr;
		i6 = *(i3_ptr + 1);
		i7 = *i4_ptr;
		i8 = *(i4_ptr + 1);
		
		i1 = F64_VEC_ADD_OP(t1, i5);
		i2 = F64_VEC_ADD_OP(t2, i6);
		i3 = F64_VEC_ADD_OP(t3, i7);
		i4 = F64_VEC_ADD_OP(t4, i8);
		i5 = F64_VEC_SUB_OP(t1, i5);
		i6 = F64_VEC_SUB_OP(t2, i6);
		i7 = F64_VEC_SUB_OP(t3, i7);
		i8 = F64_VEC_SUB_OP(t4, i8);
		
		t1 = F64_VEC_ADD_OP(r1, r3);
		t2 = F64_VEC_ADD_OP(r2, r4);
		t3 = F64_VEC_SUB_OP(r1, r3);
		t4 = F64_VEC_SUB_OP(r2, r4);
		r1 = F64_VEC_ADD_OP(r5, i7);
		r2 = F64_VEC_ADD_OP(r6, i8);
		r3 = F64_VEC_SUB_OP(r5, i7);
		r4 = F64_VEC_SUB_OP(r6, i8);
		
		*r1_ptr++ = F64_VEC_SHUFFLE(t1, r1, F64_SHUFFLE_CONST(0, 0));
		*r1_ptr++ = F64_VEC_SHUFFLE(t3, r3, F64_SHUFFLE_CONST(0, 0));
		*r2_ptr++ = F64_VEC_SHUFFLE(t2, r2, F64_SHUFFLE_CONST(0, 0));
		*r2_ptr++ = F64_VEC_SHUFFLE(t4, r4, F64_SHUFFLE_CONST(0, 0));
		*r3_ptr++ = F64_VEC_SHUFFLE(t1, r1, F64_SHUFFLE_CONST(1, 1));
		*r3_ptr++ = F64_VEC_SHUFFLE(t3, r3, F64_SHUFFLE_CONST(1, 1));
		*r4_ptr++ = F64_VEC_SHUFFLE(t2, r2, F64_SHUFFLE_CONST(1, 1));
		*r4_ptr++ = F64_VEC_SHUFFLE(t4, r4, F64_SHUFFLE_CONST(1, 1));
		
		t1 = F64_VEC_ADD_OP(i1, i3);
		t2 = F64_VEC_ADD_OP(i2, i4);
		t3 = F64_VEC_SUB_OP(i1, i3);
		t4 = F64_VEC_SUB_OP(i2, i4);
		i1 = F64_VEC_SUB_OP(i5, r7);
		i2 = F64_VEC_SUB_OP(i6, r8);
		i3 = F64_VEC_ADD_OP(i5, r7);
		i4 = F64_VEC_ADD_OP(i6, r8);
		
		*i1_ptr++ = F64_VEC_SHUFFLE(t1, i1, F64_SHUFFLE_CONST(0, 0));
		*i1_ptr++ = F64_VEC_SHUFFLE(t3, i3, F64_SHUFFLE_CONST(0, 0));
		*i2_ptr++ = F64_VEC_SHUFFLE(t2, i2, F64_SHUFFLE_CONST(0, 0));
		*i2_ptr++ = F64_VEC_SHUFFLE(t4, i4, F64_SHUFFLE_CONST(0, 0));
		*i3_ptr++ = F64_VEC_SHUFFLE(t1, i1, F64_SHUFFLE_CONST(1, 1));
		*i3_ptr++ = F64_VEC_SHUFFLE(t3, i3, F64_SHUFFLE_CONST(1, 1));
		*i4_ptr++ = F64_VEC_SHUFFLE(t2, i2, F64_SHUFFLE_CONST(1, 1));
		*i4_ptr++ = F64_VEC_SHUFFLE(t4, i4, F64_SHUFFLE_CONST(1, 1));
	}
}


void pass_1_2_reorder_simd_float(SplitFloat *input, HstFFT_UInt length)
{
	HstFFT_UInt i;
	
	vFloat *r1_ptr = (vFloat *) input->realp;
	vFloat *r2_ptr = r1_ptr + (length >> 4);
	vFloat *r3_ptr = r2_ptr + (length >> 4);
	vFloat *r4_ptr = r3_ptr + (length >> 4);
	
	vFloat *i1_ptr = (vFloat *) input->imagp;
	vFloat *i2_ptr = i1_ptr + (length >> 4);
	vFloat *i3_ptr = i2_ptr + (length >> 4);
	vFloat *i4_ptr = i3_ptr + (length >> 4);
	
	vFloat r1, r2, r3, r4, r5, r6, r7, r8;
	vFloat i1, i2, i3, i4, i5, i6, i7, i8;
	
	for (i = 0; i < length >> 4; i++)
	{		
		r5 = *r1_ptr;
		r6 = *r2_ptr;
		r3 = *r3_ptr;
		r4 = *r4_ptr;
		
		i5 = *i1_ptr;
		i6 = *i2_ptr;
		i3 = *i3_ptr;
		i4 = *i4_ptr;
		
		r1 = F32_VEC_ADD_OP(r5, r3);
		r2 = F32_VEC_ADD_OP(r6, r4);
		r3 = F32_VEC_SUB_OP(r5, r3);
		r4 = F32_VEC_SUB_OP(r6, r4);
		
		i1 = F32_VEC_ADD_OP(i5, i3);
		i2 = F32_VEC_ADD_OP(i6, i4);
		i3 = F32_VEC_SUB_OP(i5, i3);
		i4 = F32_VEC_SUB_OP(i6, i4);
		
		r5 = F32_VEC_ADD_OP(r1, r2);
		r6 = F32_VEC_SUB_OP(r1, r2);
		r7 = F32_VEC_ADD_OP(r3, i4);
		r8 = F32_VEC_SUB_OP(r3, i4);
		
		i5 = F32_VEC_ADD_OP(i1, i2);
		i6 = F32_VEC_SUB_OP(i1, i2);
		i7 = F32_VEC_SUB_OP(i3, r4);
		i8 = F32_VEC_ADD_OP(i3, r4);
		
		r1 = F32_VEC_SHUFFLE(r5, r7, F32_SHUFFLE_CONST(1, 0, 1, 0));
		r2 = F32_VEC_SHUFFLE(r5, r7, F32_SHUFFLE_CONST(3, 2, 3, 2));
		r3 = F32_VEC_SHUFFLE(r6, r8, F32_SHUFFLE_CONST(1, 0, 1, 0));
		r4 = F32_VEC_SHUFFLE(r6, r8, F32_SHUFFLE_CONST(3, 2, 3, 2));
		
		*r1_ptr++ = F32_VEC_SHUFFLE(r1, r3, F32_SHUFFLE_CONST(2, 0, 2, 0));
		*r2_ptr++ = F32_VEC_SHUFFLE(r2, r4, F32_SHUFFLE_CONST(2, 0, 2, 0));
		*r3_ptr++ = F32_VEC_SHUFFLE(r1, r3, F32_SHUFFLE_CONST(3, 1, 3, 1));
		*r4_ptr++ = F32_VEC_SHUFFLE(r2, r4, F32_SHUFFLE_CONST(3, 1, 3, 1));
		
		i1 = F32_VEC_SHUFFLE(i5, i7, F32_SHUFFLE_CONST(1, 0, 1, 0));
		i2 = F32_VEC_SHUFFLE(i5, i7, F32_SHUFFLE_CONST(3, 2, 3, 2));
		i3 = F32_VEC_SHUFFLE(i6, i8, F32_SHUFFLE_CONST(1, 0, 1, 0));
		i4 = F32_VEC_SHUFFLE(i6, i8, F32_SHUFFLE_CONST(3, 2, 3, 2));
		
		*i1_ptr++ = F32_VEC_SHUFFLE(i1, i3, F32_SHUFFLE_CONST(2, 0, 2, 0));
		*i2_ptr++ = F32_VEC_SHUFFLE(i2, i4, F32_SHUFFLE_CONST(2, 0, 2, 0));
		*i3_ptr++ = F32_VEC_SHUFFLE(i1, i3, F32_SHUFFLE_CONST(3, 1, 3, 1));
		*i4_ptr++ = F32_VEC_SHUFFLE(i2, i4, F32_SHUFFLE_CONST(3, 1, 3, 1));
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void pass_3_reorder_simd(SplitDouble *input, HstFFT_UInt length)
{
	HstFFT_UInt offset = length >> 4;
	HstFFT_UInt outer_loop = length >> 6;
	HstFFT_UInt i, j;
	
	vDouble r1, r2, r3, r4, r5, r6, r7, r8, r9, r10;
	vDouble i1, i2, i3, i4, i5, i6, i7, i8, i9, i10;
	
	vDouble twiddle_c1 = {1, SQRT_2_2};
	vDouble twiddle_s1 = {0, -SQRT_2_2};
	vDouble twiddle_c2 = {0, -SQRT_2_2};
	vDouble twiddle_s2 = {-1, -SQRT_2_2};
	
	vDouble *r1_ptr = (vDouble *) input->realp;
	vDouble *i1_ptr = (vDouble *) input->imagp;
	vDouble *r2_ptr = r1_ptr + offset;
	vDouble *i2_ptr = i1_ptr + offset;
	
	for (i = 0, j = 0; i < length >> 1; i += 8)
	{				
		// Get input
		
		r1 = *r1_ptr;
		r5 = *(r1_ptr + 1);
		r2 = *r2_ptr;
		r6 = *(r2_ptr + 1);
		i1 = *i1_ptr;
		i5 = *(i1_ptr + 1);
		i2 = *i2_ptr;
		i6 = *(i2_ptr + 1);
		
		// Multiply by twiddle
		
		r9 = F64_VEC_SUB_OP(F64_VEC_MUL_OP(r2, twiddle_c1), F64_VEC_MUL_OP(i2, twiddle_s1));
		i9 = F64_VEC_ADD_OP(F64_VEC_MUL_OP(r2, twiddle_s1), F64_VEC_MUL_OP(i2, twiddle_c1));
		r10 = F64_VEC_SUB_OP(F64_VEC_MUL_OP(r6, twiddle_c2), F64_VEC_MUL_OP(i6, twiddle_s2));
		i10 = F64_VEC_ADD_OP(F64_VEC_MUL_OP(r6, twiddle_s2), F64_VEC_MUL_OP(i6, twiddle_c2));
		
		// Get input
	
		r3 = *(r1_ptr + 2);
		r7 = *(r1_ptr + 3);
		r4 = *(r2_ptr + 2);
		r8 = *(r2_ptr + 3);
		i3 = *(i1_ptr + 2);
		i7 = *(i1_ptr + 3);
		i4 = *(i2_ptr + 2);
		i8 = *(i2_ptr + 3);
		
		// Store output (swapping as necessary)
		
		*r1_ptr = F64_VEC_ADD_OP(r1, r9);
		*(r1_ptr + 1) = F64_VEC_ADD_OP(r5, r10);
		*i1_ptr = F64_VEC_ADD_OP(i1, i9);
		*(i1_ptr + 1) = F64_VEC_ADD_OP(i5, i10);
		
		*(r1_ptr++ + 2) = F64_VEC_SUB_OP(r1, r9);
		*(r1_ptr++ + 2) = F64_VEC_SUB_OP(r5, r10);
		*(i1_ptr++ + 2) = F64_VEC_SUB_OP(i1, i9);
		*(i1_ptr++ + 2) = F64_VEC_SUB_OP(i5, i10);
		
		// Multiply by twiddle
		
		r9 = F64_VEC_SUB_OP(F64_VEC_MUL_OP(r4, twiddle_c1), F64_VEC_MUL_OP(i4, twiddle_s1));
		i9 = F64_VEC_ADD_OP(F64_VEC_MUL_OP(r4, twiddle_s1), F64_VEC_MUL_OP(i4, twiddle_c1));
		r10 = F64_VEC_SUB_OP(F64_VEC_MUL_OP(r8, twiddle_c2), F64_VEC_MUL_OP(i8, twiddle_s2));
		i10 = F64_VEC_ADD_OP(F64_VEC_MUL_OP(r8, twiddle_s2), F64_VEC_MUL_OP(i8, twiddle_c2));
		
		// Store output (swapping as necessary)
		
		*r2_ptr = F64_VEC_ADD_OP(r3, r9);
		*(r2_ptr + 1) = F64_VEC_ADD_OP(r7, r10);
		*i2_ptr = F64_VEC_ADD_OP(i3, i9);
		*(i2_ptr + 1) = F64_VEC_ADD_OP(i7, i10);
		
		*(r2_ptr++ + 2) = F64_VEC_SUB_OP(r3, r9);
		*(r2_ptr++ + 2) = F64_VEC_SUB_OP(r7, r10);
		*(i2_ptr++ + 2) = F64_VEC_SUB_OP(i3, i9);
		*(i2_ptr++ + 2) = F64_VEC_SUB_OP(i7, i10);
		
		r1_ptr += 2;
		r2_ptr += 2;
		i1_ptr += 2;
		i2_ptr += 2;
		
		if (!(++j % outer_loop))
		{			
			r1_ptr += offset;
			r2_ptr += offset;
			i1_ptr += offset;
			i2_ptr += offset;
		}
	}	
}


void pass_3_reorder_simd_float(SplitFloat *input, HstFFT_UInt length)
{
	HstFFT_UInt offset = length >> 5;
	HstFFT_UInt outer_loop = length >> 6;
	HstFFT_UInt i, j;
	
	vFloat r1, r2, r3, r4, r5;
	vFloat i1, i2, i3, i4, i5;
	vFloat twiddle_c = {1, (FFT_TYPE) SQRT_2_2, 0, (FFT_TYPE) -SQRT_2_2};
	vFloat twiddle_s = {0, (FFT_TYPE)-SQRT_2_2, -1, (FFT_TYPE) -SQRT_2_2};
	
	vFloat *r1_ptr = (vFloat *) input->realp;
	vFloat *i1_ptr = (vFloat *) input->imagp;
	vFloat *r2_ptr = r1_ptr + offset;
	vFloat *i2_ptr = i1_ptr + offset;
	
	for (i = 0, j = 0; i < length >> 1; i += 8)
	{				
		// Get input
		
		r1 = *r1_ptr;
		i1 = *i1_ptr;
		r3 = *(r1_ptr + 1);
		i3 = *(i1_ptr + 1);
		r2 = *r2_ptr;
		i2 = *i2_ptr;
		r4 = *(r2_ptr + 1);
		i4 = *(i2_ptr + 1);
		
		// Multiply by twiddle
		
		r5 = F32_VEC_SUB_OP(F32_VEC_MUL_OP(r2, twiddle_c), F32_VEC_MUL_OP(i2, twiddle_s));
		i5 = F32_VEC_ADD_OP(F32_VEC_MUL_OP(r2, twiddle_s), F32_VEC_MUL_OP(i2, twiddle_c));
		
		// Store output (swapping as necessary)
		
		*r1_ptr = F32_VEC_ADD_OP(r1, r5);
		*i1_ptr = F32_VEC_ADD_OP(i1, i5);
		
		*(r1_ptr + 1) = F32_VEC_SUB_OP(r1, r5);
		*(i1_ptr + 1) = F32_VEC_SUB_OP(i1, i5);
		
		// Multiply by twiddle
		
		r5 = F32_VEC_SUB_OP(F32_VEC_MUL_OP(r4, twiddle_c), F32_VEC_MUL_OP(i4, twiddle_s));
		i5 = F32_VEC_ADD_OP(F32_VEC_MUL_OP(r4, twiddle_s), F32_VEC_MUL_OP(i4, twiddle_c));
		
		// Store output (swapping as necessary)
		
		*r2_ptr = F32_VEC_ADD_OP(r3, r5);
		*i2_ptr = F32_VEC_ADD_OP(i3, i5);
		
		*(r2_ptr + 1) = F32_VEC_SUB_OP(r3, r5);
		*(i2_ptr + 1) = F32_VEC_SUB_OP(i3, i5);
		
		r1_ptr += 2;
		r2_ptr += 2;
		i1_ptr += 2;
		i2_ptr += 2;
		
		if (!(++j % outer_loop))
		{			
			r1_ptr += offset;
			r2_ptr += offset;
			i1_ptr += offset;
			i2_ptr += offset;			
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void pass_trig_table_reorder_simd (SplitDouble *input, FFTSetupDouble *setup, HstFFT_UInt length, HstFFT_UInt pass)
{
	HstFFT_UInt size = 2 << pass;
	HstFFT_UInt incr = size >> 2;
	HstFFT_UInt loop = size;
	HstFFT_UInt offset = length >> (pass + 2);
	HstFFT_UInt outer_loop = ((length >> 1) / size) / ((HstFFT_UInt) 1 << pass);
	HstFFT_UInt i, j;
	
	vDouble r1, r2, r3, r4, r5;
	vDouble i1, i2, i3, i4, i5;
	vDouble twiddle_c, twiddle_s;
	
	vDouble *r1_ptr = (vDouble *) input->realp;
	vDouble *i1_ptr = (vDouble *) input->imagp;
	vDouble *r2_ptr = r1_ptr + offset;
	vDouble *i2_ptr = i1_ptr + offset;

	for (j = 0, i = 0; i < (length >> 1); loop += size)
	{				
		vDouble *t_ptr_r = (vDouble *) setup->tables[pass - PASS_TRIG_OFFSET].realp; 
		vDouble *t_ptr_i = (vDouble *) setup->tables[pass - PASS_TRIG_OFFSET].imagp;
		
		for (; i < loop; i += 4)
		{
			// Get input
			
			r1 = *r1_ptr;
			i1 = *i1_ptr;
			r2 = *r2_ptr;
			i2 = *i2_ptr;
			
			// Multiply by twiddle
			
			twiddle_c = *t_ptr_r++;
			twiddle_s = *t_ptr_i++;
			
			r5 = F64_VEC_SUB_OP(F64_VEC_MUL_OP(r2, twiddle_c), F64_VEC_MUL_OP(i2, twiddle_s));
			i5 = F64_VEC_ADD_OP(F64_VEC_MUL_OP(r2, twiddle_s), F64_VEC_MUL_OP(i2, twiddle_c));
			
			// Get input
			
			r3 = *(r1_ptr + incr);
			i3 = *(i1_ptr + incr);
			r4 = *(r2_ptr + incr);
			i4 = *(i2_ptr + incr);
			
			// Store output (swapping as necessary)
			
			*r1_ptr = F64_VEC_ADD_OP(r1, r5);
			*i1_ptr = F64_VEC_ADD_OP(i1, i5);
			
			*(r1_ptr++ + incr) = F64_VEC_SUB_OP(r1, r5);
			*(i1_ptr++ + incr) = F64_VEC_SUB_OP(i1, i5);
			
			// Multiply by twiddle
			
			r5 = F64_VEC_SUB_OP(F64_VEC_MUL_OP(r4, twiddle_c), F64_VEC_MUL_OP(i4, twiddle_s));
			i5 = F64_VEC_ADD_OP(F64_VEC_MUL_OP(r4, twiddle_s), F64_VEC_MUL_OP(i4, twiddle_c));
			
			// Store output (swapping as necessary)
			
			*r2_ptr = F64_VEC_ADD_OP(r3, r5);
			*i2_ptr = F64_VEC_ADD_OP(i3, i5);
			
			*(r2_ptr++ + incr) = F64_VEC_SUB_OP(r3, r5);
			*(i2_ptr++ + incr) = F64_VEC_SUB_OP(i3, i5);
		}
		
		r1_ptr += incr;
		r2_ptr += incr;
		i1_ptr += incr;
		i2_ptr += incr;
		
		if (!(++j % outer_loop))
		{			
			r1_ptr += offset;
			r2_ptr += offset;
			i1_ptr += offset;
			i2_ptr += offset;
		}
	}
}


void pass_trig_table_reorder_simd_float(SplitFloat *input, FFTSetupFloat *setup, HstFFT_UInt length, HstFFT_UInt pass)
{
	HstFFT_UInt size = 2 << pass;
	HstFFT_UInt incr = size >> 3;
	HstFFT_UInt loop = size;
	HstFFT_UInt offset = length >> (pass + 3);
	HstFFT_UInt outer_loop = ((length >> 1) / size) / ((HstFFT_UInt) 1 << pass);
	HstFFT_UInt i, j;
	
	vFloat r1, r2, r3, r4, r5;
	vFloat i1, i2, i3, i4, i5;
	vFloat twiddle_c, twiddle_s;
	
	vFloat *r1_ptr = (vFloat *) input->realp;
	vFloat *i1_ptr = (vFloat *) input->imagp;
	vFloat *r2_ptr = r1_ptr + offset;
	vFloat *i2_ptr = i1_ptr + offset;
	
	for (j = 0, i = 0; i < length >> 1; loop += size)
	{		
		vFloat *table_r = (vFloat *) setup->tables[pass - PASS_TRIG_OFFSET].realp;
		vFloat *table_i = (vFloat *) setup->tables[pass - PASS_TRIG_OFFSET].imagp;
		
		for (; i < loop; i += 8)
		{
			// Get input
			
			r1 = *r1_ptr;
			i1 = *i1_ptr;
			r2 = *r2_ptr;
			i2 = *i2_ptr;
			
			// Get Twiddle
			
			twiddle_c = *table_r++;
			twiddle_s = *table_i++;
			
			// Multiply by twiddle
			
			r5 = F32_VEC_SUB_OP(F32_VEC_MUL_OP(r2, twiddle_c), F32_VEC_MUL_OP(i2, twiddle_s));
			i5 = F32_VEC_ADD_OP(F32_VEC_MUL_OP(r2, twiddle_s), F32_VEC_MUL_OP(i2, twiddle_c));
			
			// Get input
			
			r3 = *(r1_ptr + incr);
			i3 = *(i1_ptr + incr);
			r4 = *(r2_ptr + incr);
			i4 = *(i2_ptr + incr);
			
			// Store output (swapping as necessary)
			
			*r1_ptr = F32_VEC_ADD_OP(r1, r5);
			*i1_ptr = F32_VEC_ADD_OP(i1, i5);
			
			*(r1_ptr++ + incr) = F32_VEC_SUB_OP(r1, r5);
			*(i1_ptr++ + incr) = F32_VEC_SUB_OP(i1, i5);
			
			// Multiply by twiddle
			
			r5 = F32_VEC_SUB_OP(F32_VEC_MUL_OP(r4, twiddle_c), F32_VEC_MUL_OP(i4, twiddle_s));
			i5 = F32_VEC_ADD_OP(F32_VEC_MUL_OP(r4, twiddle_s), F32_VEC_MUL_OP(i4, twiddle_c));
			
			// Store output (swapping as necessary)
			
			*r2_ptr = F32_VEC_ADD_OP(r3, r5);
			*i2_ptr = F32_VEC_ADD_OP(i3, i5);
			
			*(r2_ptr++ + incr) = F32_VEC_SUB_OP(r3, r5);
			*(i2_ptr++ + incr) = F32_VEC_SUB_OP(i3, i5);
		}
		
		r1_ptr += incr;
		r2_ptr += incr;
		i1_ptr += incr;
		i2_ptr += incr;
		
		if (!(++j % outer_loop))
		{			
			r1_ptr += offset;
			r2_ptr += offset;
			i1_ptr += offset;
			i2_ptr += offset;			
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void pass_trig_table_simd(SplitDouble *input, FFTSetupDouble *setup, HstFFT_UInt length, HstFFT_UInt pass)
{
	HstFFT_UInt size = 2 << pass;
	HstFFT_UInt incr = size >> 2;
	HstFFT_UInt loop = size;
	
	vDouble r1, r2, i1, i2, r3, i3, twiddle_c, twiddle_s;
	
	vDouble *r1_ptr = (vDouble *) input->realp;
	vDouble *i1_ptr = (vDouble *) input->imagp;
	vDouble *r2_ptr = r1_ptr + incr;
	vDouble *i2_ptr = i1_ptr + incr;
	
	HstFFT_UInt i;
	
	for (i = 0; i < length; loop += size)
	{		
		vDouble *t_ptr_r = (vDouble *) setup->tables[pass - PASS_TRIG_OFFSET].realp; 
		vDouble *t_ptr_i = (vDouble *) setup->tables[pass - PASS_TRIG_OFFSET].imagp;
		
		for (; i < loop; i += 4)
		{
			// Get input
			
			r1 = *r1_ptr;
			i1 = *i1_ptr;
			r2 = *r2_ptr;
			i2 = *i2_ptr;
			
			// Multiply by twiddle
			
			twiddle_c = *t_ptr_r++;
			twiddle_s = *t_ptr_i++;
			
			r3 = F64_VEC_SUB_OP(F64_VEC_MUL_OP(r2, twiddle_c), F64_VEC_MUL_OP(i2, twiddle_s));
			i3 = F64_VEC_ADD_OP(F64_VEC_MUL_OP(r2, twiddle_s), F64_VEC_MUL_OP(i2, twiddle_c));
			
			// Store output (same pos as inputs)
			
			*r1_ptr++ = F64_VEC_ADD_OP(r1, r3);
			*i1_ptr++ = F64_VEC_ADD_OP(i1, i3);
			
			*r2_ptr++ = F64_VEC_SUB_OP(r1, r3);
			*i2_ptr++ = F64_VEC_SUB_OP(i1, i3);
		}
		
		r1_ptr += incr;
		r2_ptr += incr;
		i1_ptr += incr;
		i2_ptr += incr;
	}
}


void pass_trig_table_simd_float(SplitFloat *input, FFTSetupFloat *setup, HstFFT_UInt length, HstFFT_UInt pass)
{
	HstFFT_UInt size = 2 << pass;
	HstFFT_UInt incr = size >> 3;
	HstFFT_UInt loop = size;
	HstFFT_UInt i;
	
	vFloat r1, r2, r3, i1, i2, i3;
	vFloat twiddle_c, twiddle_s;
	
	vFloat *r1_ptr = (vFloat *) input->realp;
	vFloat *i1_ptr = (vFloat *) input->imagp;
	vFloat *r2_ptr = r1_ptr + incr;
	vFloat *i2_ptr = i1_ptr + incr;
	
	for (i = 0; i < length; loop += size)
	{		
		vFloat *table_r = (vFloat *) setup->tables[pass - PASS_TRIG_OFFSET].realp;
		vFloat *table_i = (vFloat *) setup->tables[pass - PASS_TRIG_OFFSET].imagp;
		
		for (; i < loop; i += 8)
		{
			// Get input
			
			r1 = *r1_ptr;
			i1 = *i1_ptr;
			r2 = *r2_ptr;
			i2 = *i2_ptr;
			
			// Get Twiddle
			
			twiddle_c = *table_r++;
			twiddle_s = *table_i++;
			
			// Multiply by twiddle
			
			r3 = F32_VEC_SUB_OP(F32_VEC_MUL_OP(r2, twiddle_c), F32_VEC_MUL_OP(i2, twiddle_s));
			i3 = F32_VEC_ADD_OP(F32_VEC_MUL_OP(r2, twiddle_s), F32_VEC_MUL_OP(i2, twiddle_c));
			
			// Store output (same pos as inputs)
			
			*r1_ptr++ = F32_VEC_ADD_OP(r1, r3);
			*i1_ptr++ = F32_VEC_ADD_OP(i1, i3);
			
			*r2_ptr++ = F32_VEC_SUB_OP(r1, r3);
			*i2_ptr++ = F32_VEC_SUB_OP(i1, i3);
		}
		
		r1_ptr += incr;
		r2_ptr += incr;
		i1_ptr += incr;
		i2_ptr += incr;
	}
}

#endif
