
#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <HIRT_Core_Functions.h>
#include <HIRT_Buffer_Access.h>

// Define common attributes and the class name (for the common attributes file)

#define OBJ_CLASSNAME t_irvalue
#define OBJ_USES_HIRT_READ_ATTR

#include <HIRT_Common_Attribute_Vars.h>


// Object class and structure

void *this_class;

typedef struct _irvalue
{
    t_pxobject x_obj;

    // Attributes

    HIRT_COMMON_ATTR

    double *db_spectrum;

    AH_UIntPtr fft_size;

    double sampling_rate;

    // Bang Outlet

    void *value_outlet;

} t_irvalue;


// This include deals with setup of common attributes - requires the object structure to be defined

#include <HIRT_Common_Attribute_Setup.h>


// Function prototypes

void *irvalue_new(t_symbol *s, short argc, t_atom *argv);
void irvalue_free(t_irvalue *x);
void irvalue_assist(t_irvalue *x, void *b, long m, long a, char *s);

void irvalue_float(t_irvalue *x, double freq);

void irvalue_set(t_irvalue *x, t_symbol *source, double smooth);


//////////////////////////////////////////////////////////////////////////
/////////////////////// Main / New / Free / Assist ///////////////////////
//////////////////////////////////////////////////////////////////////////


int main()
{
    this_class = class_new("irvalue~",
                          (method) irvalue_new,
                          (method)irvalue_free,
                          sizeof(t_irvalue),
                          0L,
                          A_GIMME,
                          0);

    class_addmethod(this_class, (method)irvalue_set, "set", A_SYM, A_DEFFLOAT, 0L);
    class_addmethod(this_class, (method)irvalue_float, "float", A_FLOAT, 0L);

    class_addmethod(this_class, (method)irvalue_assist, "assist", A_CANT, 0L);
    class_register(CLASS_BOX, this_class);

    declare_HIRT_common_attributes(this_class);
    buffer_access_init();

    return 0;
}


void *irvalue_new(t_symbol *s, short argc, t_atom *argv)
{
    t_irvalue *x = (t_irvalue *)object_alloc(this_class);

    x->value_outlet = floatout(x);

    init_HIRT_common_attributes(x);

    x->db_spectrum = NULL;
    x->fft_size = 0;
    x->sampling_rate = 0.0;

    attr_args_process(x, argc, argv);

    return(x);
}


void irvalue_free(t_irvalue *x)
{
    free_HIRT_common_attributes(x);
    free(x->db_spectrum);
}


void irvalue_assist(t_irvalue *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET)
        sprintf(s,"Instructions In");
    else
        sprintf(s,"Values");
}


//////////////////////////////////////////////////////////////////////////
///////////////////////////// User Messages //////////////////////////////
//////////////////////////////////////////////////////////////////////////


void irvalue_float(t_irvalue *x, double freq)
{
    double bin, db_lo, db_hi;
    AH_UIntPtr lo_bin;

    if (freq < 0 || freq > (x->sampling_rate / 2.0))
    {
        object_error((t_object *)x, "frequency out of range");
        return;
    }

    bin = (freq / x->sampling_rate) * x->fft_size;
    lo_bin = floor(bin);

    if (lo_bin == (x->fft_size >> 1))
    {
        lo_bin--;
        bin = 1.0;
    }
    else
        bin -= lo_bin;

    db_lo = x->db_spectrum[lo_bin];
    db_hi = x->db_spectrum[lo_bin + 1];

    outlet_float(x->value_outlet, db_lo + bin * (db_hi - db_lo));
}


void octave_smooth(double *in, double *out, AH_SIntPtr size, double oct_width)
{
    AH_SIntPtr lo;
    AH_SIntPtr hi;
    AH_SIntPtr i;

    if (oct_width)
    {
        oct_width /= 2.0;
        oct_width = pow(2.0, oct_width);

        for (i = 1; i < size; i++)
            in[i] += in[i - 1];

        out[0] = in[0];

        for (i = 1; i < size; i++)
        {
            lo = (AH_SIntPtr) (i / oct_width);
            hi = (AH_SIntPtr) (i * oct_width);

            if (lo == hi)
                lo--;

            if (hi > size - 1)
                hi = size - 1;

            out[i] = ((in[hi] - in[lo]) / (hi - lo));
        }
    }
    else
    {
        for (i = 0; i < size; i++)
            out[i] = in[i];
    }
}

void irvalue_set(t_irvalue *x, t_symbol *source, double smooth)
{
    FFT_SETUP_D fft_setup;

    FFT_SPLIT_COMPLEX_D spectrum_1;

    double *db_spectrum;
    float *in;

    AH_UIntPtr fft_size;
    AH_UIntPtr fft_size_log2;

    t_atom_long read_chan = x->read_chan - 1;

    // Get input buffer lengths

    AH_SIntPtr source_length = buffer_length(source);

    // Check input buffers

    if (buffer_check((t_object *) x, source, read_chan))
        return;

    // Calculate fft size

    fft_size = calculate_fft_size((long) (source_length), &fft_size_log2);

    if (fft_size < 8)
    {
        object_error((t_object *) x, "buffers are too short, or have no length");
        return;
    }

    // Allocate momory

    fft_setup = hisstools_create_setup_d(fft_size_log2);

    spectrum_1.realp = ALIGNED_MALLOC((2 * sizeof(double) + sizeof(float)) * fft_size);
    spectrum_1.imagp = spectrum_1.realp + fft_size;

    in = (float *) (spectrum_1.imagp + fft_size);

    db_spectrum = malloc(sizeof(double) * ((fft_size >> 1) + 1));

    if (!spectrum_1.realp || !fft_setup || !in)
    {
        object_error((t_object *) x, "could not allocate temporary memory for processing");

        hisstools_destroy_setup_d(fft_setup);
        ALIGNED_FREE(spectrum_1.realp);
        ALIGNED_FREE(in);

        return;
    }

    // Get input - convert to frequency domain - get power spectrum - convert phase

    buffer_read(source, read_chan, in, fft_size);
    time_to_halfspectrum_float(fft_setup, in, source_length, spectrum_1, fft_size);

    power_full_spectrum_from_half_spectrum(spectrum_1, fft_size);
    octave_smooth(spectrum_1.realp, db_spectrum, ((fft_size >> 1) + 1), smooth);

    for (unsigned long i = 0; i < ((fft_size >> 1) + 1); i++)
        db_spectrum[i] = pow_to_db(db_spectrum[i]);

    // Free memory

    hisstools_destroy_setup_d(fft_setup);
    ALIGNED_FREE(spectrum_1.realp);

    // Assign to object

    free(x->db_spectrum);
    x->db_spectrum = db_spectrum;
    x->fft_size =fft_size;
    x->sampling_rate = buffer_sample_rate(source);
}
