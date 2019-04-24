
#include <ext.h>
#include <ext_obex.h>
#include <ext_obex_util.h>
#include <z_dsp.h>

#include <HIRT_Core_Functions.h>
#include <HIRT_Buffer_Access.h>

#include <HIRT_Exponential_Sweeps.h>
#include <HIRT_Coloured_Noise.h>
#include <HIRT_Max_Length_Sequences.h>

// Define common attributes and the class name (for the common attributes file)

#define OBJ_CLASSNAME t_irsweeps
#define OBJ_USES_HIRT_WRITE_ATTR
#define OBJ_USES_HIRT_SWEEP_AMP_CURVE_ATTR

#include <HIRT_Common_Attribute_Vars.h>


// Excitation signal type enum

typedef enum {

    SWEEP,
    INV_SWEEP,
    MLS,
    NOISE

} t_excitation_signal;


// Object class and structure

void *this_class;

typedef struct _irsweeps
{
    t_pxobject x_obj;

    // Attributes

    HIRT_COMMON_ATTR

    AH_Boolean inv_amp;

    double amp;

    // Bang Outlet

    void *process_done;

} t_irsweeps;


// This include deals with setup of common attributes - requires the object structure to be defined

#include <HIRT_Common_Attribute_Setup.h>


// Function prototypes

void *irsweeps_new(t_symbol *s, short argc, t_atom *argv);
void irsweeps_free(t_irsweeps *x);
void irsweeps_assist(t_irsweeps *x, void *b, long m, long a, char *s);

double irsweeps_param_check(t_irsweeps *x, char *name, double val, double min, double max);

void irsweeps_gen(t_irsweeps *x, t_symbol *buffer, t_excitation_signal sig_type, AH_SIntPtr sig_length, void *params, double sample_rate);

void irsweeps_sweep(t_irsweeps *x, t_symbol *sym, long argc, t_atom *argv);
void irsweeps_sweep_internal(t_irsweeps *x, t_symbol *sym, short argc, t_atom *argv);
void irsweeps_noise(t_irsweeps *x, t_symbol *sym, long argc, t_atom *argv);
void irsweeps_noise_internal(t_irsweeps *x, t_symbol *sym, short argc, t_atom *argv);
void irsweeps_mls(t_irsweeps *x, t_symbol *sym, long argc, t_atom *argv);
void irsweep_mls_internal(t_irsweeps *x, t_symbol *sym, short argc, t_atom *argv);


//////////////////////////////////////////////////////////////////////////
/////////////////////// Main / New / Free / Assist ///////////////////////
//////////////////////////////////////////////////////////////////////////


int main()
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

    buffer_access_init();

    return 0;
}


void *irsweeps_new(t_symbol *s, short argc, t_atom *argv)
{
    t_irsweeps *x = (t_irsweeps *)object_alloc(this_class);

    x->process_done = bangout(x);

    init_HIRT_common_attributes(x);

    x->amp = -1.0;
    x->inv_amp = 0;

    attr_args_process(x, argc, argv);

    return(x);
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


double irsweeps_param_check(t_irsweeps *x, char *name, double val, double min, double max)
{
    double new_val = val;
    AH_Boolean changed = false;

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


void irsweeps_gen(t_irsweeps *x, t_symbol *buffer, t_excitation_signal sig_type, AH_SIntPtr sig_length, void *params, double sample_rate)
{
    // This can all be re-used for other signals

    t_buffer_write_error error;

    float *temp_buf = malloc(sig_length * sizeof(float));

    if (!temp_buf)
    {
        object_error((t_object *) x, "could not allocate internal memory");
        return;
    }

    // Generate signal

    switch (sig_type)
    {
        case SWEEP:
            ess_gen((t_ess *) params, temp_buf, false);
            break;

        case INV_SWEEP:
            ess_igen((t_ess *) params, temp_buf, x->inv_amp ? INVERT_ALL : INVERT_USER_CURVE_TO_FIXED_REFERENCE, false);
            break;

        case MLS:
            mls_gen((t_mls *) params, temp_buf, false);
            break;

        case NOISE:
            coloured_noise_gen((t_noise_params *)params, temp_buf, false);
            break;
    }

    // Write to buffer

    error = buffer_write_float(buffer, temp_buf, sig_length, x->resize, x->write_chan - 1, sample_rate, 1.0);
    buffer_write_error((t_object *) x, buffer, error);

    // Free temporary memory

    free(temp_buf);

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

    t_ess sweep_params;
    AH_SIntPtr sweep_length;

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
    sweep_length = ess_params(&sweep_params, f1, f2, fade_in / 1000.0, fade_out / 1000.0, length / 1000.0, sample_rate, db_to_a(x->amp), amp_curve);

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

    t_mls max_length_params;
    AH_SIntPtr mls_length;

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

    mls_params(&max_length_params, (AH_UInt32) order, db_to_a(x->amp));
    mls_length = mls_get_length(&max_length_params);

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

    double max_amp_pink;
    double max_amp_brown;
    double amp_comp = 1.0;

    t_noise_mode filter_mode = NOISE_MODE_WHITE;
    t_noise_params noise_params;
    AH_SIntPtr noise_length;

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

    coloured_noise_params(&noise_params, 0, 0, 0, 1, sample_rate, 1);
    if (filter_mode != NOISE_MODE_WHITE)
        coloured_noise_measure(&noise_params, (AH_UIntPtr) (sample_rate * length / 1000.0), &max_amp_pink, &max_amp_brown);
    coloured_noise_reset(&noise_params);

    if (filter_mode == NOISE_MODE_BROWN)
        amp_comp = max_amp_brown;
    if (filter_mode == NOISE_MODE_PINK)
        amp_comp = max_amp_pink;

    coloured_noise_params(&noise_params, filter_mode, fade_in / 1000.0, fade_out / 1000.0, length / 1000.0, sample_rate, db_to_a(x->amp) / amp_comp);
    noise_length = coloured_noise_get_length(&noise_params);

    irsweeps_gen(x, buffer, NOISE, noise_length, &noise_params, sample_rate);
}
