
#include <ext.h>
#include <ext_obex.h>
#include <ext_obex_util.h>
#include <z_dsp.h>

#include <AH_Memory_Swap.h>
#include <HIRT_Core_Functions.hpp>
#include <HIRT_Buffer_Access.hpp>

#include <HIRT_Exponential_Sweeps.hpp>
#include <HIRT_Coloured_Noise.hpp>
#include <HIRT_Max_Length_Sequences.hpp>

// Define common attributes and the class name (for the common attributes file)

#define OBJ_CLASSNAME t_irextract
#define OBJ_USES_HIRT_READ_ATTR
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

struct t_irextract
{
    t_pxobject x_obj;

    // Object Parameters

    // Request Output Size (s)

    double out_length;

    // Internal

    intptr_t fft_size;

    // Internal
    
    double last_sample_rate;
    double max_amp_pink;
    double max_amp_brown;
    
    double sample_rate;

    intptr_t out_length_samps;
    intptr_t gen_length;

    // Measurement Parameters

    t_excitation_signal measure_mode;

    t_ess sweep_params;
    t_mls max_length_params;
    t_noise_params noise_params;

    // Permanent Memory

    t_safe_mem_swap out_mem;

    // Attributes

    HIRT_COMMON_ATTR

    long inv_amp;
    long bandlimit;

    double amp;
    double ir_gain;
    
    // Bang Outlet

    void *process_done;
};


// This include deals with setup of common attributes - requires the object structure to be defined

#include <HIRT_Common_Attribute_Setup.hpp>


// Function prototypes

void *irextract_new(t_symbol *s, short argc, t_atom *argv);
void irextract_free(t_irextract *x);
void irextract_assist(t_irextract *x, void *b, long m, long a, char *s);

double irextract_param_check(t_irextract *x, const char *name, double val, double min, double max);

void irextract_sweep(t_irextract *x, t_symbol *sym, long argc, t_atom *argv);
void irextract_mls(t_irextract *x, t_symbol *sym, long argc, t_atom *argv);
void irextract_noise(t_irextract *x, t_symbol *sym, long argc, t_atom *argv);
void irextract_clear(t_irextract *x);

void irextract_process(t_irextract *x, t_symbol *rec_buffer, t_atom_long num_channels, double sample_rate);
void irextract_process_internal(t_irextract *x, t_symbol *sym, short argc, t_atom *argv);

void irextract_getir(t_irextract *x, t_symbol *sym, long argc, t_atom *argv);
void irextract_getir_internal(t_irextract *x, t_symbol *sym, short argc, t_atom *argv);
void irextract_dump(t_irextract *x, t_symbol *sym, long argc, t_atom *argv);
void irextract_dump_internal(t_irextract *x, t_symbol *sym, short argc, t_atom *argv);


//////////////////////////////////////////////////////////////////////////
/////////////////////// Main / New / Free / Assist ///////////////////////
//////////////////////////////////////////////////////////////////////////


int C74_EXPORT main()
{
    this_class = class_new("irextract~",
                          (method) irextract_new,
                          (method)irextract_free,
                          sizeof(t_irextract),
                          0L,
                          A_GIMME,
                          0);

    class_addmethod(this_class, (method)irextract_assist, "assist", A_CANT, 0L);

    class_addmethod(this_class, (method)irextract_sweep, "sweep", A_GIMME, 0L);
    class_addmethod(this_class, (method)irextract_mls, "mls", A_GIMME, 0L);
    class_addmethod(this_class, (method)irextract_noise, "white", A_GIMME, 0L);
    class_addmethod(this_class, (method)irextract_noise, "brown", A_GIMME, 0L);
    class_addmethod(this_class, (method)irextract_noise, "pink", A_GIMME, 0L);

    class_addmethod(this_class, (method)irextract_clear, "clear", 0L);

    class_addmethod(this_class, (method)irextract_getir, "getir", A_GIMME, 0L);
    class_addmethod(this_class, (method)irextract_dump, "dump", A_GIMME, 0L);

    declare_HIRT_common_attributes(this_class);

    CLASS_ATTR_DOUBLE(this_class, "amp", 0, t_irextract, amp);
    CLASS_ATTR_LABEL(this_class,"amp", 0, "Signal Amplitude (dB)");

    CLASS_ATTR_LONG(this_class, "invamp", 0, t_irextract, inv_amp);
    CLASS_ATTR_STYLE_LABEL(this_class,"invamp",0,"onoff","Invert Amplitude");

    CLASS_ATTR_DOUBLE(this_class, "irgain", 0, t_irextract, ir_gain);
    CLASS_ATTR_LABEL(this_class,"irgain", 0, "IR Gain (dB)");
    
    CLASS_ATTR_LONG(this_class, "bandlimit", 0, t_irextract, bandlimit);
    CLASS_ATTR_STYLE_LABEL(this_class,"bandlimit",0,"onoff","Bandlimit Sweep Measurements");

    class_register(CLASS_BOX, this_class);

    return 0;
}


void *irextract_new(t_symbol *s, short argc, t_atom *argv)
{
    t_irextract *x = (t_irextract *)object_alloc(this_class);

    x->process_done = bangout(x);

    init_HIRT_common_attributes(x);

    x->bandlimit = 1;
    x->amp = -1.0;
    x->inv_amp = 0;
    x->ir_gain = 0.0;

    x->last_sample_rate = 0.0;
    x->max_amp_pink = 0.0;
    x->max_amp_brown = 0.0;
    
    x->fft_size = 0;
    x->sample_rate = 0.0;
    x->out_length = 0.0;
    x->out_length_samps = 0;
    x->gen_length = 0;

    alloc_mem_swap(&x->out_mem, 0, 0);

    x->measure_mode = SWEEP;

    attr_args_process(x, argc, argv);

    return x;
}


void irextract_free(t_irextract *x)
{
    dsp_free((t_pxobject *)x);

    free_mem_swap(&x->out_mem);

    free_HIRT_common_attributes(x);
}


void irextract_assist(t_irextract *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET)
        sprintf(s,"Instructions In");
    else
        sprintf(s,"Bang on Success");
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////// Parameter Check /////////////////////////////
//////////////////////////////////////////////////////////////////////////


double irextract_param_check(t_irextract *x, const char *name, double val, double min, double max)
{
    bool changed = false;
    double new_val = val;

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
/////////////////////////// Measurement Messages /////////////////////////
//////////////////////////////////////////////////////////////////////////


void irextract_sweep(t_irextract *x, t_symbol *sym, long argc, t_atom *argv)
{
    double f1 = 20.0;
    double f2 = 22050.0;
    double length = 30000.0;
    double fade_in = 50.0;
    double fade_out = 10.0;
    double out_length = 0.0;
    double sample_rate;

    double amp_curve[33];

    t_atom_long num_channels = 1;

    t_symbol *rec_buffer = NULL;

    // Load parameters

    if (argc > 0)
    {
        rec_buffer = atom_getsym(argv++);
        sample_rate = buffer_sample_rate(rec_buffer);
        f2 = sample_rate / 2.0;
    }
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
        num_channels = atom_getlong(argv++);
    if (argc > 7)
        out_length = atom_getfloat(argv++);

    // Check parameters

    if (!rec_buffer)
    {
        object_error((t_object *)x, "no buffer given");
        return;
    }

    f1 = irextract_param_check(x, "low frequency", f1, 0.0001, sample_rate / 2.0);
    f2 = irextract_param_check(x, "high frequency", f2, f2, sample_rate / 2.0);
    length = irextract_param_check(x, "length", length, 0.0, HUGE_VAL);
    fade_in = irextract_param_check(x, "fade in time", fade_in, 0.0, length / 2.0);
    fade_out = irextract_param_check(x, "fade out time", fade_out, 0.0, length / 2.0);
    num_channels = (t_atom_long) irextract_param_check(x, "number of channels", (double) num_channels, 1, HIRT_MAX_MEASURE_CHANS);
    x->out_length = irextract_param_check(x, "output length", out_length, 0.0, HUGE_VAL) / 1000.0;

    // Check length of sweep and memory allocation

    fill_amp_curve_specifier(amp_curve, x->amp_curve_specifier, x->amp_curve_num_specifiers);

    if (ess_params(&x->sweep_params, f1, f2, fade_in / 1000.0, fade_out / 1000.0, length / 1000.0, sample_rate, (x->inv_amp ? db_to_a(x->amp) : 1) * db_to_a(-x->ir_gain), amp_curve))
    {
        // Process

        x->measure_mode = SWEEP;
        irextract_process(x, rec_buffer, num_channels, sample_rate);
    }
    else
        object_error((t_object *) x, "zero length sweep - requested length value is too small");
}


void irextract_mls(t_irextract *x, t_symbol *sym, long argc, t_atom *argv)
{
    double sample_rate;
    double out_length = 0.0;

    t_atom_long num_channels = 1;
    t_atom_long order = 18;

    t_symbol *rec_buffer = NULL;

    // Load parameters

    if (argc > 0)
    {
        rec_buffer = atom_getsym(argv++);
        sample_rate = buffer_sample_rate(rec_buffer);
    }
    if (argc > 1)
        order = atom_getlong(argv++);
    if (argc > 2)
        num_channels = atom_getlong(argv++);
    if (argc > 3)
        out_length = atom_getfloat(argv++);

    // Check parameters

    if (!rec_buffer)
    {
        object_error((t_object *)x, "no buffer given");
        return;
    }

    order = (t_atom_long) irextract_param_check(x, "order", (double) order, 1, 24);
    num_channels = (t_atom_long) irextract_param_check(x, "number of channels", (double) num_channels, 1, HIRT_MAX_MEASURE_CHANS);
    x->out_length = irextract_param_check(x, "output length", out_length, 0.0, HUGE_VAL) / 1000.0;

    // Process

    x->measure_mode = MLS;
    mls_params(&x->max_length_params, (long) order, (x->inv_amp ? db_to_a(x->amp) : 1) * db_to_a(-x->ir_gain));
    irextract_process(x, rec_buffer, num_channels, sample_rate);
}


void irextract_noise(t_irextract *x, t_symbol *sym, long argc, t_atom *argv)
{
    double length = 10000.0;
    double fade_in = 10.0;
    double fade_out = 10.0;
    double amp_comp = 1.0;
    double out_length = 0.0;
    double sample_rate;

    t_atom_long num_channels = 1;

    t_symbol *rec_buffer = NULL;

    t_noise_mode noise_mode = NOISE_MODE_WHITE;

    if (sym == gensym("brown"))
        noise_mode = NOISE_MODE_BROWN;
    if (sym == gensym("pink"))
        noise_mode = NOISE_MODE_PINK;

    // Load parameters

    if (argc > 0)
    {
        rec_buffer = atom_getsym(argv++);
        sample_rate = buffer_sample_rate(rec_buffer);
    }
    if (argc > 1)
        length = atom_getfloat(argv++);
    if (argc > 2)
        fade_in = atom_getfloat(argv++);
    if (argc > 3)
        fade_out = atom_getfloat(argv++);
    if (argc > 4)
        num_channels = atom_getlong(argv++);
    if (argc > 5)
        out_length = atom_getfloat(argv++);

    // Check parameters

    if (!rec_buffer)
    {
        object_error((t_object *)x, "no buffer given");
        return;
    }

    length = irextract_param_check(x, "length", length, 0.0, HUGE_VAL);
    fade_in = irextract_param_check(x, "fade in time", fade_in, 0.0, length / 2.0);
    fade_out = irextract_param_check(x, "fade out time", fade_out, 0.0, length / 2.0);
    num_channels = (t_atom_long) irextract_param_check(x, "number of channels", (double) num_channels, 1, HIRT_MAX_MEASURE_CHANS);
    x->out_length = irextract_param_check(x, "output length", out_length, 0.0, HUGE_VAL) / 1000.0;

    // Process

    x->measure_mode = NOISE;
    
    if (noise_mode != NOISE_MODE_WHITE)
    {
        if (x->last_sample_rate != sample_rate)
        {
            t_noise_params noise_params;
            
            coloured_noise_params(&noise_params, NOISE_MODE_WHITE, 0, 0, 1, sample_rate, 1);
            coloured_noise_measure(&noise_params, (1 << 25), &x->max_amp_pink, &x->max_amp_brown);
            
            x->last_sample_rate = sample_rate;
        }
    }
    
    if (noise_mode == NOISE_MODE_BROWN)
        amp_comp = x->max_amp_brown;
    if (noise_mode == NOISE_MODE_PINK)
        amp_comp = x->max_amp_pink;

    coloured_noise_reset(&x->noise_params);
    coloured_noise_params(&x->noise_params, noise_mode, fade_in / 1000.0, fade_out / 1000.0, length / 1000.0, sample_rate, (x->inv_amp ? db_to_a(x->amp) : 1) * db_to_a(-x->ir_gain) / amp_comp);
    
    irextract_process(x, rec_buffer, num_channels, sample_rate);
}


void irextract_clear(t_irextract *x)
{
    x->fft_size = 0;
    clear_mem_swap(&x->out_mem);
}


//////////////////////////////////////////////////////////////////////////
//////////////////////// Deconvolution Processing ////////////////////////
//////////////////////////////////////////////////////////////////////////


void irextract_process(t_irextract *x, t_symbol *rec_buffer, t_atom_long num_channels, double sample_rate)
{
    t_atom arguments[3];
    
    atom_setsym(arguments + 0, rec_buffer);
    atom_setlong(arguments + 1, num_channels);
    atom_setfloat(arguments + 2, sample_rate);
    
    defer(x, (method) irextract_process_internal, NULL, (short) 3, arguments);
}


void irextract_process_internal(t_irextract *x, t_symbol *sym, short argc, t_atom *argv)
{
    t_symbol *rec_buffer = atom_getsym(argv + 0);
    t_atom_long num_channels = atom_getlong(argv + 1);
    double sample_rate = atom_getfloat(argv + 2);
    
    FFT_SPLIT_COMPLEX_D spectrum_1;
    FFT_SPLIT_COMPLEX_D spectrum_2;
    FFT_SPLIT_COMPLEX_D spectrum_3;

    double *out_mem;
    
    t_symbol *filter = filter_retriever(x->deconvolve_filter_specifier);

    double filter_specifier[HIRT_MAX_SPECIFIER_ITEMS];
    double range_specifier[HIRT_MAX_SPECIFIER_ITEMS];

    double test_pow;
    double max_pow;
    double deconvolve_phase = phase_retriever(x->deconvolve_phase);

    long deconvolve_mode = x->deconvolve_mode;
    long bandlimit = x->measure_mode == SWEEP ? x->bandlimit : 0;
    t_atom_long read_chan = x->read_chan - 1;

    intptr_t rec_length = buffer_length(rec_buffer);
    intptr_t gen_length = 0;
    intptr_t filter_length = buffer_length(filter);
    intptr_t out_length_samps;

    uintptr_t fft_size;
    uintptr_t fft_size_log2;
    uintptr_t i;

    if (buffer_check((t_object *)x, rec_buffer) || !rec_length)
        return;

    switch (x->measure_mode)
    {
        case SWEEP:
            gen_length = ess_get_length(&x->sweep_params);
            break;

        case MLS:
            gen_length = mls_get_length(&x->max_length_params);
            break;

        case NOISE:
            gen_length = coloured_noise_get_length(&x->noise_params);
            break;
    }

    // Check and calculate lengths

    fft_size = calculate_fft_size(rec_length + gen_length, fft_size_log2);

    if (rec_length % num_channels)
        object_warn ((t_object *) x, "buffer length is not a multiple of the number of channels - number may be wrong");

    if (((rec_length / num_channels) - gen_length) < 1)
    {
        object_error ((t_object *) x, "buffer is not long enough for generated signal");
        return;
    }

    out_length_samps = ((rec_length / num_channels) - gen_length);

    if (x->out_length)
    {
        if (out_length_samps < (x->out_length * sample_rate))
            object_warn ((t_object *) x, "buffer is not long enough for requested output length");
        else
            out_length_samps = (intptr_t) (x->out_length * sample_rate);
    }

    // Allocate Temporary Memory

    temp_fft_setup fft_setup(fft_size_log2);

    temp_ptr<double> temp(fft_size * 4);
    temp_ptr<double> excitation_sig(gen_length);

    temp_ptr<float> rec_mem(rec_length);
    temp_ptr<float> filter_in(filter_length);

    spectrum_1.realp = temp.get();
    spectrum_1.imagp = spectrum_1.realp + (fft_size >> 1);
    spectrum_2.realp = spectrum_1.imagp + (fft_size >> 1);
    spectrum_2.imagp = spectrum_2.realp + (fft_size >> 1);
    spectrum_3.realp = spectrum_2.imagp + (fft_size >> 1);
    spectrum_3.imagp = spectrum_3.realp + fft_size;

    if (!fft_setup || !temp || !excitation_sig  || !rec_mem || (filter_length && !filter_in))
    {
        object_error ((t_object *) x, "could not allocate temporary memory for processing");
        return;
    }

    x->fft_size = fft_size;
    x->sample_rate = sample_rate;
    x->out_length_samps = out_length_samps;
    x->gen_length = gen_length;

    // Allocate output memory and get record memory

    out_mem = (double *) grow_mem_swap(&x->out_mem, fft_size * sizeof(double), fft_size);

    if (!out_mem)
    {
        object_error ((t_object *) x, "could not allocate memory for output storage");
        return;
    }

    // Generate Signal

    switch (x->measure_mode)
    {
        case SWEEP:
            ess_gen(&x->sweep_params, excitation_sig.get(), true);
            break;

        case MLS:
            mls_gen(&x->max_length_params, excitation_sig.get(), true);
            break;

        case NOISE:
            coloured_noise_gen(&x->noise_params, excitation_sig.get(), true);
            break;
    }

    // Transform excitation signal into complex spectrum 2

    time_to_halfspectrum_double(fft_setup, excitation_sig.get(), gen_length, spectrum_2, fft_size);

    if (bandlimit)
    {
        // Calculate standard filter for bandlimited deconvolution (sweep * inv sweep)

        ess_igen(&x->sweep_params, excitation_sig.get(), INVERT_ALL, true);
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

        for (i = 1, max_pow = 0.0; i < (fft_size >> 1); i++)
        {
            test_pow = spectrum_2.realp[i] *spectrum_2.realp[i] + spectrum_2.imagp[i] * spectrum_2.imagp[i];
            max_pow = test_pow > max_pow ? test_pow : max_pow;
        }

        max_pow = pow_to_db(max_pow);

        // Fill deconvolution filter specifiers - read filter from buffer (if specified) - make deconvolution filter

        fill_power_array_specifier(filter_specifier, x->deconvolve_filter_specifier, x->deconvolve_num_filter_specifiers);
        fill_power_array_specifier(range_specifier, x->deconvolve_range_specifier, x->deconvolve_num_range_specifiers);
        buffer_read(filter, 0, filter_in.get(), fft_size);
        make_deconvolution_filter(fft_setup, spectrum_2, spectrum_3, filter_specifier, range_specifier, max_pow, filter_in.get(), filter_length, fft_size, SPECTRUM_REAL, (t_filter_type) deconvolve_mode, deconvolve_phase, sample_rate);
    }

    // Read recording from buffer / do transform into spectrum_1 for measurement recording - deconvolve - transform back

    buffer_read(rec_buffer, read_chan, rec_mem.get(), rec_length);
    time_to_halfspectrum_float(fft_setup, rec_mem.get(), rec_length, spectrum_1, fft_size);
    deconvolve_with_filter(spectrum_1, spectrum_2, spectrum_3, fft_size, SPECTRUM_REAL);
    spectrum_to_time(fft_setup, out_mem, spectrum_1, fft_size, SPECTRUM_REAL);

    // Done

    outlet_bang(x->process_done);
}


//////////////////////////////////////////////////////////////////////////
/////////////////////////// Retrieval Routines ///////////////////////////
//////////////////////////////////////////////////////////////////////////


void irextract_getir(t_irextract *x, t_symbol *sym, long argc, t_atom *argv)
{
    defer(x, (method) irextract_getir_internal, sym, (short) argc, argv);
}


void irextract_getir_internal(t_irextract *x, t_symbol *sym, short argc, t_atom *argv)
{
    t_symbol *buffer;

    double *out_buf;
    double *out_mem;

    uintptr_t fft_size = x->fft_size;
    uintptr_t mem_size;

    intptr_t T_minus;
    intptr_t L;
    intptr_t T;
    intptr_t out_length_samps = x->out_length_samps;

    t_atom_long harmonic = 1;
    t_atom_long out_chan = 1;

    intptr_t chan_offset = (intptr_t) out_length_samps + x->gen_length;

    // Get arguments

    if (!argc)
    {
        object_error ((t_object *) x, "no arguments for getir message");
        return;
    }

    buffer = atom_getsym(argv++);

    if (argc > 1)
        out_chan = atom_getlong(argv++);

    if (argc > 2)
        harmonic = atom_getlong(argv++);

    if (!fft_size)
    {
        object_error ((t_object *) x, "no stored impulse responses");
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

    out_chan--;

    // Get and check memory

    out_mem = (double *) access_mem_swap(&x->out_mem, &mem_size);

    if (mem_size < fft_size)
    {
        object_error((t_object *) x, "storage memory is not large enough");
        return;
    }

    // Calculate offset in internal buffer

    if (x->measure_mode == SWEEP)
        T_minus = (intptr_t) ess_harm_offset(&x->sweep_params, harmonic);
    else
        T_minus = 0;

    L = out_length_samps;

    if (harmonic > 1)
    {
        intptr_t T_minus_prev = (intptr_t) ess_harm_offset(&x->sweep_params, harmonic - 1);
        intptr_t L2 = T_minus - T_minus_prev;

        if (L2 < L)
            L = L2;
    }

    T = (chan_offset * out_chan) - T_minus;

    // Wrap offset

    while (T < 0)
        T += fft_size;

    out_buf = out_mem + T;

    // Write to buffer

    auto error = buffer_write((t_object *) x, buffer, out_buf, L, x->write_chan - 1, x->resize, x->sample_rate, 1.0);
    
    // Done
    
    if (!error)
        outlet_bang(x->process_done);
}

void irextract_dump(t_irextract *x, t_symbol *sym, long argc, t_atom *argv)
{
    defer(x, (method) irextract_dump_internal, sym, (short) argc, argv);
}


void irextract_dump_internal(t_irextract *x, t_symbol *sym, short argc, t_atom *argv)
{
    double *out_mem;

    t_symbol *buffer = NULL;

    uintptr_t fft_size = x->fft_size;
    uintptr_t mem_size;

    // Get arguments

    if (!argc)
    {
        object_error ((t_object *) x, "no arguments for dump message");
        return;
    }

    buffer = atom_getsym(argv++);

    if (!fft_size)
    {
        object_error ((t_object *) x, "no stored impulse responses");
        return;
    }

    // Get and check memory

    out_mem = (double *) access_mem_swap(&x->out_mem, &mem_size);

    if (mem_size < fft_size)
    {
        object_error((t_object *) x, "storage memory is not large enough");
        return;
    }

    // Write to buffer

    auto error = buffer_write((t_object *) x, buffer, out_mem, fft_size, x->write_chan - 1, x->resize, x->sample_rate, 1.0);
    
    // Done
    
    if (!error)
        outlet_bang(x->process_done);
}
