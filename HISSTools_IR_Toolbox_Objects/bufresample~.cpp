
#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <HIRT_Core_Functions.hpp>
#include <HIRT_Buffer_Access.hpp>
#include <AH_Memory_Swap.h>

// Define common attributes and the class name (for the common attributes file)

#define OBJ_CLASSNAME t_bufresample
#define OBJ_USES_HIRT_WRITE_ATTR
#define OBJ_USES_HIRT_READ_ATTR

#include <HIRT_Common_Attribute_Vars.hpp>

// Object class and structure

t_class *this_class;


struct t_bufresample
{
    t_pxobject x_obj;

    t_safe_mem_swap filter;

    // Attributes

    HIRT_COMMON_ATTR

    // Bang Outlet

    void *process_done;
};


// This include deals with setup of common attributes - requires the object structure to be defined

#include <HIRT_Common_Attribute_Setup.hpp>


// Function prototypes

void *bufresample_new();
void bufresample_free(t_bufresample *x);
void bufresample_assist(t_bufresample *x, void *b, long m, long a, char *s);

void bufresample_process(t_bufresample *x, t_symbol *sym, long argc, t_atom *argv);
void bufresample_process_internal(t_bufresample *x, t_symbol *sym, short argc, t_atom *argv);

double *generate_filter(t_bufresample *x, long nzero, long npoints, double cf, double alpha);
void bufresample_set_filter(t_bufresample *x, t_symbol *sym, long argc, t_atom *argv);


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int C74_EXPORT main()
{
    this_class = class_new("bufresample~",
                          (method) bufresample_new,
                          (method)bufresample_free,
                          sizeof(t_bufresample),
                          0L,
                          0);

    class_addmethod(this_class, (method)bufresample_process, "both", A_GIMME, 0L);
    class_addmethod(this_class, (method)bufresample_process, "resample", A_GIMME, 0L);
    class_addmethod(this_class, (method)bufresample_process, "transpose", A_GIMME, 0L);
    class_addmethod(this_class, (method)bufresample_set_filter, "filter", A_GIMME, 0L);
    class_addmethod(this_class, (method)bufresample_assist, "assist", A_CANT, 0L);

    declare_HIRT_common_attributes(this_class);

    class_register(CLASS_BOX, this_class);

    return 0;
}


void *bufresample_new()
{
    t_bufresample *x = (t_bufresample *)object_alloc (this_class);

    x->process_done = bangout(x);

    alloc_mem_swap(&x->filter, 0, 0);
    generate_filter(x, 10, 16384, 0.455, 11);
    init_HIRT_common_attributes(x);


    return x;
}


void bufresample_free(t_bufresample *x)
{
    free_mem_swap(&x->filter);
}


void bufresample_assist(t_bufresample *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET)
        sprintf(s,"Instructions In");
    else
        sprintf(s,"Bang on Success");
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Make filter

double IZero(double x_sq)
{
    double new_term = 1;
    double b_func = 1;

    long i;

    for (i = 1; new_term > DBL_EPSILON; i++)  // Gives Maximum Accuracy With Speed!
    {
        new_term = new_term * x_sq * (1.0 / (4.0 * (double) i * (double) i));
        b_func += new_term;
    }

    return b_func;
}


double *generate_filter(t_bufresample *x, long nzero, long npoints, double cf, double alpha)
{
    double *filter;

    double alpha_bessel_recip;
    double sinc_arg;
    double val;
    double x_sq;

    long half_filter_length = nzero * npoints;
    long i;

    // Assign and check memory (N.B. + 2 points for the end point + one guard sample)

    filter = (double *) schedule_equal_mem_swap(&x->filter, (sizeof(double) * (half_filter_length + 2)), npoints | (nzero << 0x10));

    if (!filter)
        return 0;

    // First find bessel function of alpha

    alpha_bessel_recip = 1 / IZero(alpha * alpha);

    // Calculate second half of filter only

    // Limit Value

    filter[0] = 2 * cf;

    for (i = 1; i < half_filter_length + 1; i++)
    {
        // Kaiser Window

        val = ((double) i) / half_filter_length;
        x_sq = (1 - val * val) * alpha * alpha;
        val = IZero(x_sq) * alpha_bessel_recip;

        // Multiply with Sinc Function

        sinc_arg = M_PI * (double) i / npoints;
        filter[i] = (sin (2 * cf * sinc_arg) / sinc_arg) * val;
    }

    // Guard sample for linear interpolation

    filter[half_filter_length + 1] = 0;

    return filter;
}


// Arguments are - number of zero-crossings / number of points / centre frequency / alpha

void bufresample_set_filter(t_bufresample *x, t_symbol *sym, long argc, t_atom *argv)
{
    t_atom_long nzero = (argc > 0) ? atom_getlong(argv + 0): 10;
    t_atom_long npoints = (argc > 1) ? atom_getlong(argv + 1): 16384;
    double cf = (argc > 2) ? atom_getfloat(argv + 2) : 0.455;
    double alpha = (argc > 3) ? atom_getfloat(argv + 3) : 11.0;

    nzero = (nzero < 1) ? 1 : nzero;
    nzero = (nzero > 512) ? 512 : nzero;

    npoints = (npoints < 16) ? 16 : npoints;
    npoints = (npoints > 16384) ? 16384 : npoints;

    cf = (cf > 1.0) ? 1.0 : cf;
    cf = (cf < 0.0) ? 0.0 : cf;

    alpha = (alpha <= 0.) ? 1.0 : alpha;

    generate_filter(x, (long) nzero, (long) npoints, cf, alpha);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void rate_as_ratio(double rate, AH_SIntPtr *ret_num, AH_SIntPtr *ret_denom)
{
    AH_SIntPtr Cf[256];

    double npart =fabs(rate);
    double ipart;

    AH_SIntPtr num = 1;
    AH_SIntPtr denom = 1;
    AH_SIntPtr swap;

    short length = 0;
    short i, j;

    for (; npart < 1000 && npart > 0 && length < 256; length++)
    {
        ipart = floor(npart);
        npart = npart - ipart;
        npart = npart ? 1 / npart : 0;

        Cf[length] = (AH_SIntPtr) ipart;
    }

    for (i = length - 1; i >= 0; i--)
    {
        num = Cf[i];
        denom = 1;

        for (j = i - 1; j >= 0 && denom < 1000; j--)
        {
            swap = num;
            num = denom;
            denom = swap;
            num = num + (denom * Cf[j]);
        }

        if (denom < 1000)
            break;
    }

    *ret_num = num;
    *ret_denom = denom;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Get a filter value from a position 0-nzero on the RHS wing (translate for LHS) - filter_position **MUST** be in range 0 to nzero inclusive

double get_filter_value_direct(double *filter, long npoints, double filter_position)
{
    double index;
    double fract;
    double lo;
    double hi;

    long idx;

    index = npoints * filter_position;
    idx = (long) index;
    fract = index - idx;

    lo = filter[idx];
    hi = filter[idx + 1];

    return lo + fract * (hi - lo);
}


double *bufresample_calc_temp_filters(double *filter, long nzero, long npoints, AH_SIntPtr num, AH_SIntPtr denom, AH_SIntPtr *ret_max_filter_length, AH_SIntPtr *ret_filter_offset)
{
    double per_samp = num > denom ? (double) denom / (double) num : (double) 1;
    double one_over_per_samp = num > denom ? nzero * (double) num / (double) denom : nzero;
    double filter_position;
    double mul = num > denom ? (double) denom / (double) num : 1.;

    AH_SIntPtr max_filter_length = (AH_SIntPtr) (one_over_per_samp + one_over_per_samp + 1);
    AH_SIntPtr filter_offset = max_filter_length >> 1;
    AH_SIntPtr current_num;
    AH_SIntPtr i, j;

    double *temp_filters;
    double *current_filter;

    max_filter_length += (4 - (max_filter_length % 4));

    temp_filters = allocate_aligned<double>(denom * max_filter_length);
    current_filter = temp_filters;

    if (!temp_filters)
        return 0;

    for (i = 0, current_num = 0; i < denom; i++, current_filter += max_filter_length, current_num += num)
    {
        for (j = 0; j < max_filter_length; j++)
        {
            while (current_num >= denom)
                current_num -= denom;
            filter_position = fabs(per_samp * (j - (double) current_num / (double) denom - filter_offset));
            current_filter[j] = filter_position <= nzero ? mul * get_filter_value_direct(filter, npoints, filter_position) : 0;
        }
    }

    *ret_max_filter_length = max_filter_length;
    *ret_filter_offset = filter_offset;

    return temp_filters;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Get samples safely from the buffer (buffer should already be inuse)

void get_buffer_samples(ibuffer_data& bufdata, float *samples, AH_SIntPtr offset, AH_SIntPtr nsamps, long chan)
{
    AH_SIntPtr temp_offset = 0;
    AH_SIntPtr temp_nsamps = nsamps;
    AH_SIntPtr i;

    // Do not read before the buffer

    if (offset < 0)
    {
        temp_offset = -offset;
        temp_nsamps -= temp_offset;
        offset = 0;
    }

    if (temp_nsamps < 0)
    {
        temp_nsamps = 0;
        temp_offset = nsamps;
    }

    // Do not read beyond the buffer

    if (offset + temp_nsamps > bufdata.get_length())
        temp_nsamps = bufdata.get_length() - offset;

    if (temp_nsamps < 0)
        temp_nsamps = 0;

    for (i = 0; i < temp_offset; i++)
        samples[i] = 0;

    if (temp_nsamps)
        ibuffer_get_samps(bufdata, samples + temp_offset, offset, temp_nsamps, chan);
    
    for (i = temp_offset + temp_nsamps; i < nsamps; i++)
        samples[i] = 0;
}


void get_buffer_samples_local(ibuffer_data& bufdata, float *buf_samps, float *samples, AH_SIntPtr offset, AH_SIntPtr nsamps)
{
    AH_SIntPtr temp_offset = 0;
    AH_SIntPtr temp_nsamps = nsamps;
    AH_SIntPtr i;

    // Do not read before the buffer

    if (offset < 0)
    {
        temp_offset = -offset;
        temp_nsamps -= temp_offset;
        offset = 0;
    }

    if (temp_nsamps < 0)
    {
        temp_nsamps = 0;
        temp_offset = nsamps;
    }

    // Do not read beyond the buffer

    if (offset + temp_nsamps > bufdata.get_length())
        temp_nsamps = bufdata.get_length() - offset;

    if (temp_nsamps < 0)
        temp_nsamps = 0;

    for (i = 0; i < temp_offset; i++)
        samples[i] = 0;

    for (i = 0; i < temp_nsamps; i++)
        samples[i + temp_offset] = buf_samps[i + offset];

    for (i = temp_offset + temp_nsamps; i < nsamps; i++)
        samples[i] = 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


double sum_filter_mul_double(double *a, float *b, AH_SIntPtr N)
{
    double Sum = 0.;
    AH_SIntPtr i;

    for (i = 0; i + 3 < N; i += 4)
    {
        Sum += a[i+0] * b[i+0];
        Sum += a[i+1] * b[i+1];
        Sum += a[i+2] * b[i+2];
        Sum += a[i+3] * b[i+3];
    }
    for (; i < N; i++)
        Sum += a[i] * b[i];

    return Sum;
}

#ifdef TARGET_INTEL
static inline double sum_filter_mul_vector(vDouble *a, float *b, AH_SIntPtr N)
{
    vDouble Sum = {0., 0.};
    vFloat Temp;
    double results[2];
    AH_SIntPtr i;

    for (i = 0; i + 3 < (N >> 1); i += 4)
    {
        Temp = F32_VEC_ULOAD(b + 2 * i);
        Sum = F64_VEC_ADD_OP(Sum, F64_VEC_MUL_OP(a[i+0], F64_VEC_FROM_F32(Temp)));
        Sum = F64_VEC_ADD_OP(Sum, F64_VEC_MUL_OP(a[i+1], F64_VEC_FROM_F32(F32_VEC_SHUFFLE(Temp, Temp, 0x4E))));

        Temp = F32_VEC_ULOAD(b + 2 * i + 4);
        Sum = F64_VEC_ADD_OP(Sum, F64_VEC_MUL_OP(a[i+2], F64_VEC_FROM_F32(Temp)));
        Sum = F64_VEC_ADD_OP(Sum, F64_VEC_MUL_OP(a[i+3], F64_VEC_FROM_F32(F32_VEC_SHUFFLE(Temp, Temp, 0x4E))));
    }
    for (; i + 1 < N >> 1; i += 2)
    {
        Temp = F32_VEC_ULOAD(b + 2 * i);
        Sum = F64_VEC_ADD_OP(Sum, F64_VEC_MUL_OP(a[i+0], F64_VEC_FROM_F32(Temp)));
        Sum = F64_VEC_ADD_OP(Sum, F64_VEC_MUL_OP(a[i+1], F64_VEC_FROM_F32(F32_VEC_SHUFFLE(Temp, Temp, 0x4E))));
    }

    F64_VEC_USTORE(results, Sum);

    return results[0] + results[1];
}
#endif


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


double *resample_fixed_ratio(ibuffer_data& bufdata, double *filter, long nzero, long npoints, AH_SIntPtr nsamps, AH_SIntPtr num, AH_SIntPtr denom, long chan)
{
    double *current_filter;

    AH_SIntPtr current_offset;
    AH_SIntPtr buf_offset;
    AH_SIntPtr filter_offset;
    AH_SIntPtr max_filter_length;
    AH_SIntPtr first;
    AH_SIntPtr second;
    AH_SIntPtr i, j, k;

    double sum;

    // Create the relevant filters

    double *temp_filters = bufresample_calc_temp_filters(filter, nzero, npoints, num, denom, &max_filter_length, &filter_offset);

    // Allocate memory

    float *temp = allocate_aligned<float>(max_filter_length + 4);
    double *output = (double *) malloc(nsamps * sizeof(double));
    float *buf_temp = allocate_aligned<float>(bufdata.get_length());

    // Check memory

    if (!temp || !output || !temp_filters || !buf_temp || !filter)
    {
        deallocate_aligned(temp);
        deallocate_aligned(temp_filters);
        free(output);
        deallocate_aligned(buf_temp);
        return 0;
    }

    // Set buffer in use

    first = filter_offset / num;
    first = (filter_offset > first * num) ? (first + 1) * denom : first * denom;
    second = (bufdata.get_length() - (max_filter_length - filter_offset)) / num;
    second *= denom;

    if (second > nsamps)
        second = nsamps;
    if (first >= second)
        first = second = 0;

    ibuffer_get_samps(bufdata, buf_temp, 0, bufdata.get_length(), chan);

    // Resample

    for (i = 0, current_offset = -filter_offset; i < first; current_offset += num)
    {
        for (j = 0, current_filter = temp_filters; i < first && j < denom; i++, j++, current_filter += max_filter_length)
        {
            get_buffer_samples_local(bufdata, buf_temp, temp, current_offset + (j * num / denom), max_filter_length);

            for (k = 0, sum = 0; k < max_filter_length; k++)
                sum += temp[k] * current_filter[k];

            output[i] = sum;
        }
    }

    for (; i < second; current_offset += num)
    {
        for (j = 0, current_filter = temp_filters; i < second && j < denom; i++, j++, current_filter += max_filter_length)
        {
            buf_offset = current_offset + ((j * num) / denom);
#ifdef TARGET_INTEL
            output[i] = sum_filter_mul_vector((vDouble *)current_filter, buf_temp + buf_offset, max_filter_length);
#else
            output[i] = sum_filter_mul_double(current_filter, buf_temp + buf_offset, max_filter_length);
#endif
        }
    }

    for (; i < nsamps; current_offset += num)
    {
        for (j = 0, current_filter = temp_filters; i < nsamps && j < denom; i++, j++, current_filter += max_filter_length)
        {
            get_buffer_samples_local(bufdata, buf_temp, temp, current_offset + (j * num / denom), max_filter_length);

            for (k = 0, sum = 0; k < max_filter_length; k++)
                sum += temp[k] * current_filter[k];

            output[i] = sum;
        }
    }

    // Set not in use free temp memory and return

    deallocate_aligned(buf_temp);
    deallocate_aligned(temp);
    deallocate_aligned(temp_filters);

    return output;
}


double *bufresample_copy(ibuffer_data& bufdata, AH_SIntPtr nsamps, long chan)
{
    double *output = (double *) malloc(nsamps * sizeof(double));
    float *temp = allocate_aligned<float>(nsamps + 4);
    AH_SIntPtr i;

    if (!temp || !output)
    {
        deallocate_aligned(temp);
        free(output);
        return 0;
    }

    // Special case where no resampling is necessary

    get_buffer_samples(bufdata, temp, 0, nsamps, chan);

    for (i = 0; i < nsamps; i++)
        output[i] = temp[i];

    deallocate_aligned(temp);

    return output;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Arguments are - target buffer / source buffer followed by one or two more arguments depending on the message:
//    - resample: target sample rate
//    - tranpose: transposition ratio
//    - both: transposition ratio / target sample rate


void bufresample_process(t_bufresample *x, t_symbol *sym, long argc, t_atom *argv)
{
    defer(x, (method) bufresample_process_internal, sym, (short) argc, argv);
}


void bufresample_process_internal(t_bufresample *x, t_symbol *sym, short argc, t_atom *argv)
{
    t_symbol *target;
    t_symbol *source;

    double *output = 0;
    double *filter;

    double sr_convert = 0.;
    double sample_rate = 0;
    double rate;

    AH_SIntPtr num;
    AH_SIntPtr denom;
    AH_SIntPtr nsamps;

    AH_UIntPtr nom_size;

    long npoints;
    long nzero;

    t_buffer_write_error error;

    // Check arguments

    if (argc < 3 || (sym == gensym("both") && argc < 4))
    {
        object_error((t_object *)x, "not enough arguments to message %s", sym->s_name);
        return;
    }

    target = atom_getsym(argv++);
    argc--;
    source = atom_getsym(argv++);
    argc--;
    rate = atom_getfloat(argv++);
    argc--;

    if (sym == gensym("both"))
    {
        sr_convert = atom_getfloat(argv++);
        argc--;
    }

    // Check source buffer

    long chan = x->read_chan - 1;
    if (buffer_check((t_object *) x, source))
        return;

    // Default is to transpose

    ibuffer_data bufdata(source);
    sample_rate = bufdata.get_sample_rate();

    if (sym == gensym("resample"))
    {
        sample_rate = rate;
        rate = fabs(bufdata.get_sample_rate() / rate);
    }

    if (sym == gensym("both"))
    {
        sample_rate = sr_convert;
        rate = rate * fabs(bufdata.get_sample_rate() / sr_convert);
    }

    if (rate < 0.01)
    {
        object_error((t_object *) x, "very small and negative rates not allowed (less than 0.01)");
        return;
    }

    // Resample

    nsamps = (AH_SIntPtr) ceil(bufdata.get_length() / rate);

    rate_as_ratio(rate, &num, &denom);
    nsamps = (AH_SIntPtr) (ceil(((double) denom * (double) bufdata.get_length()) / (double) num));

    filter = (double *) access_mem_swap(&x->filter, &nom_size);
    npoints = nom_size & 0x7FFF;
    nzero = (nom_size >> 0x10) & 0x3FF;

    if (rate == 1.)
        output = bufresample_copy(bufdata, nsamps, chan);
    else
        output = resample_fixed_ratio(bufdata, filter, nzero, npoints, nsamps, num, denom, chan);

    if (!output)
    {
        object_error((t_object *)x, "could not allocate memory for internal processing");
        return;
    }

    // Copy out to buffer

    error = buffer_write((t_object *)x, target, output, nsamps, x->write_chan - 1, x->resize, sample_rate, 1.);
    buffer_write_error((t_object *)x, target, error);

    // Free Resources

    free(output);

    if (!error)
        outlet_bang(x->process_done);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Get a filter value from a position 0-1 on the RHS wing (translate for LHS) - filter_position **MUST** be in range 0 to 1 inclusive

double get_filter_value(double *filter, long half_length, double filter_position)
{
    double index;
    double fract;
    double lo;
    double hi;

    long idx;

    index = half_length * filter_position;
    idx = (long) index;
    fract = index - idx;

    lo = filter[idx];
    hi = filter[idx + 1];

    return lo + fract * (hi - lo);
}

// Calculate one sample

double calc_sample(ibuffer_data& bufdata, float *samples, double *filter, long nzero, long npoints, double position, double rate, long chan)
{
    double per_samp = rate > 1. ? 1. / (rate * nzero) : 1. / nzero;
    double one_over_per_samp = rate > 1. ? nzero * rate : nzero;
    double filter_position;
    double fract;
    double sum = 0.;

    long half_length = nzero * npoints;

    AH_SIntPtr offset;
    AH_SIntPtr nsamps;
    AH_SIntPtr idx;

    idx = (AH_SIntPtr) position;
    fract = position - idx;

    // Get samples

    offset = (AH_SIntPtr) (position - one_over_per_samp);
    nsamps = (AH_SIntPtr) (one_over_per_samp + one_over_per_samp + 2);
    get_buffer_samples(bufdata, samples, offset, nsamps, chan);

    // Get to first relevant sample

    for (filter_position = (idx - offset + fract) * per_samp; filter_position > 1.; filter_position -= per_samp)
        samples++;

    // Do left wing of the filter

    for (; filter_position >= 0.; filter_position -= per_samp)
        sum += *samples++ * get_filter_value(filter, half_length, filter_position);

    // Do right wing of the filter

    for (filter_position = -filter_position; filter_position <= 1.; filter_position += per_samp)
        sum += *samples++ * get_filter_value(filter, half_length, filter_position);

    return sum;
}


// Resample given a fixed rate as a double

double *resample_fixed_rate(ibuffer_data& bufdata, double *filter, long nzero, long npoints, double offset, AH_SIntPtr nsamps, double rate, long chan)
{
    double one_over_per_samp = rate > 1. ? nzero * rate : nzero;
    double mul = rate > 1. ? 1. / rate : 1;

    AH_SIntPtr temp_length = (AH_SIntPtr) (one_over_per_samp + one_over_per_samp + 2);
    AH_SIntPtr i;

    // Allocate memory

    float *temp = allocate_aligned<float>(temp_length + 4);
    double *output = (double *) malloc(nsamps * sizeof(double));

    // Check memory

    if (!temp || !output || !filter)
    {
        deallocate_aligned(temp);
        free(output);
        return 0;
    }

    // Resample

    for (i = 0; i < nsamps; i++)
        output[i] = mul * calc_sample(bufdata, temp, filter, nzero, npoints, offset + (i * rate), rate, chan);

    // Free temp memory and return

    deallocate_aligned(temp);

    return output;
}
