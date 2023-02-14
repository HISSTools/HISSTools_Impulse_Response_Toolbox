
#include <ext.h>
#include <ext_obex.h>
#include <ext_obex_util.h>
#include <z_dsp.h>

#include <HIRT_Core_Functions.hpp>
#include <HIRT_Buffer_Access.hpp>
#include <HIRT_Trim_Normalise.hpp>

// Define common attributes and the class name (for the common attributes file)

#define OBJ_CLASSNAME t_irtrimnorm
#define OBJ_USES_HIRT_WRITE_ATTR
#define OBJ_USES_HIRT_READ_ATTR

#include <HIRT_Common_Attribute_Vars.hpp>


// Object class and structure

t_class *this_class;

struct t_irtrimnorm
{
    t_pxobject x_obj;

    // Attributes

    double integration_times[2];
    double normalize;

    long fade_type;
    long crop_in_samples;
    long fade_in_samples;
    long pad_in_samples;

    long norm_mode;
    long limited_warnings;

    HIRT_COMMON_ATTR

    // Bang Outlet

    void *process_done;
};


// This include deals with setup of common attributes - requires the object structure to be defined

#include <HIRT_Common_Attribute_Setup.hpp>


// Atom helper for returning doubles

double atom_getdouble(t_atom *a)
{
    if (atom_gettype(a) == A_LONG)
        return static_cast<double>(atom_getlong(a));

    if (atom_gettype(a) == A_FLOAT)
        return atom_getfloat(a);

    return 0;
}


// Function prototypes

void *irtrimnorm_new(t_symbol *s, short argc, t_atom *argv);
void irtrimnorm_free(t_irtrimnorm *x);
void irtrimnorm_assist(t_irtrimnorm *x, void *b, long m, long a, char *s);

long irtrimnorm_check_number(t_atom *a);
long irtrimnorm_check_db(t_atom *a, double *db);

void irtrimnorm_write_internal_buffer(double *samples, double *internal_buffer, intptr_t offset, intptr_t length, intptr_t fade_in, intptr_t fade_out, intptr_t pad_in, intptr_t pad_out, t_fade_type fade_type);

double irtrimnorm_calculate_norm(t_irtrimnorm *x, double **samples, intptr_t *lengths, short N);
double irtrimnorm_calculate_trim(t_irtrimnorm *x, double **samples, intptr_t *lengths, intptr_t max_length, intptr_t total_length, short N, double in_db, double out_db, double sample_rate, intptr_t& trim_offset, intptr_t& trim_length);

long irtrimnorm_crop_check_write(t_irtrimnorm *x, t_symbol *buffer, intptr_t crop1, intptr_t crop2, intptr_t pad_in, intptr_t pad_out, intptr_t L);
long irtrimnorm_crop_write_buffer(t_irtrimnorm *x, t_symbol *buffer, double *samples, double *temp_buf, intptr_t crop1, intptr_t crop2, intptr_t fade_in, intptr_t fade_out, intptr_t pad_in, intptr_t pad_out, double norm_factor, intptr_t L, double sample_rate);
void irtrimnorm_crop(t_irtrimnorm *x, t_symbol *sym, long argc, t_atom *argv);
void irtrimnorm_crop_internal(t_irtrimnorm *x, t_symbol *sym, short argc, t_atom *argv);

long irtrimnorm_trim_check_write(t_irtrimnorm *x, t_symbol *buffer, intptr_t trim_offset, intptr_t trim_length, intptr_t fade_in, intptr_t fade_out, intptr_t pad_in, intptr_t pad_out, intptr_t L, double sample_rate);
long irtrimnorm_trim_write_buffer(t_irtrimnorm *x, t_symbol *buffer, double *samples, double *temp_buf, intptr_t trim_offset, intptr_t trim_length, intptr_t fade_in, intptr_t fade_out, intptr_t pad_in, intptr_t pad_out, double norm_factor, intptr_t L, double sample_rate);
void irtrimnorm_trim(t_irtrimnorm *x, t_symbol *sym, long argc, t_atom *argv);
void irtrimnorm_trim_internal(t_irtrimnorm *x, t_symbol *sym, short argc, t_atom *argv);

void irtrimnorm_pad(t_irtrimnorm *x, t_symbol *sym, long argc, t_atom *argv);


//////////////////////////////////////////////////////////////////////////
/////////////////////// Main / New / Free / Assist ///////////////////////
//////////////////////////////////////////////////////////////////////////


int C74_EXPORT main()
{
    this_class = class_new("irtrimnorm~",
                          (method) irtrimnorm_new,
                          (method)irtrimnorm_free,
                          sizeof(t_irtrimnorm),
                          0L,
                          A_GIMME,
                          0);

    class_addmethod(this_class, (method)irtrimnorm_assist, "assist", A_CANT, 0L);

    class_addmethod(this_class, (method)irtrimnorm_trim, "trim", A_GIMME, 0L);
    class_addmethod(this_class, (method)irtrimnorm_trim, "trimto", A_GIMME, 0L);
    class_addmethod(this_class, (method)irtrimnorm_crop, "crop", A_GIMME, 0L);
    class_addmethod(this_class, (method)irtrimnorm_crop, "cropto", A_GIMME, 0L);
    class_addmethod(this_class, (method)irtrimnorm_pad, "pad", A_GIMME, 0L);
    class_addmethod(this_class, (method)irtrimnorm_pad, "padto", A_GIMME, 0L);

    declare_HIRT_common_attributes(this_class);

    CLASS_ATTR_LONG(this_class, "fadetype", 0, t_irtrimnorm, fade_type);
    CLASS_ATTR_ENUMINDEX(this_class,"fadetype", 0, "Linear Square \"Square Root\" Cosine Sigmoid");
    CLASS_ATTR_FILTER_CLIP(this_class, "fadetype", 0, 4);
    CLASS_ATTR_LABEL(this_class,"fadetype", 0, "Fade Type");

    CLASS_ATTR_DOUBLE(this_class, "normlevel", 0, t_irtrimnorm, normalize);
    CLASS_ATTR_LABEL(this_class,"normlevel", 0, "Normalization Level (dB)");

    CLASS_ATTR_LONG(this_class, "normmode", 0, t_irtrimnorm, norm_mode);
    CLASS_ATTR_ENUMINDEX(this_class,"normmode", 0, "Off On");
    CLASS_ATTR_FILTER_CLIP(this_class, "normmode", 0, 1);
    CLASS_ATTR_LABEL(this_class,"normmode", 0, "Normalization Mode");

    CLASS_ATTR_DOUBLE_ARRAY(this_class, "integrationtimes", 0, t_irtrimnorm, integration_times, 2);
    CLASS_ATTR_FILTER_MIN(this_class, "integrationtimes", 1.0);
    CLASS_ATTR_LABEL(this_class,"integrationtimes", 0, "Integration Times");

    CLASS_ATTR_LONG(this_class, "cropinsamps", 0 , OBJ_CLASSNAME, crop_in_samples);
    CLASS_ATTR_STYLE_LABEL(this_class,"cropinsamps", 0,"onoff","Crop Times in Samples");

    CLASS_ATTR_LONG(this_class, "fadeinsamps", 0, OBJ_CLASSNAME, fade_in_samples);
    CLASS_ATTR_STYLE_LABEL(this_class,"fadeinsamps", 0,"onoff","Fade Times in Samples");

    CLASS_ATTR_LONG(this_class, "padinsamps", 0, OBJ_CLASSNAME, pad_in_samples);
    CLASS_ATTR_STYLE_LABEL(this_class,"padinsamps", 0,"onoff","Pad Times in Samples");

    CLASS_ATTR_LONG(this_class, "limitedwarnings", 0 , OBJ_CLASSNAME, limited_warnings);
    CLASS_ATTR_STYLE_LABEL(this_class,"limitedwarnings", 0,"onoff","Limited Warnings");

    class_dspinit(this_class);

    class_register(CLASS_BOX, this_class);

    return 0;
}


void *irtrimnorm_new(t_symbol *s, short argc, t_atom *argv)
{
    t_irtrimnorm *x = reinterpret_cast<t_irtrimnorm *>(object_alloc(this_class));

    x->process_done = bangout(x);

    init_HIRT_common_attributes(x);

    x->normalize = 0.0;
    x->norm_mode = 0;
    x->fade_type = 0;

    x->crop_in_samples = 0;
    x->fade_in_samples = 0;
    x->pad_in_samples = 0;

    x->integration_times[0] = 5.0;
    x->integration_times[1] = 50.0;

    x->limited_warnings = 0;

    attr_args_process(x, argc, argv);

    return x;
}


void irtrimnorm_free(t_irtrimnorm *x)
{
    free_HIRT_common_attributes(x);
}


void irtrimnorm_assist(t_irtrimnorm *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET)
        sprintf(s,"Instructions In");
    else
        sprintf(s,"Bang on Success");
}


//////////////////////////////////////////////////////////////////////////
///////////////////////// Atom Argument Checkers /////////////////////////
//////////////////////////////////////////////////////////////////////////


long irtrimnorm_check_number(t_atom *a)
{
    if (atom_gettype(a) == A_LONG || atom_gettype(a) == A_FLOAT)
        return 0;
    else
        return 1;
}


long irtrimnorm_check_db(t_atom *a, double *db)
{
    if (!irtrimnorm_check_number(a))
    {
        *db = atom_getdouble(a);
        return 0;
    }

    if (atom_gettype(a) == A_SYM)
    {
        if (atom_getsym(a) == gensym("off"))
        {
            *db = -HUGE_VAL;
            return 0;
        }
    }

    return 1;
}


//////////////////////////////////////////////////////////////////////////
///////////////////// Write To Internal Temp Buffer //////////////////////
//////////////////////////////////////////////////////////////////////////


void irtrimnorm_write_internal_buffer(double *samples, double *internal_buffer, intptr_t offset, intptr_t length, intptr_t fade_in, intptr_t fade_out, intptr_t pad_in, intptr_t pad_out, t_fade_type fade_type)
{
    for (intptr_t i = 0; i < pad_in; i++)
        internal_buffer[i] = 0.0;

    trim_copy_part(internal_buffer + pad_in, samples, offset, length);
    fade_calc_fade_in(internal_buffer + pad_in, fade_in, length, fade_type);
    fade_calc_fade_out(internal_buffer + pad_in, fade_out, length, fade_type);

    for (intptr_t i = pad_in + length; i < (pad_in + length + pad_out); i++)
        internal_buffer[i] = 0.0;
}


//////////////////////////////////////////////////////////////////////////
/////////////////// Calculate Normalisation and Trim /////////////////////
//////////////////////////////////////////////////////////////////////////


double irtrimnorm_calculate_norm(t_irtrimnorm *x, double **samples, intptr_t *lengths, short N)
{
    // Calculate Normalisation Factor

    double max = 0.0;

    if (x->norm_mode)
    {
        for (short i = 0; i < N; i++)
            max = norm_find_max(samples[i], lengths[i], max);
    }

    if (max)
        return db_to_a(x->normalize) / max;
    else
        return 1.0;
}


double irtrimnorm_calculate_trim(t_irtrimnorm *x, double **samples, intptr_t *lengths, intptr_t max_length, intptr_t total_length, short N, double in_db, double out_db, double sample_rate, intptr_t& trim_offset, intptr_t& trim_length)
{
    uintptr_t current_start = max_length;
    uintptr_t current_end = 0;
    uintptr_t window_in  = static_cast<uintptr_t>(x->integration_times[0] * sample_rate / 1000.0);
    uintptr_t window_out = static_cast<uintptr_t>(x->integration_times[1] * sample_rate / 1000.0);
    bool success = false;

    // Calculate Normalisation Factor

    const double mul = irtrimnorm_calculate_norm(x, samples, lengths, N);

    // Calculate Trim

    for (short i = 0; i < N; i++)
    {
        if (!trim_find_crossings_rms(samples[i], lengths[i], window_in, window_out, in_db, out_db, mul, &current_start, &current_end))
            success = true;
    }

    if (!success)
    {
        trim_offset = 0;
        trim_length = 0;
    }
    else
    {
        trim_offset = current_start;
        trim_length = current_end - current_start;
    }

    return mul;
}


//////////////////////////////////////////////////////////////////////////
///////////////////////////// Crop Routines //////////////////////////////
//////////////////////////////////////////////////////////////////////////


long irtrimnorm_crop_check_write(t_irtrimnorm *x, t_symbol *buffer, intptr_t crop1, intptr_t crop2, intptr_t pad_in, intptr_t pad_out, intptr_t L)
{
    crop2 -= crop1;

    if (buffer_length(buffer) < crop2 + pad_in + pad_out)
    {
        object_error((t_object *) x, "buffer %s is not long enough to complete write", buffer->s_name);
        return 1;
    }

    return 0;
}


long irtrimnorm_crop_write_buffer(t_irtrimnorm *x, t_symbol *buffer, double *samples, double *temp_buf, intptr_t crop1, intptr_t crop2, intptr_t fade_in, intptr_t fade_out, intptr_t pad_in, intptr_t pad_out, double norm_factor, intptr_t L, double sample_rate)
{
    intptr_t crop_length = crop2 - crop1;

    // Check length

    if (crop2 == 0 || L < crop2)
    {
        if (crop2 > 0 && !x->limited_warnings)
            object_warn((t_object *) x, "buffer %s is smaller than requested after crop", buffer->s_name);

        crop_length = L - crop1;

        if (fade_in > crop_length)
        {
            fade_in = crop_length;
            if (!x->limited_warnings)
                object_warn((t_object *) x, "buffer %s fade in shorter than requested", buffer->s_name);
        }

        if (fade_out > crop_length)
        {
            fade_out = crop_length;
            if (!x->limited_warnings)
                object_warn((t_object *) x, "buffer %s fade out shorter than requested", buffer->s_name);
        }
    }

    if (fade_in + fade_out > crop_length && !x->limited_warnings)
        object_warn((t_object *) x, "buffer %s has overlapping fades", buffer->s_name);

    if (L < crop1)
    {
        object_warn((t_object *) x, "nothing to output to buffer %s after crop", buffer->s_name);
        return 0;
    }

    // Crop, Fade and Pad

    irtrimnorm_write_internal_buffer(samples, temp_buf, crop1, crop_length, fade_in, fade_out, pad_in, pad_out, (t_fade_type) x->fade_type);

    // Write to buffer

    return static_cast<long>(buffer_write((t_object *)x, buffer, temp_buf, crop_length + pad_in + pad_out, x->write_chan - 1, x->resize, sample_rate, norm_factor));
}


void irtrimnorm_crop(t_irtrimnorm *x, t_symbol *sym, long argc, t_atom *argv)
{
    defer(x, (method) irtrimnorm_crop_internal, sym, (short) argc, argv);
}


void irtrimnorm_crop_internal(t_irtrimnorm *x, t_symbol *sym, short argc, t_atom *argv)
{
    t_symbol *in_buffer_names[128];
    t_symbol *out_buffer_names[128];

    double *samples[128];
    intptr_t lengths[128];

    double pad_in_time = 0.0;
    double pad_out_time = 0.0;
    double sample_rate = 0.0;

    intptr_t overall_length = 0;
    intptr_t max_length = 0;
    intptr_t offset = 0;

    t_atom_long read_chan = x->read_chan - 1;

    bool overall_error = false;

    // Get pad times if present

    if (argc > 4 && !irtrimnorm_check_number(argv + argc - 5))
    {
        pad_out_time = atom_getdouble(argv + argc - 1);
        argc--;
    }

    if (argc > 4 && !irtrimnorm_check_number(argv + argc - 5))
    {
        pad_in_time = atom_getdouble(argv + argc - 1);
        argc--;
    }

    // Check remaining number arguments

    if ((sym == gensym("cropto") && argc < 6) || (sym == gensym("crop") && argc < 5))
    {
        object_error((t_object *) x, "not enough arguments to message %s", sym->s_name);
        return;
    }

    if (irtrimnorm_check_number(argv + argc - 4) || irtrimnorm_check_number(argv + argc - 3) || irtrimnorm_check_number(argv + argc - 2) || irtrimnorm_check_number(argv + argc - 1))
    {
        object_error((t_object *) x, "expected number argument to message %s, but found symbol", sym->s_name);
        return;
    }

    // Get crop and fade times

    const double crop1_time = atom_getdouble(argv + argc - 4);
    const double crop2_time = atom_getdouble(argv + argc - 3);
    const double fade_in_time = atom_getdouble(argv + argc - 2);
    const double fade_out_time = atom_getdouble(argv + argc - 1);
    argc -= 4;

    // Check buffers, storing names and lengths +  calculate total / largest length

    short num_buffers = buffer_multiple_names((t_object *) x, in_buffer_names, out_buffer_names, lengths, argc, argv, sym == gensym("crop"), 128, &overall_length, &max_length, &sample_rate);

    if (!num_buffers)
        return;

    // Store raw times (default)

    intptr_t fade_in  = static_cast<intptr_t>(fade_in_time);
    intptr_t fade_out = static_cast<intptr_t>(fade_out_time);
    intptr_t crop1    = static_cast<intptr_t>(crop1_time);
    intptr_t crop2    = static_cast<intptr_t>(crop2_time);
    intptr_t pad_in   = static_cast<intptr_t>(pad_in_time);
    intptr_t pad_out  = static_cast<intptr_t>(pad_out_time);

    // Convert if relevant

    if (!x->fade_in_samples)
    {
        fade_in  = static_cast<intptr_t>(fade_in_time   * sample_rate / 1000.0);
        fade_out = static_cast<intptr_t>(fade_out_time * sample_rate / 1000.0);
    }

    if (!x->crop_in_samples)
    {
        crop1 = static_cast<intptr_t>(crop1_time * sample_rate / 1000.0);
        crop2 = static_cast<intptr_t>(crop2_time * sample_rate / 1000.0);
    }

    if (!x->pad_in_samples)
    {
        pad_in  = static_cast<intptr_t>(pad_in_time  * sample_rate / 1000.0);
        pad_out = static_cast<intptr_t>(pad_out_time * sample_rate / 1000.0);
    }

    // Check arguments

    if (crop2 > 0 && crop2 <= crop1)
    {
        object_error((t_object *) x, "second crop point is equal to / earlier than first crop point");
        return;
    }

    if (crop2 > 0 && fade_in > crop2 - crop1)
    {
        object_warn((t_object *) x, "requested fade in is longer than crop length");
        fade_in = crop2 - crop1;
    }

    if (crop2 > 0 && fade_out > crop2 - crop1)
    {
        object_warn((t_object *) x, "requested fade out is longer than crop length");
        fade_out = crop2 - crop1;
    }

    // Check times

    crop1 = crop1 < 0 ? 0 : crop1;
    crop2 = crop2 < 0 ? 0 : crop2;
    fade_in = fade_in < 0 ? 0 : fade_in;
    fade_out = fade_out < 0 ? 0 : fade_out;
    pad_in = pad_in < 0 ? 0 : pad_in;
    pad_out = pad_out < 0 ? 0 : pad_out;

    // Assign Temporary Memory

    temp_ptr<double> temp1(overall_length);
    temp_ptr<double> temp2(max_length + pad_in + pad_out);
    
    samples[0] = temp1.get();
    double *temp_buf_d = temp2.get();
    float *temp_buf_f = reinterpret_cast<float *>(temp_buf_d);

    // Check temporary memory

    if (!temp1 || !temp2)
    {
        object_error((t_object *) x, "could not allocate temporary memory for internal storage");
        return;
    }

    // Process

    for (short i = 0; i < num_buffers; i++)
    {
        samples[i] = samples[0] + offset;
        buffer_read(in_buffer_names[i], read_chan, temp_buf_f, lengths[i]);

        for (intptr_t j = 0; j < lengths[i]; j++)
            samples[i][j] = temp_buf_f[j];
        offset += lengths[i];
    }

    const double norm_factor = irtrimnorm_calculate_norm(x, samples, lengths, num_buffers);

    if (!x->resize)
    {
        for (short i = 0; i < num_buffers; i++)
        {
            if (irtrimnorm_crop_check_write(x, out_buffer_names[i], crop1, crop2, pad_in, pad_out, lengths[i]))
            {
                overall_error = true;
                break;
            }
        }
    }

    if (!overall_error)
    {
        for (short i = 0; i < num_buffers; i++)
            if (irtrimnorm_crop_write_buffer(x, out_buffer_names[i], samples[i], temp_buf_d, crop1, crop2, fade_in, fade_out, pad_in, pad_out, norm_factor, lengths[i], sample_rate))
                overall_error = true;
    }

    if (!overall_error)
        outlet_bang(x->process_done);
}


//////////////////////////////////////////////////////////////////////////
///////////////////////////// Trim Routines //////////////////////////////
//////////////////////////////////////////////////////////////////////////


long irtrimnorm_trim_check_write(t_irtrimnorm *x, t_symbol *buffer, intptr_t trim_offset, intptr_t trim_length, intptr_t fade_in, intptr_t fade_out, intptr_t pad_in, intptr_t pad_out, intptr_t L, double sample_rate)
{
    // Check length

    if (L < trim_offset + trim_length)
        trim_length = L - trim_offset;

    // Check fade lengths

    if (trim_offset < fade_in)
        fade_in = trim_offset;

    if (L < trim_offset + trim_length + fade_out)
        fade_out = L - (trim_offset + trim_length);

    if (buffer_length(buffer) < trim_length + fade_in + fade_out + pad_in + pad_out)
    {
        object_error((t_object *) x, "buffer %s is not long enough to complete write", buffer->s_name);
        return 1;
    }

    return 0;
}


long irtrimnorm_trim_write_buffer(t_irtrimnorm *x, t_symbol *buffer, double *samples, double *temp_buf, intptr_t trim_offset, intptr_t trim_length, intptr_t fade_in, intptr_t fade_out, intptr_t pad_in, intptr_t pad_out, double norm_factor, intptr_t L, double sample_rate)
{
    // Check length

    if (L < trim_offset + trim_length)
        trim_length = L - trim_offset;

    if (L < trim_offset)
    {
        object_warn((t_object *) x, "nothing to output to buffer %s after trim", buffer->s_name);
        return 0;
    }

    // Check fade lengths

    if (trim_offset < fade_in)
    {
        fade_in = trim_offset;
        if (!x->limited_warnings)
            object_warn((t_object *) x, "buffer %s fade in shorter than requested", buffer->s_name);
    }

    if (L < trim_offset + trim_length + fade_out)
    {
        fade_out = L - (trim_offset + trim_length);
        if (!x->limited_warnings)
            object_warn((t_object *) x, "buffer %s fade out shorter than requested", buffer->s_name);
    }

    trim_offset -= fade_in;
    trim_length += fade_in + fade_out;

    // Trim and Fade and Write To Buffer

    irtrimnorm_write_internal_buffer(samples, temp_buf, trim_offset, trim_length, fade_in, fade_out, pad_in, pad_out, (t_fade_type) x->fade_type);

    // Write to buffer

    return static_cast<long>(buffer_write((t_object *)x, buffer, temp_buf, trim_length + pad_in + pad_out, x->write_chan - 1, x->resize, sample_rate, norm_factor));
}


void irtrimnorm_trim(t_irtrimnorm *x, t_symbol *sym, long argc, t_atom *argv)
{
    defer(x, (method) irtrimnorm_trim_internal, sym, (short) argc, argv);
}


void irtrimnorm_trim_internal(t_irtrimnorm *x, t_symbol *sym, short argc, t_atom *argv)
{
    t_symbol *in_buffer_names[128];
    t_symbol *out_buffer_names[128];

    double *samples[128];
    intptr_t lengths[128];

    double in_db = -HUGE_VAL;
    double out_db = -HUGE_VAL;
    double pad_in_time = 0.0;
    double pad_out_time = 0.0;
    double sample_rate = 0.0;

    intptr_t trim_offset;
    intptr_t trim_length;
    intptr_t overall_length = 0;
    intptr_t max_length = 0;
    intptr_t offset = 0;

    t_atom_long read_chan = x->read_chan - 1;

    bool overall_error = false;

    // Get pad times if present

    if (argc > 4 && !irtrimnorm_check_number(argv + argc - 5))
    {
        pad_out_time = atom_getdouble(argv + argc - 1);
        argc--;
    }

    if (argc > 4 && !irtrimnorm_check_number(argv + argc - 5))
    {
        pad_in_time = atom_getdouble(argv + argc - 1);
        argc--;
    }

    // Check number of arguments remaining

    if ((sym == gensym("trimto") && argc < 6) || (sym == gensym("trim") && argc < 5))
    {
        object_error((t_object *) x, "not enough arguments to message %s", sym->s_name);
        return;
    }

    // Get trim values and check remaining number values

    if (irtrimnorm_check_db(argv + argc - 4, &in_db) || irtrimnorm_check_db(argv + argc - 3, &out_db))
    {
        object_error((t_object *) x, "expected number argument or 'off' to message %s, but found unexpected symbol", sym->s_name);
        return;
    }

    if (irtrimnorm_check_number(argv + argc - 2) || irtrimnorm_check_number(argv + argc - 1))
    {
        object_error((t_object *) x, "expected number argument to message %s, but found symbol", sym->s_name);
        return;
    }

    // Get fade times - N.B We retrieve the dB values above

    const double fade_in_time = atom_getdouble(argv + argc - 2);
    const double fade_out_time = atom_getdouble(argv + argc - 1);
    argc -= 4;

    // Check buffers, storing names and lengths +  calculate total / largest length

    short num_buffers = buffer_multiple_names((t_object *) x, in_buffer_names, out_buffer_names, lengths, argc, argv, sym == gensym("trim"), 128, &overall_length, &max_length, &sample_rate);

    if (!num_buffers)
        return;

    // Store raw times

    intptr_t fade_in  = static_cast<intptr_t>(fade_in_time);
    intptr_t fade_out = static_cast<intptr_t>(fade_out_time);
    intptr_t pad_in   = static_cast<intptr_t>(pad_in_time);
    intptr_t pad_out  = static_cast<intptr_t>(pad_out_time);

    // Convert times if relevant

    if (!x->fade_in_samples)
    {
        fade_in  = static_cast<intptr_t>(fade_in_time  * sample_rate / 1000.0);
        fade_out = static_cast<intptr_t>(fade_out_time * sample_rate / 1000.0);
    }

    if (!x->pad_in_samples)
    {
        pad_in  = static_cast<intptr_t>(pad_in_time  * sample_rate / 1000.0);
        pad_out = static_cast<intptr_t>(pad_out_time * sample_rate / 1000.0);
    }

    // Check times

    fade_in  = fade_in  < 0 ? 0 : fade_in;
    fade_out = fade_out < 0 ? 0 : fade_out;
    pad_in   = pad_in   < 0 ? 0 : pad_in;
    pad_out  = pad_out  < 0 ? 0 : pad_out;

    // Assign Temporary Memory

    temp_ptr<double> temp1(overall_length);
    temp_ptr<double> temp2(max_length + pad_in + pad_out);
    
    samples[0] = temp1.get();
    double *temp_buf_d = temp2.get();
    float *temp_buf_f = reinterpret_cast<float *>(temp_buf_d);

    // Check temporary memory

    if (!temp1 || !temp2)
    {
        object_error((t_object *) x, "could not allocate temporary memory for internal storage");
        return;
    }

    // Process

    for (short i = 0; i < num_buffers; i++)
    {
        samples[i] = samples[0] + offset;
        buffer_read(in_buffer_names[i], read_chan, temp_buf_f, lengths[i]);

        for (intptr_t j = 0; j < lengths[i]; j++)
             samples[i][j] = temp_buf_f[j];
        offset += lengths[i];
    }

    const double norm_factor = irtrimnorm_calculate_trim(x, samples, lengths, max_length, overall_length, num_buffers, in_db, out_db, sample_rate, trim_offset, trim_length);

    if (trim_length == 0)
    {
        object_error((t_object *) x, "requested start trim level / end trim level never reached in any given buffer");
    }
    else
    {
        if (!x->resize)
        {
            for (short i = 0; i < num_buffers; i++)
            {
                if (irtrimnorm_trim_check_write(x, out_buffer_names[i], trim_offset, trim_length, fade_in, fade_out, pad_in, pad_out, lengths[i], sample_rate))
                {
                    overall_error = true;
                    break;
                }
            }
        }

        if (!overall_error)
        {
            for (short i = 0; i < num_buffers; i++)
                if (irtrimnorm_trim_write_buffer(x, out_buffer_names[i], samples[i], temp_buf_d, trim_offset, trim_length, fade_in, fade_out, pad_in, pad_out, norm_factor, lengths[i], sample_rate))
                    overall_error = true;
        }
    }

    if (!overall_error)
        outlet_bang(x->process_done);
}


//////////////////////////////////////////////////////////////////////////
////////////////////////////// Pad Routine ///////////////////////////////
//////////////////////////////////////////////////////////////////////////


void irtrimnorm_pad(t_irtrimnorm *x, t_symbol *sym, long argc, t_atom *argv)
{
    t_symbol *call_sym;
    t_atom call_argv[134];
    t_atom pad_in;
    t_atom pad_out;
    short call_argc = 0;
    short i;

    atom_setlong(&pad_in, 0);
    atom_setlong(&pad_out, 0);

    // Set correct crop message

    if (sym == gensym("padto"))
        call_sym = gensym("cropto");
    else
        call_sym = gensym("crop");

    // Check number of arguments

    if ((sym == gensym("padto") && argc < 3) || (sym == gensym("pad") && argc < 2))
    {
        object_error((t_object *) x, "not enough arguments to message %s", sym->s_name);
        return;
    }

    // Retrieve pad times

    if (irtrimnorm_check_number(argv + argc - 1))
    {
        object_error((t_object *) x, "expected number argument to message %s, but found symbol", sym->s_name);
        return;
    }

    pad_out = argv[argc - 1];
    argc--;

    if (!irtrimnorm_check_number(argv + argc - 1))
    {
        pad_in = argv[argc - 1];
        argc--;
    }

    // Check number of arguments remaining

    if (argc > 128)
        argc = 128;

    call_argc = (short) (argc + 6);

    // Copy arguments

    for (i = 0; i < argc; i++)
        call_argv[i] = argv[i];

    atom_setlong(call_argv + i + 0, 0);
    atom_setlong(call_argv + i + 1, 0);
    atom_setlong(call_argv + i + 2, 0);
    atom_setlong(call_argv + i + 3, 0);
    call_argv[i + 4] = pad_in;
    call_argv[i + 5] = pad_out;

    defer(x, (method) irtrimnorm_crop_internal, call_sym, call_argc, call_argv);
}
