
#include <ext.h>
#include <ext_obex.h>
#include <ext_obex_util.h>
#include <z_dsp.h>

#include <HIRT_Memory.hpp>
#include <HIRT_Buffer_Access.hpp>
#include <HIRT_Multichannel_Convolution/Convolver.h>


// Object class and structure

t_class *this_class;

constexpr int MAXIMUM_MSP_CHANS = 64;


struct t_multiconvolve
{
    t_pxobject x_obj;

    HISSTools::Convolver *multi;

    long parallel_mode;
    long num_in_chans;
    long num_out_chans;

    float *ins[MAXIMUM_MSP_CHANS];
    float *outs[MAXIMUM_MSP_CHANS];

    long fixed_impulse_length;
};


// Function prototypes

t_max_err multiconvolve_fixed_size_set(t_multiconvolve *x, t_object *attr, long argc, t_atom *argv);
t_atom_long multiconvolve_arg_check(t_multiconvolve *x, char *name, t_atom_long val, t_atom_long min, t_atom_long max);

void *multiconvolve_new(t_symbol *s, short argc, t_atom *argv);
void multiconvolve_free(t_multiconvolve *x);
void multiconvolve_assist(t_multiconvolve *x, void *b, long m, long a, char *s);

void multiconvolve_clear(t_multiconvolve *x);
void multiconvolve_set(t_multiconvolve *x, t_symbol *sym, long argc, t_atom *argv);

t_int *multiconvolve_perform(t_int *w);
void multiconvolve_perform64(t_multiconvolve *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);
void multiconvolve_dsp(t_multiconvolve *x, t_signal **sp, short *count);
void multiconvolve_dsp64(t_multiconvolve *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);


//////////////////////////////////////////////////////////////////////////
////////////////////// Attribute Getters and Setters /////////////////////
//////////////////////////////////////////////////////////////////////////


t_max_err multiconvolve_fixed_size_set(t_multiconvolve *x, t_object *attr, long argc, t_atom *argv)
{
    long error = 0;

    if (argc)
    {
        t_atom_long new_fixed_size = atom_getlong(argv);
        
        new_fixed_size = new_fixed_size < 0 ? 0 : new_fixed_size;

        uintptr_t final_size = new_fixed_size ? static_cast<uintptr_t>(new_fixed_size) : uintptr_t(16384u);
        
        if (new_fixed_size != x->fixed_impulse_length && x->multi)
        {
            if (!x->parallel_mode)
            {
                for (long i = 0; i < x->num_out_chans; i++)
                    for (long j = 0; j < x->num_in_chans; j++)
                        if (x->multi->resize(j, i, final_size) == CONVOLVE_ERR_MEM_UNAVAILABLE)
                            error = 1;
            }
            else
            {
                for (long i = 0; i < x->num_in_chans; i++)
                    if (x->multi->resize(i, i, final_size) == CONVOLVE_ERR_MEM_UNAVAILABLE)
                        error = 1;
            }

            if (error)
                object_error((t_object *) x, "could not allocate memory for fixed size");
        }
        x->fixed_impulse_length = static_cast<long>(new_fixed_size);
    }

    return 0;
}


//////////////////////////////////////////////////////////////////////////
///////////////////////////// Argument Check /////////////////////////////
//////////////////////////////////////////////////////////////////////////


t_atom_long multiconvolve_arg_check(t_multiconvolve *x, const char *name, t_atom_long val, t_atom_long min, t_atom_long max)
{
    t_atom_long new_val = val;
    long check = 0;

    if (val < min)
    {
        check = 1;
        new_val = min;
    }

    if (val > max)
    {
        check = 1;
        new_val = max;
    }

    if (check)
        object_error((t_object *) x, "argument out of range: setting %s to %lf", name, new_val);

    return new_val;
}


//////////////////////////////////////////////////////////////////////////
/////////////////////// Main / New / Free / Assist ///////////////////////
//////////////////////////////////////////////////////////////////////////


int C74_EXPORT main()
{
    t_object *attr_temp;

    this_class = class_new("multiconvolve~",
                          (method)multiconvolve_new,
                          (method)multiconvolve_free,
                          sizeof(t_multiconvolve),
                          0L,
                          A_GIMME,
                          0);

    class_addmethod(this_class, (method)multiconvolve_assist, "assist", A_CANT, 0L);
    class_addmethod(this_class, (method)multiconvolve_dsp64, "dsp64", A_CANT, 0L);
    class_addmethod(this_class, (method)multiconvolve_dsp, "dsp", A_CANT, 0L);

    class_addmethod(this_class, (method)multiconvolve_set, "set", A_GIMME, 0L);
    class_addmethod(this_class, (method)multiconvolve_clear, "clear", 0L);

    CLASS_ATTR_LONG(this_class, "fixedsize", 0L, t_multiconvolve, fixed_impulse_length);
    CLASS_ATTR_ACCESSORS(this_class, "fixedsize", 0L, multiconvolve_fixed_size_set);

    // The macro in the SDK for this is not C89 friendly

    attr_temp = (t_object *) class_attr_get(this_class, gensym("fixedsize"));
    object_method(attr_temp , gensym("setflags"), (t_ptr_int) object_method(attr_temp, gensym("getflags"))| ATTR_SET_DEFER);

    CLASS_ATTR_LABEL(this_class,"fixedsize", 0L, "Fixed Impulse Length (samples)");

    class_dspinit(this_class);

    class_register(CLASS_BOX, this_class);

    return 0;
}


void *multiconvolve_new(t_symbol *s, short argc, t_atom *argv)
{
    t_multiconvolve *x = reinterpret_cast<t_multiconvolve *>(object_alloc(this_class));

    t_atom_long num_in_chans = 1;
    t_atom_long num_out_chans = 0;
    LatencyMode latency_mode = kLatencyZero;

    long actual_num_out_chans;

    if (!x)
        return x;

    if (argc && atom_gettype(argv) != A_SYM)
    {
        num_in_chans = atom_getlong(argv++);
        num_in_chans = multiconvolve_arg_check(x, "number of in channels", num_in_chans, 1, MAXIMUM_MSP_CHANS);
        argc--;
    }

    if (argc && atom_gettype(argv) != A_SYM)
    {
        num_out_chans = atom_getlong(argv++);
        num_out_chans = multiconvolve_arg_check(x, "number of in channels", num_out_chans, 1, MAXIMUM_MSP_CHANS);
        argc--;
    }

    if (argc && atom_gettype(argv) == A_SYM && atom_getsym(argv) != gensym("@fixedsize"))
    {
        t_symbol *mode = atom_getsym(argv++);

        if (mode == gensym("short"))
            latency_mode = kLatencyShort;
        if (mode == gensym("medium"))
            latency_mode = kLatencyMedium;

        if (!latency_mode && mode != gensym("zero"))
            object_error((t_object *) x, "unknown latency mode - %s", mode->s_name);

        argc--;
    }

    actual_num_out_chans = static_cast<long>(num_out_chans <= 0 ? num_in_chans : num_out_chans);

    dsp_setup((t_pxobject *)x, static_cast<long>(num_in_chans));
    for (long i = 0; i < actual_num_out_chans; i++)
        outlet_new(x, "signal");

    if (!num_out_chans)
        x->parallel_mode = 1;
    else
        x->parallel_mode = 0;

    x->fixed_impulse_length = 0;
    x->num_in_chans = static_cast<long>(num_in_chans);
    x->num_out_chans = actual_num_out_chans;

    if (x->parallel_mode)
        x->multi = new HISSTools::Convolver(x->num_in_chans, latency_mode);
    else
        x->multi = new HISSTools::Convolver(x->num_in_chans, x->num_out_chans, latency_mode);
    
    attr_args_process(x, argc, argv);

    return x;
}


void multiconvolve_free(t_multiconvolve *x)
{
    dsp_free((t_pxobject *)x);

    delete x->multi;
}


void multiconvolve_assist(t_multiconvolve *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_OUTLET)
    {
        sprintf(s,"(signal) Output %ld", a + 1);
    }
    else
    {
        sprintf(s,"(signal) Input %ld", a + 1);
    }
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////// Clear and Set IRs ///////////////////////////
//////////////////////////////////////////////////////////////////////////


void multiconvolve_clear(t_multiconvolve *x)
{
    if (!x->multi)
        return;

    x->multi->clear(!x->fixed_impulse_length);
}


void multiconvolve_set(t_multiconvolve *x, t_symbol *sym, long argc, t_atom *argv)
{
    ConvolveError set_error = CONVOLVE_ERR_NONE;

    t_symbol *buffer;

    t_atom_long in_chan;
    t_atom_long out_chan;
    t_atom_long read_chan;

    if (argc < 1)
    {
        object_error((t_object *) x, "not enough arguments to message %s", sym->s_name);
        return;
    }

    // Get input channel if present

    if (atom_gettype(argv) == A_LONG)
    {
        in_chan = atom_getlong(argv++) - 1;
        argc--;
    }
    else
        in_chan = 0;

    // Get out channel if present

    if (atom_gettype(argv) == A_LONG)
    {
        if (argc == 1)
        {
            object_error((t_object *) x, "no buffer given for set message");
            return;
        }
        out_chan = atom_getlong(argv++) - 1;
        argc--;
    }
    else
        out_chan = in_chan;

    if (x->parallel_mode && out_chan != in_chan)
    {
        object_error((t_object *) x, "parallel mode disallows mismatching input/output channels (%ld to %ld requested)", in_chan + 1, out_chan + 1);
        return;
    }

    // Get buffer

    buffer = atom_getsym(argv++);
    argc--;

    // Get read channel

    if (argc)
    {
        read_chan = atom_getlong(argv++) - 1;
        argc--;
    }
    else
        read_chan = 0;

    // Check buffer

    if (buffer_check((t_object *) x, buffer))
        return;

    // Load to temporary buffer

    intptr_t impulse_length = buffer_length(buffer);
    temp_ptr<float> temp(impulse_length);

    if (!temp)
    {
        object_error((t_object *) x, "could not allocate temporary memory for temporary storage");
        return;
    }

    buffer_read(buffer, read_chan, temp.get(), impulse_length);

    if (x->multi)
        set_error = x->multi->set(in_chan, out_chan, temp.get(), impulse_length, !x->fixed_impulse_length);
    
    if (set_error == CONVOLVE_ERR_IN_CHAN_OUT_OF_RANGE)
        object_error((t_object *) x, "input channel %ld out of range", in_chan + 1);
    if (set_error == CONVOLVE_ERR_OUT_CHAN_OUT_OF_RANGE)
        object_error((t_object *) x, "output channel %ld out of range", out_chan + 1);
    if (set_error == CONVOLVE_ERR_MEM_UNAVAILABLE)
        object_error((t_object *) x, "memory unavailable for set operation");
    if (set_error == CONVOLVE_ERR_MEM_ALLOC_TOO_SMALL)
        object_error((t_object *) x, "requested buffer / length too large for set fixed size");
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////// Perform Routines ////////////////////////////
//////////////////////////////////////////////////////////////////////////


t_int *multiconvolve_perform(t_int *w)
{
    t_multiconvolve *x = reinterpret_cast<t_multiconvolve *>(w[1]);
    long vec_size = static_cast<long>(w[2]);

    x->multi->process(x->ins, x->outs, x->num_in_chans, x->num_out_chans, vec_size);

    return w + 3;
}

void multiconvolve_perform64(t_multiconvolve *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
    x->multi->process(ins, outs, x->num_in_chans, x->num_out_chans, vec_size);
}


//////////////////////////////////////////////////////////////////////////
///////////////////////////// DSP Routines ///////////////////////////////
//////////////////////////////////////////////////////////////////////////


void multiconvolve_dsp(t_multiconvolve *x, t_signal **sp, short *count)
{
    for (long i = 0; i < x->num_in_chans; i++)
        x->ins[i] = reinterpret_cast<float *>(sp[i]->s_vec);
    for (long i = 0; i < x->num_out_chans; i++)
        x->outs[i] = reinterpret_cast<float *>(sp[i + x->num_in_chans]->s_vec);

    if (x->multi)
        dsp_add(multiconvolve_perform, 2, x, sp[0]->s_n);
}


void multiconvolve_dsp64(t_multiconvolve *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
    if (x->multi)
        object_method(dsp64, gensym("dsp_add64"), x, multiconvolve_perform64, 0, nullptr);
}
