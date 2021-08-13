
#include <ext.h>
#include <ext_obex.h>
#include <ext_obex_util.h>
#include <z_dsp.h>

#include <HIRT_Core_Functions.hpp>
#include <HIRT_Buffer_Access.hpp>

#include <HIRT_Exponential_Sweeps.hpp>
#include <HIRT_Coloured_Noise.hpp>
#include <HIRT_Max_Length_Sequences.hpp>

// Define common attributes and the class name (for the common attributes file)

#define OBJ_CLASSNAME t_irsweeps
#define OBJ_USES_HIRT_WRITE_ATTR
#define OBJ_USES_HIRT_SWEEP_AMP_CURVE_ATTR

#include <HIRT_Common_Attribute_Vars.hpp>


// Excitation signal type enum

enum t_excitation_signal
{
    SWEEP,
    INV_SWEEP,
    MLS,
    NOISE
};


// Object class and structure

t_class *this_class;

struct t_irsweeps
{
    t_pxobject x_obj;

    // Attributes

    HIRT_COMMON_ATTR

    bool inv_amp;

    double amp;

    // Internal
    
    double last_sample_rate;
    double max_amp_pink;
    double max_amp_brown;
    
    // Bang Outlet

    void *process_done;
};


// This include deals with setup of common attributes - requires the object structure to be defined

#include <HIRT_Common_Attribute_Setup.hpp>


// Function prototypes

void *irsweeps_new(t_symbol *s, short argc, t_atom *argv);
void irsweeps_free(t_irsweeps *x);
void irsweeps_assist(t_irsweeps *x, void *b, long m, long a, char *s);

double irsweeps_param_check(t_irsweeps *x, char *name, double val, double min, double max);

void irsweeps_gen(t_irsweeps *x, t_symbol *buffer, t_excitation_signal sig_type, intptr_t sig_length, void *params, double sample_rate);

void irsweeps_sweep(t_irsweeps *x, t_symbol *sym, long argc, t_atom *argv);
void irsweeps_sweep_internal(t_irsweeps *x, t_symbol *sym, short argc, t_atom *argv);
void irsweeps_noise(t_irsweeps *x, t_symbol *sym, long argc, t_atom *argv);
void irsweeps_noise_internal(t_irsweeps *x, t_symbol *sym, short argc, t_atom *argv);
void irsweeps_mls(t_irsweeps *x, t_symbol *sym, long argc, t_atom *argv);
void irsweep_mls_internal(t_irsweeps *x, t_symbol *sym, short argc, t_atom *argv);


//////////////////////////////////////////////////////////////////////////
/////////////////////// Main / New / Free / Assist ///////////////////////
//////////////////////////////////////////////////////////////////////////


int C74_EXPORT main()
{
    this_class = class_new("irsweeps~",
                          (method) irsweeps_new,
                          (method)irsweeps_free,
                          sizeof(t_irsweeps),
                          0L,
                          A_GIMME,
                          0);

    class_addmethod(this_class, (method)irsweeps_assist, "assist", A_CANT, 0L);

    class_addmethod(this_class, (method)irsweeps_sweep, "sweep", A_GIMME, 0L);
    class_addmethod(this_class, (method)irsweeps_sweep, "invsweep", A_GIMME, 0L);
    class_addmethod(this_class, (method)irsweeps_mls, "mls", A_GIMME, 0L);
    class_addmethod(this_class, (method)irsweeps_noise, "white", A_GIMME, 0L);
    class_addmethod(this_class, (method)irsweeps_noise, "pink", A_GIMME, 0L);
    class_addmethod(this_class, (method)irsweeps_noise, "brown", A_GIMME, 0L);

    class_register(CLASS_BOX, this_class);

    declare_HIRT_common_attributes(this_class);

    CLASS_ATTR_DOUBLE(this_class, "amp", 0, t_irsweeps, amp);
    CLASS_ATTR_LABEL(this_class,"amp", 0, "Maximum Amplitude (dB)");

    CLASS_ATTR_LONG(this_class, "invamp", 0, t_irsweeps, inv_amp);
    CLASS_ATTR_STYLE_LABEL(this_class,"invamp",0,"onoff","Invert Amplitude");

    return 0;
}


void *irsweeps_new(t_symbol *s, short argc, t_atom *argv)
{
    t_irsweeps *x = (t_irsweeps *)object_alloc(this_class);

    x->process_done = bangout(x);

    init_HIRT_common_attributes(x);

    x->amp = -1.0;
    x->inv_amp = 0;
    
    x->last_sample_rate = 0.0;
    x->max_amp_pink = 0.0;
    x->max_amp_brown = 0.0;
    
    attr_args_process(x, argc, argv);

    return x;
}


void irsweeps_free(t_irsweeps *x)
{
    free_HIRT_common_attributes(x);
}


void irsweeps_assist(t_irsweeps *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET)
        sprintf(s,"Instructions In");
    else
        sprintf(s,"Bang on Success");
}


//////////////////////////////////////////////////////////////////////////
///////////////////////////// Parameter Check ////////////////////////////
//////////////////////////////////////////////////////////////////////////


double irsweeps_param_check(t_irsweeps *x, const char *name, double val, double min, double max)
{
    double new_val = val;
    bool changed = false;

    if (val < min)
    {
        changed = true;
        new_val = min;
    }

    if (val > max)
    {
        changed = true;
        new_val = max;
    }

    if (changed == true)
        object_error((t_object *) x, "parameter out of range: setting %s to %lf", name, new_val);

    return new_val;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////// Generate Signal to Buffer ///////////////////////
//////////////////////////////////////////////////////////////////////////


void irsweeps_gen(t_irsweeps *x, t_symbol *buffer, t_excitation_signal sig_type, intptr_t sig_length, void *params, double sample_rate)
{
    temp_ptr<float> temp_buf(sig_length);

    if (!temp_buf)
    {
        object_error((t_object *) x, "could not allocate internal memory");
        return;
    }

    // Generate signal

    switch (sig_type)
    {
        case SWEEP:
            ((t_ess *) params)->gen(temp_buf.get(), false);
            break;

        case INV_SWEEP:
            ((t_ess *) params)->igen(temp_buf.get(), x->inv_amp ? INVERT_ALL : INVERT_USER_CURVE_TO_FIXED_REFERENCE, false);
            break;

        case MLS:
            ((t_mls *) params)->gen(temp_buf.get(), false);
            break;

        case NOISE:
            ((t_noise_params *) params)->gen(temp_buf.get(), false);
            break;
    }

    // Write to buffer

    auto error = buffer_write_float((t_object *)x, buffer, temp_buf.get(), sig_length, x->resize, x->write_chan - 1, sample_rate, 1.0);

    if (!error)
        outlet_bang(x->process_done);
}


//////////////////////////////////////////////////////////////////////////
///////////////////////////// User Messages //////////////////////////////
//////////////////////////////////////////////////////////////////////////


void irsweeps_sweep(t_irsweeps *x, t_symbol *sym, long argc, t_atom *argv)
{
    defer(x, (method) irsweeps_sweep_internal, sym, (short) argc, argv);
}

void irsweeps_sweep_internal(t_irsweeps *x, t_symbol *sym, short argc, t_atom *argv)
{
    t_symbol *buffer = NULL;
    double f1 = 20.0;
    double f2 = sys_getsr() / 2.0;
    double length = 30000.0;
    double fade_in = 50.0;
    double fade_out = 10.0;
    double sample_rate = sys_getsr();

    double amp_curve[33];

    intptr_t sweep_length;

    if (!argc || atom_gettype(argv) != A_SYM)
    {
        object_error((t_object *) x, "buffer name expected!");
        return;
    }

    // Get arguments

    buffer = atom_getsym(argv++);

    if (argc > 1)
        f1 = atom_getfloat(argv++);
    if (argc > 2)
        f2 = atom_getfloat(argv++);
    if (argc > 3)
        length = atom_getfloat(argv++);
    if (argc > 4)
        fade_in = atom_getfloat(argv++);
    if (argc > 5)
        fade_out = atom_getfloat(argv++);
    if (argc > 6)
        sample_rate = atom_getfloat(argv++);

    // Check ranges

    f1 = irsweeps_param_check(x, "low frequency", f1, 0.0001, sys_getsr() / 2.0);
    f2 = irsweeps_param_check(x, "high frequency", f2, f2, sys_getsr() / 2.0);
    length = irsweeps_param_check(x, "length", length, 0.0, HUGE_VAL);
    fade_in = irsweeps_param_check(x, "fade in time", fade_in, 0.0, length / 2.0);
    fade_out = irsweeps_param_check(x, "fade out time", fade_out, 0.0, length / 2.0);
    sample_rate = irsweeps_param_check(x, "sampling rate", sample_rate, 0.0001, HUGE_VAL);

    // Get sweep length and allocate temmporary memory

    fill_amp_curve_specifier(amp_curve, x->amp_curve_specifier, x->amp_curve_num_specifiers);
    t_ess sweep_params(f1, f2, fade_in / 1000.0, fade_out / 1000.0, length / 1000.0, sample_rate, db_to_a(x->amp), amp_curve);
    sweep_length = sweep_params.length();

    if (!sweep_length)
    {
        object_error((t_object *) x, "zero length sweep - requested length value is too small");
        return;
    }

    irsweeps_gen(x, buffer, sym == gensym("sweep") ? SWEEP : INV_SWEEP, sweep_length, &sweep_params, sample_rate);
}


void irsweeps_mls(t_irsweeps *x, t_symbol *sym, long argc, t_atom *argv)
{
    defer(x, (method) irsweep_mls_internal, sym, (short) argc, argv);
}


void irsweep_mls_internal(t_irsweeps *x, t_symbol *sym, short argc, t_atom *argv)
{
    t_symbol *buffer = NULL;
    t_atom_long order = 18;
    double sample_rate = sys_getsr();

    intptr_t mls_length;

    if (!argc || atom_gettype(argv) != A_SYM)
    {
        object_error((t_object *) x, "buffer name expected!");
        return;
    }

    // Get arguments

    buffer = atom_getsym(argv++);

    if (argc > 1)
        order = atom_getlong(argv++);
    if (argc > 2)
        sample_rate = atom_getfloat(argv++);

    order = (t_atom_long) irsweeps_param_check(x, "order", (double) order, 1, 24);

    t_mls max_length_params((uint32_t) order, db_to_a(x->amp));
    mls_length = max_length_params.length();

    irsweeps_gen(x, buffer, MLS, mls_length, &max_length_params, sample_rate);
}


void irsweeps_noise(t_irsweeps *x, t_symbol *sym, long argc, t_atom *argv)
{
    defer(x, (method) irsweeps_noise_internal, sym, (short) argc, argv);
}

void irsweeps_noise_internal(t_irsweeps *x, t_symbol *sym, short argc, t_atom *argv)
{
    t_symbol *buffer = NULL;
    double length = 10000.0;
    double fade_in = 10.0;
    double fade_out = 10.0;
    double sample_rate = sys_getsr();

    double amp_comp = 1.0;

    t_noise_mode filter_mode = NOISE_MODE_WHITE;
    intptr_t noise_length;

    if (sym == gensym("brown"))
        filter_mode = NOISE_MODE_BROWN;
    if (sym == gensym("pink"))
        filter_mode = NOISE_MODE_PINK;

    if (!argc || atom_gettype(argv) != A_SYM)
    {
        object_error((t_object *) x, "buffer name expected!");
        return;
    }

    // Get arguments

    buffer = atom_getsym(argv++);

    if (argc > 1)
        length = atom_getfloat(argv++);
    if (argc > 2)
        fade_in = atom_getfloat(argv++);
    if (argc > 3)
        fade_out = atom_getfloat(argv++);
    if (argc > 4)
        sample_rate = atom_getfloat(argv++);

    length = irsweeps_param_check(x, "length", length, 0.0, HUGE_VAL);
    fade_in = irsweeps_param_check(x, "fade in time", fade_in, 0.0, length / 2.0);
    fade_out = irsweeps_param_check(x, "fade out time", fade_out, 0.0, length / 2.0);

    if (filter_mode != NOISE_MODE_WHITE && x->last_sample_rate != sample_rate)
    {
        t_noise_params noise_params(NOISE_MODE_WHITE, 0, 0, 1, sample_rate, 1);
        noise_params.measure((1 << 25), x->max_amp_pink, x->max_amp_brown);
        
        x->last_sample_rate = sample_rate;
    }
    
    if (filter_mode == NOISE_MODE_BROWN)
        amp_comp = x->max_amp_brown;
    if (filter_mode == NOISE_MODE_PINK)
        amp_comp = x->max_amp_pink;

    t_noise_params noise_params(filter_mode, fade_in / 1000.0, fade_out / 1000.0, length / 1000.0, sample_rate, db_to_a(x->amp) / amp_comp);
    noise_length = noise_params.length();

    irsweeps_gen(x, buffer, NOISE, noise_length, &noise_params, sample_rate);
}
