
#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <HIRT_Core_Functions.hpp>
#include <HIRT_Buffer_Access.hpp>

// Define common attributes and the class name (for the common attributes file)

#define OBJ_CLASSNAME t_iruser
#define OBJ_USES_HIRT_WRITE_ATTR
#define OBJ_USES_HIRT_OUT_PHASE_ATTR

#include <HIRT_Common_Attribute_Vars.hpp>


// Object class and structure

t_class *this_class;

struct t_iruser
{
    t_pxobject x_obj;

    // Data

    t_atom *response_specifier;

    long num_specifiers;

    // Attributes

    HIRT_COMMON_ATTR

    // Bang Outlet

    void *make_done;
};


// This include deals with setup of common attributes - requires the object structure to be defined

#include <HIRT_Common_Attribute_Setup.hpp>


// Function prototypes

t_max_err impulse_specification_setter(OBJ_CLASSNAME *x, t_object *attr, long argc, t_atom *argv);
t_max_err response_specification_getter(OBJ_CLASSNAME *x, t_object *attr, long *argc, t_atom **argv);

void *iruser_new(t_symbol *s, short argc, t_atom *argv);
void iruser_free(t_iruser *x);
void iruser_assist(t_iruser *x, void *b, long m, long a, char *s);

void iruser_make(t_iruser *x, t_symbol *sym, long argc, t_atom *argv);
void iruser_make_internal(t_iruser *x, t_symbol *sym, short argc, t_atom *argv);


//////////////////////////////////////////////////////////////////////////
////////////////////// Attribute Getters and Setters /////////////////////
//////////////////////////////////////////////////////////////////////////


t_max_err impulse_specification_setter(OBJ_CLASSNAME *x, t_object *attr, long argc, t_atom *argv)
{
    long type;
    long i;

    if (argc && argv)
    {
        if (argc > 1 && argc & 1)
        {
            object_error((t_object *) x, "frequency without level found in frequency response specification list - removing last value");
            argc--;
        }

        for (i = 0; i < argc; i++, argv++)
        {
            type = atom_gettype(argv);

            if (type == A_LONG || type == A_FLOAT)
                x->response_specifier[i] = *argv;
            else
            {
                atom_setlong(x->response_specifier + i, 0);
                object_error((t_object *) x, "symbol found in frequency response specification list - set to 0");
            }
        }

        x->num_specifiers = argc;
    }
    else
    {
        atom_setlong(x->response_specifier, 0);
        x->num_specifiers = 1;
    }

    return MAX_ERR_NONE;
}


t_max_err response_specification_getter(OBJ_CLASSNAME *x, t_object *attr, long *argc, t_atom **argv)
{
    char alloc;
    long i;

    atom_alloc_array (x->num_specifiers, argc, argv, &alloc);

    for (i = 0; i < x->num_specifiers; i++)
        (*argv)[i] = x->response_specifier[i];

    return MAX_ERR_NONE;
}


//////////////////////////////////////////////////////////////////////////
/////////////////////// Main / New / Free / Assist ///////////////////////
//////////////////////////////////////////////////////////////////////////


int C74_EXPORT main()
{
    this_class = class_new("iruser~",
                          (method) iruser_new,
                          (method)iruser_free,
                          sizeof(t_iruser),
                          0L,
                          A_GIMME,
                          0);

    class_addmethod(this_class, (method)iruser_make, "make", A_GIMME, 0L);
    class_addmethod(this_class, (method)iruser_assist, "assist", A_CANT, 0L);

    declare_HIRT_common_attributes(this_class);

    CLASS_ATTR_ATOM_VARSIZE(this_class, "response", 0, OBJ_CLASSNAME, response_specifier, num_specifiers, HIRT_MAX_SPECIFIER_ITEMS);
    CLASS_ATTR_ACCESSORS(this_class, "response", (method) response_specification_getter, (method) impulse_specification_setter);
    CLASS_ATTR_LABEL(this_class, "response", 0 , "Frequency Specification");

    class_register(CLASS_BOX, this_class);

    return 0;
}


void *iruser_new(t_symbol *s, short argc, t_atom *argv)
{
    t_iruser *x = reinterpret_cast<t_iruser *>(object_alloc(this_class));

    x->make_done = bangout(x);
    x->response_specifier = (t_atom *) malloc(sizeof(t_atom) * HIRT_MAX_SPECIFIER_ITEMS);

    if (!x->response_specifier)
    {
        object_error((t_object *) x, "cannot allocate memory for frequency response specifier");
        object_free(x);
        return 0;
    }

    impulse_specification_setter(x, 0, 0, 0);

    init_HIRT_common_attributes(x);
    attr_args_process(x, argc, argv);

    return x;
}


void iruser_free(t_iruser *x)
{
    free_HIRT_common_attributes(x);
    free(x->response_specifier);
}


void iruser_assist(t_iruser *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET)
        sprintf(s,"Instructions In");
    else
        sprintf(s,"Bang on Success");
}


//////////////////////////////////////////////////////////////////////////
///////////////////////////// User Messages //////////////////////////////
//////////////////////////////////////////////////////////////////////////


void iruser_make(t_iruser *x, t_symbol *sym, long argc, t_atom *argv)
{
    defer(x, (method) iruser_make_internal, sym, (short) argc, argv);
}


void iruser_make_internal(t_iruser *x, t_symbol *sym, short argc, t_atom *argv)
{
    FFT_SPLIT_COMPLEX_D spectrum_1;

    double impulse_specifier[HIRT_MAX_SPECIFIER_ITEMS];

    t_atom_long fft_size = 4096;

    double sample_rate = 0.0;

    if (!argc)
    {
        object_error((t_object *) x, "no arguments to message %s", sym->s_name);
        return;
    }

    // Load in arguments

    t_symbol *target = atom_getsym(argv++);
    if (argc > 1)
        fft_size = atom_getlong(argv++);
    if (argc > 2)
        sample_rate = atom_getfloat(argv++);

    // Check sample rate

    sample_rate = sample_rate < 0.0 ? -sample_rate : sample_rate;
    sample_rate = sample_rate == 0.0 ? sys_getsr() : sample_rate;
    sample_rate = sample_rate == 0.0 ? 44100.0 : sample_rate;

    // Calculate FFT size

    uintptr_t fft_size_log2;
    t_atom_long old_fft_size = fft_size;
    fft_size = fft_size < 0x10 ? 0x10 : fft_size;
    fft_size = fft_size > 0x10000000 ? 0x10000000 : fft_size;
    fft_size = calculate_fft_size(fft_size, fft_size_log2);

    if (old_fft_size != fft_size)
        object_warn((t_object *) x, "fft size out of range, or non-power of two - using %ld", fft_size);

    // Allocate Memory

    temp_fft_setup fft_setup(fft_size_log2);
    
    temp_ptr<double> temp(3 * fft_size);
    
    double *out_temp = temp.get();
    spectrum_1.realp = out_temp + fft_size;
    spectrum_1.imagp = spectrum_1.realp + fft_size;

    // Check memory allocation

    if (!fft_setup || !temp)
    {
        object_error((t_object *) x, "could not allocate temporary memory");
        return;
    }

    // Get specifier - make power array - zero imaginary part - change phase

    fill_power_array_specifier(impulse_specifier, x->response_specifier, x->num_specifiers);
    make_freq_dependent_power_array(spectrum_1.realp, impulse_specifier, fft_size, sample_rate, 0);
    for (t_atom_long i = 0; i < fft_size; i++)
        spectrum_1.imagp[i] = 0.0;
    variable_phase_from_power_spectrum(fft_setup, spectrum_1, fft_size, phase_retriever(x->out_phase), false);

    // Inverse FFT - copy out to buffer

    spectrum_to_time(fft_setup, out_temp, spectrum_1, fft_size, SPECTRUM_FULL);
    auto error = buffer_write((t_object *)x, target, out_temp, fft_size, x->write_chan - 1, x->resize, sample_rate, 1.0);

    if (!error)
        outlet_bang(x->make_done);
}
