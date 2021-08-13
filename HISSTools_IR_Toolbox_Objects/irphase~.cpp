
#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <HIRT_Core_Functions.hpp>
#include <HIRT_Buffer_Access.hpp>

// Define common attributes and the class name (for the common attributes file)

#define OBJ_CLASSNAME t_irphase
#define OBJ_USES_HIRT_WRITE_ATTR
#define OBJ_USES_HIRT_READ_ATTR
#define OBJ_USES_HIRT_DECONVOLUTION_ATTR

#include <HIRT_Common_Attribute_Vars.hpp>


// Phase type enum

enum t_phase_type {

    MODE_MINPHASE,
    MODE_MAXPHASE,
    MODE_LINPHASE,
    MODE_ALLPASS,
    MODE_MIXEDPHASE,
    MODE_IMPOSE
};


// Object class and structure

t_class *this_class;

struct t_irphase
{
    t_pxobject x_obj;

    // Attributes

    HIRT_COMMON_ATTR

    // Bang Outlet

    void *process_done;
};


// This include deals with setup of common attributes - requires the object structure to be defined

#include <HIRT_Common_Attribute_Setup.hpp>


// Function prototypes

void *irphase_new(t_symbol *s, short argc, t_atom *argv);
void irphase_free(t_irphase *x);
void irphase_assist(t_irphase *x, void *b, long m, long a, char *s);

void irphase_userphase(t_irphase *x, t_symbol *sym, long argc, t_atom *argv);

void irphase_process(t_irphase *x, t_symbol *target, t_symbol *source, double phase, double time_mul, t_phase_type mode);
void irphase_process_internal(t_irphase *x, t_symbol *sym, short argc, t_atom *argv);


//////////////////////////////////////////////////////////////////////////
/////////////////////// Main / New / Free / Assist ///////////////////////
//////////////////////////////////////////////////////////////////////////


int C74_EXPORT main()
{
    this_class = class_new("irphase~",
                          (method) irphase_new,
                          (method)irphase_free,
                          sizeof(t_irphase),
                          0L,
                          A_GIMME,
                          0);

    class_addmethod(this_class, (method)irphase_userphase, "minphase", A_GIMME, 0L);
    class_addmethod(this_class, (method)irphase_userphase, "maxphase", A_GIMME, 0L);
    class_addmethod(this_class, (method)irphase_userphase, "linphase", A_GIMME, 0L);
    class_addmethod(this_class, (method)irphase_userphase, "mixedphase", A_GIMME, 0L);
    class_addmethod(this_class, (method)irphase_userphase, "allpass", A_GIMME, 0L);

    class_addmethod(this_class, (method)irphase_assist, "assist", A_CANT, 0L);
    class_register(CLASS_BOX, this_class);

    declare_HIRT_common_attributes(this_class);

    return 0;
}


void *irphase_new(t_symbol *s, short argc, t_atom *argv)
{
    t_irphase *x = (t_irphase *)object_alloc(this_class);

    x->process_done = bangout(x);

    init_HIRT_common_attributes(x);

    // Change filter specifier to more appropriate default

    atom_setlong(x->deconvolve_filter_specifier + 0, -1000);
    atom_setlong(&x->deconvolve_delay, 0);
    x->deconvolve_num_filter_specifiers = 1;

    attr_args_process(x, argc, argv);

    return x;
}


void irphase_free(t_irphase *x)
{
    free_HIRT_common_attributes(x);
}


void irphase_assist(t_irphase *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET)
        sprintf(s,"Instructions In");
    else
        sprintf(s,"Bang on Success");
}


//////////////////////////////////////////////////////////////////////////
///////////////////////////// User Messages //////////////////////////////
//////////////////////////////////////////////////////////////////////////


void irphase_userphase(t_irphase *x, t_symbol *sym, long argc, t_atom *argv)
{
    t_symbol *target;
    t_symbol *source;
    double phase;
    double time_mul = 1.0;
    t_phase_type mode;

    if ((sym != gensym("mixedphase") && argc < 2) || (sym == gensym("mixedphase") && argc < 3))
    {
        object_error((t_object *) x, "not enough arguments to message %s", sym->s_name);
        return;
    }

    target = atom_getsym(argv++);
    source = atom_getsym(argv++);
    argc--;
    argc--;

    if (sym == gensym("minphase"))
    {
        mode = MODE_MINPHASE;
        phase = 0.0;
    }

    if (sym == gensym("maxphase"))
    {
        mode = MODE_MAXPHASE;
        phase = 1.0;
    }

    if (sym == gensym("linphase"))
    {
        mode = MODE_LINPHASE;
        phase = 0.5;
    }

    if (sym == gensym("allpass"))
    {
        mode = MODE_ALLPASS;
        phase = 0.0;
    }

    if (sym == gensym("mixedphase"))
    {
        mode = MODE_MIXEDPHASE;
        phase = atom_getfloat(argv++);
        argc--;
    }

    if (argc)
    {
        time_mul = atom_getfloat(argv++);
        argc--;
    }


    irphase_process(x, target, source, phase, time_mul, mode);
}


//////////////////////////////////////////////////////////////////////////
/////////////////////// Common Processing Routines ////////////////////////
//////////////////////////////////////////////////////////////////////////


void irphase_process(t_irphase *x, t_symbol *target, t_symbol *source, double phase, double time_mul, t_phase_type mode)
{
    t_atom args[5];

    atom_setsym(args + 0, target);
    atom_setsym(args + 1, source);
    atom_setfloat(args + 2, phase);
    atom_setfloat(args + 3, time_mul);
    atom_setlong(args + 4, mode);

    defer(x, (method) irphase_process_internal, 0, 5, args);
}


void irphase_process_internal(t_irphase *x, t_symbol *sym, short argc, t_atom *argv)
{
    FFT_SPLIT_COMPLEX_D spectrum_1;
    FFT_SPLIT_COMPLEX_D spectrum_2;
    FFT_SPLIT_COMPLEX_D spectrum_3;

    t_symbol *filter = filter_retriever(x->deconvolve_filter_specifier);
    t_symbol *target = atom_getsym(argv++);
    t_symbol *source = atom_getsym(argv++);

    double filter_specifier[HIRT_MAX_SPECIFIER_ITEMS];
    double range_specifier[HIRT_MAX_SPECIFIER_ITEMS];

    double phase = atom_getfloat(argv++);
    double time_mul = atom_getfloat(argv++);
    double sample_rate = buffer_sample_rate(source);
    double deconvolve_delay;
    double deconvolve_phase;

    t_phase_type mode = (t_phase_type) atom_getlong(argv++);

    uintptr_t fft_size;
    uintptr_t fft_size_log2;
    uintptr_t i;

    t_filter_type deconvolve_mode;
    t_atom_long read_chan = x->read_chan - 1;

    // Get input buffer lengths

    intptr_t source_length_1 = buffer_length(source);
    intptr_t filter_length = buffer_length(filter);
    intptr_t max_length = source_length_1;

    // Check input buffers

    if (buffer_check((t_object *) x, source))
        return;

    // Calculate fft size

    time_mul = time_mul == 0.0 ? 1.0 : time_mul;

    if (time_mul < 1.0)
    {
        object_warn((t_object *) x, "time multiplier cannot be less than 1 (using 1)");
        time_mul = 1.0;
    }

    fft_size = calculate_fft_size((long) (max_length * time_mul), &fft_size_log2);

    if (fft_size < 8)
    {
        object_error((t_object *) x, "buffers are too short, or have no length");
        return;
    }

    deconvolve_mode = (t_filter_type) x->deconvolve_mode;
    deconvolve_phase = phase_retriever(x->deconvolve_phase);
    deconvolve_delay = delay_retriever(x->deconvolve_delay, fft_size, sample_rate);
    
    // Allocate momory

    temp_fft_setup fft_setup(fft_size_log2);
    
    temp_ptr<double> temp(fft_size * (mode == MODE_ALLPASS ? 6 : 3));
    temp_ptr<float> filter_in(filter_length);

    spectrum_1.realp = temp.get();
    spectrum_1.imagp = spectrum_1.realp + fft_size;
    spectrum_2.realp = spectrum_1.imagp + fft_size;
    spectrum_2.imagp = mode == MODE_ALLPASS ? spectrum_2.realp + fft_size : 0;
    spectrum_3.realp = mode == MODE_ALLPASS ? spectrum_2.imagp + fft_size : 0;
    spectrum_3.imagp = mode == MODE_ALLPASS ? spectrum_3.realp + fft_size : 0;

    double *out_buf = mode == MODE_ALLPASS ? spectrum_3.realp : spectrum_2.realp;
    float *in = reinterpret_cast<float *>(out_buf);

    if (!fft_setup || !temp || (filter_length && !filter_in))
    {
        object_error((t_object *) x, "could not allocate temporary memory for processing");
        return;
    }

    // Get input - convert to frequency domain - get power spectrum - convert phase

    buffer_read(source, read_chan, in, fft_size);
    time_to_spectrum_float(fft_setup, in, source_length_1, spectrum_1, fft_size);
    power_spectrum(spectrum_1, fft_size, SPECTRUM_FULL);
    variable_phase_from_power_spectrum(fft_setup, spectrum_1, fft_size, phase, false);

    if (mode == MODE_ALLPASS)
    {
        // Copy minimum phase spectrum to spectrum_2

        for (i = 0; i < fft_size; i++)
        {
            spectrum_2.realp[i] = spectrum_1.realp[i];
            spectrum_2.imagp[i] = spectrum_1.imagp[i];

        }

        // Get input again

        time_to_spectrum_float(fft_setup, in, source_length_1, spectrum_1, fft_size);

        // Fill deconvolution filter specifiers - read filter from buffer (if specified) - deconvolve input by minimum phase spectrum

        fill_power_array_specifier(filter_specifier, x->deconvolve_filter_specifier, x->deconvolve_num_filter_specifiers);
        fill_power_array_specifier(range_specifier, x->deconvolve_range_specifier, x->deconvolve_num_range_specifiers);
        buffer_read(filter, 0, filter_in.get(), fft_size);
        deconvolve(fft_setup, spectrum_1, spectrum_2, spectrum_3, filter_specifier, range_specifier, 0, filter_in.get(), filter_length, fft_size, SPECTRUM_FULL, deconvolve_mode, deconvolve_phase, deconvolve_delay, sample_rate);
    }

    // Convert to time domain - copy out to buffer

    spectrum_to_time(fft_setup, out_buf, spectrum_1, fft_size, SPECTRUM_FULL);
    auto error = buffer_write((t_object *)x, target, out_buf, fft_size, x->write_chan - 1, x->resize, sample_rate, 1);

    if (!error)
        outlet_bang(x->process_done);
}
