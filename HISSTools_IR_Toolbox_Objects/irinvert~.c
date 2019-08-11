
#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <HIRT_Core_Functions.h>
#include <HIRT_Buffer_Access.h>
#include <HIRT_Matrix_Math.h>

// Define common attributes and the class name (for the common attributes file)

#define OBJ_CLASSNAME t_irinvert
#define OBJ_USES_HIRT_WRITE_ATTR
#define OBJ_USES_HIRT_READ_ATTR
#define OBJ_USES_HIRT_DECONVOLUTION_ATTR

#include <HIRT_Common_Attribute_Vars.h>


// Object class and structure

void *this_class;

typedef struct _irinvert
{
    t_pxobject x_obj;

    // Attributes

    HIRT_COMMON_ATTR

    // Bang Out

    void *process_done;

} t_irinvert;


// This include deals with setup of common attributes - requires the object structure to be defined

#include <HIRT_Common_Attribute_Setup.h>


// Function prototypes

void *irinvert_new(t_symbol *s, short argc, t_atom *argv);
void irinvert_free(t_irinvert *x);
void irinvert_assist(t_irinvert *x, void *b, long m, long a, char *s);

void irinvert_process(t_irinvert *x, t_symbol *sym, long argc, t_atom *argv);
void irinvert_process_internal(t_irinvert *x, t_symbol *sym, short argc, t_atom *argv);

long irinvert_matrix_mimo(t_irinvert *x, t_matrix_complex *out, t_matrix_complex *in, t_matrix_complex *temp1, t_matrix_complex *temp2, double regularization);
long irinvert_mimo_deconvolution(t_irinvert *x, FFT_SPLIT_COMPLEX_D *impulses, AH_UIntPtr fft_size, t_atom_long sources, t_atom_long receivers, double *regularization);
void irinvert_mimo(t_irinvert *x, t_symbol *sym, long argc, t_atom *argv);
void irinvert_mimo_internal(t_irinvert *x, t_symbol *sym, short argc, t_atom *argv);


//////////////////////////////////////////////////////////////////////////
/////////////////////// Main / New / Free / Assist ///////////////////////
//////////////////////////////////////////////////////////////////////////


int main(void)
{
    this_class = class_new("irinvert~",
                          (method) irinvert_new,
                          (method)irinvert_free,
                          sizeof(t_irinvert),
                          0L,
                          A_GIMME,
                          0);

    class_addmethod(this_class, (method)irinvert_process, "invert", A_GIMME, 0L);
    class_addmethod(this_class, (method)irinvert_mimo, "mimo", A_GIMME, 0L);
    class_addmethod(this_class, (method)irinvert_mimo, "mimoto", A_GIMME, 0L);
    class_addmethod(this_class, (method)irinvert_assist, "assist", A_CANT, 0L);

    declare_HIRT_common_attributes(this_class);

    class_register(CLASS_BOX, this_class);

    buffer_access_init();

    return 0;
}


void *irinvert_new(t_symbol *s, short argc, t_atom *argv)
{
    t_irinvert *x = (t_irinvert *)object_alloc(this_class);

    x->process_done = bangout(x);

    init_HIRT_common_attributes(x);
    attr_args_process(x, argc, argv);

    return(x);
}


void irinvert_free(t_irinvert *x)
{
    free_HIRT_common_attributes(x);
}


void irinvert_assist(t_irinvert *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET)
        sprintf(s,"Instructions In");
    else
        sprintf(s,"Bang on Success");
}


//////////////////////////////////////////////////////////////////////////
////////////////////////////// Invert Message ////////////////////////////
//////////////////////////////////////////////////////////////////////////


void irinvert_process(t_irinvert *x, t_symbol *sym, long argc, t_atom *argv)
{
    t_atom temp_argv[3];
    double time_mul = 1.;

    // Load and check arguments

    if (argc < 2)
    {
        object_error((t_object *) x, "not enough arguments to message %s", sym->s_name);
        return;
    }

    if (argc > 2)
    {
        time_mul = atom_getfloat(argv + 2);

        if (time_mul < 1.)
        {
            object_warn((t_object *) x, " time multiplier cannot be less than 1 (using 1)");
            time_mul = 1;
        }
    }

    temp_argv[0] = *argv++;
    temp_argv[1] = *argv++;
    atom_setfloat(temp_argv + 2, time_mul);

    defer(x, (method) irinvert_process_internal, sym, 3, temp_argv);
}


void irinvert_process_internal(t_irinvert *x, t_symbol *sym, short argc, t_atom *argv)
{
    FFT_SETUP_D fft_setup;

    FFT_SPLIT_COMPLEX_D spectrum_1;
    FFT_SPLIT_COMPLEX_D spectrum_2;
    FFT_SPLIT_COMPLEX_D spectrum_3;

    double *out_buf;
    float *in_temp;
    float *filter_in;

    t_symbol *target = atom_getsym(argv++);
    t_symbol *source_1 = atom_getsym(argv++);
    t_symbol *filter = filter_retriever(x->deconvolve_filter_specifier);

    double filter_specifier[HIRT_MAX_SPECIFIER_ITEMS];
    double range_specifier[HIRT_MAX_SPECIFIER_ITEMS];

    double time_mul = atom_getfloat(argv++);
    double sample_rate = buffer_sample_rate(source_1);
    double deconvolve_phase = phase_retriever(x->deconvolve_phase);
    double deconvolve_delay;

    AH_SIntPtr source_length_1 = buffer_length(source_1);
    AH_SIntPtr filter_length = buffer_length(filter);

    AH_UIntPtr fft_size;
    AH_UIntPtr fft_size_log2;

    t_atom_long read_chan = x->read_chan - 1;
    long deconvolve_mode = x->deconvolve_mode;
    t_buffer_write_error error;

    // Check input buffers

    if (buffer_check((t_object *) x, source_1))
        return;

    // Check and calculate length

    fft_size = calculate_fft_size((AH_UIntPtr) (source_length_1 * time_mul), &fft_size_log2);
    deconvolve_delay = delay_retriever(x->deconvolve_delay, fft_size, sample_rate);

    if (fft_size < 8)
    {
        object_error((t_object *) x, "buffers are too short, or have no length");
        return;
    }

    // Allocate Memory

    fft_setup = hisstools_create_setup_d(fft_size_log2);

    spectrum_1.realp = ALIGNED_MALLOC(sizeof(double) * fft_size * 4);
    spectrum_1.imagp = spectrum_1.realp + (fft_size >> 1);
    spectrum_2.realp = spectrum_1.imagp + (fft_size >> 1);
    spectrum_2.imagp = spectrum_2.realp + (fft_size >> 1);
    spectrum_3.realp = spectrum_2.imagp + (fft_size >> 1);
    spectrum_3.imagp = spectrum_3.realp + fft_size;

    filter_in = filter_length ? ALIGNED_MALLOC(sizeof(float *) * filter_length) : 0;

    in_temp = (float *) spectrum_3.realp;
    out_buf = spectrum_3.realp;

    // Check memory allocations

    if (!fft_setup || !spectrum_1.realp || (filter_length && !filter_in))
    {
        object_error((t_object *) x, "could not allocate temporary memory for processing");

        hisstools_destroy_setup_d(fft_setup);
        ALIGNED_FREE(spectrum_1.realp);
        ALIGNED_FREE(filter_in);

        return;
    }

    // Create modelling spike - get input - convert to frequency domain

    spike_spectrum(spectrum_1, fft_size, SPECTRUM_REAL, deconvolve_delay);
    buffer_read(source_1, read_chan, in_temp, source_length_1);
    time_to_halfspectrum_float(fft_setup, in_temp, source_length_1, spectrum_2, fft_size);

    // Fill deconvolution filter specifiers - read filter from buffer (if specified) - deconvolve

    fill_power_array_specifier(filter_specifier, x->deconvolve_filter_specifier, x->deconvolve_num_filter_specifiers);
    fill_power_array_specifier(range_specifier, x->deconvolve_range_specifier, x->deconvolve_num_range_specifiers);
    buffer_read(filter, 0, filter_in, fft_size);
    deconvolve(fft_setup, spectrum_1, spectrum_2, spectrum_3, filter_specifier, range_specifier, 0, filter_in, filter_length, fft_size, SPECTRUM_REAL, deconvolve_mode, deconvolve_phase, 0, sample_rate);

    // Convert to time domain - copy out to buffer

    spectrum_to_time(fft_setup, out_buf, spectrum_1, fft_size, SPECTRUM_REAL);
    error = buffer_write(target, out_buf, fft_size, x->write_chan - 1, x->resize, sample_rate, 1.);
    buffer_write_error((t_object *) x, target, error);

    // Free resources

    hisstools_destroy_setup_d(fft_setup);
    ALIGNED_FREE(spectrum_1.realp);
    ALIGNED_FREE(filter_in);

    if (!error)
        outlet_bang(x->process_done);
}


//////////////////////////////////////////////////////////////////////////
////////////////////////////// MIMO Inversion ////////////////////////////
//////////////////////////////////////////////////////////////////////////


// MIMO (out-of-place only)

long irinvert_matrix_mimo(t_irinvert *x, t_matrix_complex *out, t_matrix_complex *in, t_matrix_complex *temp1, t_matrix_complex *temp2, double regularization)
{
    AH_UIntPtr n_dim = in->n_dim;
    AH_UIntPtr i;

    MATRIX_REF_COMPLEX(out)
    MATRIX_REF_COMPLEX(temp2)

    // Dereference

    MATRIX_DEREF(out)
    MATRIX_DEREF(temp2)

    // Calculate

    matrix_conjugate_transpose_complex(temp1, in);
    matrix_multiply_complex(out, temp1, in);

    for (i = 0; i < n_dim; i++)
        MATRIX_ELEMENT(out, i, i) = CADD(MATRIX_ELEMENT(out, i, i), CSET(regularization, 0));

    matrix_choelsky_decompose_complex(temp2, out);
    matrix_choelsky_solve_complex(out, temp2, temp1);

    return 0;
}



long irinvert_mimo_deconvolution(t_irinvert *x, FFT_SPLIT_COMPLEX_D *impulses, AH_UIntPtr fft_size, t_atom_long sources, t_atom_long receivers, double *regularization)
{
    t_matrix_complex *in;
    t_matrix_complex *out;
    t_matrix_complex *temp1;
    t_matrix_complex *temp2;

    AH_UIntPtr fft_size_halved = fft_size >> 1;
    AH_UIntPtr i;

    t_atom_long j, k;

    COMPLEX_DOUBLE out_val;

    MATRIX_REF_COMPLEX(in)
    MATRIX_REF_COMPLEX(out)

    // N.B. size of out is set such as to take temporary calculation of square matrix

    in = matrix_alloc_complex(receivers, sources);
    out = matrix_alloc_complex(sources > receivers ? sources : receivers, sources > receivers ? sources : receivers);
    temp1 = matrix_alloc_complex(sources, receivers);
    temp2 = matrix_alloc_complex(sources, sources);

    if (!in || !out || !temp1 || !temp2)
    {
        object_error((t_object *) x, "could not allocate mmemory for matrix operations");
        goto mimo_bail;
    }

    MATRIX_DEREF(in)
    MATRIX_DEREF(out)

    // Do DC

    for (j = 0; j < receivers; j++)
        for (k = 0; k < sources; k++)
            MATRIX_ELEMENT(in, j, k) = CSET(impulses[j * sources + k].realp[0], 0);

    if (irinvert_matrix_mimo(x, out, in, temp1, temp2, regularization[0]))
        goto mimo_bail;

    for (j = 0; j < receivers; j++)
        for (k = 0; k < sources; k++)
            impulses[j * sources + k].realp[0] = CREAL(MATRIX_ELEMENT(out, j, k));

    // Do Nyquist

    for (j = 0; j < receivers; j++)
        for (k = 0; k < sources; k++)
            MATRIX_ELEMENT(in, j, k) = CSET(impulses[j * sources + k].imagp[0], 0);

    if (irinvert_matrix_mimo(x, out, in, temp1, temp2, regularization[fft_size_halved]))
        goto mimo_bail;

    for (j = 0; j < receivers; j++)
        for (k = 0; k < sources; k++)
            impulses[j * sources + k].imagp[0] = CREAL(MATRIX_ELEMENT(out, j, k));

    // Do Other Bins

    for (i = 1; i < fft_size_halved; i++)
    {
        for (j = 0; j < receivers; j++)
        {
            for (k = 0; k < sources; k++)
                MATRIX_ELEMENT(in, j, k) = CSET(impulses[j * sources + k].realp[i], impulses[j * sources + k].imagp[i]);
        }

        if (irinvert_matrix_mimo(x, out, in, temp1, temp2, regularization[i]))
            break;

        for (j = 0; j < receivers; j++)
        {
            for (k = 0; k < sources; k++)
            {
                out_val = MATRIX_ELEMENT(out, j, k);
                impulses[j * sources + k].realp[i] = CREAL(out_val);
                impulses[j * sources + k].imagp[i] = CIMAG(out_val);
            }
        }
    }

    if (i == fft_size_halved)
    {
        matrix_destroy_complex(in);
        matrix_destroy_complex(out);
        matrix_destroy_complex(temp1);
        matrix_destroy_complex(temp2);

        return 0;
    }

mimo_bail:

    matrix_destroy_complex(in);
    matrix_destroy_complex(out);
    matrix_destroy_complex(temp1);
    matrix_destroy_complex(temp2);

    return 1;
}


void irinvert_mimo(t_irinvert *x, t_symbol *sym, long argc, t_atom *argv)
{
    defer(x, (method) irinvert_mimo_internal, sym, (short) argc, argv);
}


void irinvert_mimo_internal(t_irinvert *x, t_symbol *sym, short argc, t_atom *argv)
{
    FFT_SETUP_D fft_setup;

    FFT_SPLIT_COMPLEX_D impulses[128];

    t_symbol *in_buffer_names[128];
    t_symbol *out_buffer_names[128];

    double filter_specifier[HIRT_MAX_SPECIFIER_ITEMS];

    double *temp_buffer_d;
    double *regularization;
    float *temp_buffer_f;

    AH_SIntPtr lengths[128];

    double sample_rate = 0.0;
    double time_mul = 1.;
    double deconvolve_delay;

    t_atom_long receivers;
    t_atom_long sources;

    AH_UIntPtr fft_size;
    AH_UIntPtr fft_size_log2;

    AH_SIntPtr overall_length = 0;
    AH_SIntPtr max_length = 0;
    AH_SIntPtr length;
    AH_SIntPtr num_buffers = 0;
    AH_SIntPtr i;

    t_buffer_write_error error;
    long in_place = 1;
    t_atom_long read_chan = x->read_chan - 1;
    t_atom_long write_chan = x->write_chan - 1;

    AH_Boolean overall_error = false;

    if (sym == gensym("mimoto"))
        in_place = 0;

    // Get number of sources / receivers

    if (argc < 2)
    {
        object_error((t_object *) x, "not enough arguments to message %s", sym->s_name);
        return;
    }

    sources = atom_getlong(argv++);
    argc--;

    if (sources <= 0)
    {
        object_error((t_object *) x, "not enough arguments to message %s", sym->s_name);
        return;
    }

    if (argc && (atom_gettype(argv) == A_LONG || atom_gettype(argv) == A_FLOAT))
    {
        time_mul = atom_getfloat(argv++);
        argc--;

        if (time_mul < 1)
        {
            object_warn((t_object *) x, " time multiplier cannot be less than 1 (using 1)");
            time_mul = 1;
        }
    }

    receivers = in_place ? argc / sources : argc / (sources * 2);
    num_buffers = in_place ? argc : argc / 2;

    if (sources * receivers != num_buffers)
    {
        object_error((t_object *) x, "number of specified buffers is not divisible by the number of sources - additional or missing IRs");
        return;
    }

    // Check buffers, storing names and lengths +  calculate total / largest length

    num_buffers = buffer_multiple_names((t_object *) x, in_buffer_names, out_buffer_names, lengths, argc, argv, in_place, 128, &overall_length, &max_length, &sample_rate);

    if (!num_buffers)
        return;

    // Calculate fft size

    fft_size = calculate_fft_size((AH_UIntPtr) (max_length * time_mul), &fft_size_log2);

    deconvolve_delay = delay_retriever(x->deconvolve_delay, fft_size, sample_rate);

    // Check length of buffers for writing

    if (!x->resize)
    {
        for (i = 0; i < num_buffers; i++)
        {
            if (buffer_length(out_buffer_names[i]) < (AH_SIntPtr) fft_size)
            {
                object_error((t_object *) x, "buffer %s is not long enough to complete write (no buffers altered)", out_buffer_names[i]->s_name);
                return;
            }
        }
    }

    // Allocate Resources

    fft_setup = hisstools_create_setup_d(fft_size_log2);
    temp_buffer_d = ALIGNED_MALLOC(sizeof(double) * fft_size * 2);
    temp_buffer_f = (float *) temp_buffer_d;
    regularization = temp_buffer_d + fft_size;
    impulses[0].realp = ALIGNED_MALLOC(sizeof(double) * fft_size * sources * receivers);

    // Check Memory Allocations

    if (!fft_setup || !temp_buffer_d || !impulses[0].realp)
    {
        object_error((t_object *) x, "could not allocate temporary memory for processing");

        hisstools_destroy_setup_d(fft_setup);
        ALIGNED_FREE(impulses[0].realp);
        ALIGNED_FREE(temp_buffer_d);

        return;
    }

    // Set pointers for impulses

    for (i = 0; i < sources * receivers; i++)
    {
        impulses[i].realp = impulses[0].realp + (fft_size * i);
        impulses[i].imagp = impulses[i].realp + (fft_size >> 1);
    }

    // Do transforms in

    for (i = 0; i < receivers * sources; i++)
    {
        length = buffer_read(in_buffer_names[i], read_chan, temp_buffer_f, fft_size);
        time_to_halfspectrum_float(fft_setup, temp_buffer_f, length, impulses[i], fft_size);
    }

    // Prepare regularisation

    fill_power_array_specifier(filter_specifier, x->deconvolve_filter_specifier, x->deconvolve_num_filter_specifiers);
    make_freq_dependent_power_array(regularization, filter_specifier, fft_size, sample_rate, 0);

    // Deconvolve

    if (!irinvert_mimo_deconvolution(x, impulses, fft_size, sources, receivers, regularization))
    {
        // Do transforms out

        for (i = 0; i < receivers * sources; i++)
        {
            delay_spectrum(impulses[i], fft_size, SPECTRUM_REAL, deconvolve_delay);
            spectrum_to_time(fft_setup, temp_buffer_d, impulses[i], fft_size, SPECTRUM_REAL);
            error = buffer_write(out_buffer_names[i], temp_buffer_d, fft_size, write_chan, x->resize, sample_rate, 1.);
            buffer_write_error((t_object *) x, out_buffer_names[i], error);

            if (error)
                overall_error = true;
        }
    }

    hisstools_destroy_setup_d(fft_setup);
    ALIGNED_FREE(impulses[0].realp);
    ALIGNED_FREE(temp_buffer_d);

    // Bang on success

    if (overall_error == false)
        outlet_bang(x->process_done);
}


