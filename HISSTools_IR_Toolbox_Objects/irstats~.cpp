
#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <algorithm>

#include <HIRT_Core_Functions.hpp>
#include <HIRT_Buffer_Access.hpp>

// Define common attributes and the class name (for the common attributes file)

#define OBJ_CLASSNAME t_irstats
#define OBJ_USES_HIRT_READ_ATTR

#include <HIRT_Common_Attribute_Vars.hpp>


// Calculation Parameter Defines

constexpr int MAX_REPORT_LENGTH = 64;

constexpr int ONSET_HALF_SIZE_MS = 8;
constexpr double ONSET_HOP_SIZE_MS = 0.5;

constexpr int NED_HALF_WINDOW_MS = 30;
constexpr int NED_HOP_SIZE_MS = 2;

constexpr double DIRECT_HALF_WINDOW_MS = 3.5;
constexpr double DIRECT_HOP_SIZE_MS = 0.5;

constexpr int DIRECT_ONSET_TO_PEAK_MS = 30;
constexpr int DIRECT_SEARCH_MS = 15;

constexpr int CROSS_RMS_HALF_WINDOW_MS = 25;


// Object class and structure

t_class *this_class;

struct t_irstats
{
    t_pxobject x_obj;

    // Attributes

    HIRT_COMMON_ATTR

    long time_in_samples;

    long num_clip_mixing;
    double clip_mixing[2];

    // Bang Out

    void *results_outlet;
};


// This include deals with setup of common attributes - requires the object structure to be defined

#include <HIRT_Common_Attribute_Setup.hpp>


// Stats calculation structure

struct t_stats_calc
{
    t_stats_calc()
    : integration(0)
    , integration_db(0)
    , window(0)
    , in(0)
    , samples(0)
    , in_length(0)
    , T20(-1)
    , T30(-1)
    , EDT(-1)
    , center(-1)
    , clarity(HUGE_VAL)
    , rms(HUGE_VAL)
    , peak(HUGE_VAL)
    , LR_onset_rms(HUGE_VAL)
    , ned(-1)
    , gain(HUGE_VAL)
    , maxgain(HUGE_VAL)
    , integration_conv_db_val(0)
    , ir_length(-1)
    , onset(-1)
    , LR_onset(-1)
    , direct_end(-1)
    , direct_exists(-1)
    , integration_conv_db_pos(0)
    , calc_integration(false)
    , calc_integration_db(false)
    , x(nullptr)
    {}
    
    // Memory

    temp_ptr<double> integration;
    temp_ptr<double> integration_db;
    temp_ptr<double> window;

    temp_ptr<float> in;
    temp_ptr<float> samples;

    // Input properties

    intptr_t in_length;

    long time_in_samples;

    double sample_rate;
    double min_mixing;
    double max_mixing;

    // Stats

    double T20;
    double T30;
    double EDT;
    double center;
    double clarity;
    double rms;
    double peak;
    double LR_onset_rms;
    double ned;
    double gain;
    double maxgain;
    double integration_conv_db_val;

    intptr_t ir_length;
    intptr_t onset;
    intptr_t LR_onset;
    intptr_t direct_end;
    intptr_t direct_exists;
    intptr_t integration_conv_db_pos;

    // Calculation Flags

    bool calc_integration;
    bool calc_integration_db;

    // Object pointer

    t_irstats *x;
};



// Globals

double freq_bands[10] = {16, 32, 64, 125, 250, 500, 1000, 2000, 4000, 8000};

// Constant for ned computation - the value is given by: 1.0 / erfc(1.0 / sqrt(2.0))_

double ned_constant = 3.15148718753437728778976634203;

// Symbols

t_symbol *ps_t20;
t_symbol *ps_t30;
t_symbol *ps_edt;
t_symbol *ps_onset;
t_symbol *ps_directend;
t_symbol *ps_directexists;
t_symbol *ps_mix;
t_symbol *ps_center;
t_symbol *ps_clarity;
t_symbol *ps_rms;
t_symbol *ps_peak;
t_symbol *ps_mixrms;
t_symbol *ps_ned;
t_symbol *ps_gain;
t_symbol *ps_maxgain;
t_symbol *ps_length;
t_symbol *ps_sr;


// Function prototypes

void *irstats_new(t_symbol *s, short argc, t_atom *argv);
void irstats_free(t_irstats *x);
void irstats_assist(t_irstats *x, void *b, long m, long a, char *s);

intptr_t mstosamps(double ms, double sample_rate);
double sampstoms(double samps, double sample_rate);

double calc_mean(double *data, intptr_t length);
double calc_variance(double *data, double mean, intptr_t length);
double calc_peak(float *data, intptr_t length);
double calc_norm_kurtosis(float *data, intptr_t length);
double calc_rms(float *data, intptr_t length);

void integrate_pow(double *integration, float *ir, intptr_t length);
void backwards_integrate(double *integration_db, double *integration, intptr_t length);
intptr_t conv_db(double *integration_db, double conv_db, intptr_t conv_pos, intptr_t length);

void get_samples(float *out, float *buffer, intptr_t length, intptr_t offset, intptr_t nsamps);
void make_von_hann_window(double *window, intptr_t window_size);
void apply_window(double *output, float *samples, double *window, intptr_t window_size);
double sum_pow_window(float *samples, double *window, intptr_t window_size);

intptr_t calc_onset(float *ir, float *samples, double *window, intptr_t length, double sample_rate);
intptr_t calc_direct(float *ir, float *samples, double *window, intptr_t onset, intptr_t length, double sample_rate);
long calc_direct_exists(float *ir, intptr_t onset, intptr_t direct_end, intptr_t LR_onset, double sample_rate);

void linear_regression(double *data, intptr_t length, double *slope, double *offset);
double calc_reverb_time(double *integration_db, intptr_t length, double hi_evaluate, double lo_evaluate, double db_intersection);

double calc_ned(float *sample_window, double *window, intptr_t window_size);
double calc_ned_no_window(float *sample_window, intptr_t window_size);
double calc_ned_average(float *ir, float *samples, double *window, intptr_t length, double sample_rate);
intptr_t get_sample_time(double time_val, long time_in_samples, double sample_rate);
intptr_t calc_LR_onset(float *ir, float *samples, double *window, intptr_t length, intptr_t onset, long time_in_samples, double min_mix, double max_mix, double sample_rate);

double calc_clarity(double *integration, intptr_t length, double sample_rate);
double calc_center(float *ir, intptr_t length, intptr_t onset);
double calc_LR_onset_rms(float *ir, float *samples, double *window, intptr_t LR_onset, intptr_t length, double sample_rate);
double calc_gain(t_irstats *x, float *ir, intptr_t length, double sample_rate, double *max_oct);

void do_integration(t_stats_calc& stats);
void do_integration_db(t_stats_calc& stats);

intptr_t retrieve_onset(t_stats_calc& stats);
double retrieve_T20(t_stats_calc& stats);
double retrieve_T30(t_stats_calc& stats);
double retrieve_EDT(t_stats_calc& stats);
double retrieve_center(t_stats_calc& stats);
double retrieve_clarity(t_stats_calc& stats);
double retrieve_peak(t_stats_calc& stats);
double retrieve_rms(t_stats_calc& stats);
double retrieve_ned(t_stats_calc& stats);
intptr_t retrieve_LR_onset(t_stats_calc& stats);
intptr_t retrieve_direct_end(t_stats_calc& stats);
intptr_t retrieve_direct_exists(t_stats_calc& stats);
double retrieve_LR_onset_rms(t_stats_calc& stats);
double retrieve_gain(t_stats_calc& stats);
double retrieve_maxgain(t_stats_calc& stats);

void time_store(t_atom *a, double time_in_samples, long mode, double sample_rate);
void store_stat(t_atom *a, double stat);
void irstats_stats(t_irstats *x, t_symbol *sym, short argc, t_atom *argv);


//////////////////////////////////////////////////////////////////////////
/////////////////////// Main / New / Free / Assist ///////////////////////
//////////////////////////////////////////////////////////////////////////


int C74_EXPORT main()
{
    this_class = class_new("irstats~",
                          (method) irstats_new,
                          (method)irstats_free,
                          sizeof(t_irstats),
                          0L,
                          A_GIMME,
                          0);

    class_addmethod(this_class, (method)irstats_stats, "stats", A_GIMME, 0L);
    class_addmethod(this_class, (method)irstats_assist, "assist", A_CANT, 0L);

    class_register(CLASS_BOX, this_class);

    declare_HIRT_common_attributes(this_class);
    CLASS_ATTR_LONG(this_class, "timeinsamps", 0, OBJ_CLASSNAME, time_in_samples);
    CLASS_ATTR_STYLE_LABEL(this_class,"timeinsamps", 0,"onoff","Times in Samples");

    CLASS_ATTR_DOUBLE_VARSIZE(this_class, "clipmixing", 0, OBJ_CLASSNAME, clip_mixing, num_clip_mixing, 2);
    CLASS_ATTR_FILTER_MIN(this_class, "clipmixing", 0);
    CLASS_ATTR_LABEL(this_class,"clipmixing", 0, "Clip Mixing Time");

    ps_t20 = gensym("t20");
    ps_t30 = gensym("t30");
    ps_edt = gensym("edt");
    ps_onset = gensym("onset");
    ps_directend = gensym("directend");
    ps_directexists = gensym("directexists");
    ps_mix = gensym("mixing");
    ps_center = gensym("center");
    ps_clarity = gensym("clarity");
    ps_rms = gensym("rms");
    ps_peak = gensym("peak");
    ps_mixrms = gensym("mixingrms");
    ps_ned = gensym("ned");
    ps_gain = gensym("gain");
    ps_maxgain = gensym("maxgain");
    ps_length = gensym("length");
    ps_sr = gensym("sr");

    return 0;
}


void *irstats_new(t_symbol *s, short argc, t_atom *argv)
{
    t_irstats *x = reinterpret_cast<t_irstats *>(object_alloc(this_class));

    x->results_outlet = listout(x);

    x->time_in_samples = 0;

    init_HIRT_common_attributes(x);
    attr_args_process(x, argc, argv);

    return x;
}


void irstats_free(t_irstats *x)
{
    free_HIRT_common_attributes(x);
}


void irstats_assist(t_irstats *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET)
        sprintf(s,"Instructions In");
    else
        sprintf(s,"Results");
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////// Time Conversions ////////////////////////////
//////////////////////////////////////////////////////////////////////////


intptr_t mstosamps(double ms, double sample_rate)
{
    return static_cast<intptr_t>(ms * (sample_rate / 1000.0));
}


double sampstoms(double samps, double sample_rate)
{
    return (samps * 1000.0) / sample_rate;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////// Basic Stats Calculations ////////////////////////
//////////////////////////////////////////////////////////////////////////


double calc_mean(double *data, intptr_t length)
{
    double sum = 0.0;

    for (intptr_t i = 0; i < length; i++)
        sum += data[i];

    return sum / static_cast<double>(length);
}


double calc_variance(double *data, double mean, intptr_t length)
{
    double sum = 0.0;

    for (intptr_t i = 0; i < length; i++)
    {
        double val =  data[i] - mean;
        sum += val * val;
    }

    return sum / static_cast<double>(length);
}


double calc_peak(float *data, intptr_t length)
{
    float max = -HUGE_VALF;

    for (intptr_t i = 0; i < length; i++)
        max = std::max(max, std::fabs(data[i]));

    return a_to_db(max);
}


double calc_norm_kurtosis(float *data, intptr_t length)
{
    double var_sum = 0.0;
    double kur_sum = 0.0;

    // N.B. Data is assumed to have a zero mean

    for (intptr_t i = 0; i < length; i++)
    {
        double val = data[i];
        val *= val;
        var_sum += val;
        kur_sum += val * val;
    }

    return (static_cast<double>(length) * kur_sum / (var_sum * var_sum)) - 3.0;
}


double calc_rms(float *data, intptr_t length)
{
    double sum = 0.0;

    for (intptr_t i = 0; i < length; i++)
        sum += data[i] * data[i];

    return pow_to_db(sum / static_cast<double>(length));
}


//////////////////////////////////////////////////////////////////////////
////////////////////// Integration and DB Conversion /////////////////////
//////////////////////////////////////////////////////////////////////////


void integrate_pow(double *integration, float *ir, intptr_t length)
{
    double sum = 0.0;

    for (intptr_t i = 0; i < length; i++)
    {
        sum += ir[i] * ir[i];
        integration[i] = sum;
    }
}


void backwards_integrate(double *integration_db, double *integration, intptr_t length)
{
    double max_integration = integration[length - 1];
    double max_integration_recip = 1.0 / max_integration;

    // N.B. It is a waste of CPU to convert to db more than we need, so do this separately

    for (intptr_t i = 0; i < length; i++)
        integration_db[i] = (max_integration - integration[i]) * max_integration_recip;
}


intptr_t conv_db(double *integration_db, double conv_db, intptr_t conv_pos, intptr_t length)
{
    intptr_t i;
    
    for (i = conv_pos; i < length; i++)
    {
        double val = integration_db[i] = pow_to_db(integration_db[i]);

        if (val < conv_db)
            break;
    }

    return i + 1;
}


//////////////////////////////////////////////////////////////////////////
//////////////// Safe Sample Block Retrival and Windowing ////////////////
//////////////////////////////////////////////////////////////////////////


void get_samples(float *out, float *buffer, intptr_t length, intptr_t offset, intptr_t nsamps)
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

    if (offset + temp_nsamps > length)
        temp_nsamps = length - offset;

    if (temp_nsamps < 0)
        temp_nsamps = 0;

    for (intptr_t i = 0; i < temp_offset; i++)
        out[i] = 0;

    for (intptr_t i = 0; i < temp_nsamps; i++)
        out[i + temp_offset] = buffer[i + offset];

    for (intptr_t i = temp_nsamps + temp_offset; i < nsamps; i++)
        out[i] = 0;
}


void make_von_hann_window(double *window, intptr_t window_size)
{
    double sum = 0.0;

    for (intptr_t i = 0; i < window_size; i++)
    {
        window[i] = 0.5 - (0.5 * std::cos(PI * 2.0 * (static_cast<double>(i) / static_cast<double>(window_size))));
        sum += window[i];
    }

    sum = 1.0 / sum;

    for (intptr_t i = 0; i < window_size; i++)
        window[i] *= sum;
}


void apply_window(double *output, float *samples, double *window, intptr_t window_size)
{
    for (intptr_t i = 0; i < window_size; i++)
        output[i] = samples[i] * window[i];
}



double sum_pow_window(float *samples, double *window, intptr_t window_size)
{
    double sum = 0.0;

    for (intptr_t i = 0; i < window_size; i++)
        sum += samples[i] * samples[i] * window[i];

    return sum;
}

//////////////////////////////////////////////////////////////////////////
/////////////////////// Calculate Onset and Direct ///////////////////////
//////////////////////////////////////////////////////////////////////////


intptr_t calc_onset(float *ir, float *samples, double *window, intptr_t length, double sample_rate)
{
    double prior_max_energy = 0.0;
    double max_ratio = 1.0;
    double max_value = 0.0;
    double max_energy = 0.0;
    double value, ratio, energy;
    double last_energy = 0.0;
    double energy_thresh = db_to_pow(-60);

    intptr_t hop_size = mstosamps(ONSET_HOP_SIZE_MS, sample_rate);
    intptr_t half_window_size = mstosamps(ONSET_HALF_SIZE_MS, sample_rate);
    intptr_t window_size = half_window_size + half_window_size + 1;
    intptr_t prior_max_energy_offset = 0;
    intptr_t max_offset = 0;
    intptr_t max_ratio_offset = 0;
    intptr_t max_energy_offset = 0;
    intptr_t first_thresh_frame = -1;

    make_von_hann_window(window, window_size);

    for (intptr_t i = 0; i < length; i++)
    {
        value = ir[i];
        value *= value;

        if (value > max_value)
        {
            max_value = value;
            max_offset = i;
        }
    }

    energy_thresh *= max_value;

    // Find max ratio increase

    for (intptr_t i = 0; i < max_offset; i += hop_size)
    {
        get_samples(samples, ir, length , i - half_window_size, window_size);
        energy = sum_pow_window(samples, window, window_size);
        ratio = energy / last_energy;

        if (energy > max_energy)
        {
            max_energy = energy;
            max_energy_offset = i - hop_size;
        }

        if (i && ratio > max_ratio)
        {
            max_ratio = ratio;
            max_ratio_offset = i - hop_size;
            prior_max_energy = max_energy;
            prior_max_energy_offset = max_energy_offset;
        }

        if (first_thresh_frame == -1 && energy > energy_thresh)
            first_thresh_frame = i - hop_size;

        last_energy = energy;
    }

    // Suspect an issue if the onset is more than halfway through the IR and use a basic threshold instead

    if (prior_max_energy_offset > (length >> 1))
    {
        first_thresh_frame = first_thresh_frame < 0 ? 0 : first_thresh_frame;
        first_thresh_frame = first_thresh_frame >= length ? length - 1 : first_thresh_frame;

        return first_thresh_frame;
    }

    prior_max_energy *= db_to_pow(-5);

    // Backtrack to find the earliest energy within 5dB of the maximum so far

    for (intptr_t i = prior_max_energy_offset - hop_size; i >= 0; i -= hop_size)
    {
        get_samples (samples, ir, length , i - half_window_size, window_size);
        energy = sum_pow_window (samples, window, window_size);

        if (energy > prior_max_energy)
            prior_max_energy_offset = i - hop_size;
    }

    prior_max_energy_offset = prior_max_energy_offset < 0 ? 0 : prior_max_energy_offset;
    prior_max_energy_offset = prior_max_energy_offset >= length ? length - 1 : prior_max_energy_offset;

    return prior_max_energy_offset;
}


intptr_t calc_direct(float *ir, float *samples, double *window, intptr_t onset, intptr_t length, double sample_rate)
{
    double energy[60 + 10];

    double min_value = HUGE_VAL;
    double max_value = 0.0;

    intptr_t start_search;
    intptr_t end_search;
    intptr_t search_length = mstosamps(DIRECT_ONSET_TO_PEAK_MS, sample_rate);
    intptr_t hop_size = mstosamps(DIRECT_HOP_SIZE_MS, sample_rate);
    intptr_t half_window_size = mstosamps(DIRECT_HALF_WINDOW_MS, sample_rate);
    intptr_t window_size = half_window_size + half_window_size + 1;
    intptr_t direct_end;
    intptr_t min_offset = -1;
    intptr_t end;
    intptr_t i, j;

    make_von_hann_window(window, window_size);

    // Find max peak value

    for (i = onset; i < onset + search_length && i < length; i++)
        max_value = std::fabs(ir[i]) > max_value ? std::fabs(ir[i]) : max_value;

    max_value = db_to_a(a_to_db(max_value) - 5);

    // Find first peak value

    for (i = onset + 1; i < (onset + search_length) && (i < length - 1); i++)
        if (std::fabs(ir[i]) > max_value && std::fabs(ir[i]) > std::fabs(ir[i - 1]) && std::fabs(ir[i]) > std::fabs(ir[i + 1]))
            break;

    start_search = i;

    // Calculate energy levels

    end_search = start_search + mstosamps(DIRECT_SEARCH_MS, sample_rate);

    for (i = 0, j = start_search; j < end_search; i++, j += hop_size)
    {
        get_samples(samples, ir, length , j - half_window_size, window_size);
        energy[i] = sum_pow_window(samples, window, window_size);
    }
    end = i;
    max_value = 0;

    for (i = 1; i < end - 1; i++)
    {
        if (energy[i] < min_value && energy[i] < energy[i-1])
        {
            min_value = energy[i];
            min_offset = i;

            if (energy[i] < energy[i+1])
                break;
        }
    }

    direct_end = (min_offset * hop_size) + start_search;

    return direct_end;
}


long calc_direct_exists(float *ir, intptr_t onset, intptr_t direct_end, intptr_t LR_onset, double sample_rate)
{
    double kurtosis_value_1 = 0.0;
    double kurtosis_value_2 = 0.0;

    intptr_t start = direct_end + mstosamps(15, sample_rate);
    intptr_t length = LR_onset - start;
    intptr_t max_search_length = mstosamps(100, sample_rate);

    length = length > max_search_length ? max_search_length : length;

    if (onset >= direct_end)
        return 0;

    // Calculate kurtosis values

    kurtosis_value_1 = calc_norm_kurtosis(ir + onset, direct_end - onset);

    if (length > 0)
        kurtosis_value_2 = calc_norm_kurtosis(ir + start, length);
    else
        kurtosis_value_2 = 0.0;

    if (kurtosis_value_1 > 10.0 && kurtosis_value_1 > kurtosis_value_2 && ((kurtosis_value_2 < 10.0) || kurtosis_value_1 / kurtosis_value_2 > 3.0))
        return 1;
    else
        return 0;
}

//////////////////////////////////////////////////////////////////////////
///////////////////////////// Calculate RT60 /////////////////////////////
//////////////////////////////////////////////////////////////////////////


void linear_regression(double *data, intptr_t length, double *slope, double *offset)
{
    double sum1 = 0.0;
    double sum2 = 0.0;
    double xmean = (length - 1) / 2.0;
    double ymean = calc_mean(data, length);

    for (intptr_t i = 0; i < length; i++)
        sum1 += (i - xmean) * (data[i] - ymean);

    for (intptr_t i = 0; i < length; i++)
        sum2 += (i - xmean) * (i - xmean);

    *slope = sum1 / sum2;
    *offset = ymean - *slope * xmean;
}


double calc_reverb_time(double *integration_db, intptr_t length, double hi_evaluate, double lo_evaluate, double db_intersection)
{
    double value = 0.0;
    double slope;
    double offset;

    intptr_t hi_offset = 0;
    intptr_t lo_offset = 0;
    intptr_t i;

    for (i = 0; value > hi_evaluate && i < length; i++)
        value = integration_db[i];

    hi_offset = i;

    for (; value > lo_evaluate && i < length; i++)
        value = integration_db[i];

    lo_offset = i;

    linear_regression(integration_db + hi_offset, lo_offset - hi_offset, &slope, &offset);

    if (slope == 0.0 || std::isnan(slope) || std::isinf(slope))
        return 1.0;

    return ((db_intersection - offset) / slope) + hi_offset;
}


//////////////////////////////////////////////////////////////////////////
///////////////////// Calculate NED and LR Onset Time ////////////////////
//////////////////////////////////////////////////////////////////////////


double calc_ned(float *sample_window, double *window, intptr_t window_size)
{
    double variance = sum_pow_window(sample_window, window, window_size);
    double sum = 0.0;

    for (intptr_t i = 0; i < window_size; i++)
    {
        if (sample_window[i] * sample_window[i] > variance)
            sum += window[i];
    }

    return sum * ned_constant;
}


double calc_ned_no_window(float *sample_window, intptr_t window_size)
{
    double variance = 0.0;
    double sum = 0.0;

    for (intptr_t i = 0; i < window_size; i++)
        variance += sample_window[i] * sample_window[i];

    variance /= window_size;

    for (intptr_t i = 0; i < window_size; i++)
    {
        if (sample_window[i] * sample_window[i] > variance)
            sum += 1;
    }

    return (sum / window_size) * ned_constant;
}


double calc_ned_average(float *ir, float *samples, double *window, intptr_t length, double sample_rate)
{
    double ned = 0.0;
    intptr_t hop_size = mstosamps(NED_HOP_SIZE_MS, sample_rate);
    intptr_t half_window_size = mstosamps(NED_HALF_WINDOW_MS, sample_rate);
    intptr_t window_size = half_window_size + half_window_size + 1;
    intptr_t count = 0;

    make_von_hann_window(window, window_size);

    for (intptr_t i = 0; i < length; i += hop_size)
    {
        get_samples(samples, ir, length , i - half_window_size, window_size);
        ned += calc_ned(samples, window, window_size);
        count++;
    }

    return ned / static_cast<double>(count);
}


intptr_t get_sample_time(double time_val, long time_in_samples, double sample_rate)
{
    if (time_in_samples)
        return static_cast<intptr_t>(time_val);
    else
        return static_cast<intptr_t>(std::round(time_val * sample_rate / 1000.0));
}


intptr_t calc_LR_onset(float *ir, float *samples, double *window, intptr_t length, intptr_t onset, long time_in_samples, double min_mix, double max_mix, double sample_rate)
{
    double ned = 0.0;
    intptr_t hop_size = mstosamps(NED_HOP_SIZE_MS, sample_rate);
    intptr_t half_window_size = mstosamps(NED_HALF_WINDOW_MS, sample_rate);
    intptr_t window_size = half_window_size + half_window_size + 1;
    intptr_t min_mixing = onset + get_sample_time(min_mix, time_in_samples, sample_rate);
    intptr_t max_mixing = length;
    intptr_t i;

    make_von_hann_window(window, window_size);

    if (max_mix)
    {
        max_mixing = onset + get_sample_time(max_mix, time_in_samples, sample_rate);
        max_mixing = max_mixing < length ? max_mixing : length;
    }

    for (i = onset; i < max_mixing; i += hop_size)
    {
        get_samples(samples, ir, length , i - half_window_size, window_size);
        ned = calc_ned(samples, window, window_size);

        if (ned >= 1.0)
            break;
    }

    if (i >= max_mixing)
        return max_mixing;

    if (i <= min_mixing)
        return min_mixing;

    return i;
}


//////////////////////////////////////////////////////////////////////////
////////////////////////// Calculate Misc Stats  /////////////////////////
//////////////////////////////////////////////////////////////////////////


double calc_clarity(double *integration, intptr_t length, double sample_rate)
{
    intptr_t early_time = mstosamps(80, sample_rate);
    double full_integration = integration[length - 1];
    double early_integration;

    if (early_time < length)
        early_integration = integration[early_time];
    else
        early_integration = full_integration;

    return pow_to_db(early_integration / (full_integration - early_integration));
}


double calc_center(float *ir, intptr_t length, intptr_t onset)
{
    double weighted_sum = 0.0;
    double sum = 0.0;

    for (intptr_t i = onset; i < length; i++)
    {
        double energy = ir[i] * ir[i];
        weighted_sum += i * energy;
        sum += energy;
    }

    return weighted_sum / sum;
}


double calc_LR_onset_rms(float *ir, float *samples, double *window, intptr_t LR_onset, intptr_t length, double sample_rate)
{
    intptr_t half_window_size = mstosamps(CROSS_RMS_HALF_WINDOW_MS, sample_rate);
    intptr_t window_size = half_window_size + half_window_size + 1;

    make_von_hann_window(window, window_size);
    get_samples(samples, ir, length , LR_onset - half_window_size, window_size);

    return pow_to_db(sum_pow_window(samples, window, window_size));
}


double calc_gain(t_irstats *x, float *ir, intptr_t length, double sample_rate, double *max_oct)
{
    FFT_SPLIT_COMPLEX_D spectrum;

    double max_gain = -HUGE_VAL;
    double current_octave;
    double overall_gain;

    intptr_t octave_count = 0;
    intptr_t start, end;
    intptr_t fft_size;
    intptr_t i, j;

    // Calculate FFT size

    uintptr_t fft_size_log2;
    fft_size = calculate_fft_size(16384, fft_size_log2);
    double fft_ratio = std::fabs(fft_size / sample_rate);

    // Allocate and check temporary memory

    temp_fft_setup fft_setup(fft_size_log2);
    
    temp_ptr<double> temp(fft_size * 2);
    
    spectrum.realp = temp.get();
    spectrum.imagp = spectrum.realp + (fft_size >> 1);

    double *accumulate = temp.get() + fft_size;

    if (!fft_setup || !temp)
    {
        object_error((t_object *) x, "could not allocate internal memory for analysis");
        return -HUGE_VAL;
    }

    for (j = 0; j < fft_size; j++)
        accumulate[j] = 0;

    for (i = 0; i < length; i += fft_size)
    {
        for (j = 0; j < ((length - i) >= fft_size ? fft_size : (length - i)); j++)
            accumulate[j] += ir[i + j];
    }

    // Read buffer and do FFT

    time_to_halfspectrum_double(fft_setup, accumulate, fft_size, spectrum, fft_size);

    // Zero nyquist for ease

    spectrum.imagp[0] = 0;

    for (i = 0, overall_gain = 0.0; i < 9; i++)
    {
        start = static_cast<intptr_t>(freq_bands[i] * fft_ratio);
        end = static_cast<intptr_t>(freq_bands[i + 1] * fft_ratio);

        // Add relevant power amount

        for (current_octave = 0.0, j = start; j < end && j < (fft_size >> 1); j++)
            current_octave += spectrum.realp[j] = (spectrum.realp[j] * spectrum.realp[j]) + (spectrum.imagp[j] * spectrum.imagp[j]);

        if (j > start)
        {
            current_octave = pow_to_db(current_octave / static_cast<double>(j - start));
            if (current_octave > max_gain)
                max_gain = current_octave;
            overall_gain += current_octave;
            octave_count++;
        }
    }

    *max_oct = max_gain;
    return overall_gain / static_cast<double>(octave_count);
}


//////////////////////////////////////////////////////////////////////////
///////////////////////////// IR Integration /////////////////////////////
//////////////////////////////////////////////////////////////////////////


void do_integration(t_stats_calc& stats)
{
    if (!stats.calc_integration)
    {
        retrieve_onset(stats);
        integrate_pow(stats.integration.get(), stats.in.get() + stats.onset, stats.ir_length);
    }

    stats.calc_integration = true;
}


void do_integration_db(t_stats_calc& stats)
{
    if (!stats.calc_integration_db)
    {
        do_integration(stats);
        backwards_integrate(stats.integration_db.get(), stats.integration.get(), stats.ir_length);
    }

    stats.integration_conv_db_pos = conv_db(stats.integration_db.get(), stats.integration_conv_db_val, stats.integration_conv_db_pos, stats.ir_length);
    stats.calc_integration_db = true;
}


//////////////////////////////////////////////////////////////////////////
//// Retrieve Stats (call relevant functions avoiding recalculation) /////
//////////////////////////////////////////////////////////////////////////


intptr_t retrieve_onset(t_stats_calc& stats)
{
    if (stats.onset == -1)
        stats.onset = calc_onset(stats.in.get(), stats.samples.get(), stats.window.get(), stats.in_length, stats.sample_rate);

    stats.ir_length = stats.in_length - stats.onset;

    return stats.onset;
}


double retrieve_T20(t_stats_calc& stats)
{
    if (stats.T20 == -1)
    {
        stats.integration_conv_db_val = -25;
        do_integration_db(stats);
        stats.T20 = calc_reverb_time(stats.integration_db.get(), stats.ir_length, -5, -25, -60);
    }

    return stats.T20;
}


double retrieve_T30(t_stats_calc& stats)
{
    if (stats.T30 == -1)
    {
        stats.integration_conv_db_val = -35;
        do_integration_db(stats);
        stats.T30 = calc_reverb_time(stats.integration_db.get(), stats.ir_length, -5, -35, -60);
    }

    return stats.T30;
}


double retrieve_EDT(t_stats_calc& stats)
{
    if (stats.EDT == -1)
    {
        stats.integration_conv_db_val = -10;
        do_integration_db(stats);
        stats.EDT = calc_reverb_time(stats.integration_db.get(), stats.ir_length, 0, -10, -60);
    }

    return stats.EDT;
}


double retrieve_center(t_stats_calc& stats)
{
    if (stats.center == -1)
    {
        retrieve_onset(stats);
        stats.center = calc_center(stats.in.get(), stats.ir_length, stats.onset);
    }

    return stats.center;
}


double retrieve_clarity(t_stats_calc& stats)
{
    if (stats.clarity == HUGE_VAL)
    {
        do_integration(stats);
        stats.clarity = calc_clarity(stats.integration.get(), stats.ir_length, stats.sample_rate);
    }

    return stats.clarity;
}


double retrieve_peak(t_stats_calc& stats)
{
    if (stats.peak == HUGE_VAL)
        stats.peak = calc_peak(stats.in.get(), stats.in_length);

    return stats.peak;
}


double retrieve_rms(t_stats_calc& stats)
{
    if (stats.rms == HUGE_VAL)
        stats.rms = calc_rms(stats.in.get(), stats.in_length);

    return stats.rms;
}


double retrieve_ned(t_stats_calc& stats)
{
    if (stats.ned == -1)
        stats.ned = calc_ned_average(stats.in.get(), stats.samples.get(), stats.window.get(), stats.in_length, stats.sample_rate);

    return stats.ned;
}


intptr_t retrieve_LR_onset(t_stats_calc& stats)
{
    if (stats.LR_onset == -1)
        stats.LR_onset = calc_LR_onset(stats.in.get(), stats.samples.get(), stats.window.get(), stats.in_length, retrieve_onset(stats), stats.time_in_samples, stats.min_mixing, stats.max_mixing, stats.sample_rate);

    return stats.LR_onset;
}


intptr_t retrieve_direct_end(t_stats_calc& stats)
{
    if (stats.direct_end == -1)
    {
        intptr_t onset = retrieve_onset(stats);
        stats.direct_end = calc_direct(stats.in.get(), stats.samples.get(), stats.window.get(), onset, stats.ir_length, stats.sample_rate);
    }

    return stats.direct_end;
}


intptr_t retrieve_direct_exists(t_stats_calc& stats)
{
    if (stats.direct_exists == -1)
        stats.direct_exists = calc_direct_exists(stats.in.get(), retrieve_onset(stats), retrieve_direct_end(stats), retrieve_LR_onset(stats), stats.sample_rate);

    return stats.direct_exists;
}


double retrieve_LR_onset_rms(t_stats_calc& stats)
{
    if (stats.LR_onset_rms == HUGE_VAL)
        stats.LR_onset_rms = calc_LR_onset_rms(stats.in.get(), stats.samples.get(), stats.window.get(), retrieve_LR_onset(stats), stats.in_length, stats.sample_rate);

    return stats.LR_onset_rms;
}


double retrieve_gain(t_stats_calc& stats)
{
    if (stats.gain == HUGE_VAL)
        stats.gain = calc_gain(stats.x, stats.in.get(), stats.in_length, stats.sample_rate, &stats.maxgain);

    return stats.gain;
}


double retrieve_maxgain(t_stats_calc& stats)
{
    if (stats.maxgain == HUGE_VAL)
        stats.gain = calc_gain(stats.x, stats.in.get(), stats.in_length, stats.sample_rate, &stats.maxgain);

    return stats.maxgain;
}


//////////////////////////////////////////////////////////////////////////
/////////////// User Stats Routine and Atom Storage Helpers //////////////
//////////////////////////////////////////////////////////////////////////


void time_store(t_atom *a, double time_in_samples, long mode, double sample_rate)
{
    if (mode)
        atom_setfloat(a, sampstoms(time_in_samples, sample_rate));
    else
        atom_setlong(a, static_cast<t_atom_long>(time_in_samples));
}


void store_stat(t_atom *a, double stat)
{
    atom_setfloat(a, stat);
}


void irstats_stats(t_irstats *x, t_symbol *sym, short argc, t_atom *argv)
{
    t_stats_calc stats;

    t_atom report[MAX_REPORT_LENGTH];

    double max_half_window_ms;
    double sample_rate;

    long time_mode = !x->time_in_samples;
    t_atom_long read_chan = x->read_chan - 1;

    if (argc < 2)
    {
        object_error((t_object *) x, "not enough arguments to message %s", sym->s_name);
        return;
    }

    t_symbol *source = atom_getsym(argv++);
    argc --;

    if (argc > MAX_REPORT_LENGTH)
    {
        object_warn((t_object *) x, "too many stats requested, returning the first %ld", MAX_REPORT_LENGTH);
        argc = MAX_REPORT_LENGTH;
    }

    // Check input buffers

    if (buffer_check((t_object *) x, source))
        return;

    // Get length

    stats.in_length = buffer_length(source);

    // Get sample rate

    stats.sample_rate = sample_rate = buffer_sample_rate(source);

    stats.time_in_samples = x->time_in_samples;
    stats.min_mixing = x->num_clip_mixing > 0 ? x->clip_mixing[0] : 0;
    stats.max_mixing = x->num_clip_mixing > 1 ? x->clip_mixing[1] : 0;

    // Allocate Memory

    max_half_window_ms = ONSET_HALF_SIZE_MS;
    max_half_window_ms = NED_HALF_WINDOW_MS > max_half_window_ms ? NED_HALF_WINDOW_MS : max_half_window_ms;
    max_half_window_ms = DIRECT_HALF_WINDOW_MS > max_half_window_ms ? DIRECT_HALF_WINDOW_MS : max_half_window_ms;

    intptr_t max_window_size =  2 * mstosamps(max_half_window_ms, sample_rate) + 1;

    stats.in = temp_ptr<float>(stats.in_length);
    stats.integration = temp_ptr<double>(stats.in_length);
    stats.integration_db = temp_ptr<double>(stats.in_length);
    stats.samples = temp_ptr<float>(max_window_size);
    stats.window = temp_ptr<double>(max_window_size);

    // Check memory allocations

    if (!stats.in || !stats.integration || !stats.integration_db || !stats.samples || !stats.window)
    {
        object_error((t_object *) x, "could not allocate temporary memory for processing");
        return;
    }

    // Get input

    buffer_read(source, read_chan, stats.in.get(), stats.in_length);

    // Get Stats

    for (short i = 0; i < argc; i++)
    {
        double current_stat = HUGE_VAL;

        if (atom_getsym(argv + i) == ps_t20)
        {
            current_stat = retrieve_T20(stats);
            time_store(report + i, current_stat, time_mode, sample_rate);
            continue;
        }
        if (atom_getsym(argv + i) == ps_t30)
        {
            current_stat = retrieve_T30(stats);
            time_store(report + i, current_stat, time_mode, sample_rate);
            continue;
        }
        if (atom_getsym(argv + i) == ps_edt)
        {
            current_stat = retrieve_EDT(stats);
            time_store(report + i, current_stat, time_mode, sample_rate);
            continue;
        }
        if (atom_getsym(argv + i) == ps_length)
        {
            current_stat = static_cast<double>(stats.in_length);
            time_store(report + i, current_stat, time_mode, sample_rate);
            continue;
        }
        if (atom_getsym(argv + i) == ps_sr)
        {
            current_stat = stats.sample_rate;
            store_stat(report + i, current_stat);
            continue;
        }
        if (atom_getsym(argv + i) == ps_onset)
        {
            current_stat = static_cast<double>(retrieve_onset(stats));
            time_store(report + i, current_stat, time_mode, sample_rate);
            continue;
        }
        if (atom_getsym(argv + i) == ps_directend)
        {
            current_stat = static_cast<double>(retrieve_direct_end(stats));
            time_store(report + i, current_stat, time_mode, sample_rate);
            continue;
        }
        if (atom_getsym(argv + i) == ps_directexists)
        {
            current_stat = static_cast<double>(retrieve_direct_exists(stats));
            store_stat(report + i, current_stat);
            continue;
        }
        if (atom_getsym(argv + i) == ps_mix)
        {
            current_stat = static_cast<double>(retrieve_LR_onset(stats));
            time_store(report + i, current_stat, time_mode, sample_rate);
            continue;
        }
        if (atom_getsym(argv + i) == ps_center)
        {
            current_stat = retrieve_center(stats);
            time_store(report + i, current_stat, time_mode, sample_rate);
            continue;
        }
        if (atom_getsym(argv + i) == ps_clarity)
        {
            current_stat = retrieve_clarity(stats);
            store_stat(report + i, current_stat);
            continue;
        }
        if (atom_getsym(argv + i) == ps_rms)
        {
            current_stat = retrieve_rms(stats);
            store_stat(report + i, current_stat);
            continue;
        }
        if (atom_getsym(argv + i) == ps_peak)
        {
            current_stat = retrieve_peak(stats);
            store_stat(report + i, current_stat);
            continue;
        }
        if (atom_getsym(argv + i) == ps_mixrms)
        {
            current_stat = retrieve_LR_onset_rms(stats);
            store_stat(report + i, current_stat);
            continue;
        }
        if (atom_getsym(argv + i) == ps_ned)
        {
            current_stat = retrieve_ned(stats);
            store_stat(report + i, current_stat);
            continue;
        }
        if (atom_getsym(argv + i) == ps_gain)
        {
            current_stat = retrieve_gain(stats);
            store_stat(report + i, current_stat);
            continue;
        }
        if (atom_getsym(argv + i) == ps_maxgain)
        {
            current_stat = retrieve_maxgain(stats);
            store_stat(report + i, current_stat);
            continue;
        }

        object_error((t_object *) x, "unknown stat - index %ld", i + 1);
        store_stat(report + i, current_stat);
    }

    outlet_list(x->results_outlet, gensym("list"), argc, report);
}
