
#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <HIRT_Core_Functions.h>
#include <HIRT_Buffer_Access.h>

// Define common attributes and the class name (for the common attributes file)

#define OBJ_CLASSNAME tirshift
#define OBJ_USES_HIRT_WRITE_ATTR
#define OBJ_USES_HIRT_READ_ATTR

#include <HIRT_Common_Attribute_Vars.h>


// Object class and structure

void *this_class;

typedef struct irshift
{
    t_pxobject x_obj;
	
	// Attributes
	
	HIRT_COMMON_ATTR 
	
	// Bang Outlet
	
	void *process_done;
	
} tirshift;


// This include deals with setup of common attributes - requires the object structure to be defined

#include <HIRT_Common_Attribute_Setup.h>


// Function prototypes

void *irshift_new(t_symbol *s, short argc, t_atom *argv);
void irshift_free(tirshift *x);
void irshift_assist(tirshift *x, void *b, long m, long a, char *s);

void irshift_usershift(tirshift *x, t_symbol *sym, long argc, t_atom *argv);

void irshift_process(tirshift *x, t_symbol *target, t_symbol *source, double shift);
void irshift_process_internal(tirshift *x, t_symbol *sym, short argc, t_atom *argv);


//////////////////////////////////////////////////////////////////////////
/////////////////////// Main / New / Free / Assist ///////////////////////
//////////////////////////////////////////////////////////////////////////


int main()
{
    this_class = class_new("irshift~",
							(method) irshift_new, 
							(method)irshift_free, 
							sizeof(tirshift), 
							0L,
							A_GIMME,
							0);
	
	class_addmethod(this_class, (method)irshift_usershift, "shift", A_GIMME, 0L);

	class_addmethod(this_class, (method)irshift_assist, "assist", A_CANT, 0L);
	class_register(CLASS_BOX, this_class);
	
	declare_HIRT_common_attributes(this_class);
	buffer_access_init();
	
	return 0;
}


void *irshift_new(t_symbol *s, short argc, t_atom *argv)
{
    tirshift *x = (tirshift *)object_alloc(this_class);
	
	x->process_done = bangout(x);
	
	init_HIRT_common_attributes(x);
	
	attr_args_process(x, argc, argv);

	return(x);
}


void irshift_free(tirshift *x)
{
	free_HIRT_common_attributes(x);
}


void irshift_assist(tirshift *x, void *b, long m, long a, char *s)
{
	if (m == ASSIST_INLET)
		sprintf(s,"Instructions In");
	else
		sprintf(s,"Bang on Success");
}


//////////////////////////////////////////////////////////////////////////
///////////////////////////// User Messages //////////////////////////////
//////////////////////////////////////////////////////////////////////////


void irshift_usershift(tirshift *x, t_symbol *sym, long argc, t_atom *argv)
{
	t_symbol *target;
	t_symbol *source;
    double shift;
	
	if ((argc < 3))
	{
		object_error((t_object *) x, "not enough arguments to message %s", sym->s_name);
		return;
	}
	
	target = atom_getsym(argv++);
	source = atom_getsym(argv++);
	argc--;
	argc--;
    shift = atom_getfloat(argv++);
    argc--;
    
	irshift_process(x, target, source, shift);
}


//////////////////////////////////////////////////////////////////////////
/////////////////////// Common Processing Routines ////////////////////////
//////////////////////////////////////////////////////////////////////////


void irshift_process(tirshift *x, t_symbol *target, t_symbol *source, double shift)
{
	t_atom args[3];

	atom_setsym(args + 0, target);
	atom_setsym(args + 1, source);
	atom_setfloat(args + 2, shift);
	
	defer(x, (method) irshift_process_internal, 0, 3, args);
}


void irshift_process_internal(tirshift *x, t_symbol *sym, short argc, t_atom *argv)
{
	FFT_SETUP_D fft_setup;
	
	FFT_SPLIT_COMPLEX_D spectrum_1;
	FFT_SPLIT_COMPLEX_D spectrum_2;

    t_symbol *target = atom_getsym(argv++);
	t_symbol *source = atom_getsym(argv++);
	
    double sample_rate = buffer_sample_rate(source);
    double shift = atom_getfloat(argv++);
    //double mainshift = shift;
    double lo, hi;
    
	AH_UIntPtr fft_size;
	AH_UIntPtr fft_size_log2;
    long shift_int;
    
    double *out_buf;
    float *in;
    
	t_buffer_write_error error;
	
	// Get input buffer lengths
	
	AH_SIntPtr source_length_1 = buffer_length(source);
	
	// Check input buffers
	
	if (buffer_check((t_object *) x, source))
		return;
	
	// Calculate fft size
	
	fft_size = calculate_fft_size((long) (source_length_1), &fft_size_log2);

	if (fft_size < 8)
	{
		object_error((t_object *) x, "buffers are too short, or have no length");
		return;
	}
	
    // Allocate momory

	fft_setup = hisstools_create_setup_d(fft_size_log2);
	
	spectrum_1.realp = ALIGNED_MALLOC(sizeof(double) * fft_size * 4);
	spectrum_1.imagp = spectrum_1.realp + fft_size;
	spectrum_2.realp = spectrum_1.imagp + fft_size;
	spectrum_2.imagp = spectrum_2.realp + fft_size;
		
	out_buf = spectrum_1.realp;
	in = (float *) spectrum_2.realp;

	if (!spectrum_1.realp || !fft_setup)
	{
		object_error((t_object *) x, "could not allocate temporary memory for processing");
		
		hisstools_destroy_setup_d(fft_setup);
		ALIGNED_FREE(spectrum_1.realp);
		
		return;
	}
	
	// Get input - convert to frequency domain - get power spectrum - convert phase
	
	buffer_read(source, x->read_chan - 1, in, fft_size);
	time_to_spectrum_float(fft_setup, in, source_length_1, spectrum_1, fft_size);

    // Do shifting...
    
    shift *= -(fft_size / sample_rate);
    shift_int = floor(shift);
    lo = shift - shift_int;
    hi = 1.0 - lo;
    
    for (long i = 0; i < ((fft_size >> 1) + 1); i++)
    {
        /*
        double freq = (double) i / (fft_size >> 1);
        freq = pow(freq, mainshift);
        
        shift = shift - freq;
        shift_int = floor(shift);
        lo = shift - shift_int;
        hi = 1.0 - lo;*/
        
        spectrum_2.realp[i] = 0.0;
        spectrum_2.imagp[i] = 0.0;

        if ((i + shift_int) > 0 && (i + shift_int) < (fft_size >> 1))
        {
            spectrum_2.realp[i] += spectrum_1.realp[i + shift_int] * hi;
            spectrum_2.imagp[i] += spectrum_1.imagp[i + shift_int] * hi;
        }
        
        if ((i + shift_int + 1) > 0 && (i + shift_int + 1) < (fft_size >> 1))
        {
            spectrum_2.realp[i] += spectrum_1.realp[i + shift_int + 1] * lo;
            spectrum_2.imagp[i] += spectrum_1.imagp[i + shift_int + 1] * lo;
        }
    }
    
    spectrum_2.imagp[0] = spectrum_2.realp[(fft_size >> 1)];
    

	// Convert to time domain - copy out to buffer
	
	spectrum_to_time(fft_setup, out_buf, spectrum_2, fft_size, SPECTRUM_REAL);
	error = buffer_write(target, out_buf, fft_size, x->write_chan - 1, x->resize, sample_rate, 1.0);
	buffer_write_error((t_object *) x, target, error);
	
	// Free memory
	
	hisstools_destroy_setup_d(fft_setup);
	ALIGNED_FREE(spectrum_1.realp);
	
	if (!error)
		outlet_bang(x->process_done);
}
