
#include <ext.h>
#include <ext_obex.h>
#include <ext_obex_util.h>
#include <z_dsp.h>

#include <AH_Memory_Swap.h>
#include <HIRT_Core_Functions.hpp>
#include <HIRT_Buffer_Access.hpp>

// Define common attributes and the class name (for the common attributes file)

#define OBJ_CLASSNAME t_irreference
#define OBJ_USES_HIRT_WRITE_ATTR
#define OBJ_USES_HIRT_DECONVOLUTION_ATTR
#define OBJ_USES_HIRT_SMOOTH_ATTR

#include <HIRT_Common_Attribute_Vars.hpp>


// Object class and structure

t_class *this_class;

struct t_irreference
{
    t_pxobject x_obj;

    // Object Parameters

    // Requested Recording

    double length;
    double out_length;

    long num_active_ins;

    // Internal

    double current_out_length;
    double sample_rate;

    intptr_t current_length;
    intptr_t T;
    intptr_t current_t;
    intptr_t fft_size;

    long start_rec;
    long stop_rec;
    long current_num_active_ins;
    long num_in_chans;

    // Input Pointers

    void *in_chans[HIRT_MAX_MEASURE_CHANS + 1];

    // Permanent Memory

    t_safe_mem_swap rec_mem;
    t_safe_mem_swap out_mem;

    // Attributes

    HIRT_COMMON_ATTR

    long abs_progress;

    // Bang Outlet

    void *process_done;
};


// This include deals with setup of common attributes - requires the object structure to be defined

#include <HIRT_Common_Attribute_Setup.hpp>


// Function protypes

void *irreference_new(t_symbol *s, short argc, t_atom *argv);
void irreference_free(t_irreference *x);
void irreference_assist(t_irreference *x, void *b, long m, long a, char *s);

intptr_t irreference_calc_mem_size(t_irreference *x, long active_ins);

double irreference_param_check(t_irreference *x, const char *name, double val, double min, double max);

void irreference_rec(t_irreference *x, t_symbol *sym, short argc, t_atom *argv);
void irreference_stop(t_irreference *x);
void irreference_finish(t_irreference *x);
void irreference_clear(t_irreference *x);

void irreference_active_ins (t_irreference *x, t_atom_long num_active_ins);

void irreference_smooth(FFT_SETUP_D fft_setup, FFT_SPLIT_COMPLEX_D spectrum, FFT_SPLIT_COMPLEX_D temp_full_spectrum, long mode, uintptr_t fft_size, double smooth_lo, double smooth_hi);
void irreference_reprocess(t_irreference *x);
void irreference_process(t_irreference *x, t_symbol *sym, short argc, t_atom *argv);

void irreference_extract(t_irreference *x, t_symbol *sym, long argc, t_atom *argv);
void irreference_extract_internal(t_irreference *x, t_symbol *sym, short argc, t_atom *argv);
void irreference_dump(t_irreference *x, t_symbol *sym, long argc, t_atom *argv);
void irreference_dump_internal(t_irreference *x, t_symbol *sym, short argc, t_atom *argv);
void irreference_getir(t_irreference *x, t_symbol *sym, long argc, t_atom *argv);
void irreference_getir_internal(t_irreference *x, t_symbol *sym, short argc, t_atom *argv);

t_int *irreference_perform(t_int *w);
void irreference_perform64(t_irreference *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam);

void irreference_dsp_common(t_irreference *x, double samplerate);
void irreference_dsp(t_irreference *x, t_signal **sp, short *count);
void irreference_dsp64(t_irreference *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);


//////////////////////////////////////////////////////////////////////////
/////////////////////// Main / New / Free / Assist ///////////////////////
//////////////////////////////////////////////////////////////////////////


int C74_EXPORT main()
{
    this_class = class_new("irreference~",
                          (method) irreference_new,
                          (method)irreference_free,
                          sizeof(t_irreference),
                          0L,
                          A_GIMME,
                          0);

    class_addmethod(this_class, (method)irreference_assist, "assist", A_CANT, 0L);
    class_addmethod(this_class, (method)irreference_dsp, "dsp", A_CANT, 0L);
    class_addmethod(this_class, (method)irreference_dsp64, "dsp64", A_CANT, 0L);

    class_addmethod(this_class, (method)irreference_rec, "rec", A_GIMME, 0L);
    class_addmethod(this_class, (method)irreference_stop, "stop", 0L);
    class_addmethod(this_class, (method)irreference_finish, "finish", 0L);
    class_addmethod(this_class, (method)irreference_clear, "clear", 0L);

    class_addmethod(this_class, (method)irreference_active_ins, "activeins", A_LONG, 0L);

    class_addmethod(this_class, (method)irreference_reprocess, "reprocess", 0L);

    class_addmethod(this_class, (method)irreference_extract, "extract", A_GIMME, 0L);
    class_addmethod(this_class, (method)irreference_dump, "dump", A_GIMME, 0L);
    class_addmethod(this_class, (method)irreference_getir, "getir", A_GIMME, 0L);

    declare_HIRT_common_attributes(this_class);

    CLASS_ATTR_LONG(this_class, "absprogress", 0, t_irreference, abs_progress);
    CLASS_ATTR_STYLE_LABEL(this_class,"absprogress",0,"onoff","Absolute Progress");

    class_dspinit(this_class);

    class_register(CLASS_BOX, this_class);

    return 0;
}


void *irreference_new(t_symbol *s, short argc, t_atom *argv)
{
    t_irreference *x = (t_irreference *)object_alloc(this_class);
    t_atom_long num_in_chans = 1;

    if (argc && atom_gettype(argv) == A_LONG)
    {
        num_in_chans = atom_getlong(argv++);
        num_in_chans = num_in_chans < 1 ? 1 : num_in_chans;
        num_in_chans = num_in_chans > HIRT_MAX_MEASURE_CHANS ? HIRT_MAX_MEASURE_CHANS : num_in_chans;
        argc--;
    }

    dsp_setup((t_pxobject *)x, (long) (num_in_chans + 1));

    x->process_done = bangout(x);
    outlet_new(x, "signal");

    init_HIRT_common_attributes(x);

    x->abs_progress = 0;

    x->T = 0;
    x->current_t = 0;
    x->fft_size = 0;
    x->start_rec = 0;
    x->stop_rec = 0;
    x->sample_rate = sys_getsr();

    if (!x->sample_rate)
        x->sample_rate = 44100.0;

    alloc_mem_swap(&x->rec_mem, 0, 0);
    alloc_mem_swap(&x->out_mem, 0, 0);

    x->length = 0.0;
    x->current_length = 0;
    x->current_out_length = 0.0;
    x->out_length = 0.0;
    x->num_in_chans = (long) num_in_chans;
    x->num_active_ins = (long) num_in_chans;
    x->current_num_active_ins = (long) num_in_chans;

    x->smooth_mode = 1;

    atom_setlong(&x->deconvolve_delay, 10);

    attr_args_process(x, argc, argv);

    return x;
}


void irreference_free(t_irreference *x)
{
    dsp_free((t_pxobject *)x);

    free_mem_swap(&x->rec_mem);
    free_mem_swap(&x->out_mem);

    free_HIRT_common_attributes(x);
}


void irreference_assist(t_irreference *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_OUTLET)
    {
        if (a == 0)
            sprintf(s,"(signal) Record Progess");
        else
            sprintf(s,"Bangs When Done");
    }
    else
    {
        if (a == 0)
            sprintf(s,"(signal) Audio Reference / Instructions In");
        else
            sprintf(s,"(signal) Measurement Input %ld", a);
    }
}


//////////////////////////////////////////////////////////////////////////
///////////////////////// Calculate Memory Size //////////////////////////
//////////////////////////////////////////////////////////////////////////


intptr_t irreference_calc_mem_size(t_irreference *x, long active_ins)
{
    intptr_t rec_length = (intptr_t) (x->length * x->sample_rate);

    return rec_length * sizeof(double) * (1 + active_ins);
}


//////////////////////////////////////////////////////////////////////////
///////////////////////////// Parameter Check ////////////////////////////
//////////////////////////////////////////////////////////////////////////



double irreference_param_check(t_irreference *x, const char *name, double val, double min, double max)
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


void irreference_rec(t_irreference *x, t_symbol *sym, short argc, t_atom *argv)
{
    double length = 30000.0;
    double out_length = 5000.0;

    long num_active_ins = x->num_active_ins;

    intptr_t mem_size;

    // Load arguments

    if (argc > 0)
        length = atom_getfloat(argv++);
    if (argc > 1)
        out_length = atom_getfloat(argv++);

    // Check parameters

    length = irreference_param_check(x, "length", length, 0.0, HUGE_VAL);
    out_length = irreference_param_check(x, "ir length", out_length, 0.0, length);

    // Store parameters

    x->length = length / 1000.0;
    x->out_length = out_length / 1000.0;

    // Check length of recording and memory allocation

    if (length)
    {
        mem_size = irreference_calc_mem_size(x, num_active_ins);
        if (!schedule_grow_mem_swap(&x->rec_mem, mem_size, mem_size))
            object_error((t_object *) x, "not able to allocate adequate memory for recording");

        // Start measurement

        x->current_num_active_ins = num_active_ins;
        x->fft_size = 0;
        x->stop_rec = 0;
        x->start_rec = 1;
    }
    else
    {
        object_error((t_object *) x, "zero length recording");
        x->stop_rec = 1;
    }
}


void irreference_stop(t_irreference *x)
{
    x->start_rec = 0;
    x->stop_rec = 1;
}


void irreference_finish(t_irreference *x)
{
    x->start_rec = 0;
    x->stop_rec = 2;
}


void irreference_clear(t_irreference *x)
{
    irreference_stop(x);

    x->fft_size = 0;
    clear_mem_swap(&x->rec_mem);
    clear_mem_swap(&x->out_mem);
}


//////////////////////////////////////////////////////////////////////////
//////////////////////// Set Number of Active Inputs /////////////////////
//////////////////////////////////////////////////////////////////////////


void irreference_active_ins(t_irreference *x, t_atom_long num_active_ins)
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

    x->num_active_ins = (long) num_active_ins;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////// Deconvolution Processing ////////////////////////
//////////////////////////////////////////////////////////////////////////


void irreference_smooth(FFT_SETUP_D fft_setup, FFT_SPLIT_COMPLEX_D spectrum, FFT_SPLIT_COMPLEX_D temp_full_spectrum, long mode, uintptr_t fft_size, double smooth_lo, double smooth_hi)
{
    uintptr_t i;

    // Copy spectrum to temp

    for (i = 0; i < fft_size >> 1; i++)
    {
        temp_full_spectrum.realp[i] = spectrum.realp[i];
        temp_full_spectrum.imagp[i] = spectrum.imagp[i];
    }

    power_full_spectrum_from_half_spectrum(temp_full_spectrum, fft_size);
    smooth_power_spectrum(temp_full_spectrum, (t_smooth_mode) mode, fft_size, smooth_lo, smooth_hi);
    minimum_phase_from_power_spectrum(fft_setup, temp_full_spectrum, fft_size);

    // Copy back to original spectrum

    for (i = 0; i < fft_size >> 1; i++)
    {
        spectrum.realp[i] = temp_full_spectrum.realp[i];
        spectrum.imagp[i] = temp_full_spectrum.imagp[i];
    }

    // Copy nyquist

    spectrum.imagp[0] = temp_full_spectrum.realp[i];
}


void irreference_reprocess(t_irreference *x)
{
    if (!x->fft_size)
    {
        object_error((t_object *) x, "no complete recording to process (you may still be recording)");
        return;
    }

    defer(x, (method) irreference_process, 0, 0, 0);
}


void irreference_process(t_irreference *x, t_symbol *sym, short argc, t_atom *argv)
{
    FFT_SPLIT_COMPLEX_D spectrum_1;
    FFT_SPLIT_COMPLEX_D spectrum_2;
    FFT_SPLIT_COMPLEX_D spectrum_3;
    FFT_SPLIT_COMPLEX_D spectrum_4;

    double *rec_mem1;
    double *rec_mem2;
    double *out_mem;
    double *out_buf;

    t_symbol *filter = filter_retriever(x->deconvolve_filter_specifier);

    double filter_specifier[HIRT_MAX_SPECIFIER_ITEMS];
    double range_specifier[HIRT_MAX_SPECIFIER_ITEMS];

    double sample_rate = x->sample_rate;
    double deconvolve_phase = phase_retriever(x->deconvolve_phase);
    double deconvolve_delay;

    long deconvolve_mode = deconvolve_mode = x->deconvolve_mode;
    long smoothing_on = x->num_smooth;

    intptr_t alloc_rec_length = x->T;
    intptr_t rec_length = x->current_length;
    intptr_t filter_length = buffer_length(filter);

    uintptr_t fft_size;
    uintptr_t fft_size_log2;
    uintptr_t mem_size;
    uintptr_t i;

    // Sanity check

    if (!rec_length)
        return;

    // Check and calculate lengths

    fft_size = calculate_fft_size(rec_length * 2, fft_size_log2);
    deconvolve_delay = delay_retriever(x->deconvolve_delay, fft_size, sample_rate);

    // Allocate Temporary Memory

    temp_fft_setup fft_setup(fft_size_log2);

    temp_ptr<double> temp(smoothing_on ? fft_size * 6 : fft_size * 4);
    temp_ptr<float> filter_in(filter_length);

    spectrum_1.realp = temp.get();
    spectrum_1.imagp = spectrum_1.realp + (fft_size >> 1);
    spectrum_2.realp = spectrum_1.imagp + (fft_size >> 1);
    spectrum_2.imagp = spectrum_2.realp + (fft_size >> 1);
    spectrum_3.realp = spectrum_2.imagp + (fft_size >> 1);
    spectrum_3.imagp = spectrum_3.realp + fft_size;

    if (smoothing_on)
    {
        spectrum_4.realp = spectrum_3.imagp + fft_size;
        spectrum_4.imagp = spectrum_4.realp + fft_size;
    }
    else
    {
        spectrum_4.realp = nullptr;
        spectrum_4.imagp = nullptr;
    }

    if (!fft_setup || !temp || (filter_length && !filter_in))
    {
        object_error ((t_object *) x, "could not allocate temporary memory for processing");
        return;
    }

    x->fft_size = fft_size;

    // Allocate output memory and get record memory

    rec_mem1 = (double *) access_mem_swap(&x->rec_mem, &mem_size);
    out_mem = (double *) grow_mem_swap(&x->out_mem, fft_size * x->current_num_active_ins * sizeof(double), fft_size);

    if (!out_mem)
    {
        object_error ((t_object *) x, "could not allocate memory for output storage");
        return;
    }

    // Transform reference into spectrum 2 - [smooth]

    time_to_halfspectrum_double(fft_setup, rec_mem1, rec_length, spectrum_2, fft_size);
    if (smoothing_on)
        irreference_smooth(fft_setup, spectrum_2, spectrum_4, x->smooth_mode, fft_size, x->num_smooth > 1 ? x->smooth[0] : 0.0, x->num_smooth > 1 ? x->smooth[1] : x->smooth[0]);

    // Fill deconvolution filter specifiers - read filter from buffer (if specified) - make deconvolution filter - delay filter

    fill_power_array_specifier(filter_specifier, x->deconvolve_filter_specifier, x->deconvolve_num_filter_specifiers);
    fill_power_array_specifier(range_specifier, x->deconvolve_range_specifier, x->deconvolve_num_range_specifiers);
    buffer_read(filter, 0, filter_in.get(), fft_size);
    make_deconvolution_filter(fft_setup, spectrum_2, spectrum_3, filter_specifier, range_specifier, 0, filter_in.get(), filter_length, fft_size, SPECTRUM_REAL, (t_filter_type) deconvolve_mode, deconvolve_phase, sample_rate);
    delay_spectrum(spectrum_3, fft_size, SPECTRUM_REAL, deconvolve_delay);

    // Deconvolve each input

    for (i = 0; i < (uintptr_t) x->current_num_active_ins; i++)
    {
        // Get current input and output buffers

        rec_mem2 = rec_mem1 + ((i + 1) * alloc_rec_length);
        out_buf = out_mem + (i * fft_size);

        // Do transform into spectrum_1 - [smooth] - deconvolve - [delay] - transform back

        time_to_halfspectrum_double(fft_setup, rec_mem2, rec_length, spectrum_1, fft_size);
        if (smoothing_on)
            irreference_smooth(fft_setup, spectrum_1, spectrum_4, x->smooth_mode, fft_size, x->num_smooth > 1 ? x->smooth[0] : 0.0, x->num_smooth > 1 ? x->smooth[1] : x->smooth[0]);
        deconvolve_with_filter(spectrum_1, spectrum_2, spectrum_3, fft_size, SPECTRUM_REAL);
        spectrum_to_time(fft_setup, out_buf, spectrum_1, fft_size, SPECTRUM_REAL);
    }

    // Done

    outlet_bang(x->process_done);
}


//////////////////////////////////////////////////////////////////////////
/////////////////////////// Retrieval Routines ///////////////////////////
//////////////////////////////////////////////////////////////////////////


void irreference_extract(t_irreference *x, t_symbol *sym, long argc, t_atom *argv)
{
    defer(x, (method) irreference_extract_internal, sym, (short) argc, argv);
}


void irreference_extract_internal(t_irreference *x, t_symbol *sym, short argc, t_atom *argv)
{
    double *rec_mem;

    t_atom_long in_chan = 1;
    t_symbol *buffer = NULL;

    intptr_t rec_length = x->current_length;

    uintptr_t fft_size = x->fft_size;
    uintptr_t mem_size;

    rec_mem = (double *) access_mem_swap(&x->rec_mem, &mem_size);

    // Get arguments

    if (!argc)
    {
        object_error ((t_object *) x, "no arguments for extract message");
        return;
    }

    in_chan = argc > 1 ? atom_getlong(argv++) : 1;
    buffer = atom_getsym(argv++);

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

    buffer_write((t_object *)x, buffer, rec_mem + rec_length * in_chan, rec_length, x->write_chan - 1, x->resize, x->sample_rate, 1.0);
}


void irreference_dump(t_irreference *x, t_symbol *sym, long argc, t_atom *argv)
{
    defer(x, (method) irreference_dump_internal, sym, (short) argc, argv);
}


void irreference_dump_internal(t_irreference *x, t_symbol *sym, short argc, t_atom *argv)
{
    double *out_mem;

    t_atom_long in_chan = 1;
    t_symbol *buffer = NULL;

    uintptr_t fft_size = x->fft_size;
    uintptr_t mem_size;

    // Get arguments

    if (!argc)
    {
        object_error ((t_object *) x, "no arguments for dump message");
        return;
    }

    in_chan = argc > 1 ? atom_getlong(argv++) : 1;
    buffer = atom_getsym(argv++);

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

    out_mem = (double *) access_mem_swap(&x->out_mem, &mem_size) + (in_chan * fft_size);

    if (mem_size < fft_size)
    {
        object_error((t_object *) x, "storage memory is not large enough");
        return;
    }

    // Write to buffer

    buffer_write((t_object *)x, buffer, out_mem, fft_size, x->write_chan - 1, x->resize, x->sample_rate, 1.0);
}


void irreference_getir(t_irreference *x, t_symbol *sym, long argc, t_atom *argv)
{
    defer(x, (method) irreference_getir_internal, sym, (short) argc, argv);
}


void irreference_getir_internal(t_irreference *x, t_symbol *sym, short argc, t_atom *argv)
{
    t_symbol *buffer;

    double *out_mem;

    uintptr_t fft_size = x->fft_size;
    uintptr_t mem_size;

    intptr_t L;

    t_atom_long in_chan = 1;

    // Get arguments

    if (!argc)
    {
        object_error ((t_object *) x, "no arguments for getir message");
        return;
    }

    if (argc > 1)
        in_chan = atom_getlong(argv++);

    buffer = atom_getsym(argv++);

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

    out_mem = (double *) access_mem_swap(&x->out_mem, &mem_size);

    if (mem_size < fft_size)
    {
        object_error((t_object *) x, "storage memory is not large enough");
        return;
    }

    out_mem += fft_size * in_chan;
    L = (intptr_t) (x->sample_rate * x->current_out_length);

    // Write to buffer

    buffer_write((t_object *)x, buffer, out_mem, L, x->write_chan - 1, x->resize, x->sample_rate, 1.0);
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////// Perform Routines ////////////////////////////
//////////////////////////////////////////////////////////////////////////


t_int *irreference_perform(t_int *w)
{
    // Set pointers

    long vec_size = (long) w[1];
    t_irreference *x = (t_irreference *) w[2];

    float *out = (float *) w[3];
    float *in;

    double *rec_ptr;
    double *rec_mem;

    double sample_rate = x->sample_rate;
    double progress_mul = 1.0;

    intptr_t T;
    intptr_t current_t;
    intptr_t current_t2;
    intptr_t mem_size;
    intptr_t i, j;

    long record_on = 0;
    long mem_check;

    // Check for stop / start

    if (x->start_rec)
    {
        x->T = (intptr_t) (sample_rate * x->length);
        x->current_length = x->T;
        x->current_out_length = x->out_length;
        x->current_t = 0;
    }

    if (x->stop_rec)
    {
        x->current_length = x->current_t;
        x->current_t = x->T;
        record_on = x->stop_rec == 2;
    }

    x->start_rec = 0;
    x->stop_rec = 0;
    T = x->T;

    // Get counters

    current_t = x->current_t;
    current_t2 = current_t;
    record_on = record_on || current_t2 < T;

    // Check memory

    attempt_mem_swap(&x->rec_mem);
    rec_mem = (double *) x->rec_mem.current_ptr;
    mem_size = irreference_calc_mem_size(x, x->current_num_active_ins);
    mem_check = x->rec_mem.current_size >= (uintptr_t) mem_size;

    if (mem_check)
    {
        // Record Inputs

        for (j = 0; j < x->current_num_active_ins + 1; j++)
        {
            in = (float *) x->in_chans[j];
            rec_ptr = rec_mem + (j * T);
            for (i = 0, current_t2 = current_t; (i < vec_size) && (current_t2 < T); i++, current_t2++)
                rec_ptr[current_t2] = *in++;
        }
    }

    if (x->abs_progress)
        progress_mul = 1000.0 / sample_rate;
    else if (T)
        progress_mul = 1.0 / T;

    for (i = 0, current_t2 = current_t; i < vec_size && current_t2 < T && mem_check; i++)
        *out++ = (float) (current_t2++ * progress_mul);
    for (; i < vec_size; i++)
        *out++ = (float) (progress_mul * T);

    // Store accumulators

    x->current_t = current_t2;

    // Process when done

    if (record_on && current_t2 >= T)
        defer(x, (method) irreference_process, 0, 0, 0);

    return w + 4;
}


void irreference_perform64(t_irreference *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long vec_size, long flags, void *userparam)
{
    double *out = outs[0];

    double *rec_ptr;
    double *in;
    double *rec_mem;

    double sample_rate = x->sample_rate;
    double progress_mul = 1.0;

    intptr_t T;
    intptr_t current_t;
    intptr_t current_t2;
    intptr_t mem_size;
    intptr_t i, j;

    long record_on = 0;
    long mem_check;

    // Check for stop / start

    if (x->start_rec)
    {
        x->T = (intptr_t) (sample_rate * x->length);
        x->current_length = x->T;
        x->current_out_length = x->out_length;
        x->current_t = 0;
    }

    if (x->stop_rec)
    {
        x->current_length = x->current_t;
        x->current_t = x->T;
        record_on = x->stop_rec == 2;
    }

    x->start_rec = 0;
    x->stop_rec = 0;
    T = x->T;

    // Get counters

    current_t = x->current_t;
    current_t2 = current_t;
    record_on = record_on || current_t2 < T;

    // Check memory

    attempt_mem_swap(&x->rec_mem);
    rec_mem = (double *) x->rec_mem.current_ptr;
    mem_size = irreference_calc_mem_size(x, x->current_num_active_ins);
    mem_check = x->rec_mem.current_size >= (uintptr_t) mem_size;

    if (mem_check)
    {
        // Record Inputs

        for (j = 0; j < x->current_num_active_ins + 1; j++)
        {
            in = ins[j];
            rec_ptr = rec_mem + (j * T);
            for (i = 0, current_t2 = current_t; (i < vec_size) && (current_t2 < T); i++, current_t2++)
                rec_ptr[current_t2] = *in++;
        }
    }

    if (x->abs_progress)
        progress_mul = 1000.0 / sample_rate;
    else if (T)
        progress_mul = 1.0 / T;

    for (i = 0, current_t2 = current_t; i < vec_size && current_t2 < T && mem_check; i++)
        *out++ = current_t2++ * progress_mul;
    for (; i < vec_size; i++)
        *out++ = progress_mul * T;

    // Store accumulators

    x->current_t = current_t2;

    // Process when done

    if (record_on && current_t2 >= T)
        defer(x, (method) irreference_process, 0, 0, 0);
}


//////////////////////////////////////////////////////////////////////////
///////////////////////////// DSP Routines ///////////////////////////////
//////////////////////////////////////////////////////////////////////////


void irreference_dsp_common(t_irreference *x, double samplerate)
{
    double old_sr;

    uintptr_t mem_size;

    // Store sample rate

    old_sr = x->sample_rate;
    x->sample_rate = samplerate;

    if (x->start_rec)
    {
        // Resize memory if we are just about to record

        mem_size = irreference_calc_mem_size(x, x->current_num_active_ins);

        if (!schedule_grow_mem_swap(&x->rec_mem, mem_size, mem_size))
            object_error((t_object *) x, "not able to allocate adequate memory for recording");
    }
    else
    {
        if (x->sample_rate != old_sr)
        {
            // Clear memory if the sample rate has changed and we have not just started measuring

            irreference_clear(x);
        }
    }
}


void irreference_dsp(t_irreference *x, t_signal **sp, short *count)
{
    long i;

    // Store pointers to ins and outs

    for (i = 0; i < x->num_in_chans + 1; i++)
        x->in_chans[i] = sp[i]->s_vec;

    irreference_dsp_common(x, sp[0]->s_sr);
    dsp_add ((t_perfroutine)irreference_perform, 3, sp[0]->s_n, x, sp[x->num_in_chans + 1]->s_vec);
}


void irreference_dsp64(t_irreference *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
    irreference_dsp_common(x, samplerate);
    object_method(dsp64, gensym("dsp_add64"), x, irreference_perform64, 0, NULL);
}
