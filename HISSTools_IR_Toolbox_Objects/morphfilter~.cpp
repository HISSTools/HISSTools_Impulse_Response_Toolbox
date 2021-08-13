
#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <HIRT_Core_Functions.hpp>
#include <HIRT_Buffer_Access.hpp>

// Define common attributes and the class name (for the common attributes file)

#define OBJ_CLASSNAME t_morphfilter
#define OBJ_USES_HIRT_WRITE_ATTR
#define OBJ_USES_HIRT_READ_ATTR

#include <HIRT_Common_Attribute_Vars.hpp>

// Object class and structure

t_class *this_class;

enum t_iir_filter_type
{
    GAIN,
    LO_SHELF,
    HI_SHELF,
    PEAKING
};

struct t_filter_params
{
    t_iir_filter_type filter_type;

    double f0;
    double qs;
    double g_db0;
    double g_db_alter;
    double rate;
};

struct t_morphfilter
{
    t_pxobject x_obj;

    t_filter_params filter[16];

    long nfilters;

    HIRT_COMMON_ATTR

    // Bang Outlet

    void *process_done;
};


// This include deals with setup of common attributes - requires the object structure to be defined

#include <HIRT_Common_Attribute_Setup.hpp>


// Function prototypes

void *morphfilter_new();
void morphfilter_free(t_morphfilter *x);
void morphfilter_assist(t_morphfilter *x, void *b, long m, long a, char *s);

void morphfilter_addfilter(t_morphfilter *x, t_symbol *sym, long argc, t_atom *argv);

void morphfilter_clear(t_morphfilter *x);

void morphfilter_process(t_morphfilter *x, t_symbol *target, t_symbol *source);
void morphfilter_process_internal(t_morphfilter *x, t_symbol *sym, short argc, t_atom *argv);


int C74_EXPORT main()
{
    this_class = class_new("morphfilter~",
                          (method) morphfilter_new,
                          (method)morphfilter_free,
                          sizeof(t_morphfilter),
                          0L,
                          0);

    class_addmethod(this_class, (method)morphfilter_process, "process", A_SYM, A_SYM, 0L);
    class_addmethod(this_class, (method)morphfilter_addfilter, "addfilter", A_GIMME, 0L);

    class_addmethod(this_class, (method)morphfilter_clear, "clear", 0L);

    class_addmethod(this_class, (method)morphfilter_assist, "assist", A_CANT, 0L);

    declare_HIRT_common_attributes(this_class);

    class_register(CLASS_BOX, this_class);

    return 0;
}


void *morphfilter_new()
{
    t_morphfilter *x = (t_morphfilter *)object_alloc (this_class);

    x->process_done = bangout(x);
    x->nfilters = 0;

    init_HIRT_common_attributes(x);

    return x;
}


void morphfilter_free(t_morphfilter *x)
{
    free_HIRT_common_attributes(x);
}


void morphfilter_assist(t_morphfilter *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET)
        sprintf(s,"Instructions In");
    else
        sprintf(s,"Bang on Success");
}


// Arguments are - filter type / [centre frequency] / [q or s] / [starting gain] / [gain alteration amount] / [gain amount rate]

void morphfilter_addfilter(t_morphfilter *x, t_symbol *sym, long argc, t_atom *argv)
{
    long nfilters = x->nfilters;
    t_iir_filter_type filter_type = GAIN;

    t_symbol *ftype;
    double f0 = 0;
    double qs = 0;
    double g_db0 = 0;
    double g_db_alter = 0;
    double rate = 0;

    ftype = argc > 0 ? atom_getsym(argv + 0) : 0;
    f0 = argc > 1 ? atom_getfloat(argv + 1) : 0;
    qs = argc > 2 ? atom_getfloat(argv + 2) : 0;
    g_db0 = argc > 3 ? atom_getfloat(argv + 3) : 0;
    g_db_alter = argc > 4 ? atom_getfloat(argv + 4) : 0;
    rate = argc > 5 ? atom_getfloat(argv + 5) : 0;

    if (argc > 6)
        object_warn((t_object *) x, "additional arguments to message addfilter");

    if (nfilters < 16)
    {
        if (ftype == gensym("lowshelf"))
            filter_type = LO_SHELF;
        if (ftype == gensym("highshelf"))
            filter_type = HI_SHELF;
        if (ftype == gensym("peaknotch"))
            filter_type = PEAKING;
        if (filter_type == GAIN && ftype != gensym("gain"))
            object_error((t_object *)x, "unknown filter type =- defaulting to gain");

        x->filter[nfilters].filter_type = filter_type;
        x->filter[nfilters].f0 = f0;
        x->filter[nfilters].qs = qs;
        x->filter[nfilters].g_db0 = g_db0;
        x->filter[nfilters].g_db_alter = g_db_alter;
        x->filter[nfilters].rate = rate;

        x->nfilters = nfilters + 1;

    }
    else
        object_error((t_object *)x, "no space for new filters");
}


void morphfilter_clear (t_morphfilter *x)
{
    x->nfilters = 0;
}


static inline double morphfilter_biquad (double in, double a0, double a1, double a2, double b0, double b1, double b2, double *x1, double *x2, double *y1, double *y2)
{
    double out = (b0 * in + b1 * *x1 + b2 * *x2 - a1 * *y1 - a2 * *y2) / a0;

    *x2 = *x1;
    *x1 = in;
    *y2 = *y1;
    *y1 = out;

    return out;
}


double dbtoa(double db)
{
    return pow (10.0, db / 20.);
}


void morphfilter_apply_filter(double *samples, t_filter_params *filter, double sr, intptr_t length)
{
    double a0, a1, a2, b0, b1, b2, x1, x2, y1, y2, g_db1, g_db2, rate, A, sqrtA, f0, w0, qs, cosw, sinw;
    double Ap1, Am1, Ap1c, Am1c;
    double alpha, alpha_shelf, alpha_mul_A, alpha_div_A;
    double gain_mul;

    intptr_t i;

    f0 = filter->f0;
    rate = filter->rate;
    g_db1 = filter->g_db0;
    g_db2 = rate ? g_db1 + (filter->g_db_alter / filter->rate * (1000. * length / sr)) : g_db1;
    qs =  filter->qs;

    w0 = 2.0 * PI * f0 / sr;
    cosw = cos(w0);
    sinw = sin(w0);

    x1 = x2 = y1 = y2 = 0.;

    A = dbtoa(filter->g_db0 / 2.);
    gain_mul = rate == 0 ? 1 : dbtoa(filter->g_db_alter / (2 * (filter->rate * sr) / 1000.));

    switch (filter->filter_type)
    {
        case GAIN:

            for (i = 0; i < length; i++, A *= gain_mul)
                samples[i] = samples[i] * A;
            break;

        case LO_SHELF:

            sqrtA = sqrt(A);
            gain_mul = sqrt(gain_mul);

            for (i = 0; i < length; i++, sqrtA *= gain_mul)
            {
                alpha = sinw / (2.0 * qs);
                alpha_shelf = 2.0 * sqrtA * alpha;

                A = sqrtA * sqrtA;
                Ap1 = A + 1.0;
                Am1 = A - 1.0;
                Ap1c = Ap1 * cosw;
                Am1c = Am1 * cosw;

                a0 =        Ap1 + Am1c + alpha_shelf;
                a1 =       (Am1 + Ap1c);
                a1 = -(a1 + a1);
                a2 =       (Ap1 + Am1c - alpha_shelf);
                b0 =  (A * (Ap1 - Am1c + alpha_shelf));
                b1 =  (A * (Am1 - Ap1c));
                b1 =  (b1 + b1);
                b2 =  (A * (Ap1 - Am1c - alpha_shelf));

                samples[i] = morphfilter_biquad(samples[i], a0, a1, a2, b0, b1, b2, &x1, &x2, &y1, &y2);
            }
            break;

        case HI_SHELF:

            sqrtA = sqrt(A);
            gain_mul = sqrt(gain_mul);

            for (i = 0; i < length; i++, sqrtA *= gain_mul)
            {
                alpha = sinw / (2.0 * qs);
                alpha_shelf = 2.0 * sqrtA * alpha;

                A = sqrtA * sqrtA;
                Ap1 = A + 1.0;
                Am1 = A - 1.0;
                Ap1c = Ap1 * cosw;
                Am1c = Am1 * cosw;

                a0 =        Ap1 - Am1c + alpha_shelf;
                a1 =       (Am1 - Ap1c);
                a1 =  (a1 + a1);
                a2 =       (Ap1 - Am1c - alpha_shelf);
                b0 =  (A * (Ap1 + Am1c + alpha_shelf));
                b1 =  (A * (Am1 + Ap1c));
                b1 = -(b1 + b1);
                b2 =  (A * (Ap1 + Am1c - alpha_shelf));

                samples[i] = morphfilter_biquad(samples[i], a0, a1, a2, b0, b1, b2, &x1, &x2, &y1, &y2);
            }
            break;

        case PEAKING:

            for (i = 0; i < length; i++, A *= gain_mul)
            {
                alpha = sinw / (2.0 * qs);
                alpha_mul_A = alpha * A;
                alpha_div_A = alpha / A;

                a0 =       1.0 + alpha_div_A;
                a1 = b1 = (-2.0 * cosw);
                a2 =      (1.0 - alpha_div_A);
                b0 =      (1.0 + alpha_mul_A);
                b2 =      (1.0 - alpha_mul_A);

                samples[i] = morphfilter_biquad(samples[i], a0, a1, a2, b0, b1, b2, &x1, &x2, &y1, &y2);
            }
            break;
    }
}


// Arguments are - target buffer / source buffer (can alias)

void morphfilter_process(t_morphfilter *x, t_symbol *target, t_symbol *source)
{
    t_atom args[2];

    atom_setsym(args + 0, target);
    atom_setsym(args + 1, source);

    defer(x, (method) morphfilter_process_internal, 0, 2, args);
}


void morphfilter_process_internal(t_morphfilter *x, t_symbol *sym, short argc, t_atom *argv)
{
    t_symbol *target = atom_getsym(argv++);
    t_symbol *source = atom_getsym(argv++);

    intptr_t length = buffer_length(source);
    intptr_t full_length = length;
    intptr_t i;

    t_atom_long read_chan = x->read_chan - 1;
    double sample_rate = 0;

    // Check source buffer

    if (buffer_check((t_object *) x, source))
        return;
    sample_rate = buffer_sample_rate(source);

    // Allocate Memory

    temp_ptr<float> temp1(length);
    temp_ptr<double> temp2(length);

    // Check momory allocation

    if (!temp1 || !temp2)
    {
        object_error((t_object *)x, "could not allocate temporary memory for processing");
        return;
    }

    // Read from buffer

    buffer_read(source, read_chan, temp1.get(), length);

    // Copy to double precision version

    for (i = 0; i < full_length; i++)
         temp2[i] = temp1[i];

    // Do filtering

    for (i = 0; i < x->nfilters; i++)
        morphfilter_apply_filter(temp2.get(), x->filter + i, sample_rate, length);

    // Copy out to buffer

    auto error = buffer_write((t_object *)x, target, temp2.get(), length, x->write_chan - 1, x->resize, sample_rate, 1.);

    if (!error)
        outlet_bang(x->process_done);
}

