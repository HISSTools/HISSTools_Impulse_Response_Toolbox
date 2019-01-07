
#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <HIRT_Core_Functions.h>
#include <HIRT_Buffer_Access.h>

// Define common attributes and the class name (for the common attributes file)

#define OBJ_CLASSNAME t_iraverage
#define OBJ_USES_HIRT_WRITE_ATTR
#define OBJ_USES_HIRT_READ_ATTR
#define OBJ_USES_HIRT_OUT_PHASE_ATTR
#define OBJ_USES_HIRT_SMOOTH_ATTR

#include <HIRT_Common_Attribute_Vars.h>


// Object class and structure

void *this_class;

typedef struct _iraverage
{
    t_pxobject x_obj;
	
	// Attributes
	
	HIRT_COMMON_ATTR 
		
	// Bang Outlet
	
	void *process_done;
	
} t_iraverage;


// This include deals with setup of common attributes - requires the object structure to be defined

#include <HIRT_Common_Attribute_Setup.h>


// Function prototypes

void *iraverage_new(t_symbol *s, short argc, t_atom *argv);
void iraverage_free(t_iraverage *x);
void iraverage_assist(t_iraverage *x, void *b, long m, long a, char *s);

void iraverage_process(t_iraverage *x, t_symbol *sym, long argc, t_atom *argv);
void iraverage_process_internal(t_iraverage *x, t_symbol *sym, short argc, t_atom *argv);

void iraverage_average(t_iraverage *x, t_symbol *sym, long argc, t_atom *argv);
void iraverage_average_internal(t_iraverage *x, t_symbol *sym, short argc, t_atom *argv);


//////////////////////////////////////////////////////////////////////////
/////////////////////// Main / New / Free / Assist ///////////////////////
//////////////////////////////////////////////////////////////////////////


int main()
{
    this_class = class_new("iraverage~",
							(method) iraverage_new, 
							(method)iraverage_free, 
							sizeof(t_iraverage), 
							0L,		
							A_GIMME,
							0);
		
	class_addmethod(this_class, (method)iraverage_process, "process", A_GIMME, 0L);
	class_addmethod(this_class, (method)iraverage_average, "average", A_GIMME, 0L);
		
	class_addmethod(this_class, (method)iraverage_assist, "assist", A_CANT, 0L);
	
	declare_HIRT_common_attributes(this_class);
	
	class_register(CLASS_BOX, this_class);
	
	buffer_access_init();
	
	return 0;
}


void *iraverage_new(t_symbol *s, short argc, t_atom *argv)
{
    t_iraverage *x = (t_iraverage *)object_alloc(this_class);

	x->process_done = bangout(x);
							  	
	init_HIRT_common_attributes(x);
	attr_args_process(x, argc, argv);

	return(x);
}


void iraverage_free(t_iraverage *x)
{	
	free_HIRT_common_attributes(x);
}


void iraverage_assist(t_iraverage *x, void *b, long m, long a, char *s)
{
	if (m == ASSIST_INLET)
		sprintf(s,"Instructions In");
	else
		sprintf(s,"Bang on Success");
}


//////////////////////////////////////////////////////////////////////////
///////////////////////////// User messages //////////////////////////////
//////////////////////////////////////////////////////////////////////////


void iraverage_process(t_iraverage *x, t_symbol *sym, long argc, t_atom *argv)
{
	defer(x, (method) iraverage_process_internal, sym, (short) argc, argv);
}


void iraverage_process_internal(t_iraverage *x, t_symbol *sym, short argc, t_atom *argv)
{
	FFT_SETUP_D fft_setup;

	FFT_SPLIT_COMPLEX_D spectrum_1;
	FFT_SPLIT_COMPLEX_D spectrum_2;

	double *temp;
	
	t_symbol *target;
	t_symbol *buffer_names[128];

	double time_mul = 1.0;
	double sample_rate = 0.0;
	
	AH_SIntPtr lengths[128];
	
	AH_UIntPtr fft_size;
	AH_UIntPtr fft_size_log2;
	
	AH_SIntPtr num_buffers;
	AH_SIntPtr max_length;
	AH_SIntPtr read_length;
	AH_SIntPtr overall_length;
	AH_SIntPtr i, j;
	
    t_atom_long read_chan = x->read_chan - 1;
    t_atom_long write_chan = x->write_chan - 1;
    
	t_buffer_write_error error;
	
	if (!argc)
	{
		object_error((t_object *) x, "not enough arguments to message %s", sym->s_name);
		return;
	}	
	
	target = atom_getsym(argv++);
	argc--;
		
	if (argc && (atom_gettype(argv) == A_LONG || atom_gettype(argv) == A_FLOAT))
	{
		time_mul = atom_getfloat(argv++);
		argc--;
		
		if (time_mul < 1.0)
		{
			object_warn((t_object *) x, " time multiplier cannot be less than 1 (using 1)");
			time_mul = 1;
		}
	}
				
	// Check buffers, storing names and lengths +  calculate total / largest length
	
	num_buffers = buffer_multiple_names((t_object *) x, buffer_names, buffer_names, lengths, argc, argv, read_chan, write_chan, 1, 128, &overall_length, &max_length, &sample_rate);
	
	if (!num_buffers)
		return;
	
	// Calculate FFT size
	
	fft_size = calculate_fft_size((AH_UIntPtr) (max_length * time_mul), &fft_size_log2);
	
	// Allocate Memory 
	
	fft_setup = hisstools_create_setup_d(fft_size_log2);

	temp = (double *) ALIGNED_MALLOC(5 * fft_size * sizeof(double));
	spectrum_1.realp = temp + fft_size;
	spectrum_1.imagp = spectrum_1.realp + fft_size;
	spectrum_2.realp = spectrum_1.imagp + fft_size;
	spectrum_2.imagp = spectrum_2.realp + fft_size;
	
	// Check momory allocation
	
	if (!fft_setup || !temp)
	{
		object_error((t_object *) x, "could not allocate temporary memory for processing");
		
		hisstools_destroy_setup_d(fft_setup);
		ALIGNED_FREE(temp);
		
		return;
	}
	
	// Zero accumulation 
	
	for (j = 0; j < (AH_SIntPtr) fft_size; j++)
	{
		spectrum_1.realp[j] = 0.0;
		spectrum_1.imagp[j] = 0.0;
	}
		
	// Take FFTs and average
	
	for (i = 0; i < argc; i++)
	{		
		// Read buffer - convert to frequency domain - take power spectrum
		
		read_length = buffer_read(buffer_names[i], read_chan, (float *) temp, fft_size);
		time_to_spectrum_float(fft_setup, (float *) temp, read_length, spectrum_2, fft_size);
		power_spectrum(spectrum_2, fft_size, SPECTRUM_FULL);
		
		// Accumulate
		
		for (j = 0; j < (AH_SIntPtr) fft_size; j++)
			spectrum_1.realp[j] += spectrum_2.realp[j] / num_buffers;
	}
	
	// Do smoothing
	
	if (x->num_smooth)
		smooth_power_spectrum(spectrum_1, x->smooth_mode, fft_size, x->num_smooth > 1 ? x->smooth[0] : 0.0, x->num_smooth > 1 ? x->smooth[1] : x->smooth[0]);

	// Change phase - convert to time domain - copy out to buffer
	
	variable_phase_from_power_spectrum(fft_setup, spectrum_1, fft_size, phase_retriever(x->out_phase), false);
	spectrum_to_time(fft_setup, temp, spectrum_1, fft_size, SPECTRUM_FULL);
	error = buffer_write(target, temp, fft_size, write_chan, x->resize, sample_rate, 1.0);
	buffer_write_error((t_object *) x, target, error);

	// Free Resources
	
	hisstools_destroy_setup_d(fft_setup);
	ALIGNED_FREE(temp);
	
	if (!error)
		outlet_bang(x->process_done);
}


void iraverage_average(t_iraverage *x, t_symbol *sym, long argc, t_atom *argv)
{
	defer(x, (method) iraverage_average_internal, sym, (short) argc, argv);
}


void iraverage_average_internal(t_iraverage *x, t_symbol *sym, short argc, t_atom *argv)
{
	double *accum;
	float *temp;			
	
	t_symbol *target;
	t_symbol *buffer_names[128];
	
	AH_SIntPtr lengths[128];
	
	AH_SIntPtr num_buffers;
	AH_SIntPtr max_length;
	AH_SIntPtr read_length;
	AH_SIntPtr overall_length;
	AH_SIntPtr i, j;
	
	double num_buf_recip;
	double sample_rate = 0.0;
	
    t_atom_long read_chan = x->read_chan - 1;
    t_atom_long write_chan = x->write_chan - 1;

	t_buffer_write_error error;
	
	// Check there are some arguments
	
	if (!argc)
	{
		object_error((t_object *) x, "not enough arguments to message %s", sym->s_name);
		return;
	}	
	
	target = atom_getsym(argv++);
	argc--;
		
	// Check buffers, storing names and lengths +  calculate total / largest length
	
	num_buffers = buffer_multiple_names((t_object *) x, buffer_names, buffer_names, lengths, argc, argv, read_chan, write_chan, 1, 128, &overall_length, &max_length, &sample_rate);
	num_buf_recip = 1.0 / num_buffers;
	
	if (!num_buffers)
		return;
	
	// Allocate memory 
	
	temp = (float *) ALIGNED_MALLOC(max_length * (sizeof(float) + sizeof(double)));
	accum = (double *) (temp + max_length);
	
	// Check memory allocation
	
	if (!temp)
	{
		object_error((t_object *) x, "could not allocate temporary memory for processing");
		free(temp);
		return;
	}
	
	// Zero accumulation 
	
	for (j = 0; j < max_length; j++)
		accum[j] = 0.0;
	
	// Average
	
	for (i = 0; i < num_buffers; i++)
	{		
		read_length = buffer_read(buffer_names[i], read_chan, (float *) temp, max_length);
		
		for (j = 0; j < read_length; j++)
			accum[j] += temp[j];
	}
	
	// Divide by number of buffers
	
	for (j = 0; j < max_length; j++)
		accum[j] *= num_buf_recip;
	
	// Copy out to buffer
	
	error = buffer_write(target, accum, max_length, write_chan, x->resize, sample_rate, 1.0);
	buffer_write_error((t_object *) x, target, error);
	
	// Free Resources
	
	ALIGNED_FREE(temp);
	
	if (!error)
		outlet_bang(x->process_done);
}
