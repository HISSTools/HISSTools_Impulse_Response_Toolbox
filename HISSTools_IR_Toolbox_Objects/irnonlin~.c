
#include <ext.h>
#include <ext_obex.h>
#include <ext_obex_util.h>
#include <z_dsp.h>

#include <HIRT_Core_Functions.h>
#include <HIRT_Buffer_Access.h>
#include <HIRT_Matrix_Math.h>

// Define common attributes and the class name (for the common attributes file)

#define OBJ_CLASSNAME t_irnonlin
#define OBJ_USES_HIRT_WRITE_ATTR
#define OBJ_USES_HIRT_READ_ATTR

#include <HIRT_Common_Attribute_Vars.h>


// Object class and structure

void *this_class;

typedef struct _irnonlin
{
    t_pxobject x_obj;
		
	// Attributes
	
	HIRT_COMMON_ATTR 
	
	// Bang Outlet
	
	void *process_done;
		
} t_irnonlin;


// This include deals with setup of common attributes - requires the object structure to be defined

#include <HIRT_Common_Attribute_Setup.h>


// Function prototypes

void *irnonlin_new (t_symbol *s, short argc, t_atom *argv);
void irnonlin_free (t_irnonlin *x);
void irnonlin_assist (t_irnonlin *x, void *b, long m, long a, char *s);

void irnonlin_nonlin (t_irnonlin *x, t_symbol *sym, long argc, t_atom *argv);
void irnonlin_nonlin_internal (t_irnonlin *x, t_symbol *sym, short argc, t_atom *argv);


//////////////////////////////////////////////////////////////////////////
/////////////////////// Main / New / Free / Assist ///////////////////////
//////////////////////////////////////////////////////////////////////////


int main (void)
{
    this_class = class_new ("irnonlin~",
							(method) irnonlin_new, 
							(method)irnonlin_free, 
							sizeof(t_irnonlin), 
							0L,
							A_GIMME,	
							0);
	
	class_addmethod (this_class, (method)irnonlin_assist, "assist", A_CANT, 0L);
	
	class_addmethod (this_class, (method)irnonlin_nonlin, "convert", A_GIMME, 0L);
	class_addmethod (this_class, (method)irnonlin_nonlin, "convertto", A_GIMME, 0L);
	
	declare_HIRT_common_attributes(this_class);
		
	class_register(CLASS_BOX, this_class);
		
	buffer_access_init();
	
	return 0;
}



void *irnonlin_new (t_symbol *s, short argc, t_atom *argv)
{
    t_irnonlin *x = (t_irnonlin *)object_alloc (this_class);
	
	x->process_done = bangout(x);
	
	init_HIRT_common_attributes(x);
	attr_args_process(x, argc, argv);

	return(x);
}


void irnonlin_free(t_irnonlin *x)
{	
	free_HIRT_common_attributes(x);
}


void irnonlin_assist (t_irnonlin *x, void *b, long m, long a, char *s)
{
	if (m == ASSIST_INLET)
		sprintf(s,"Instructions In");
	else
		sprintf(s,"Bang on Success");
}

//////////////////////////////////////////////////////////////////////////
///////////////////////////// Matrix Routines ////////////////////////////
//////////////////////////////////////////////////////////////////////////


// These functions are used to calculate the necessary matrix for inversion in order to the multiplication coefficients for conversion

COMPLEX_DOUBLE m1_cpow (AH_UIntPtr i, AH_UIntPtr j)
{
	COMPLEX_DOUBLE val = CSET(1, 0);
	AH_UIntPtr int_part = (i << 1) + (j >> 1);
	
	if (int_part & 1)
		val = CSET(-1, 0);
	
	if (!(j & 1))
		val = CMUL(val, CSET(0, 1));
		
	return val;
}


double factorial (double k)
{
	return (k <= 1) ? 1 : k * factorial (k - 1);
}


double binom (double n, double k)
{
	if (k <= 0) 
		return 1;
	
	return factorial(n) / ( factorial(k) * factorial(n - k));
}


// Solves the inversion of the matrix so as to determine the correct (complex) multiplication factor for each measured harmonic for the Hammerstein model

t_matrix_complex *matrix_non_linear(AH_UIntPtr size)
{
	t_matrix_complex *mat = matrix_alloc_complex(size, size);
	
	MATRIX_REF_COMPLEX(mat)
	
	COMPLEX_DOUBLE pivot_recip, row_mult;
	AH_UIntPtr i, j, k;
	
	if (!mat)
		return 0;
	
	MATRIX_DEREF(mat)
	
	// N.B. Indices offset here to 1 (and stored correctly below)
	
	for (i = 1; i <= size; i++)
	{
		for(j = 1; j <= size; j++)
		{
			if ((i >= j) && ((i + j + 1) & 1))
				MATRIX_ELEMENT(mat, j - 1, i - 1) = CDIV(CMUL(m1_cpow(i, j), CSET(binom((double) i, (double) ((i - j) / 2)), 0)), CSET(pow(2, (double) (i - 1)), 0));
			else
				MATRIX_ELEMENT(mat, j - 1, i - 1) = CSET(0, 0);
		}
	}
		
	for (i = 0; i < size; i++)
	{
		// Multiply rows
		
		pivot_recip = CDIV(CSET(1, 0), MATRIX_ELEMENT(mat, i, i));
		MATRIX_ELEMENT(mat, i, i) = CSET(1, 0);
		
		for (j = 0; j < size; j++)
			MATRIX_ELEMENT(mat, i, j) = CMUL(MATRIX_ELEMENT(mat, i, j), pivot_recip);
		
		// Subtract rows
		
		for (j = 0; j < size; j++)
		{
			if (j == i)
				continue;
			
			row_mult = MATRIX_ELEMENT(mat, j, i);
			MATRIX_ELEMENT(mat, j, i) = CSET(0, 0);
			
			for (k = i; k < size; k++) 
				MATRIX_ELEMENT(mat, j, k) = CSUB(MATRIX_ELEMENT(mat, j, k), CMUL(MATRIX_ELEMENT(mat, i, k), row_mult));
		}
	}
		
	return mat;
}


//////////////////////////////////////////////////////////////////////////
///////////////////////////// User Messages //////////////////////////////
//////////////////////////////////////////////////////////////////////////


void irnonlin_nonlin (t_irnonlin *x, t_symbol *sym, long argc, t_atom *argv)
{
	defer(x, (method) irnonlin_nonlin_internal, sym, (short) argc, argv);
}


// Converts from harmonic IRs to harmerstein model IRs

void irnonlin_nonlin_internal (t_irnonlin *x, t_symbol *sym, short argc, t_atom *argv)
{
	FFT_SETUP_D fft_setup;
	
	FFT_SPLIT_COMPLEX_D impulses[128];
	
	t_symbol *in_buffer_names[128];
	t_symbol *out_buffer_names[128];
	AH_SIntPtr lengths[128];
	
	double sample_rate = 0;
	double current_coeff;
	double real;
	double imag;
	
	double *temp_buffer_d;
	float *temp_buffer_f;
	
	AH_UIntPtr fft_size;
	AH_UIntPtr fft_size_log2;
	
	AH_SIntPtr num_buffers = 0;
	AH_SIntPtr overall_length = 0;
	AH_SIntPtr length;
	AH_SIntPtr max_length = 0;
	AH_SIntPtr i, j, k;
		
	t_buffer_write_error error;
	
	AH_Boolean overall_error = false;
	
	t_matrix_complex *coeff;
	
	MATRIX_REF_COMPLEX(coeff)
	
	// Check buffers, storing names and lengths +  calculate total / largest length
	
	num_buffers = buffer_multiple_names((t_object *) x, in_buffer_names, out_buffer_names, lengths, argc, argv, (sym == gensym("convert")), 128, &overall_length, &max_length, &sample_rate);

	if (!num_buffers)
		return;
	
	// Calculate fft size
	
	fft_size = calculate_fft_size(max_length, &fft_size_log2);
	
	// Check length of buffers for writing
	
	if (!x->resize)
	{
		for (i = 0; i < num_buffers; i++)
		{
			if ((AH_UIntPtr) buffer_length(out_buffer_names[i]) < fft_size)
			{
				object_error((t_object *) x, "buffer %s is not long enough to complete write (no buffers altered)", out_buffer_names[i]->s_name);
				return;
			}
		}
	}
	   
	// Allocate Resources
	
	fft_setup = hisstools_create_setup_d(fft_size_log2);
	temp_buffer_d = ALIGNED_MALLOC(sizeof(double) * fft_size);				
	temp_buffer_f = (float *) temp_buffer_d;
 	impulses[0].realp = ALIGNED_MALLOC(sizeof(double) * fft_size * num_buffers);
	
	// Solve linear equations to generate multiplicative coeffients for harmonics
	
	coeff = matrix_non_linear(num_buffers);
						
	// Check Memory Allocations
		
	if (!fft_setup || !temp_buffer_d || !impulses[0].realp || !coeff)
	{
		object_error((t_object *) x, "could not allocate temporary memory for processing");
		
		hisstools_destroy_setup_d(fft_setup);
		ALIGNED_FREE(impulses[0].realp);
		ALIGNED_FREE(temp_buffer_d);
		matrix_destroy_complex(coeff);
		
		return;
	}
	
	// Set pointers to impulses and derference the coefficient matrix
	
	for (i = 0; i < num_buffers; i++)
	{
		impulses[i].realp = impulses[0].realp + (fft_size * i);
		impulses[i].imagp = impulses[i].realp + (fft_size >> 1);
	}
	
	MATRIX_DEREF(coeff)
	
	// Do Transforms In
	
	for (i = 0; i < num_buffers; i++)
	{
		length = buffer_read(in_buffer_names[i], x->read_chan - 1, temp_buffer_f, fft_size);
		time_to_halfspectrum_float(fft_setup, temp_buffer_f, length, impulses[i], fft_size);
	}
	
	// Convert
	
	for (i = 0; i < num_buffers; i++)
	{
		if (!(i & 1))
		{
			// Odd harmonics (N.B. - i is offset by 1)
			
			// Copy first IR
			
			current_coeff = CREAL(MATRIX_ELEMENT(coeff, i, i));

			for (j = 0; j < ((AH_SIntPtr) fft_size >> 1); j++)
			{
				impulses[i].realp[j] *= current_coeff;
				impulses[i].imagp[j] *= current_coeff;
			}
			
			// Accumulate other IRs
			
			for (k = i + 2; k < num_buffers; k += 2)
			{
				current_coeff = CREAL(MATRIX_ELEMENT(coeff, i, k));

				for (j = 0; j < ((AH_SIntPtr) fft_size >> 1); j++)
				{
					impulses[i].realp[j] += impulses[k].realp[j] * current_coeff;
					impulses[i].imagp[j] += impulses[k].imagp[j] * current_coeff;
				}
			}
		}
		else 
		{
			// Even harmonics (N.B. - i is offset by 1)
			
			// Copy first IR (multiplying by j first)
			
			current_coeff = CIMAG(MATRIX_ELEMENT(coeff, i, i));

			// Zero DC / Nyquist (need to be real for real signal)
			
			impulses[i].realp[0] = 0;
			impulses[i].imagp[0] = 0;

			for (j = 1; j < ((AH_SIntPtr) fft_size >> 1); j++)
			{
				real = -impulses[i].imagp[j];
				imag = impulses[i].realp[j];
				impulses[i].realp[j] = real * current_coeff;
				impulses[i].imagp[j] = imag * current_coeff;
			}
			
			// Accumulate by other IRs (multiplying by j first)
			
			for (k = i + 2; k < num_buffers; k += 2)
			{
				current_coeff = CIMAG(MATRIX_ELEMENT(coeff, i, k));

				for (j = 1; j < ((AH_SIntPtr) fft_size >> 1); j++)
				{
					impulses[i].realp[j] += -impulses[k].imagp[j] * current_coeff; 
					impulses[i].imagp[j] += impulses[k].realp[j] * current_coeff;
				}
			}
		}

	}
	
	// Do Transforms Out
	
	for (i = 0; i < num_buffers; i++)
	{		
		spectrum_to_time(fft_setup, temp_buffer_d, impulses[i], fft_size, SPECTRUM_REAL);
		error = buffer_write(out_buffer_names[i], temp_buffer_d, fft_size, x->write_chan - 1, x->resize, sample_rate, 1.);
		overall_error = error ? true : error;
		buffer_write_error((t_object *) x, out_buffer_names[i], error);
	}
	
	// Free Resources
	
	hisstools_destroy_setup_d(fft_setup);
	ALIGNED_FREE(impulses[0].realp);
	ALIGNED_FREE(temp_buffer_d);
	matrix_destroy_complex(coeff);
	
	// Bang on success
	
	if (overall_error == false)
		outlet_bang(x->process_done);
}


