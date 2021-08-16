
#ifndef __HIRT_COMMON_ATTRIBUTE_SETUP__
#define __HIRT_COMMON_ATTRIBUTE_SETUP__

// Phase Parsing and Retrieval

#if defined OBJ_USES_HIRT_OUT_PHASE_ATTR || defined OBJ_USES_HIRT_DECONVOLUTION_ATTR
t_atom phase_parser(t_atom a)
{
    if (atom_gettype(&a) == A_FLOAT || atom_gettype(&a) == A_LONG)
    {
            double phase = atom_getfloat(&a);
            phase = phase < 0.0 ? 0.0 : phase;
            phase = phase < 0.0 ? 0.0 : phase;

            atom_setfloat(&a, phase);
    }
    else
    {
        t_symbol *sym = atom_getsym(&a);

        if (sym != gensym("lin") && sym != gensym("linear") && sym != gensym("min") && sym != gensym("minimum") && sym != gensym("max") && sym != gensym("maximum"))
            atom_setsym(&a, gensym("minimum"));

    }

    return a;
}

double phase_retriever(t_atom a)
{
    t_symbol *sym = atom_getsym(&a);

    if (atom_gettype(&a) == A_FLOAT || atom_gettype(&a) == A_LONG)
        return atom_getfloat(&a);

    if (sym == gensym("lin") || sym == gensym("linear"))
        return 0.5;

    if (sym == gensym("min") || sym == gensym("minimum"))
        return 0.0;

    if (sym == gensym("max") || sym == gensym("maximum"))
        return 1.0;

    return 0;
}
#endif

// Fill a Power Array Specifier from an Array of t_atom Specifiers (single db val, or freq/db pairs)

void fill_power_array_specifier(double *array, t_atom *specifier, long num_specifiers)
{
    long i;

    num_specifiers = num_specifiers > HIRT_MAX_SPECIFIER_ITEMS ? HIRT_MAX_SPECIFIER_ITEMS : num_specifiers;

    if (atom_gettype(specifier) == A_SYM)
    {
        array[0] = -100.;
        array[1] = HUGE_VAL;
        return;
    }

    for (i = 0; i < num_specifiers; i++)
        array[i] = atom_getfloat(specifier + i);

    if (num_specifiers < HIRT_MAX_SPECIFIER_ITEMS)
        array[i] = HUGE_VAL;
}

// Deconvolution Attribute Getters / Setters / Retrieval

#if defined OBJ_USES_HIRT_DECONVOLUTION_ATTR && !defined OBJ_DOES_NOT_USE_HIRT_DECONVOLUTION_DELAY
t_max_err deconvolve_delay_setter(OBJ_CLASSNAME *x, t_object *attr, long argc, t_atom *argv)
{
    if (argc && argv && (atom_gettype(argv) == A_FLOAT || atom_gettype(argv) == A_LONG))
        x->deconvolve_delay = *argv;
    else
        atom_setsym(&x->deconvolve_delay, gensym("center"));

    if (argc && argv && atom_gettype(argv) == A_SYM && atom_getsym(argv) != gensym("center"))
        object_error((t_object *) x, "unknown symbol for for deconvolution delay (using center)");

    return MAX_ERR_NONE;
}
#endif

#ifdef OBJ_USES_HIRT_DECONVOLUTION_ATTR
t_max_err deconvolve_phase_setter(OBJ_CLASSNAME *x, t_object *attr, long argc, t_atom *argv)
{
    if (argc && argv)
        x->deconvolve_phase = phase_parser(*argv);
    else
        atom_setsym(&x->deconvolve_phase, gensym("minimum"));

    return MAX_ERR_NONE;
}

t_max_err filter_specification_setter(OBJ_CLASSNAME *x, t_object *attr, long argc, t_atom *argv)
{
    long i;

    if (!attr)
    {
        atom_setlong(x->deconvolve_filter_specifier + 0, 20);
        atom_setlong(x->deconvolve_filter_specifier + 1, -20);
        atom_setlong(x->deconvolve_filter_specifier + 2, 30);
        atom_setlong(x->deconvolve_filter_specifier + 3, -100);
        atom_setlong(x->deconvolve_filter_specifier + 4, 19000);
        atom_setlong(x->deconvolve_filter_specifier + 5, -100);
        atom_setlong(x->deconvolve_filter_specifier + 6, 22050);
        atom_setlong(x->deconvolve_filter_specifier + 7, -20);

        x->deconvolve_num_filter_specifiers = 8;

        return MAX_ERR_NONE;
    }

    if (argc && argv)
    {
        if (argc > 1 && argc & 1)
        {
            object_error((t_object *) x, "frequency without level found in filter specification list - removing last value");
            argc--;
        }

        for (i = 0; i < argc; i++, argv++)
        {
            long type = atom_gettype(argv);

            if (type == A_LONG || type == A_FLOAT || !i)
                x->deconvolve_filter_specifier[i] = *argv;
            else
            {
                atom_setlong(x->deconvolve_filter_specifier + i, 0);
                object_error((t_object *) x, "symbol found in filter specification list - set to 0");
            }
        }

        x->deconvolve_num_filter_specifiers = argc;
    }
    else
    {
        atom_setlong(x->deconvolve_filter_specifier, -100);
        x->deconvolve_num_filter_specifiers = 1;
    }

    return MAX_ERR_NONE;
}

t_max_err filter_specification_getter(OBJ_CLASSNAME *x, t_object *attr, long *argc, t_atom **argv)
{
    char alloc;
    long i;

    atom_alloc_array (x->deconvolve_num_filter_specifiers, argc, argv, &alloc);

    for (i = 0; i < x->deconvolve_num_filter_specifiers; i++)
        (*argv)[i] = x->deconvolve_filter_specifier[i];

    return MAX_ERR_NONE;
}

t_max_err range_specification_setter(OBJ_CLASSNAME *x, t_object *attr, long argc, t_atom *argv)
{
    long i;

    if (argc && argv)
    {
        if (argc > 1 && argc & 1)
        {
            object_error((t_object *) x, "frequency without level found in range specification list - removing last value");
            argc--;
        }

        for (i = 0; i < argc; i++, argv++)
        {
            long type = atom_gettype(argv);

            if (type == A_LONG || type == A_FLOAT || !i)
                x->deconvolve_range_specifier[i] = *argv;
            else
            {
                atom_setlong(x->deconvolve_range_specifier + i, 0);
                object_error((t_object *) x, "symbol found in range specification list - set to 0");
            }
        }

        x->deconvolve_num_range_specifiers = argc;
    }
    else
    {
        atom_setlong(x->deconvolve_range_specifier, 1000);
        x->deconvolve_num_range_specifiers = 1;
    }

    return MAX_ERR_NONE;
}

t_max_err range_specification_getter(OBJ_CLASSNAME *x, t_object *attr, long *argc, t_atom **argv)
{
    char alloc;
    long i;

    atom_alloc_array (x->deconvolve_num_range_specifiers, argc, argv, &alloc);

    for (i = 0; i < x->deconvolve_num_range_specifiers; i++)
        (*argv)[i] = x->deconvolve_range_specifier[i];

    return MAX_ERR_NONE;
}

double delay_retriever(t_atom delay, uintptr_t fft_size, double sample_rate)
{
    return (atom_gettype(&delay) == A_SYM) ? (fft_size >> 1) : (atom_getfloat(&delay) * sample_rate) / 1000.0;
}

t_symbol *filter_retriever(t_atom *specifier)
{
    if (atom_gettype(specifier) == A_SYM)
        return atom_getsym(specifier);
    else
        return 0;
}
#endif

// Output Phase Attribute Setter

#ifdef OBJ_USES_HIRT_OUT_PHASE_ATTR
t_max_err phase_setter(OBJ_CLASSNAME *x, t_object *attr, long argc, t_atom *argv)
{
    if (argc && argv)
        x->out_phase = phase_parser(*argv);
    else
        atom_setsym(&x->out_phase, gensym("minimum"));

    return MAX_ERR_NONE;
}
#endif

// Amp Specifications

#ifdef OBJ_USES_HIRT_SWEEP_AMP_CURVE_ATTR
t_max_err amp_curve_specification_setter(OBJ_CLASSNAME *x, t_object *attr, long argc, t_atom *argv)
{
    long i;

    if (argc && argv)
    {
        if (argc > 1 && argc & 1)
        {
            object_error((t_object *) x, "frequency without level found in amp curve specification list - removing last value");
            argc--;
        }

        for (i = 0; i < argc; i++, argv++)
        {
            long type = atom_gettype(argv);

            if (type == A_LONG || type == A_FLOAT || !i)
                x->amp_curve_specifier[i] = *argv;
            else
            {
                atom_setlong(x->amp_curve_specifier + i, 0);
                object_error((t_object *) x, "symbol found in amp curve specification list - set to 0");
            }
        }

        x->amp_curve_num_specifiers = argc;
    }
    else
    {
        x->amp_curve_num_specifiers = 0;
    }

    return MAX_ERR_NONE;
}

t_max_err amp_curve_specification_getter(OBJ_CLASSNAME *x, t_object *attr, long *argc, t_atom **argv)
{
    char alloc;
    long i;

    atom_alloc_array (x->amp_curve_num_specifiers, argc, argv, &alloc);

    for (i = 0; i < x->amp_curve_num_specifiers; i++)
        (*argv)[i] = x->amp_curve_specifier[i];

    return MAX_ERR_NONE;
}

void fill_amp_curve_specifier(double *array, t_atom *specifier, long num_specifiers)
{
    long i;

    for (i = 0; i < num_specifiers; i++)
        array[i] = atom_getfloat(specifier + i);

    array[i] = HUGE_VAL;
}
#endif

// Declare Common Attributes

static inline void declare_HIRT_common_attributes(t_class *c)
{
    CLASS_STICKY_ATTR(c, "category", 0L, "Buffer");

#ifdef OBJ_USES_HIRT_WRITE_ATTR
    CLASS_ATTR_ATOM_LONG(c, "writechan", 0L, OBJ_CLASSNAME, write_chan);
    CLASS_ATTR_FILTER_MIN(c, "writechan", 1);
    CLASS_ATTR_LABEL(c,"writechan", 0L, "Buffer Write Channel");

    CLASS_ATTR_LONG(c, "resize", 0L, OBJ_CLASSNAME, resize);
    CLASS_ATTR_STYLE_LABEL(c,"resize", 0L,"onoff","Buffer Resize");
#endif

#ifdef OBJ_USES_HIRT_READ_ATTR
    CLASS_ATTR_ATOM_LONG(c, "readchan", 0L, OBJ_CLASSNAME, read_chan);
    CLASS_ATTR_FILTER_MIN(c, "readchan", 1);
    CLASS_ATTR_LABEL(c,"readchan", 0L, "Buffer Read Channel");
#endif

    CLASS_STICKY_ATTR_CLEAR(c, "category");

#ifdef OBJ_USES_HIRT_OUT_PHASE_ATTR
    CLASS_ATTR_ATOM(c, "phase", 0L, OBJ_CLASSNAME, out_phase);
    CLASS_ATTR_ACCESSORS(c, "phase", 0L, (method) phase_setter);
    CLASS_ATTR_LABEL(c,"phase", 0L, "Output Phase");
#endif

#ifdef OBJ_USES_HIRT_SMOOTH_ATTR
    CLASS_STICKY_ATTR(c, "category", 0L, "Smoothing");

    CLASS_ATTR_LONG(c, "smoothmode", 0L, OBJ_CLASSNAME, smooth_mode);
    CLASS_ATTR_ENUMINDEX(c,"smoothmode", 0L, "Hi-Quality Fast");
    CLASS_ATTR_FILTER_CLIP(c, "smoothmode", 0, 3);
    CLASS_ATTR_LABEL(c,"smoothmode", 0L, "Smoothing Mode");

    CLASS_ATTR_DOUBLE_VARSIZE(c, "smoothamount", 0, OBJ_CLASSNAME, smooth, num_smooth, 2);
    CLASS_ATTR_FILTER_CLIP(c, "smoothamount", 0, 1);
    CLASS_ATTR_LABEL(c,"smoothamount", 0L, "Smoothing Amounts");

    CLASS_STICKY_ATTR_CLEAR(c, "category");
#endif

#ifdef OBJ_USES_HIRT_DECONVOLUTION_ATTR
    CLASS_STICKY_ATTR(c, "category", 0, "Deconvolution");
#endif

#if defined OBJ_USES_HIRT_DECONVOLUTION_ATTR && !defined OBJ_DOES_NOT_USE_HIRT_DECONVOLUTION_DELAY
    CLASS_ATTR_ATOM(c, "deconvdelay", 0, OBJ_CLASSNAME, deconvolve_delay);
    CLASS_ATTR_ACCESSORS(c, "deconvdelay", 0, (method) deconvolve_delay_setter);
    CLASS_ATTR_LABEL(c,"deconvdelay",0, "Deconvolution Modelling Delay");
#endif

#ifdef OBJ_USES_HIRT_DECONVOLUTION_ATTR
    CLASS_ATTR_ATOM(c, "deconvphase", 0, OBJ_CLASSNAME, deconvolve_phase);
    CLASS_ATTR_ACCESSORS(c, "deconvphase", 0, (method) deconvolve_phase_setter);
    CLASS_ATTR_LABEL(c,"deconvphase", 0, "Deconvolution Filter Phase");

    CLASS_ATTR_ATOM_VARSIZE(c, "deconvfilter", 0, OBJ_CLASSNAME, deconvolve_filter_specifier, deconvolve_num_filter_specifiers, HIRT_MAX_SPECIFIER_ITEMS);
    CLASS_ATTR_ACCESSORS(c, "deconvfilter", (method) filter_specification_getter, (method) filter_specification_setter);
    CLASS_ATTR_LABEL(c, "deconvfilter", 0 , "Deconvolution Filter Specification");

    CLASS_ATTR_ATOM_VARSIZE(c, "deconvrange", 0, OBJ_CLASSNAME, deconvolve_range_specifier, deconvolve_num_range_specifiers, HIRT_MAX_SPECIFIER_ITEMS);
    CLASS_ATTR_ACCESSORS(c, "deconvrange", (method) range_specification_getter, (method) range_specification_setter);
    CLASS_ATTR_LABEL(c, "deconvrange", 0 , "Deconvolution Filter Range");

    CLASS_ATTR_LONG(c, "deconvmode", 0, OBJ_CLASSNAME, deconvolve_mode);
    CLASS_ATTR_ENUMINDEX(c,"deconvmode", 0, "Regularisation Clipping Filter");
    CLASS_ATTR_FILTER_CLIP(c, "deconvmode", 0, 2);
    CLASS_ATTR_LABEL(c,"deconvmode", 0, "Deconvolution Filter Mode");

    CLASS_STICKY_ATTR_CLEAR(c, "category");
#endif

#ifdef OBJ_USES_HIRT_SWEEP_AMP_CURVE_ATTR
    CLASS_ATTR_ATOM_VARSIZE(c, "ampcurve", 0, OBJ_CLASSNAME, amp_curve_specifier, amp_curve_num_specifiers, 32);
    CLASS_ATTR_ACCESSORS(c, "ampcurve", (method) amp_curve_specification_getter, (method) amp_curve_specification_setter);
    CLASS_ATTR_LABEL(c, "ampcurve", 0 , "Sweep Amplitude Curve Specification");
#endif
}

// Init Common Attributes

static inline long init_HIRT_common_attributes(OBJ_CLASSNAME *x)
{
#ifdef OBJ_USES_HIRT_WRITE_ATTR
    x->write_chan = 1;
    x->resize = 1;
#endif

#ifdef OBJ_USES_HIRT_READ_ATTR
    x->read_chan = 1;
#endif

#ifdef OBJ_USES_HIRT_DECONVOLUTION_ATTR
    x->deconvolve_mode = 0;

    x->deconvolve_filter_specifier = (t_atom *) malloc(sizeof(t_atom) * HIRT_MAX_SPECIFIER_ITEMS);
    x->deconvolve_range_specifier = (t_atom *) malloc(sizeof(t_atom) * HIRT_MAX_SPECIFIER_ITEMS);

    if (!x->deconvolve_filter_specifier || !x->deconvolve_range_specifier)
    {
        free(x->deconvolve_filter_specifier);
        free(x->deconvolve_range_specifier);

        x->deconvolve_filter_specifier = nullptr;
        x->deconvolve_range_specifier = nullptr;

        object_error((t_object *) x, "could not allocate space for attribute storage");

        return 1;
    }

    deconvolve_phase_setter(x, 0, 0, 0);
    filter_specification_setter(x, 0, 0, 0);
    range_specification_setter(x, 0, 0, 0);
#endif

#if defined OBJ_USES_HIRT_DECONVOLUTION_ATTR && !defined OBJ_DOES_NOT_USE_HIRT_DECONVOLUTION_DELAY
    deconvolve_delay_setter(x, 0, 0, 0);
#endif

#ifdef OBJ_USES_HIRT_OUT_PHASE_ATTR
    phase_setter(x, 0, 0, 0);
#endif

#ifdef OBJ_USES_HIRT_SMOOTH_ATTR
    x->num_smooth = 0;
    x->smooth_mode = 0;
#endif

    return 0;
}

// Free Common Attributes

static inline void free_HIRT_common_attributes(OBJ_CLASSNAME *x)
{
#ifdef OBJ_USES_HIRT_DECONVOLUTION_ATTR
    free(x->deconvolve_filter_specifier);
    free(x->deconvolve_range_specifier);
#endif
}

#endif  /* __HIRT_COMMON_ATTRIBUTE_SETUP__ */
