
#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <algorithm>
#include <memory>

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

void generate_filter(t_bufresample *x, long nzero, long npoints, double cf, double alpha);
void bufresample_set_filter(t_bufresample *x, t_symbol *sym, long argc, t_atom *argv);


//////////////////////////////////////////////////////////////////////////
/////////////////////// Main / New / Free / Assist ///////////////////////
//////////////////////////////////////////////////////////////////////////


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
    free_HIRT_common_attributes(x);
    free_mem_swap(&x->filter);
}


void bufresample_assist(t_bufresample *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET)
        sprintf(s,"Instructions In");
    else
        sprintf(s,"Bang on Success");
}


//////////////////////////////////////////////////////////////////////////
///////////////////////////// Create Filters /////////////////////////////
//////////////////////////////////////////////////////////////////////////


// Make filter

double IZero(double x_sq)
{
    double new_term = 1;
    double b_func = 1;

    for (long i = 1; new_term > DBL_EPSILON; i++)  // Gives Maximum Accuracy With Speed!
    {
        new_term = new_term * x_sq * (1.0 / (4.0 * (double) i * (double) i));
        b_func += new_term;
    }

    return b_func;
}


// Generate Filters

void generate_filter(t_bufresample *x, long nzero, long npoints, double cf, double alpha)
{
    long half_filter_length = nzero * npoints;

    // Assign and check memory (N.B. + 2 points for the end point + one guard sample)

    double *filter = (double *) schedule_equal_mem_swap(&x->filter, (sizeof(double) * (half_filter_length + 2)), npoints | (nzero << 0x10));

    if (!filter)
        return;

    // First find bessel function of alpha

    const double alpha_bessel_recip = 1.0 / IZero(alpha * alpha);

    // Calculate second half of filter only

    // Limit Value

    filter[0] = 2 * cf;

    for (long i = 1; i < half_filter_length + 1; i++)
    {
        // Kaiser Window

        const double v1 = ((double) i) / half_filter_length;
        const double x_sq = (1 - v1 * v1) * alpha * alpha;
        const double v2 = IZero(x_sq) * alpha_bessel_recip;

        // Multiply with Sinc Function

        const double sinc_arg = M_PI * (double) i / npoints;
        filter[i] = (sin (2 * cf * sinc_arg) / sinc_arg) * v2;
    }

    // Guard sample for linear interpolation

    filter[half_filter_length + 1] = 0;
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

    generate_filter(x, static_cast<long>(nzero),  static_cast<long>(npoints), cf, alpha);
}


//////////////////////////////////////////////////////////////////////////
///////////////////////////// Rate To Ratio //////////////////////////////
//////////////////////////////////////////////////////////////////////////


void rate_as_ratio(double rate, intptr_t& num, intptr_t& denom)
{
    intptr_t Cf[256];

    double npart = std::fabs(rate);
    short length = 0;

    num = 1;
    denom = 1;

    for (; npart < 1000 && npart > 0 && length < 256; length++)
    {
        const double ipart = std::floor(npart);
        npart = npart - ipart;
        npart = npart ? 1 / npart : 0;

        Cf[length] = static_cast<intptr_t>(ipart);
    }

    for (short i = length - 1; i >= 0; i--)
    {
        num = Cf[i];
        denom = 1;

        for (short j = i - 1; j >= 0 && denom < 1000; j--)
        {
            std::swap(num, denom);
            num = num + (denom * Cf[j]);
        }

        if (denom < 1000)
            break;
    }
}


//////////////////////////////////////////////////////////////////////////
///////////////////////////// Filter Values //////////////////////////////
//////////////////////////////////////////////////////////////////////////




// Get a filter value from a position 0-nzero on the RHS wing (translate for LHS) - EITHER
// - filter_position *MUST* be in range 0 to nzero inclusive (passing nzero as mul) OR
// - filter_position *MUST* be in range 0 to 1 inclusive (passing nzero * npoints / half length as mul)

double get_filter_value(double *filter, long mul, double filter_position)
{
    const double index = mul * filter_position;
    const long idx = static_cast<long>(index);
    const double fract = index - idx;
    
    const double lo = filter[idx];
    const double hi = filter[idx + 1];
    
    return lo + fract * (hi - lo);
}


temp_ptr<double>&& bufresample_calc_temp_filters(double *filter, long nzero, long npoints, intptr_t num, intptr_t denom, intptr_t& max_filter_length, intptr_t& filter_offset)
{
    double per_samp = num > denom ? (double) denom / (double) num : (double) 1;
    double one_over_per_samp = num > denom ? nzero * (double) num / (double) denom : nzero;
    double filter_position;
    double mul = num > denom ? (double) denom / (double) num : 1.;

    max_filter_length = static_cast<intptr_t>(one_over_per_samp + one_over_per_samp + 1);
    filter_offset = max_filter_length >> 1;
    max_filter_length += (4 - (max_filter_length % 4));
    
    temp_ptr<double> temp_filters(denom * max_filter_length);
    double *current_filter = temp_filters.get();

    if (!temp_filters)
        return std::move(temp_ptr<double>(0));

    for (intptr_t i = 0, current_num = 0; i < denom; i++, current_filter += max_filter_length, current_num += num)
    {
        for (intptr_t j = 0; j < max_filter_length; j++)
        {
            while (current_num >= denom)
                current_num -= denom;
            filter_position = std::fabs(per_samp * (j - (double) current_num / (double) denom - filter_offset));
            current_filter[j] = filter_position <= nzero ? mul * get_filter_value(filter, npoints, filter_position) : 0;
        }
    }

    return std::move(temp_filters);
}


//////////////////////////////////////////////////////////////////////////
//////////////////////// Get Samples From a Buffer ///////////////////////
//////////////////////////////////////////////////////////////////////////


// Get samples safely from the buffer (buffer should already be inuse)

void get_buffer_samples(ibuffer_data& bufdata, float *samples, intptr_t offset, intptr_t nsamps, long chan)
{
    intptr_t temp_offset = 0;
    intptr_t temp_nsamps = nsamps;

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

    for (intptr_t i = 0; i < temp_offset; i++)
        samples[i] = 0;

    if (temp_nsamps)
        ibuffer_get_samps(bufdata, samples + temp_offset, offset, temp_nsamps, chan);
    
    for (intptr_t i = temp_offset + temp_nsamps; i < nsamps; i++)
        samples[i] = 0;
}


void get_buffer_samples_local(ibuffer_data& bufdata, float *buf_samps, float *samples, intptr_t offset, intptr_t nsamps)
{
    intptr_t temp_offset = 0;
    intptr_t temp_nsamps = nsamps;

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

    for (intptr_t i = 0; i < temp_offset; i++)
        samples[i] = 0;

    for (intptr_t i = 0; i < temp_nsamps; i++)
        samples[i + temp_offset] = buf_samps[i + offset];

    for (intptr_t i = temp_offset + temp_nsamps; i < nsamps; i++)
        samples[i] = 0;
}


//////////////////////////////////////////////////////////////////////////
///////////////////////////// Convolve Filter ////////////////////////////
//////////////////////////////////////////////////////////////////////////



double sum_filter_mul(double *a, float *b, intptr_t N)
{
    double Sum = 0.;
    intptr_t i;

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
static inline double sum_filter_mul(vDouble *a, float *b, intptr_t N)
{
    vDouble Sum = {0., 0.};
    vFloat Temp;
    double results[2];
    intptr_t i;

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

//////////////////////////////////////////////////////////////////////////
////////////////////////// Fast Resample or Copy /////////////////////////
//////////////////////////////////////////////////////////////////////////


temp_ptr<double>&& resample_fixed_ratio(ibuffer_data& bufdata, double *filter, long nzero, long npoints, intptr_t nsamps, intptr_t num, intptr_t denom, long chan)
{
    double *current_filter;

    intptr_t current_offset;
    intptr_t max_filter_length;
    intptr_t filter_offset;
    intptr_t i, j, k;

    // Create the relevant filters

    temp_ptr<double> temp_filters = bufresample_calc_temp_filters(filter, nzero, npoints, num, denom, max_filter_length, filter_offset);

    // Allocate memory

    temp_ptr<float> temp(max_filter_length + 4);
    temp_ptr<float> buf_temp(bufdata.get_length());

    temp_ptr<double> output(nsamps);

    // Check memory

    if (!temp || !output || !temp_filters || !buf_temp || !filter)
        return std::move(temp_ptr<double>(0));

    // Set buffer in use

    intptr_t first = filter_offset / num;
    first = (filter_offset > first * num) ? (first + 1) * denom : first * denom;
    intptr_t second = (bufdata.get_length() - (max_filter_length - filter_offset)) / num;
    second *= denom;

    if (second > nsamps)
        second = nsamps;
    if (first >= second)
        first = second = 0;

    ibuffer_get_samps(bufdata, buf_temp.get(), 0, bufdata.get_length(), chan);

    // Resample

    for (i = 0, current_offset = -filter_offset; i < first; current_offset += num)
    {
        for (j = 0, current_filter = temp_filters.get(); i < first && j < denom; i++, j++, current_filter += max_filter_length)
        {
            double sum = 0.0;
            
            get_buffer_samples_local(bufdata, buf_temp.get(), temp.get(), current_offset + (j * num / denom), max_filter_length);

            for (k = 0; k < max_filter_length; k++)
                sum += temp[k] * current_filter[k];

            output[i] = sum;
        }
    }

    for (; i < second; current_offset += num)
    {
        for (j = 0, current_filter = temp_filters.get(); i < second && j < denom; i++, j++, current_filter += max_filter_length)
        {
            intptr_t buf_offset = current_offset + ((j * num) / denom);
#ifdef TARGET_INTEL
            output[i] = sum_filter_mul((vDouble *)current_filter, buf_temp.get() + buf_offset, max_filter_length);
#else
            output[i] = sum_filter_mul(current_filter, buf_temp.get() + buf_offset, max_filter_length);
#endif
        }
    }

    for (; i < nsamps; current_offset += num)
    {
        for (j = 0, current_filter = temp_filters.get(); i < nsamps && j < denom; i++, j++, current_filter += max_filter_length)
        {
            double sum = 0.0;
            
            get_buffer_samples_local(bufdata, buf_temp.get(), temp.get(), current_offset + (j * num / denom), max_filter_length);

            for (k = 0; k < max_filter_length; k++)
                sum += temp[k] * current_filter[k];

            output[i] = sum;
        }
    }

    return std::move(output);
}


temp_ptr<double>&& bufresample_copy(ibuffer_data& bufdata, intptr_t nsamps, long chan)
{
    temp_ptr<double> output(nsamps);
    temp_ptr<float> temp(nsamps);

    if (!temp || !output)
        return std::move(temp_ptr<double>(0));

    // Special case where no resampling is necessary

    get_buffer_samples(bufdata, temp.get(), 0, nsamps, chan);

    for (intptr_t i = 0; i < nsamps; i++)
        output[i] = temp[i];

    return std::move(output);
}


//////////////////////////////////////////////////////////////////////////
///////////////////////////// User messages //////////////////////////////
//////////////////////////////////////////////////////////////////////////


// Arguments are - target buffer / source buffer followed by one or two more arguments depending on the message:
//  - resample: target sample rate
//  - tranpose: transposition ratio
//  - both: transposition ratio / target sample rate


void bufresample_process(t_bufresample *x, t_symbol *sym, long argc, t_atom *argv)
{
    defer(x, (method) bufresample_process_internal, sym, (short) argc, argv);
}


void bufresample_process_internal(t_bufresample *x, t_symbol *sym, short argc, t_atom *argv)
{
    temp_ptr<double> output(0);

    double sr_convert = 0.0;

    intptr_t num;
    intptr_t denom;

    uintptr_t nom_size;

    // Check arguments

    if (argc < 3 || (sym == gensym("both") && argc < 4))
    {
        object_error((t_object *)x, "not enough arguments to message %s", sym->s_name);
        return;
    }

    t_symbol *target = atom_getsym(argv++);
    argc--;
    t_symbol *source = atom_getsym(argv++);
    argc--;
    double rate = atom_getfloat(argv++);
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
    double sample_rate = bufdata.get_sample_rate();

    if (sym == gensym("resample"))
    {
        sample_rate = rate;
        rate = std::fabs(bufdata.get_sample_rate() / rate);
    }

    if (sym == gensym("both"))
    {
        sample_rate = sr_convert;
        rate = rate * std::fabs(bufdata.get_sample_rate() / sr_convert);
    }

    if (rate < 0.01)
    {
        object_error((t_object *) x, "very small and negative rates not allowed (less than 0.01)");
        return;
    }

    // Resample

    double *filter = (double *) access_mem_swap(&x->filter, &nom_size);
    long npoints = nom_size & 0x7FFF;
    long nzero = (nom_size >> 0x10) & 0x3FF;

    rate_as_ratio(rate, num, denom);
    intptr_t nsamps = static_cast<intptr_t>(std::ceil(((double) denom * (double) bufdata.get_length()) / (double) num));
        
    if (filter)
    {
        if (rate == 1.0)
            output = bufresample_copy(bufdata, nsamps, chan);
        else
            output = resample_fixed_ratio(bufdata, filter, nzero, npoints, nsamps, num, denom, chan);
    }

    if (!output)
    {
        object_error((t_object *)x, "could not allocate memory for internal processing");
        return;
    }

    // Copy out to buffer

    auto error = buffer_write((t_object *)x, target, output.get(), nsamps, x->write_chan - 1, x->resize, sample_rate, 1.);

    if (!error)
        outlet_bang(x->process_done);
}


//////////////////////////////////////////////////////////////////////////
///////////////////////// Slower Rate Resampling /////////////////////////
//////////////////////////////////////////////////////////////////////////


// Calculate one sample

double calc_sample(ibuffer_data& bufdata, float *samples, double *filter, long nzero, long npoints, double position, double rate, long chan)
{
    double per_samp = rate > 1.0 ? 1.0 / (rate * nzero) : 1.0 / nzero;
    double one_over_per_samp = rate > 1.0 ? nzero * rate : nzero;
    double filter_position;
    double sum = 0.0;

    long half_length = nzero * npoints;

    const intptr_t idx = static_cast<intptr_t>(position);
    const double fract = position - idx;

    // Get samples

    const intptr_t offset = static_cast<intptr_t>(position - one_over_per_samp);
    const intptr_t nsamps = static_cast<intptr_t>(one_over_per_samp + one_over_per_samp + 2);
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

temp_ptr<double>&& resample_fixed_rate(ibuffer_data& bufdata, double *filter, long nzero, long npoints, double offset, intptr_t nsamps, double rate, long chan)
{
    double one_over_per_samp = rate > 1.0 ? nzero * rate : nzero;
    double mul = rate > 1.0 ? 1.0 / rate : 1.0;

    intptr_t temp_length = static_cast<intptr_t>(one_over_per_samp + one_over_per_samp + 2);

    // Allocate memory

    temp_ptr<double> output(nsamps);
    temp_ptr<float> temp(temp_length);

    // Check memory

    if (!temp || !output || !filter)
        return std::move(temp_ptr<double>(0));

    // Resample

    for (intptr_t i = 0; i < nsamps; i++)
        output[i] = mul * calc_sample(bufdata, temp.get(), filter, nzero, npoints, offset + (i * rate), rate, chan);

    return std::move(output);
}
