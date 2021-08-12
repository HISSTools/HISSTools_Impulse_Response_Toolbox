
#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <HIRT_Core_Functions.h>
#include <HIRT_Buffer_Access.h>

// Define common attributes and the class name (for the common attributes file)

#define OBJ_CLASSNAME t_irpiecewiseapprox
#define OBJ_USES_HIRT_READ_ATTR
#define OBJ_USES_HIRT_SMOOTH_ATTR

#include <HIRT_Common_Attribute_Vars.h>


// Object class and structure

t_class *this_class;

struct t_irpiecewiseapprox
{
    t_pxobject x_obj;

    // Attributes

    HIRT_COMMON_ATTR

    long target_segments;
    long norm_mode;

    // Approximation Outlet

    void *approximation_outlet;

};


// This include deals with setup of common attributes - requires the object structure to be defined

#include <HIRT_Common_Attribute_Setup.h>


// PLA data structure

struct t_PLA_data
{
    AH_UIntPtr start_pos;
    AH_UIntPtr end_pos;

    t_PLA_data *next_data;
};


// Function prototypes

void *irpiecewiseapprox_new(t_symbol *s, short argc, t_atom *argv);
void irpiecewiseapprox_free(t_irpiecewiseapprox *x);
void irpiecewiseapprox_assist(t_irpiecewiseapprox *x, void *b, long m, long a, char *s);

void irpiecewiseapprox_process(t_irpiecewiseapprox *x, t_symbol *source);
void irpiecewiseapprox_process_internal(t_irpiecewiseapprox *x, t_symbol *source, short argc, t_atom *argv);

t_PLA_data *calc_PLA(double *x_vals, double *y_vals, long target_segments, AH_UIntPtr N);
void PLA_bottom_up(double *x_vals, double *y_vals, t_PLA_data *data, AH_UIntPtr target_segments, AH_UIntPtr num_segments);
double PLA_calc_merged_cost(double *x_vals, double *y_vals, AH_UIntPtr start_pos, AH_UIntPtr end_pos);


//////////////////////////////////////////////////////////////////////////
/////////////////////// Main / New / Free / Assist ///////////////////////
//////////////////////////////////////////////////////////////////////////


int main()
{
    this_class = class_new("irplapprox~",
                          (method) irpiecewiseapprox_new,
                          (method)irpiecewiseapprox_free,
                          sizeof(t_irpiecewiseapprox),
                          0L,
                          A_GIMME,
                          0);

    class_addmethod(this_class, (method)irpiecewiseapprox_process, "approx", A_SYM, 0L);

    class_addmethod(this_class, (method)irpiecewiseapprox_assist, "assist", A_CANT, 0L);
    class_register(CLASS_BOX, this_class);

    declare_HIRT_common_attributes(this_class);

    CLASS_ATTR_LONG(this_class, "normmode", 0, t_irpiecewiseapprox, norm_mode);
    CLASS_ATTR_STYLE_LABEL(this_class, "normmode",0, "onoff", "Normalise Mode");

    CLASS_ATTR_LONG(this_class, "segments", 0, t_irpiecewiseapprox, target_segments);
    CLASS_ATTR_LABEL(this_class,"segments", 0, "Target Number of Segments");
    CLASS_ATTR_FILTER_CLIP(this_class, "segments", 1, (HIRT_MAX_SPECIFIER_ITEMS / 2));

    return 0;
}


void *irpiecewiseapprox_new(t_symbol *s, short argc, t_atom *argv)
{
    t_irpiecewiseapprox *x = (t_irpiecewiseapprox *)object_alloc(this_class);

    x->approximation_outlet = listout(x);

    x->target_segments = 10;
    x->norm_mode = 0;

    init_HIRT_common_attributes(x);

    // Default to fast smoothing

    x->smooth_mode = 1;

    attr_args_process(x, argc, argv);

    return(x);
}


void irpiecewiseapprox_free(t_irpiecewiseapprox *x)
{
    free_HIRT_common_attributes(x);
}


void irpiecewiseapprox_assist(t_irpiecewiseapprox *x, void *b, long m, long a, char *s)
{
    sprintf(s,"Instructions In");
}


//////////////////////////////////////////////////////////////////////////
///////////////////////////// User Messages //////////////////////////////
//////////////////////////////////////////////////////////////////////////


void irpiecewiseapprox_process(t_irpiecewiseapprox *x, t_symbol *source)
{
    defer(x, (method) irpiecewiseapprox_process_internal, source, 0, 0);
}


void irpiecewiseapprox_process_internal(t_irpiecewiseapprox *x, t_symbol *source, short argc, t_atom *argv)
{
     FFT_SETUP_D fft_setup;

    FFT_SPLIT_COMPLEX_D spectrum_1;

    double *log_freqs;
    float *in;

    t_PLA_data *PLA_data;
    t_PLA_data *current_data;

    t_atom out_list[HIRT_MAX_SPECIFIER_ITEMS];

    double sample_rate;
    double max_val = 0.0;

    AH_UIntPtr num_output_pairs;
    AH_UIntPtr pos;
    AH_UIntPtr fft_size;
    AH_UIntPtr fft_size_halved;
    AH_UIntPtr fft_size_log2;
    AH_UIntPtr i;

    // Get input lengths

    AH_SIntPtr source_length = buffer_length(source);

    // Check and calculate lengths

    fft_size = calculate_fft_size(source_length, &fft_size_log2);
    fft_size_halved = fft_size >> 1;

    // Allocate Memory

    hisstools_create_setup(&fft_setup, fft_size_log2);

    spectrum_1.realp = allocate_aligned<double>((fft_size * 2) + (fft_size_halved + 2));
    spectrum_1.imagp = spectrum_1.realp + fft_size;
    log_freqs = spectrum_1.imagp + fft_size;

    in = allocate_aligned<float>(source_length);

    if (!fft_setup || !spectrum_1.realp || !in)
    {
        hisstools_destroy_setup(fft_setup);

        deallocate_aligned(in);
        deallocate_aligned(spectrum_1.realp);

        return;
    }

    // Get input and sample rate

    buffer_read(source, x->read_chan - 1, in, source_length);
    sample_rate = buffer_sample_rate(source);

    // Convert to frequency domain (for inversion only one buffer is given - use modelling spike for the other input)

    time_to_spectrum_float(fft_setup, in, source_length, spectrum_1, fft_size);
    power_spectrum(spectrum_1, fft_size, SPECTRUM_FULL);

    // Smooth

    if (x->num_smooth)
        smooth_power_spectrum(spectrum_1, (t_smooth_mode) x->smooth_mode, fft_size, x->num_smooth > 1 ? x->smooth[0] : 0.0, x->num_smooth > 1 ? x->smooth[1] : x->smooth[0]);

    // Calculate log frequencies and  convert powers to db

    for (i = 0; i < fft_size_halved + 1; i++)
        log_freqs[i] = log((i / ((double) fft_size)) * sample_rate);
    pow_to_db_array(spectrum_1.realp, fft_size_halved + 1);

    // End stop frequencies and db vals with duplicates

    log_freqs[i] = log_freqs[i - 1];
    spectrum_1.realp[i] = spectrum_1.realp[i - 1];

    // Do PLA

    PLA_data = calc_PLA(log_freqs + 1, spectrum_1.realp + 1, x->target_segments, fft_size_halved);

    if (PLA_data)
    {
        // If norm mode is on find the maximum output value

        if (x->norm_mode)
        {
            for (max_val = -HUGE_VAL, current_data = PLA_data; current_data; current_data = current_data->next_data)
            {
                pos = current_data->start_pos;
                max_val = (spectrum_1.realp[pos + 1] > max_val) ? spectrum_1.realp[pos + 1] : max_val;
            }
        }

        for (num_output_pairs = 0, current_data = PLA_data; current_data; current_data = current_data->next_data, num_output_pairs++)
        {
            pos = current_data->start_pos;

            atom_setfloat(out_list + (num_output_pairs * 2) + 0, exp(log_freqs[pos + 1]));
            atom_setfloat(out_list + (num_output_pairs * 2) + 1, spectrum_1.realp[pos + 1] - max_val);
        }

        outlet_list(x->approximation_outlet, 0, (short) (num_output_pairs * 2), out_list);
    }
    else
    {
        object_error((t_object *) x, "could not allocate temporary linear approximation memory");
    }

    // Free resources

    hisstools_destroy_setup(fft_setup);
    deallocate_aligned(in);
    deallocate_aligned(spectrum_1.realp);
    free(PLA_data);
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////// PLA Calculations ////////////////////////////
//////////////////////////////////////////////////////////////////////////


t_PLA_data *calc_PLA(double *x_vals, double *y_vals, long target_segments, AH_UIntPtr N)
{
    t_PLA_data *PLA_data = NULL;
    AH_UIntPtr i;

    // Allocate Resources

    PLA_data = (t_PLA_data *) malloc(sizeof(t_PLA_data) * ((N >> 1) + 1));

    if (!PLA_data)
        return NULL;

    // Prepare Linear Approximation

    for (i = 0; i < (N >> 1); i++)
    {
        PLA_data[i].start_pos = i * 2;
        PLA_data[i].end_pos = (i + 1) * 2;
        PLA_data[i].next_data = PLA_data + i + 1;
    }

    PLA_data[i].start_pos = (i * 2);
    PLA_data[i].end_pos = (i * 2);
    PLA_data[i].next_data = NULL;

    // Do Bottom up PLA

    PLA_bottom_up(x_vals, y_vals, PLA_data, target_segments, N / 2);

    return PLA_data;
}


void PLA_bottom_up(double *x_vals, double *y_vals, t_PLA_data *data, AH_UIntPtr target_segments, AH_UIntPtr num_segments)
{
    t_PLA_data *current_data;
    t_PLA_data *merge_data = NULL;

    double min_cost;
    double cost;

    AH_UIntPtr i;

    while (num_segments > target_segments && num_segments >= 1)
    {
        min_cost = HUGE_VAL;
        current_data = data;

        for (i = 0; i < num_segments - 1; i++, current_data = current_data->next_data)
        {
            cost = PLA_calc_merged_cost(x_vals, y_vals, current_data->start_pos, current_data->next_data->end_pos);
            if (cost < min_cost)
            {
                min_cost = cost;
                merge_data = current_data;
            }
        }

        merge_data->end_pos = merge_data->next_data->end_pos;
        merge_data->next_data = merge_data->next_data->next_data;
        merge_data->next_data->start_pos = merge_data->end_pos;
        num_segments--;
    }
}


double PLA_calc_merged_cost(double *x_vals, double *y_vals, AH_UIntPtr start_pos, AH_UIntPtr end_pos)
{
    double gradient = (y_vals[end_pos] - y_vals[start_pos]) / (x_vals[end_pos] - x_vals[start_pos]);
    double offset = y_vals[start_pos] - (x_vals[start_pos] * gradient);
    double difference;
    double sum = 0.0;

    AH_UIntPtr i;

    for (i = start_pos; i < end_pos; i++)
    {
        difference = y_vals[i] - (x_vals[i] * gradient + offset);
        sum += difference * difference;
    }

    return sum;
}
