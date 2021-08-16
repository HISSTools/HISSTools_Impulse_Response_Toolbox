
#include <ext.h>
#include <ext_obex.h>
#include <ext_obex_util.h>
#include <z_dsp.h>

#include <AH_Memory_Swap.h>
#include <algorithm>

#include <HIRT_Core_Functions.hpp>
#include <HIRT_Buffer_Access.hpp>

#include <HIRT_Exponential_Sweeps.hpp>
#include <HIRT_Coloured_Noise.hpp>
#include <HIRT_Max_Length_Sequences.hpp>

// Define common attributes and the class name (for the common attributes file)

#define OBJ_CLASSNAME t_irmeasure
#define OBJ_USES_HIRT_WRITE_ATTR
#define OBJ_USES_HIRT_DECONVOLUTION_ATTR
#define OBJ_DOES_NOT_USE_HIRT_DECONVOLUTION_DELAY
#define OBJ_USES_HIRT_SWEEP_AMP_CURVE_ATTR

#include <HIRT_Common_Attribute_Vars.hpp>


// Excitation signal type enum

enum t_excitation_signal
{
    SWEEP,
    MLS,
    NOISE
};


// Object class and structure

t_class *this_class;

struct t_irmeasure
{
    t_pxobject x_obj;

    // Object Parameters

    // Requested Sweep / MLS / Noise Parameters

    double lo_f;
    double hi_f;
    double fade_in;
    double fade_out;
    double length;
    double out_length;
    double sample_rate;

    long order;
    long num_active_ins;
    long num_active_outs;

    t_noise_mode noise_mode;

    // Internal

    double test_tone_freq;
    double current_out_length;
    double phase;

    long start_measurement;
    long stop_measurement;
    long test_tone;
    long inv_amp;
    long no_dsp;

    intptr_t current_t;
    intptr_t T;
    intptr_t T2;
    intptr_t chan_offset[HIRT_MAX_MEASURE_CHANS];
    uintptr_t fft_size;

    long num_in_chans;
    long num_out_chans;
    long current_num_active_ins;
    long current_num_active_outs;

    void *in_chans[HIRT_MAX_MEASURE_CHANS];
    void *out_chans[HIRT_MAX_MEASURE_CHANS + 1];

    // Noise Amp Compensation

    double max_amp_brown;
    double max_amp_pink;

    // Measurement Parameters

    t_excitation_signal measure_mode;

    t_ess sweep_params;
    t_mls max_length_params;
    t_noise_params noise_params;

    // Permanent Memory

    t_safe_mem_swap rec_mem;
    t_safe_mem_swap out_mem;

    // Amplitude Curve Temp

    double amp_curve[33];

    // Attributes

    HIRT_COMMON_ATTR

    long abs_progress;
    long bandlimit;

    double amp;
    double ir_gain;

    // Bang Outlet

    void *process_done;
};


// This include deals with setup of common attributes - requires the object structure to be defined

#include <HIRT_Common_Attribute_Setup.hpp>


// Function prototypes

void *irmeasure_new(t_symbol *s, short argc, t_atom *argv);
void irmeasure_free(t_irmeasure *x);
void irmeasure_assist(t_irmeasure *x, void *b, long m, long a, char *s);

intptr_t irmeasure_calc_sweep_mem_size(t_ess& sweep_params, long num_out_chans, double out_length, double sample_rate);
intptr_t irmeasure_calc_mls_mem_size(long order, long num_out_chans, double out_length, double sample_rate);
intptr_t irmeasure_calc_noise_mem_size(double length, long num_out_chans, double out_length, double sample_rate);
intptr_t irmeasure_calc_mem_size(t_irmeasure *x, long num_in_chans, long num_out_chans, double sample_rate);

double irmeasure_param_check(t_irmeasure *x, char *name, double val, double min, double max);

void irmeasure_sweep(t_irmeasure *x, t_symbol *sym, long argc, t_atom *argv);
void irmeasure_mls(t_irmeasure *x, t_symbol *sym, long argc, t_atom *argv);
void irmeasure_noise(t_irmeasure *x, t_symbol *sym, long argc, t_atom *argv);
void irmeasure_tone(t_irmeasure *x, double freq, t_atom_long chan);
void irmeasure_stop(t_irmeasure *x);
void irmeasure_clear(t_irmeasure *x);

void irmeasure_active_ins(t_irmeasure *x, t_atom_long num_active_ins);
void irmeasure_active_outs(t_irmeasure *x, t_atom_long num_active_outs);

void irmeasure_reprocess(t_irmeasure *x);
void irmeasure_process(t_irmeasure *x, t_symbol *sym, short argc, t_atom *argv);

void irmeasure_extract(t_irmeasure *x, t_symbol *sym, long argc, t_atom *argv);
void irmeasure_extract_internal(t_irmeasure *x, t_symbol *sym, short argc, t_atom *argv);
void irmeasure_dump(t_irmeasure *x, t_symbol *sym, long argc, t_atom *argv);
void irmeasure_dump_internal(t_irmeasure *x, t_symbol *sym, short argc, t_atom *argv);
void irmeasure_getir(t_irmeasure *x, t_symbol *sym, long argc, t_atom *argv);
void irmeasure_getir_internal(t_irmeasure *x, t_symbol *sym, short argc, t_atom *argv);

void irmeasure_sweep_params(t_irmeasure *x);
void irmeasure_mls_params(t_irmeasure *x);
void irmeasure_noise_params(t_irmeasure *x);
void irmeasure_params(t_irmeasure *x);

static inline void irmeasure_perform_excitation(t_irmeasure *x, void *out, long current_t, long vec_size, bool double_precision);
t_int *irmeasure_perform(t_int *w);
void irmeasure_perform64(t_irmeasure *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);

void irmeasure_dsp_common(t_irmeasure *x, double samplerate);
void irmeasure_dsp(t_irmeasure *x, t_signal **sp, short *count);
void irmeasure_dsp64(t_irmeasure *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);


//////////////////////////////////////////////////////////////////////////
/////////////////////// Main / New / Free / Assist ///////////////////////
//////////////////////////////////////////////////////////////////////////


int C74_EXPORT main()
{
    this_class = class_new("irmeasure~",
                          (method) irmeasure_new,
                          (method)irmeasure_free,
                          sizeof(t_irmeasure),
                          0L,
                          A_GIMME,
                          0);

    class_addmethod(this_class, (method)irmeasure_assist, "assist", A_CANT, 0L);
    class_addmethod(this_class, (method)irmeasure_dsp, "dsp", A_CANT, 0L);
    class_addmethod(this_class, (method)irmeasure_dsp64, "dsp64", A_CANT, 0L);

    class_addmethod(this_class, (method)irmeasure_sweep, "sweep", A_GIMME, 0L);
    class_addmethod(this_class, (method)irmeasure_mls, "mls", A_GIMME, 0L);
    class_addmethod(this_class, (method)irmeasure_noise, "white", A_GIMME, 0L);
    class_addmethod(this_class, (method)irmeasure_noise, "brown", A_GIMME, 0L);
    class_addmethod(this_class, (method)irmeasure_noise, "pink", A_GIMME, 0L);

    class_addmethod(this_class, (method)irmeasure_tone, "tone", A_FLOAT, A_DEFLONG, 0L);
    class_addmethod(this_class, (method)irmeasure_stop, "stop", 0L);
    class_addmethod(this_class, (method)irmeasure_clear, "clear", 0L);

    class_addmethod(this_class, (method)irmeasure_active_ins, "activeins", A_LONG, 0L);
    class_addmethod(this_class, (method)irmeasure_active_outs, "activeouts", A_LONG, 0L);

    class_addmethod(this_class, (method)irmeasure_reprocess, "reprocess", 0L);

    class_addmethod(this_class, (method)irmeasure_extract, "extract", A_GIMME, 0L);
    class_addmethod(this_class, (method)irmeasure_dump, "dump", A_GIMME, 0L);
    class_addmethod(this_class, (method)irmeasure_getir, "getir", A_GIMME, 0L);

    declare_HIRT_common_attributes(this_class);

    CLASS_ATTR_LONG(this_class, "absprogress", 0, t_irmeasure, abs_progress);
    CLASS_ATTR_STYLE_LABEL(this_class,"absprogress",0,"onoff","Absolute Progress");

    CLASS_ATTR_DOUBLE(this_class, "amp", 0, t_irmeasure, amp);
    CLASS_ATTR_LABEL(this_class,"amp", 0, "Signal Amplitude (dB)");

    CLASS_ATTR_LONG(this_class, "invamp", 0, t_irmeasure, inv_amp);
    CLASS_ATTR_STYLE_LABEL(this_class,"invamp",0,"onoff","Invert Amplitude");

    CLASS_ATTR_DOUBLE(this_class, "irgain", 0, t_irmeasure, ir_gain);
    CLASS_ATTR_LABEL(this_class,"irgain", 0, "IR Gain (dB)");
    
    CLASS_ATTR_LONG(this_class, "bandlimit", 0, t_irmeasure, bandlimit);
    CLASS_ATTR_STYLE_LABEL(this_class,"bandlimit",0,"onoff","Bandlimit Sweep Measurements");

    class_dspinit(this_class);

    class_register(CLASS_BOX, this_class);

    return 0;
}


void *irmeasure_new(t_symbol *s, short argc, t_atom *argv)
{
    t_irmeasure *x = reinterpret_cast<t_irmeasure *>(object_alloc(this_class));
    t_atom_long num_out_chans = 1;
    t_atom_long num_in_chans = 1;
    long i;

    if (argc && atom_gettype(argv) == A_LONG)
    {
        num_in_chans = atom_getlong(argv++);
        num_in_chans = num_in_chans < 1 ? 1 : num_in_chans;
        num_in_chans = num_in_chans > HIRT_MAX_MEASURE_CHANS ? HIRT_MAX_MEASURE_CHANS : num_in_chans;
        argc--;
    }

    if (argc && atom_gettype(argv) == A_LONG)
    {
        num_out_chans = atom_getlong(argv++);
        num_out_chans = num_out_chans < 1 ? 1 : num_out_chans;
        num_out_chans = num_out_chans > HIRT_MAX_MEASURE_CHANS ? HIRT_MAX_MEASURE_CHANS : num_out_chans;
        argc--;
    }

    x->process_done = bangout(x);

    for (i = 0; i < num_out_chans + 1; i++)
        outlet_new(x, "signal");
    dsp_setup((t_pxobject *)x, static_cast<long>(num_in_chans));

    init_HIRT_common_attributes(x);

    x->bandlimit = 1;
    x->abs_progress = 0;
    x->amp = -1.0;
    x->inv_amp = 0;
    x->ir_gain = 0.0;

    x->T2 = 0;
    x->current_t = 0;
    x->fft_size = 0;
    x->start_measurement = 0;
    x->stop_measurement = 0;
    x->test_tone = 0;
    x->no_dsp = 1;
    x->sample_rate = sys_getsr();

    if (!x->sample_rate)
        x->sample_rate = 44100.0;

    alloc_mem_swap(&x->rec_mem, 0, 0);
    alloc_mem_swap(&x->out_mem, 0, 0);
    
    x->num_in_chans = static_cast<long>(num_in_chans);
    x->num_out_chans = static_cast<long>(num_out_chans);
    x->num_active_ins = static_cast<long>(num_in_chans);
    x->num_active_outs = static_cast<long>(num_out_chans);
    x->current_num_active_ins = static_cast<long>(num_in_chans);
    x->current_num_active_outs = static_cast<long>(num_out_chans);

    x->measure_mode = SWEEP;
    x->phase = 0.0;

    attr_args_process(x, argc, argv);

    return x;
}


void irmeasure_free(t_irmeasure *x)
{
    dsp_free((t_pxobject *)x);

    free_mem_swap(&x->rec_mem);
    free_mem_swap(&x->out_mem);

    free_HIRT_common_attributes(x);
}


void irmeasure_assist(t_irmeasure *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_OUTLET)
    {
        if (a == x->num_out_chans + 1)
            sprintf(s,"Bang When Done");
        if (a == x->num_out_chans)
            sprintf(s,"(signal) Record Progress");
        if (a < x->num_out_chans)
            sprintf(s,"(signal) Audio Output %ld", a + 1);
    }
    else
    {
        if (a == 0)
            sprintf(s,"(signal) Audio Input 1 / Instructions In");
        else
            sprintf(s,"(signal) Audio Input %ld", a + 1);
    }
}


//////////////////////////////////////////////////////////////////////////
///////////////////////// Calculate Memory Size //////////////////////////
//////////////////////////////////////////////////////////////////////////


intptr_t irmeasure_calc_sweep_mem_size(t_ess& sweep_params, long num_out_chans, double out_length, double sample_rate)
{
    intptr_t gen_length = sweep_params.length();
    intptr_t rec_length = static_cast<intptr_t>(num_out_chans * ((out_length * sample_rate) + gen_length));

    return rec_length * sizeof(double);
}


intptr_t irmeasure_calc_mls_mem_size(long order, long num_out_chans, double out_length, double sample_rate)
{
    intptr_t gen_length = (1 << order) - 1;
    intptr_t rec_length = static_cast<intptr_t>(num_out_chans * ((out_length * sample_rate) + gen_length));

    return rec_length * sizeof(double);
}


intptr_t irmeasure_calc_noise_mem_size(double length, long num_out_chans, double out_length, double sample_rate)
{
    intptr_t gen_length = static_cast<intptr_t>(length * sample_rate);
    intptr_t rec_length = static_cast<intptr_t>(num_out_chans * ((out_length * sample_rate) + gen_length));

    return rec_length * sizeof(double);
}


intptr_t irmeasure_calc_mem_size(t_irmeasure *x, long num_in_chans, long num_out_chans, double sample_rate)
{
    switch (x->measure_mode)
    {
        case SWEEP:
            return num_in_chans * irmeasure_calc_sweep_mem_size(x->sweep_params, num_out_chans, x->current_out_length, sample_rate);

        case MLS:
            return num_in_chans * irmeasure_calc_mls_mem_size(x->max_length_params.order(), num_out_chans, x->current_out_length, sample_rate);

        case NOISE:
            return num_in_chans * irmeasure_calc_noise_mem_size(x->length, num_out_chans, x->current_out_length, sample_rate);
    }

    return 0;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////// Parameter Check /////////////////////////////
//////////////////////////////////////////////////////////////////////////


double irmeasure_param_check(t_irmeasure *x, const char *name, double val, double min, double max)
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

    if (changed)
        object_error((t_object *) x, "parameter out of range: setting %s to %lf", name, new_val);

    return new_val;
}


//////////////////////////////////////////////////////////////////////////
/////////////////////////// Measurement Messages /////////////////////////
//////////////////////////////////////////////////////////////////////////


void irmeasure_sweep(t_irmeasure *x, t_symbol *sym, long argc, t_atom *argv)
{
    double f1 = 20.0;
    double f2 = sys_getsr() / 2.0;
    double length = 30000.0;
    double fade_in = 50.0;
    double fade_out = 10.0;
    double out_length = 5000.0;

    long num_active_ins = x->num_active_ins;
    long num_active_outs = x->num_active_outs;

    // Load parameters

    if (argc > 0)
        f1 = atom_getfloat(argv++);
    if (argc > 1)
        f2 = atom_getfloat(argv++);
    if (argc > 2)
        length = atom_getfloat(argv++);
    if (argc > 3)
        fade_in = atom_getfloat(argv++);
    if (argc > 4)
        fade_out = atom_getfloat(argv++);
    if (argc > 5)
        out_length = atom_getfloat(argv++);

    // Check parameters

    f1 = irmeasure_param_check(x, "low frequency", f1, 0.0001, x->sample_rate / 2.0);
    f2 = irmeasure_param_check(x, "high frequency", f2, f2, x->sample_rate / 2.0);
    length = irmeasure_param_check(x, "length", length, 0.0, HUGE_VAL);
    fade_in = irmeasure_param_check(x, "fade in time", fade_in, 0.0, length / 2.0);
    fade_out = irmeasure_param_check(x, "fade out time", fade_out, 0.0, length / 2.0);
    out_length = irmeasure_param_check(x, "ir length", out_length, 0.0, HUGE_VAL);

    // Store parameters

    x->lo_f = f1;
    x->hi_f = f2;
    x->fade_in = fade_in / 1000.0;
    x->fade_out = fade_out / 1000.0;
    x->length = length / 1000.0;
    x->out_length = out_length / 1000.0;

    // Check length of sweep and memory allocation

    t_ess sweep_params(x->lo_f, x->hi_f, x->fade_in, x->fade_out, x->length, x->sample_rate, db_to_a(x->amp), 0);
    
    if (sweep_params.length())
    {
        intptr_t mem_size = num_active_ins * irmeasure_calc_sweep_mem_size(sweep_params, num_active_outs, x->out_length, x->sample_rate);
        if (!schedule_grow_mem_swap(&x->rec_mem, mem_size, mem_size))
            object_error((t_object *) x, "not able to allocate adequate memory for recording");

        // Get amplitude curve

        fill_amp_curve_specifier(x->amp_curve, x->amp_curve_specifier, x->amp_curve_num_specifiers);

        // Start measurement

        x->current_num_active_ins = num_active_ins;
        x->current_num_active_outs = num_active_outs;

        x->measure_mode = SWEEP;
        x->fft_size = 0;
        x->test_tone = 0;
        x->stop_measurement = 0;
        x->start_measurement = 1;
    }
    else
    {
        object_error((t_object *) x, "zero length sweep - requested length value is too small");
        x->stop_measurement = 1;
    }
}


void irmeasure_mls(t_irmeasure *x, t_symbol *sym, long argc, t_atom *argv)
{
    double out_length = 5000.0;

    long num_active_ins = x->num_active_ins;
    long num_active_outs = x->num_active_outs;

    t_atom_long order = 18;

    // Load parameters

    if (argc > 0)
        order = atom_getlong(argv++);
    if (argc > 1)
        out_length = atom_getfloat(argv++);

    // Check parameters

    order = (t_atom_long) irmeasure_param_check(x, "order", static_cast<double>(order), 1, 24);
    out_length = irmeasure_param_check(x, "ir length", out_length, 0.0, HUGE_VAL);

    // Store parameters

    x->order = static_cast<long>(order);
    x->out_length = out_length / 1000.0;

    // Allocate memory

    intptr_t mem_size = num_active_ins * irmeasure_calc_mls_mem_size(x->order, num_active_outs, x->out_length, x->sample_rate);
    if (!schedule_grow_mem_swap(&x->rec_mem, mem_size, mem_size))
        object_error((t_object *) x, "not able to allocate adequate memory for recording");

    // Start measurement

    x->current_num_active_ins = num_active_ins;
    x->current_num_active_outs = num_active_outs;

    x->measure_mode = MLS;
    x->fft_size = 0;
    x->test_tone = 0;
    x->stop_measurement = 0;
    x->start_measurement = 1;
}


void irmeasure_noise(t_irmeasure *x, t_symbol *sym, long argc, t_atom *argv)
{
    double length = 10000.0;
    double fade_in = 10.0;
    double fade_out = 10.0;
    double out_length = 5000.0;

    long num_active_ins = x->num_active_ins;
    long num_active_outs = x->num_active_outs;

    t_noise_mode noise_mode = NOISE_MODE_WHITE;

    if (sym == gensym("brown"))
        noise_mode = NOISE_MODE_BROWN;
    if (sym == gensym("pink"))
        noise_mode = NOISE_MODE_PINK;

    // Load parameters

    if (argc > 0)
        length = atom_getfloat(argv++);
    if (argc > 1)
        fade_in = atom_getfloat(argv++);
    if (argc > 2)
        fade_out = atom_getfloat(argv++);
    if (argc > 3)
        out_length = atom_getfloat(argv++);

    // Check parameters

    length = irmeasure_param_check(x, "length", length, 0.0, HUGE_VAL);
    fade_in = irmeasure_param_check(x, "fade in time", fade_in, 0.0, length / 2.0);
    fade_out = irmeasure_param_check(x, "fade out time", fade_out, 0.0, length / 2.0);
    out_length = irmeasure_param_check(x, "ir length", out_length, 0.0, HUGE_VAL);

    // Store parameters

    x->noise_mode = noise_mode;
    x->length = length / 1000.0;
    x->fade_in = fade_in / 1000.0;
    x->fade_out = fade_out / 1000.0;
    x->out_length = out_length / 1000.0;

    // Allocate memory

    intptr_t mem_size = num_active_ins * irmeasure_calc_noise_mem_size(x->length, num_active_outs, x->out_length, x->sample_rate);
    if (!schedule_grow_mem_swap(&x->rec_mem, mem_size, mem_size))
        object_error((t_object *) x, "not able to allocate adequate memory for recording");

    // Start measurement

    x->current_num_active_ins = num_active_ins;
    x->current_num_active_outs = num_active_outs;

    x->measure_mode = NOISE;
    x->fft_size = 0;
    x->test_tone = 0;
    x->stop_measurement = 0;
    x->start_measurement = 1;
}


void irmeasure_tone(t_irmeasure *x, double freq, t_atom_long chan)
{
    x->test_tone_freq = freq;

    if (chan < 1)
        chan = -1;

    if (chan > x->num_out_chans)
        chan = x->num_out_chans;

    x->start_measurement = 0;
    x->stop_measurement = 1;
    x->test_tone = static_cast<long>(chan);
}


void irmeasure_stop(t_irmeasure *x)
{
    x->start_measurement = 0;
    x->test_tone = 0;
    x->stop_measurement = 1;
}


void irmeasure_clear(t_irmeasure *x)
{
    irmeasure_stop(x);

    x->fft_size = 0;
    clear_mem_swap(&x->rec_mem);
    clear_mem_swap(&x->out_mem);
}


//////////////////////////////////////////////////////////////////////////
/////////////////////////////// Set Active IO ////////////////////////////
//////////////////////////////////////////////////////////////////////////


void irmeasure_active_ins(t_irmeasure *x, t_atom_long num_active_ins)
{
    if (num_active_ins < 1)
    {
        object_error((t_object *) x, "at least one input channel must be active");
        num_active_ins = 1;
    }

    if (num_active_ins > x->num_in_chans)
    {
        object_error((t_object *) x, "cannot have more active inputs than actual inputs");
        num_active_ins = x->num_in_chans;
    }

    x->num_active_ins = static_cast<long>(num_active_ins);
}


void irmeasure_active_outs(t_irmeasure *x, t_atom_long num_active_outs)
{
    if (num_active_outs < 1)
    {
        object_error((t_object *) x, "at least one output channel must be active");
        num_active_outs = 1;
    }

    if (num_active_outs > x->num_out_chans)
    {
        object_error((t_object *) x, "cannot have more active outputs than actual outputs");
        num_active_outs = x->num_out_chans;
    }

    x->num_active_outs = static_cast<long>(num_active_outs);
}


//////////////////////////////////////////////////////////////////////////
//////////////////////// Deconvolution Processing ////////////////////////
//////////////////////////////////////////////////////////////////////////


void irmeasure_reprocess(t_irmeasure *x)
{
    if (!x->fft_size)
    {
        object_error((t_object *) x, "no complete recording to process (you may still be recording)");
        return;
    }

    defer(x, (method) irmeasure_process, 0, 0, 0);
}


void irmeasure_process(t_irmeasure *x, t_symbol *sym, short argc, t_atom *argv)
{
    FFT_SPLIT_COMPLEX_D spectrum_1;
    FFT_SPLIT_COMPLEX_D spectrum_2;
    FFT_SPLIT_COMPLEX_D spectrum_3;

    t_symbol *filter = filter_retriever(x->deconvolve_filter_specifier);

    double filter_specifier[HIRT_MAX_SPECIFIER_ITEMS];
    double range_specifier[HIRT_MAX_SPECIFIER_ITEMS];

    double sample_rate = x->sample_rate;
    double deconvolve_phase = phase_retriever(x->deconvolve_phase);
    double amp_comp = 1.0;
    
    t_filter_type deconvolve_mode = static_cast<t_filter_type>(x->deconvolve_mode);
    long bandlimit = x->measure_mode == SWEEP ? x->bandlimit : 0;

    intptr_t rec_length = x->T2;
    intptr_t gen_length = 0;
    intptr_t filter_length = buffer_length(filter);

    t_ess sweep_params(x->sweep_params);
    t_mls max_length_params(x->max_length_params);
    t_noise_params noise_params(x->noise_params);

    switch (x->measure_mode)
    {
        case SWEEP:
            sweep_params.set_amp((x->inv_amp ? sweep_params.amp() : 1.0) * db_to_a(-x->ir_gain));
            gen_length = sweep_params.length();
            break;

        case MLS:
            max_length_params.set_amp((x->inv_amp ? max_length_params.amp() : 1.0) * db_to_a(-x->ir_gain));
            gen_length = max_length_params.length();
            break;

        case NOISE:

            if (x->noise_params.mode() == NOISE_MODE_BROWN)
                amp_comp = x->max_amp_brown;
            if (x->noise_params.mode() == NOISE_MODE_PINK)
                amp_comp = x->max_amp_pink;
            
            noise_params.set_amp((x->inv_amp ? noise_params.amp() : 1.0) * db_to_a(-x->ir_gain) / amp_comp);
            gen_length = noise_params.length();
            break;
    }

    // Check and calculate lengths

    uintptr_t fft_size_log2;
    uintptr_t fft_size = calculate_fft_size(rec_length + gen_length, fft_size_log2);

    // Allocate Temporary Memory

    temp_fft_setup fft_setup(fft_size_log2);

    temp_ptr<double> temp(fft_size * 4);
    temp_ptr<double> excitation_sig(gen_length);
    
    temp_ptr<float> filter_in(filter_length);

    spectrum_1.realp = temp.get();
    spectrum_1.imagp = spectrum_1.realp + (fft_size >> 1);
    spectrum_2.realp = spectrum_1.imagp + (fft_size >> 1);
    spectrum_2.imagp = spectrum_2.realp + (fft_size >> 1);
    spectrum_3.realp = spectrum_2.imagp + (fft_size >> 1);
    spectrum_3.imagp = spectrum_3.realp + fft_size;

    if (!fft_setup || !temp || !excitation_sig || (filter_length && !filter_in))
    {
        object_error ((t_object *) x, "could not allocate temporary memory for processing");
        return;
    }

    // Allocate output memory and get record memory

    uintptr_t mem_size;
    double *rec_mem = (double *) access_mem_swap(&x->rec_mem, &mem_size);
    double *out_mem = (double *) grow_mem_swap(&x->out_mem, fft_size * x->current_num_active_ins * sizeof(double), fft_size * x->current_num_active_ins);

    if (!out_mem)
    {
        object_error ((t_object *) x, "could not allocate memory for output storage");
        return;
    }

    // Generate Signal

    switch (x->measure_mode)
    {
        case SWEEP:     sweep_params.gen(excitation_sig.get(), true);         break;
        case MLS:       max_length_params.gen(excitation_sig.get(), true);    break;
        case NOISE:     noise_params.gen(excitation_sig.get(), true);         break;
    }

    // Transform excitation signal into complex spectrum 2

    time_to_halfspectrum_double(fft_setup, excitation_sig.get(), gen_length, spectrum_2, fft_size);

    if (bandlimit)
    {
        // Calculate standard filter for bandlimited deconvolution (sweep * inv sweep)

        sweep_params.igen(excitation_sig.get(), INVERT_ALL, true);
        time_to_halfspectrum_double(fft_setup, excitation_sig.get(), gen_length, spectrum_3, fft_size);
        convolve(spectrum_3, spectrum_2, fft_size, SPECTRUM_REAL);

        // Calculate full power spectrum from half spectrum - convert filter to have the required phase

        power_full_spectrum_from_half_spectrum(spectrum_3, fft_size);
        variable_phase_from_power_spectrum(fft_setup, spectrum_3, fft_size, deconvolve_phase, true);

        // Convert back to real format

        spectrum_3.imagp[0] = spectrum_3.realp[fft_size >> 1];
    }
    else
    {
        // Find maximum power to scale

        double max_pow = 0.0;
        
        for (uintptr_t i = 1; i < (fft_size >> 1); i++)
        {
            const double test_pow = spectrum_2.realp[i] * spectrum_2.realp[i] + spectrum_2.imagp[i] * spectrum_2.imagp[i];
            max_pow = std::max(test_pow, max_pow);
        }

        max_pow = pow_to_db(max_pow);

        // Fill deconvolution filter specifiers - read filter from buffer (if specified) - make deconvolution filter

        fill_power_array_specifier(filter_specifier, x->deconvolve_filter_specifier, x->deconvolve_num_filter_specifiers);
        fill_power_array_specifier(range_specifier, x->deconvolve_range_specifier, x->deconvolve_num_range_specifiers);
        buffer_read(filter, 0, filter_in.get(), fft_size);
        make_deconvolution_filter(fft_setup, spectrum_2, spectrum_3, filter_specifier, range_specifier, max_pow, filter_in.get(), filter_length, fft_size, SPECTRUM_REAL, deconvolve_mode, deconvolve_phase, sample_rate);
    }

    // Deconvolve each input channel

    for (long i = 0; i < x->current_num_active_ins; i++)
    {
        // Get current input and output buffers

        double *measurement_rec = rec_mem + (i * rec_length);
        double *out_ptr = out_mem + (i * fft_size);

        // Do transform into spectrum_1 for measurement recording - deconvolve - transform back

        time_to_halfspectrum_double(fft_setup, measurement_rec, rec_length, spectrum_1, fft_size);
        deconvolve_with_filter(spectrum_1, spectrum_2, spectrum_3, fft_size, SPECTRUM_REAL);
        spectrum_to_time(fft_setup, out_ptr, spectrum_1, fft_size, SPECTRUM_REAL);
    }

    // Done

    x->fft_size = fft_size;

    outlet_bang(x->process_done);
}


//////////////////////////////////////////////////////////////////////////
/////////////////////////// Retrieval Routines ///////////////////////////
//////////////////////////////////////////////////////////////////////////


void irmeasure_extract(t_irmeasure *x, t_symbol *sym, long argc, t_atom *argv)
{
    defer(x, (method) irmeasure_extract_internal, sym, (short) argc, argv);
}


void irmeasure_extract_internal(t_irmeasure *x, t_symbol *sym, short argc, t_atom *argv)
{
    intptr_t rec_length = x->T2;
    uintptr_t fft_size = x->fft_size;
    uintptr_t mem_size;

    double *rec_mem = (double *) access_mem_swap(&x->rec_mem, &mem_size);

    // Get arguments

    if (!argc)
    {
        object_error ((t_object *) x, "no arguments for extract message");
        return;
    }

    t_atom_long in_chan = argc > 1 ? atom_getlong(argv++) : 1;
    t_symbol *buffer = atom_getsym(argv++);

    // Range check

    if (in_chan < 1 || in_chan > x->current_num_active_ins)
    {
        object_error((t_object *) x, "input channel %ld out of range", in_chan);
        return;
    }

    // Decrement input channel (reference to zero)

    in_chan--;

    if (!fft_size)
    {
        object_error ((t_object *) x, "no stored impulse responses - you may still be recording");
        return;
    }

    // Write to buffer

    buffer_write((t_object *) x, buffer, rec_mem + rec_length * in_chan, rec_length, x->write_chan - 1, x->resize, x->sample_rate, 1.0);
}


void irmeasure_dump(t_irmeasure *x, t_symbol *sym, long argc, t_atom *argv)
{
    defer(x, (method) irmeasure_dump_internal, sym, (short) argc, argv);
}


void irmeasure_dump_internal(t_irmeasure *x, t_symbol *sym, short argc, t_atom *argv)
{
    uintptr_t fft_size = x->fft_size;

    // Get arguments

    if (!argc)
    {
        object_error ((t_object *) x, "no arguments for dump message");
        return;
    }

    t_atom_long in_chan = argc > 1 ? atom_getlong(argv++) : 1;
    t_symbol *buffer = atom_getsym(argv++);

    // Range check

    if (in_chan < 1 || in_chan > x->current_num_active_ins)
    {
        object_error((t_object *) x, "input channel %ld out of range", in_chan);
        return;
    }

    // Decrement input channel (reference to zero)

    in_chan--;

    if (!fft_size)
    {
        object_error ((t_object *) x, "no stored impulse responses - you may still be recording");
        return;
    }

    // Get and check memory

    uintptr_t mem_size;
    double *out_ptr = (double *) access_mem_swap(&x->out_mem, &mem_size) + (in_chan * fft_size);

    if (mem_size < fft_size * x->current_num_active_ins)
    {
        object_error((t_object *) x, "storage memory is not large enough");
        return;
    }

    // Write to buffer

    buffer_write((t_object *) x, buffer, out_ptr, fft_size, x->write_chan - 1, x->resize, x->sample_rate, 1.0);
}


void irmeasure_getir(t_irmeasure *x, t_symbol *sym, long argc, t_atom *argv)
{
    defer(x, (method) irmeasure_getir_internal, sym, (short) argc, argv);
}


void irmeasure_getir_internal(t_irmeasure *x, t_symbol *sym, short argc, t_atom *argv)
{
    t_symbol *buffer;

    uintptr_t fft_size = x->fft_size;
    
    intptr_t T_minus;

    t_atom_long harmonic = 1;
    t_atom_long in_chan = 1;
    t_atom_long out_chan = 1;

    // Get arguments

    if (!argc)
    {
        object_error ((t_object *) x, "no arguments for getir message");
        return;
    }

    if (atom_gettype(argv) == A_LONG && argc > 1)
    {
        in_chan = atom_getlong(argv++);
        buffer = atom_getsym(argv++);
        argc--;
    }
    else
        buffer = atom_getsym(argv++);

    if (argc > 1)
        out_chan = atom_getlong(argv++);

    if (argc > 2)
        harmonic = atom_getlong(argv++);

    if (!fft_size)
    {
        object_error ((t_object *) x, "no stored impulse responses - you may still be recording");
        return;
    }

    // Range checks

    if (in_chan < 1 || in_chan > x->current_num_active_ins)
    {
        object_error((t_object *) x, "input channel %ld out of range", in_chan);
        return;
    }

    if (out_chan < 1 || out_chan > x->current_num_active_outs)
    {
        object_error((t_object *) x, "output channel %ld out of range", out_chan);
        return;
    }

    if (harmonic != 1 && x->measure_mode != SWEEP)
    {
        object_warn((t_object *) x, "harmonics only available using sweep measurement");
        harmonic = 1;
    }

    if (harmonic < 1)
    {
        object_warn((t_object *) x, "harmonic must be an integer of one or greater");
        harmonic = 1;
    }

    // Decrement channels (reference to zero)

    in_chan--;
    out_chan--;

    // Get and check memory

    uintptr_t mem_size;
    double *out_mem = (double *) access_mem_swap(&x->out_mem, &mem_size);

    if (mem_size < fft_size * x->current_num_active_ins)
    {
        object_error((t_object *) x, "storage memory is not large enough");
        return;
    }

    // Calculate offset in internal buffer

    if (x->measure_mode == SWEEP)
        T_minus = static_cast<intptr_t>(x->sweep_params.harm_offset(harmonic));
    else
        T_minus = 0;

    intptr_t L = static_cast<intptr_t>(x->current_out_length * x->sample_rate);

    if (harmonic > 1)
    {
        intptr_t T_minus_prev = static_cast<intptr_t>(x->sweep_params.harm_offset(harmonic - 1));
        intptr_t L2 = T_minus - T_minus_prev;

        if (L2 < L)
            L = L2;
    }

    intptr_t T = x->chan_offset[out_chan] - T_minus;

    // Wrap offset

    while (T < 0)
        T += fft_size;

    double *out_ptr = out_mem + (in_chan * fft_size) + T;

    // Write to buffer

    buffer_write((t_object *) x, buffer, out_ptr, L, x->write_chan - 1, x->resize, x->sample_rate, 1.0);
}


//////////////////////////////////////////////////////////////////////////
////////////////////// Update Excitation Parameters //////////////////////
//////////////////////////////////////////////////////////////////////////


void irmeasure_sweep_params(t_irmeasure *x)
{
    double out_length = x->out_length;
    double sample_rate = x->sample_rate;

    x->sweep_params = t_ess(x->lo_f, x->hi_f, x->fade_in, x->fade_out, x->length, sample_rate, db_to_a(x->amp), x->amp_curve);
    uintptr_t sweep_length = x->sweep_params.length();

    intptr_t chan_offset = static_cast<intptr_t>((out_length * sample_rate) + sweep_length);

    for (long i = 0; i < x->current_num_active_outs; i++)
        x->chan_offset[i] = (i * chan_offset);

    x->T = sweep_length;
    x->T2 = static_cast<intptr_t>(sweep_length + x->chan_offset[x->current_num_active_outs - 1] + (out_length * sample_rate));
    x->current_out_length = out_length;
    x->current_t = 0;
}


void irmeasure_mls_params(t_irmeasure *x)
{
    double out_length = x->out_length;
    double sample_rate = x->sample_rate;

    long order = x->order;
    long mls_length = (1 << order) - 1;
    long chan_offset = static_cast<long>((out_length * sample_rate) + mls_length);
    long i;

    x->max_length_params = t_mls(static_cast<uint32>(order), db_to_a(x->amp));

    for (i = 0; i < x->current_num_active_outs; i++)
        x->chan_offset[i] = (i * chan_offset);

    x->T = mls_length;
    x->T2 = static_cast<intptr_t>(mls_length + x->chan_offset[x->current_num_active_outs - 1] + (out_length * sample_rate));
    x->current_out_length = out_length;
    x->current_t = 0;
}


void irmeasure_noise_params(t_irmeasure *x)
{
    t_noise_mode noise_mode = x->noise_mode;

    double length = x->length;
    double out_length = x->out_length;
    double sample_rate = x->sample_rate;

    long noise_length = static_cast<long>(sample_rate * length);
    long chan_offset = static_cast<long>((out_length * sample_rate) + noise_length);
    long i;

    double amp_comp = 1.0;

    if (noise_mode == NOISE_MODE_BROWN)
        amp_comp = x->max_amp_brown;
    if (noise_mode == NOISE_MODE_PINK)
        amp_comp = x->max_amp_pink;

    x->noise_params = t_noise_params(noise_mode, x->fade_in, x->fade_out, length, sample_rate, db_to_a(x->amp) / amp_comp);

    for (i = 0; i < x->current_num_active_outs; i++)
        x->chan_offset[i] = (i * chan_offset);

    x->T = noise_length;
    x->T2 = static_cast<intptr_t>(noise_length + x->chan_offset[x->current_num_active_outs - 1] + (out_length * sample_rate));
    x->current_out_length = out_length;
    x->current_t = 0;
}


void irmeasure_params(t_irmeasure *x)
{
    switch (x->measure_mode)
    {
        case SWEEP:     irmeasure_sweep_params(x);      break;
        case MLS:       irmeasure_mls_params(x);        break;
        case NOISE:     irmeasure_noise_params(x);      break;
    }
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////// Perform Routines ////////////////////////////
//////////////////////////////////////////////////////////////////////////


static inline void irmeasure_perform_excitation(t_irmeasure *x, void *out, long current_t, long vec_size, bool double_precision)
{
    // Calculate where signal starts

    long start = std::max(0L, -current_t);

    if (double_precision)
        out = reinterpret_cast<double *>(out) + start;
    else
        out = reinterpret_cast<float *>(out) + start;

    // Calculate where sweep ends

    long todo = std::max(0L, std::min(vec_size - start, std::min(x->T, (x->T - current_t))));

    // Reset

    if (current_t <= 0 && todo)
    {
        x->max_length_params.reset();
        x->noise_params.reset();
    }

    // Generate values

    if (start < vec_size && todo)
    {
        switch (x->measure_mode)
        {
            case SWEEP:     x->sweep_params.gen(out, current_t, todo, double_precision);    break;
            case MLS:       x->max_length_params.gen(out, todo, double_precision);          break;
            case NOISE:     x->noise_params.gen(out, current_t, todo, double_precision);    break;
        }
    }
}


t_int *irmeasure_perform(t_int *w)
{
    // Set pointers

    long vec_size = static_cast<long>(w[1]);
    t_irmeasure *x = reinterpret_cast<t_irmeasure *>(w[2]);

    float *in;
    float *out;

    double phase = x->phase;
    double phase_inc = x->test_tone_freq / x->sample_rate;
    double amp = db_to_a(x->amp);
    double sample_rate = x->sample_rate;
    double progress_mul = 0.0;

    long test_tone = x->test_tone;
    long current_num_active_ins = x->current_num_active_ins;
    long current_num_active_outs = x->current_num_active_outs;
    long num_out_chans = x->num_out_chans;
    long i, j;
    
    // Check for stop / start

    if (x->start_measurement)
        irmeasure_params(x);

    if (x->stop_measurement)
        x->current_t = x->T2;

    x->start_measurement = 0;
    x->stop_measurement = 0;
    intptr_t T2 = x->T2;

    // Get counters

    intptr_t current_t = x->current_t;
    intptr_t current_t2 = current_t;
    bool excitation_playing = current_t2 < T2;

    // Check memory

    attempt_mem_swap(&x->rec_mem);
    double *rec_mem = (double *) x->rec_mem.current_ptr;

    intptr_t mem_size = irmeasure_calc_mem_size(x, current_num_active_ins, current_num_active_outs, sample_rate);
    bool mem_check = x->rec_mem.current_size >= static_cast<uintptr_t>(mem_size);

    if (mem_check)
    {
        // Record Input

        for (j = 0; j < current_num_active_ins; j++)
        {
            in = reinterpret_cast<float *>(x->in_chans[j]);
            double *measurement_rec = rec_mem + (j * T2);
            current_t2 = current_t;
            for (i = 0; i < vec_size && current_t2 < T2; i++, current_t2++)
                measurement_rec[current_t2] = *in++;
        }
    }

    // Zero all outputs (including progress)

    for (j = 0; j < num_out_chans + 1; j++)
        for (i = 0, out = reinterpret_cast<float *>(x->out_chans[j]); i < vec_size; i++)
            *out++ = 0;

    // Choose between test tones and sweeps

    if (test_tone)
    {
        // Do Test tones

        if (test_tone == -1)
            out = reinterpret_cast<float *>(x->out_chans[0]);
        else
            out = reinterpret_cast<float *>(x->out_chans[test_tone - 1]);

        // Generate one channel of test tone

        for (i = 0; i < vec_size; i++)
        {
            *out++ = static_cast<float>(amp * std::sin(phase * M_PI * 2));
            phase += phase_inc;
        }

        // Wrap tone generator phase

        while (phase < 0.0)
            phase += 1.0;
        while (phase > 1.0)
            phase -= 1.0;

        // Copy to all other channels if relevant

        if (test_tone == -1)
        {
            for (j = 1, in = reinterpret_cast<float *>(x->out_chans[0]); j < current_num_active_outs; j++)
            {
                for (i = 0, out = reinterpret_cast<float *>(x->out_chans[j]); i < vec_size; i++)
                    out[i] = in[i];
            }
        }
    }
    else
    {
        if (mem_check)
        {
            // Do Output

            for (j = 0; j < current_num_active_outs; j++)
                irmeasure_perform_excitation(x, x->out_chans[j], static_cast<long>(x->current_t - x->chan_offset[j]), vec_size, false);

            if (x->abs_progress)
                progress_mul = 1000.0 / sample_rate;
            else
                if (T2)
                    progress_mul = 1.0 / T2;

            // Progress output

            out = reinterpret_cast<float *>(x->out_chans[num_out_chans]);
            current_t2 = x->current_t;
            for (i = 0; i < vec_size && current_t2 < T2; i++, current_t2++)
                out[i] = static_cast<float>(progress_mul * current_t2);
            for (; i < vec_size; i++)
                out[i] = static_cast<float>(progress_mul * T2);
        }
    }

    // Store accumulators

    x->current_t = current_t2;
    x->phase = phase;

    // Process when done

    if (excitation_playing && current_t2 >= T2)
        defer(x, (method) irmeasure_process, 0, 0, 0);

    return w + 3;
}


void irmeasure_perform64(t_irmeasure *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
    double *measurement_rec;
    double *in;
    double *out;

    double phase = x->phase;
    double phase_inc = x->test_tone_freq / x->sample_rate;
    double amp = db_to_a(x->amp);
    double sample_rate = x->sample_rate;
    double progress_mul = 0.0;

    long test_tone = x->test_tone;
    long current_num_active_ins = x->current_num_active_ins;
    long current_num_active_outs = x->current_num_active_outs;
    long num_out_chans = x->num_out_chans;
    long i, j;
    
    // Check for stop / start

    if (x->start_measurement)
        irmeasure_params(x);

    if (x->stop_measurement)
        x->current_t = x->T2;

    x->start_measurement = 0;
    x->stop_measurement = 0;
    intptr_t T2 = x->T2;

    // Get counters

    intptr_t current_t = x->current_t;
    intptr_t current_t2 = current_t;
    bool excitation_playing = current_t2 < T2;

    // Check memory

    attempt_mem_swap(&x->rec_mem);
    double *rec_mem = (double *) x->rec_mem.current_ptr;
    
    intptr_t mem_size = irmeasure_calc_mem_size(x, current_num_active_ins, current_num_active_outs, sample_rate);
    bool mem_check = x->rec_mem.current_size >= static_cast<uintptr_t>(mem_size);

    if (mem_check)
    {
        // Record Input

        for (j = 0; j < current_num_active_ins; j++)
        {
            in = ins[j];
            measurement_rec = rec_mem + (j * T2);
            current_t2 = current_t;
            for (i = 0; i < vec_size && current_t2 < T2; i++, current_t2++)
                measurement_rec[current_t2] = *in++;
        }
    }

    // Zero all outputs (including progress)

    for (j = 0; j < num_out_chans + 1; j++)
        for (i = 0, out = outs[j]; i < vec_size; i++)
            *out++ = 0;

    // Choose between test tones and sweeps

    if (test_tone)
    {
        // Do Test tones

        if (test_tone == -1)
            out = outs[0];
        else
            out = outs[test_tone - 1];

        // Generate one channel of test tone

        for (i = 0; i < vec_size; i++)
        {
            *out++ = amp * std::sin(phase * M_PI * 2);
            phase += phase_inc;
        }

        // Wrap tone generator phase

        while (phase < 0.0)
            phase += 1.0;
        while (phase > 1.0)
            phase -= 1.0;

        // Copy to all other channels if relevant

        if (test_tone == -1)
        {
            for (j = 1, in = outs[0]; j < current_num_active_outs; j++)
            {
                for (i = 0, out = outs[j]; i < vec_size; i++)
                    out[i] = in[i];
            }
        }
    }
    else
    {
        if (mem_check)
        {
            // Do Output

            for (j = 0; j < current_num_active_outs; j++)
                irmeasure_perform_excitation(x, outs[j], static_cast<long>(x->current_t - x->chan_offset[j]), vec_size, true);

            if (x->abs_progress)
                progress_mul = 1000.0 / sample_rate;
            else
                if (T2)
                    progress_mul = 1.0 / T2;

            // Progress output

            out = outs[num_out_chans];
            current_t2 = x->current_t;
            for (i = 0; i < vec_size && current_t2 < T2; i++, current_t2++)
                out[i] = progress_mul * current_t2;
            for (; i < vec_size; i++)
                out[i] = progress_mul * T2;
        }
    }

    // Store accumulators

    x->current_t = current_t2;
    x->phase = phase;

    // Process when done

    if (excitation_playing && current_t2 >= T2)
        defer(x, (method) irmeasure_process, 0, 0, 0);
}


//////////////////////////////////////////////////////////////////////////
///////////////////////////// DSP Routines ///////////////////////////////
//////////////////////////////////////////////////////////////////////////


void irmeasure_dsp_common(t_irmeasure *x, double samplerate)
{
    // Store sample rate

    const double old_sr = x->sample_rate;
    x->sample_rate = samplerate;

    if (x->sample_rate != old_sr || x->no_dsp)
    {
        t_noise_params noise_params(NOISE_MODE_WHITE, 0, 0, 1, x->sample_rate, 1);
        noise_params.measure((1 << 25), x->max_amp_pink, x->max_amp_brown);
        x->no_dsp = 0;
    }

    if (x->start_measurement)
    {
        intptr_t mem_size = 0;
        
        switch (x->measure_mode)
        {
            case SWEEP:
            {
                t_ess sweep_params(x->lo_f, x->hi_f, x->fade_in, x->fade_out, x->length, x->sample_rate, db_to_a(x->amp), nullptr);
                mem_size = x->current_num_active_ins * irmeasure_calc_sweep_mem_size(sweep_params, x->current_num_active_outs, x->out_length, x->sample_rate);
                break;
            }

            case MLS:
                mem_size = x->current_num_active_ins * irmeasure_calc_mls_mem_size(x->order, x->current_num_active_outs, x->out_length, x->sample_rate);
                break;

            case NOISE:
                mem_size = x->current_num_active_ins * irmeasure_calc_noise_mem_size(x->length, x->current_num_active_outs, x->out_length, x->sample_rate);
                break;
        }

        if (!schedule_grow_mem_swap(&x->rec_mem, mem_size, mem_size))
            object_error((t_object *) x, "not able to allocate adequate memory for recording");
    }
    else
    {
        // Clear memory if the sample rate has changed and we have not just started measuring

        if (x->sample_rate != old_sr)
            irmeasure_clear(x);
    }
}


void irmeasure_dsp(t_irmeasure *x, t_signal **sp, short *count)
{
    long i;

    irmeasure_dsp_common(x, sp[0]->s_sr);

    // Store pointers to ins and outs

    for (i = 0; i < x->num_in_chans; i++)
        x->in_chans[i] = sp[i]->s_vec;
    for (; i < HIRT_MAX_MEASURE_CHANS; i++)
        x->in_chans[i] = 0;

    for (i = 0; i < x->num_out_chans; i++)
        x->out_chans[i] = sp[i + x->num_in_chans]->s_vec;
    x->out_chans[i] = sp[i + x->num_in_chans]->s_vec;
    for (i++; i < HIRT_MAX_MEASURE_CHANS + 1; i++)
        x->out_chans[i] = 0;

    dsp_add((t_perfroutine)irmeasure_perform, 2, sp[0]->s_n, x);
}


void irmeasure_dsp64(t_irmeasure *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
    irmeasure_dsp_common(x, samplerate);
    object_method(dsp64, gensym("dsp_add64"), x, irmeasure_perform64, 0, nullptr);
}
