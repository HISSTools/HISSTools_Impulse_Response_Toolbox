
#include "HIRT_Core_Functions.hpp"

//////////////////////////////////////////////////////////////////////////
////////////////////////// FFT Size Calculation //////////////////////////
//////////////////////////////////////////////////////////////////////////

uintptr_t int_log2(uintptr_t in, bool& inexact)
{
    uintptr_t temp = in;
    uintptr_t out = 0;

    if (!in)
        return 0;

    while (temp)
    {
        temp >>= 1;
        out++;
    }

    if (in == uintptr_t(1u) << (out - uintptr_t(1u)))
    {
        out--;
        inexact = 0;
    }
    else
        inexact = 1;

    return out;
}

uintptr_t calculate_fft_size(uintptr_t input_size, uintptr_t& fft_size_log2)
{
    bool inexact = false;
    uintptr_t calc_temp = int_log2(input_size, inexact);

    fft_size_log2 = calc_temp;

    if (!input_size)
        return 0;

    return uintptr_t(1u) << calc_temp;
}

//////////////////////////////////////////////////////////////////////////
/////////////////////// DB / Pow Array Conversions ///////////////////////
//////////////////////////////////////////////////////////////////////////

void db_to_pow_array(double *in, uintptr_t length)
{
    for (uintptr_t i = 0; i < length; i++)
        in[i] = db_to_pow(in[i]);
}

void pow_to_db_array(double *in, uintptr_t length)
{
    for (uintptr_t i = 0; i < length; i++)
        in[i] = pow_to_db(in[i]);
}

//////////////////////////////////////////////////////////////////////////
//////////////////// Time-Freq / Freq-Time Transforms ////////////////////
//////////////////////////////////////////////////////////////////////////

void time_to_spectrum_float(FFT_SETUP_D fft_setup, float *in_buf, uintptr_t in_length, FFT_SPLIT_COMPLEX_D spectrum, uintptr_t fft_size)
{
    bool inexact = false;
    uintptr_t fft_size_log2 = int_log2(fft_size, inexact);
    uintptr_t i;

    if (inexact)
        return;

    // get samples

    for (i = 0; i < in_length; i++)
    {
        spectrum.realp[i] = in_buf[i];
        spectrum.imagp[i] = 0.0;
    }

    // zero pad

    for (; i < fft_size; i++)
    {
        spectrum.realp[i] = 0.0;
        spectrum.imagp[i] = 0.0;
    }

    // take the complex fft

    hisstools_fft(fft_setup, &spectrum, fft_size_log2);
}

void time_to_halfspectrum_float(FFT_SETUP_D fft_setup, float *in_buf, uintptr_t in_length, FFT_SPLIT_COMPLEX_D spectrum, uintptr_t fft_size)
{
    bool inexact = false;
    uintptr_t fft_size_log2 = int_log2(fft_size, inexact);

    if (inexact)
        return;

    // take the real fft

    hisstools_unzip_zero(in_buf, &spectrum, in_length, fft_size_log2);
    hisstools_rfft(fft_setup, &spectrum, fft_size_log2);

    for (uintptr_t i = 0; i < fft_size >> 1; i++)
    {
        spectrum.realp[i] *= 0.5;
        spectrum.imagp[i] *= 0.5;
    }
}

void time_to_spectrum_double(FFT_SETUP_D fft_setup, double *in_buf, uintptr_t in_length, FFT_SPLIT_COMPLEX_D spectrum, uintptr_t fft_size)
{
    bool inexact = false;
    uintptr_t fft_size_log2 = int_log2(fft_size, inexact);
    uintptr_t i;

    if (inexact)
        return;

    // get samples

    for (i = 0; i < in_length; i++)
    {
        spectrum.realp[i] = in_buf[i];
        spectrum.imagp[i] = 0.0;
    }

    // zero pad

    for (; i < fft_size; i++)
    {
        spectrum.realp[i] = 0.0;
        spectrum.imagp[i] = 0.0;
    }

    // take the complex fft

    hisstools_fft(fft_setup, &spectrum, fft_size_log2);
}

void time_to_halfspectrum_double(FFT_SETUP_D fft_setup, double *in_buf, uintptr_t in_length, FFT_SPLIT_COMPLEX_D spectrum, uintptr_t fft_size)
{
    bool inexact = false;
    uintptr_t fft_size_log2 = int_log2(fft_size, inexact);

    if (inexact)
        return;

    // take the real fft

    hisstools_unzip_zero(in_buf, &spectrum, in_length, fft_size_log2);
    hisstools_rfft(fft_setup, &spectrum, fft_size_log2);

    for (uintptr_t i = 0; i < fft_size >> 1; i++)
    {
        spectrum.realp[i] *= 0.5;
        spectrum.imagp[i] *= 0.5;
    }
}

void spectrum_to_time(FFT_SETUP_D fft_setup, double *out_buf, FFT_SPLIT_COMPLEX_D spectrum, uintptr_t fft_size, t_spectrum_format half_spectrum)
{
    bool inexact = false;
    uintptr_t fft_size_log2 = int_log2(fft_size, inexact);

    double scale = 1.0 / (fft_size);

    if (inexact)
        return;

    if (half_spectrum == SPECTRUM_FULL)
        spectrum.imagp[0] = spectrum.realp[fft_size >> 1];

    // now convert to time domain with fft back

    hisstools_rifft(fft_setup, &spectrum, fft_size_log2);
    hisstools_zip(&spectrum, out_buf, fft_size_log2);

    // scale

    for (uintptr_t i = 0; i < fft_size; i++)
        out_buf[i] *= scale;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////// Calculate Power Spectrum ////////////////////////
//////////////////////////////////////////////////////////////////////////

void power_spectrum(FFT_SPLIT_COMPLEX_D spectrum, uintptr_t fft_size, t_spectrum_format format)
{
    double *real = spectrum.realp;
    double *imag = spectrum.imagp;
    uintptr_t to = format == SPECTRUM_REAL ? fft_size >> 1 : fft_size;

    // discard nyquist bin for a half spectrum

    if (format == SPECTRUM_REAL)
        spectrum.imagp[0] = 0.0;

    // Calculate power

    for (uintptr_t i = 0; i < to; i++)
    {
        real[i] = (real[i] * real[i]) + (imag[i] * imag[i]);
        imag[i] = 0.0;
    }
}

void power_full_spectrum_from_half_spectrum(FFT_SPLIT_COMPLEX_D spectrum, uintptr_t fft_size)
{
    double *real = spectrum.realp;
    double *imag = spectrum.imagp;
    uintptr_t i;

    // Get DC

    real[0] *= spectrum.realp[0];

    // Calculate power

    for (i = 1; i < fft_size >> 1; i++)
        real[i] = (real[i] * real[i]) + (imag[i] * imag[i]);

    // Get Nyquist

    real[i++] = imag[0] * imag[0];

    // Copy Negative frequency values

    for (; i < fft_size; i++)
        real[i] = real[fft_size - i];

    // Zero imaginary values

    for (i = 0; i < fft_size; i++)
        imag[i] = 0.0;
}

//////////////////////////////////////////////////////////////////////////
/////////////////////////// Spectral Smoothing ///////////////////////////
//////////////////////////////////////////////////////////////////////////

double hann_table[4097];

long hann_setup_flag = 0;

void setup_hann_wind()
{
    for (uintptr_t i = 0; i < 4097; i++)
        hann_table[i] = 0.5 * (1.0 - std::cos(M_PI * static_cast<double>(4095 - i) / 4095.0));
}

static inline double fast_hann_wind(double in)
{
    // N.B. - in must be between 0 and 1

    uintptr_t index;

    double lo;
    double hi;
    double f_index;
    double fract;

    f_index = in * 4095;
    index = static_cast<uintptr_t>(f_index);
    fract = f_index - index;

    lo = hann_table[index];
    hi = hann_table[index + 1];

    return lo + fract * (hi - lo);
}

void smooth_power_spectrum(FFT_SPLIT_COMPLEX_D spectrum, t_smooth_mode mode, uintptr_t fft_size, double smooth_lo, double smooth_hi)
{
    double *spectrum_out = spectrum.realp;
    double *spectrum_in = spectrum.imagp;
    double *temp_filter;
    
    double filter_val, oct_width, accum;
    double half_width_recip = 1.0;
    
    intptr_t half_width, current_half_width, loop_size;
    intptr_t nyquist_bin = (fft_size >> 1);
    intptr_t limit = fft_size - 1;
    intptr_t i, j, k;

    smooth_lo = smooth_lo > 1.0 ? 1.0 : smooth_lo;
    smooth_hi = smooth_hi > 1.0 ? 1.0 : smooth_hi;
    const double smooth_mul = smooth_hi - smooth_lo;

    auto calc_half_width = [&](intptr_t i)
    {
        const double normalised = static_cast<double>(i) / static_cast<double>(nyquist_bin);
		return static_cast<intptr_t>(((normalised * smooth_mul) + smooth_lo) * static_cast<double>(nyquist_bin));
	};
    
    // Copy power spectrum from real part (spectrum_out) to imaginary part (spectrum_in) for calculation

    for (uintptr_t i = 0; i < fft_size; i++)
        spectrum_in[i] = spectrum_out[i];

    switch (mode)
    {
        case SMOOTH_MODE_FULL:

            // N.B. FWIW - this is not threadsafe....

            if (!hann_setup_flag)
            {
                setup_hann_wind();
                hann_setup_flag = 1;
            }

            current_half_width = -1;
            temp_filter = spectrum_out + nyquist_bin + 1;
            
            for (i = 0; i < nyquist_bin + 1; i++)
            {
                // Linear relationship between bin and width

                half_width = calc_half_width(i);
                half_width = (half_width >= nyquist_bin) ? nyquist_bin - 1 : half_width;
                
                // Make a temporary filter each time half_width changes
                
                if (current_half_width != half_width)
                {
                    current_half_width = half_width;
                    half_width_recip = half_width > 1 ? 2.0 / (2 * half_width - 1): 1.0;

                    for (j = 1; j < half_width; j++)
                        temp_filter[j - 1] =  fast_hann_wind(j * half_width_recip);
                }

                // Apply filter
                
                loop_size = i < half_width ? i + 1 : half_width;
                filter_val = spectrum_in[i];

                for (j = 1; j < loop_size; j++)
                    filter_val += temp_filter[j - 1] * (spectrum_in[i - j] + spectrum_in[i + j]);
                
                for (k = 1; j < half_width; j++, k++)
                    filter_val += temp_filter[j - 1] * (spectrum_in[k] + spectrum_in[i + j]);

                spectrum_out[i] = filter_val * half_width_recip;
            }
            break;

        case SMOOTH_MODE_FAST:

            for (uintptr_t i = 1; i < fft_size; i++)
                spectrum_in[i] += spectrum_in[i - 1];

            for (i = 0; i < nyquist_bin + 1; i++)
            {
                // Linear relationship between bin and width

                half_width = calc_half_width(i);
                accum = 0.0;

                intptr_t lo = i - half_width;
                intptr_t hi = i + half_width;

                if (lo < 0)
                {
                    accum = (spectrum_in[-lo] - spectrum_in[0]) / -lo;
                    lo = 0;
                }

                if (lo == hi)
                    hi++;

                if (hi > limit)
                    hi = limit;

                spectrum_out[i] = accum + (spectrum_in[hi] - spectrum_in[lo]) / ((hi - lo));
            }
            break;

        case SMOOTH_MODE_FAST_OCT:

            for (uintptr_t i = 1; i < fft_size; i++)
                spectrum_in[i] += spectrum_in[i - 1];

            spectrum_out[0] = spectrum_in[0];

            for (i = 1; i < nyquist_bin + 1; i++)
            {
                oct_width = (static_cast<double>(i) / static_cast<double>(nyquist_bin) * smooth_mul) + smooth_lo;
                oct_width = std::pow(2.0, oct_width * 0.5);

                intptr_t lo = static_cast<intptr_t>(i / oct_width);
                intptr_t hi = static_cast<intptr_t>(i * oct_width);

                if (lo == hi)
                    lo--;

                if (hi > limit)
                    hi = limit;

                spectrum_out[i] = (spectrum_in[hi] - spectrum_in[lo]) / ((hi - lo));
            }
            break;
    }

    // Copy upper part of spectrum

    for (uintptr_t i = nyquist_bin + 1; i < fft_size; i++)
        spectrum_out[i] = spectrum_out[fft_size - i];

    // Zero imaginary part

    for (uintptr_t i = 0; i < fft_size; i++)
        spectrum_in[i] = 0;
}

//////////////////////////////////////////////////////////////////////////
///////////////////////////// Phase Routines /////////////////////////////
//////////////////////////////////////////////////////////////////////////

void zero_phase_from_power_spectrum(FFT_SPLIT_COMPLEX_D spectrum, uintptr_t fft_size, t_spectrum_format format)
{
    uintptr_t to = format == SPECTRUM_REAL ? (fft_size >> 1) : (fft_size >> 1) + 1;

    for (uintptr_t i = 0; i < to; i++)
        spectrum.realp[i] = sqrt(spectrum.realp[i]);

    if (format == SPECTRUM_FULL)
    {
        for (uintptr_t i = to; i < fft_size; i++)
            spectrum.realp[i] = spectrum.realp[fft_size - i];
    }
}

void linear_phase_from_power_spectrum(FFT_SPLIT_COMPLEX_D spectrum, uintptr_t fft_size, t_spectrum_format format)
{
    zero_phase_from_power_spectrum (spectrum, fft_size, format);
    delay_spectrum(spectrum, fft_size, format, static_cast<double>(fft_size >> 1));
}

void minimum_phase_components_from_power_spectrum(FFT_SETUP_D fft_setup, FFT_SPLIT_COMPLEX_D spectrum, uintptr_t fft_size)
{
    double scale = 1.0 / fft_size;
    double min_power = db_to_pow(-1000.0);

    bool inexact;
    uintptr_t fft_size_halved = fft_size >> 1;
    uintptr_t fft_size_log2 = int_log2(fft_size, inexact);

    // take log of power spectrum - assume real input so calculate half and copy

    for (uintptr_t i = 0; i < fft_size_halved + 1; i++)
    {
        spectrum.realp[i] = spectrum.realp[i] < min_power ? min_power : spectrum.realp[i];
        spectrum.realp[i] = 0.5 * std::log(spectrum.realp[i]);
    }

    for (uintptr_t i = fft_size_halved + 1; i < fft_size; i++)
        spectrum.realp[i] = spectrum.realp[fft_size - i];

    // do complex inverse fft in position

    hisstools_ifft(fft_setup, &spectrum, fft_size_log2);

    // double the causal values

    for (uintptr_t i = 1; i < fft_size_halved; i++)
    {
        spectrum.realp[i] += spectrum.realp[i];
        spectrum.imagp[i] += spectrum.imagp[i];
    }

    for (uintptr_t i = fft_size_halved + 1; i < fft_size; i++)
    {
        spectrum.realp[i] = 0.0;
        spectrum.imagp[i] = 0.0;
    }

    // scale

    for (uintptr_t i = 0; i < fft_size_halved + 1; i++)
    {
        spectrum.realp[i] *= scale;
        spectrum.imagp[i] *= scale;
    }

    // fft back

    hisstools_fft(fft_setup, &spectrum, fft_size_log2);
}

void minimum_phase_from_power_spectrum(FFT_SETUP_D fft_setup, FFT_SPLIT_COMPLEX_D spectrum, uintptr_t fft_size)
{
    using complex = std::complex<double>;
    uintptr_t fft_size_halved = fft_size >> 1;

    minimum_phase_components_from_power_spectrum (fft_setup, spectrum, fft_size);

    // take complex exponential

    for (uintptr_t i = 0; i < fft_size_halved + 1; i++)
    {
        complex c1 = complex(spectrum.realp[i], spectrum.imagp[i]);
        complex c2 = std::exp(c1);

        spectrum.realp[i] = c2.real();
        spectrum.imagp[i] = c2.imag();
    }

    // copy top half of spectrum

    for (uintptr_t i = fft_size_halved + 1; i < fft_size; i++)
    {
        spectrum.realp[i] = spectrum.realp[fft_size - i];
        spectrum.imagp[i] = -spectrum.imagp[fft_size - i];
    }
}

void noncausal_maximum_phase_from_power_spectrum(FFT_SETUP_D fft_setup, FFT_SPLIT_COMPLEX_D spectrum, uintptr_t fft_size)
{
    // Minimum Phase

    minimum_phase_from_power_spectrum (fft_setup, spectrum, fft_size);

    // Time reversal

    for (uintptr_t i = 0; i < fft_size; i++)
        spectrum.imagp[i] = -spectrum.imagp[i];
}

void maximum_phase_from_power_spectrum(FFT_SETUP_D fft_setup, FFT_SPLIT_COMPLEX_D spectrum, uintptr_t fft_size)
{
    noncausal_maximum_phase_from_power_spectrum(fft_setup, spectrum, fft_size);

    // Rotate by one sample

    delay_spectrum(spectrum, fft_size, SPECTRUM_FULL,-1.0);
}

void mixed_phase_from_power_spectrum(FFT_SETUP_D fft_setup, FFT_SPLIT_COMPLEX_D spectrum, uintptr_t fft_size, double phase, bool zero_center)
{
    uintptr_t fft_size_halved = fft_size >> 1;

    double linphase_mul;
    double minphase_mul;
    double interp_phase;
    double amp;

    phase = phase < 0 ? 0.0 : phase;
    phase = phase > 1 ? 1.0 : phase;

    // N.B. we induce a delay of -1 sample for anything over linear to avoid wraparound to the first sample

    minphase_mul = 1 - (2 * phase);
    linphase_mul = zero_center ? 0.0 : (phase <= 0.5 ? -(2 * M_PI * phase) : (-2 * M_PI * (phase - 1.0 / fft_size)));

    minimum_phase_components_from_power_spectrum(fft_setup, spectrum, fft_size);

    // interpolate phase and calculate complex representation

    for (uintptr_t i = 0; i < fft_size_halved; i++)
    {
        amp = exp(spectrum.realp[i]);
        interp_phase = linphase_mul * i + minphase_mul * spectrum.imagp[i];

        spectrum.realp[i] = amp * std::cos(interp_phase);
        spectrum.imagp[i] = amp * std::sin(interp_phase);
    }

    // do nyquist

    spectrum.realp[fft_size_halved] = exp(spectrum.realp[fft_size_halved]);
    spectrum.imagp[fft_size_halved] = 0.0;

    // copy upper half of spectrum

    for (uintptr_t i = fft_size_halved + 1; i < fft_size; i++)
    {
        spectrum.realp[i] = spectrum.realp[fft_size - i];
        spectrum.imagp[i] = -spectrum.imagp[fft_size - i];
    }
}

void variable_phase_from_power_spectrum(FFT_SETUP_D fft_setup, FFT_SPLIT_COMPLEX_D spectrum, uintptr_t fft_size, double phase, bool zero_center)
{
    if (phase == 0.0)
    {
        minimum_phase_from_power_spectrum(fft_setup, spectrum, fft_size);
        return;
    }

    if (phase == 0.5)
    {
        if (zero_center)
            zero_phase_from_power_spectrum(spectrum, fft_size, SPECTRUM_FULL);
        else
            linear_phase_from_power_spectrum(spectrum, fft_size, SPECTRUM_FULL);
        return;
    }

    if (phase == 1.0)
    {
        if (zero_center)
            noncausal_maximum_phase_from_power_spectrum(fft_setup, spectrum, fft_size);
        else
            maximum_phase_from_power_spectrum(fft_setup, spectrum, fft_size);
        return;
    }

    mixed_phase_from_power_spectrum(fft_setup, spectrum, fft_size, phase, zero_center);
}

//////////////////////////////////////////////////////////////////////////
///////////////////// Frequency Specified Power Array ////////////////////
//////////////////////////////////////////////////////////////////////////

void make_freq_dependent_power_array(double *power_array, double *specifier_array, uintptr_t fft_size, double sample_rate, double db_offset)
{
    uintptr_t fft_size_halved = fft_size >> 1;
    uintptr_t num_items = 0;
    uintptr_t list_pos;
    uintptr_t i;

    double freq_mul = sample_rate / static_cast<double>(fft_size);
    double prev_log_freq = -HUGE_VAL;
    double next_log_freq;
    double bin_log_freq;
    double gradient;
    double offset;

    // Find last item (count number of items)

    for (i = 0; i < HIRT_MAX_SPECIFIER_ITEMS; i++)
    {
        if (isinf(specifier_array[i]))
            break;
    }

    num_items = i;

    // Allow a single fixed value (in db) or only one db value specified

    if (num_items <= 2)
    {
        double pow_val = num_items == 1 ? db_to_pow(specifier_array[0] + db_offset) : db_to_pow(specifier_array[1] + db_offset);

        for (i = 0; i < fft_size; i++)
            power_array[i] = pow_val;

        return;
    }

    // Otherwise expect freq / db duples - set DC bin

    num_items >>= 1;
    power_array[0] = specifier_array[1] + db_offset;

    for (i = 1, list_pos = 0, gradient = 0, offset = specifier_array[1], next_log_freq = std::log(specifier_array[0]); i < fft_size_halved + 1; i++)
    {
        // Find place in list and calculate new line if relevant

        bin_log_freq = std::log(i * freq_mul);

        if (bin_log_freq > next_log_freq)
        {
            // Search for the point in the list

            for (; bin_log_freq > next_log_freq && list_pos < num_items; list_pos++, prev_log_freq = next_log_freq, next_log_freq = std::log(specifier_array[list_pos << 1]));

            if (list_pos == num_items)
            {
                // End of list reached

                gradient = 0.0;
                offset = specifier_array[(list_pos << 1) - 1];
                next_log_freq = HUGE_VAL;
            }
            else
            {
                // Calculate the new line

                gradient = (specifier_array[(list_pos << 1) + 1] - specifier_array[(list_pos << 1) - 1]) / (next_log_freq - prev_log_freq);
                offset = specifier_array[(list_pos << 1) - 1] - (prev_log_freq * gradient);
            }
        }

        power_array[i] = bin_log_freq * gradient + offset + db_offset;
    }

    db_to_pow_array(power_array, fft_size_halved + 1);

    for (i = fft_size_halved + 1; i < fft_size; i++)
        power_array[i] = power_array[fft_size - i];
}

//////////////////////////////////////////////////////////////////////////
////////////////////////////// Convolution ///////////////////////////////
//////////////////////////////////////////////////////////////////////////

void convolve(FFT_SPLIT_COMPLEX_D fft_data_1, FFT_SPLIT_COMPLEX_D fft_data_2, uintptr_t fft_size, t_spectrum_format format)
{
    double *real1 = fft_data_1.realp;
    double *real2 = fft_data_2.realp;
    double *imag1 = fft_data_1.imagp;
    double *imag2 = fft_data_2.imagp;

    double a, b, c, d;

    uintptr_t from = format == SPECTRUM_REAL ? 1 : 0;
    uintptr_t to = format == SPECTRUM_REAL ? fft_size >> 1 : fft_size;
    uintptr_t i;

    if (format == SPECTRUM_REAL)
    {
        real1[0] = real1[0] * real2[0];
        imag1[0] = imag1[0] * imag2[0];
    }

    for (i = from; i < to; i++)
    {
        a = real1[i];
        b = imag1[i];
        c = real2[i];
        d = imag2[i];

        real1[i] = (a * c) - (b * d);
        imag1[i] = (b * c) + (a * d);
    }
}

//////////////////////////////////////////////////////////////////////////
/////////////////// Deconvolution (Zero Phase Filter) ////////////////////
//////////////////////////////////////////////////////////////////////////

void deconvolve_with_amp_filter(FFT_SPLIT_COMPLEX_D spectrum_1, FFT_SPLIT_COMPLEX_D spectrum_2,
                                 double *filter_amps, uintptr_t fft_size, t_spectrum_format format)
{
    using complex = std::complex<double>;

    double *real1 = spectrum_1.realp;
    double *imag1 = spectrum_1.imagp;
    double *real2 = spectrum_2.realp;
    double *imag2 = spectrum_2.imagp;

    complex c1, c2;

    double mul;

    uintptr_t from = format == SPECTRUM_REAL ? 1 : 0;
    uintptr_t to = format == SPECTRUM_REAL ? fft_size >> 1 : fft_size;
    uintptr_t i;

    if (format == SPECTRUM_REAL)
    {
        real1[0] = (real1[0] / real2[0]) * filter_amps[0];
        imag1[0] = (imag1[0] / imag2[0]) * filter_amps[fft_size >> 1];

        real1[0] = isinf(real1[0]) ? 0.0 : real1[0];
        imag1[0] = isinf(imag1[0]) ? 0.0 : imag1[0];
    }

    for (i = from; i < to; i++)
    {
        c1 = complex(real1[i], imag1[i]);
        c2 = complex(real2[i], imag2[i]);

        c1 = c1 * std::conj(c2);

        mul = filter_amps[i] / std::norm(c2);
        mul = isinf(mul) ? 0.0 : mul;

        real1[i] = c1.real() * mul;
        imag1[i] = c1.imag() * mul;
    }
}

void deconvolve_regularised_zero_phase(FFT_SPLIT_COMPLEX_D spectrum_1, FFT_SPLIT_COMPLEX_D spectrum_2,
                                        double *beta_in, uintptr_t fft_size, t_spectrum_format format)
{
    double *real1 = spectrum_1.realp;
    double *imag1 = spectrum_1.imagp;
    double *real2 = spectrum_2.realp;
    double *imag2 = spectrum_2.imagp;

    double a, b, c, d, e;

    uintptr_t from = format == SPECTRUM_REAL ? 1 : 0;
    uintptr_t to = format == SPECTRUM_REAL ? fft_size >> 1 : fft_size;

    if (format == SPECTRUM_REAL)
    {
        a = real1[0];
        b = imag1[0];
        c = real2[0];
        d = imag2[0];

        real1[0] = a * c / (c * c + beta_in[0]);
        imag1[0] = b * d / (d * d + beta_in[fft_size >> 1]);
    }

    for (uintptr_t i = from; i < to; i++)
    {
        a = real1[i];
        b = imag1[i];
        c = real2[i];
        d = imag2[i];

        e = 1.0 / (((c * c) + (d * d)) + beta_in[i]);

        real1[i] = ((a * c) + (b * d)) * e;
        imag1[i] = ((b * c) - (a * d)) * e;
    }
}

void deconvolve_clip_zero_phase(FFT_SPLIT_COMPLEX_D spectrum_1, FFT_SPLIT_COMPLEX_D spectrum_2,
                                 double *clip_min, double *clip_max, uintptr_t fft_size, t_spectrum_format format)
{
    double *real1 = spectrum_1.realp;
    double *real2 = spectrum_2.realp;
    double *imag1 = spectrum_1.imagp;
    double *imag2 = spectrum_2.imagp;

    double a, b, c, d, e, f, min_sq, max_sq;

    uintptr_t from = format == SPECTRUM_REAL ? 1 : 0;
    uintptr_t to = format == SPECTRUM_REAL ? fft_size >> 1 : fft_size;

    if (format == SPECTRUM_REAL)
    {
        a = real1[0];
        b = imag1[0];
        c = real2[0];
        d = imag2[0];

        min_sq = clip_min[0];
        max_sq = clip_max[0];
        c = (c * c < min_sq) ? sqrt(min_sq) : c;
        c = (c * c > max_sq) ? sqrt(max_sq) : c;

        min_sq = clip_min[fft_size >> 1];
        max_sq = clip_max[fft_size >> 1];
        d = (d * d < min_sq) ? sqrt(min_sq) : d;
        d = (d * d > max_sq) ? sqrt(max_sq) : d;

        real1[0] = a / c;
        imag1[0] = b / d;
    }

    for (uintptr_t i = from; i < to; i++)
    {
        min_sq = clip_min[i];
        max_sq = clip_max[i];

        a = real1[i];
        b = imag1[i];
        c = real2[i];
        d = imag2[i];

        e = ((c * c) + (d * d));

        f = e < min_sq ? min_sq : e;
        f = f > max_sq ? max_sq : f;

        if (f != e)
        {
            e = sqrt(f / e);
            c *= e;
            d *= e;
            e = ((c * c) + (d * d));
        }

        e = 1.0 / e;

        real1[i] = ((a * c) + (b * d)) * e;
        imag1[i] = ((b * c) - (a * d)) * e;
    }
}

//////////////////////////////////////////////////////////////////////////
///////////////// Deconvolution (Variable Phase Filter) //////////////////
//////////////////////////////////////////////////////////////////////////

// N.B. All spectra must have the same format

void deconvolve_with_filter(FFT_SPLIT_COMPLEX_D spectrum_1, FFT_SPLIT_COMPLEX_D spectrum_2,
                             FFT_SPLIT_COMPLEX_D filter_spectrum, uintptr_t fft_size, t_spectrum_format format)
{
    double *real1 = spectrum_1.realp;
    double *imag1 = spectrum_1.imagp;
    double *real2 = spectrum_2.realp;
    double *imag2 = spectrum_2.imagp;
    double *real3 = filter_spectrum.realp;
    double *imag3 = filter_spectrum.imagp;

    std::complex<double> c1, c2, c3;

    double mul;

    uintptr_t from = format == SPECTRUM_REAL ? 1 : 0;
    uintptr_t to = format == SPECTRUM_REAL ? fft_size >> 1 : fft_size;

    if (format == SPECTRUM_REAL)
    {
        real1[0] = (real1[0] / real2[0]) * real3[0];
        imag1[0] = (imag1[0] / imag2[0]) * imag3[0];

        real1[0] = isinf(real1[0]) ? 0.0 : real1[0];
        imag1[0] = isinf(imag1[0]) ? 0.0 : imag1[0];
    }

    for (uintptr_t i = from; i < to; i++)
    {
        c1 = std::complex<double>(real1[i], imag1[i]);
        c2 = std::complex<double>(real2[i], imag2[i]);
        c3 = std::complex<double>(real3[i], imag3[i]);

        c1 = (c1 * std::conj(c2)) * c3;

        mul = 1.0 / std::norm(c2);
        mul = isinf(mul) ? 0.0 : mul;

        real1[i] = c1.real() * mul;
        imag1[i] = c1.imag() * mul;
    }
}

// N.B. The filter spectrum will be returned in the correct format, but must be large enough to fit a full spectrum

void make_regularisation_filter(FFT_SETUP_D fft_setup, FFT_SPLIT_COMPLEX_D denominator_spectrum, FFT_SPLIT_COMPLEX_D filter_spectrum,
                                 double *beta_in, uintptr_t fft_size, t_spectrum_format format, double phase)
{
    double *real1 = denominator_spectrum.realp;
    double *imag1 = denominator_spectrum.imagp;
    double *real2 = filter_spectrum.realp;
    double *imag2 = filter_spectrum.imagp;

    double filter_amp;

    uintptr_t from = format == SPECTRUM_REAL ? 1 : 0;
    uintptr_t to = format == SPECTRUM_REAL ? fft_size >> 1 : (fft_size >> 1) + 1;

    if (format == SPECTRUM_REAL)
    {
        filter_amp = 1.0 / (1.0 + beta_in[0] / (real1[0] * real1[0]));
        real2[0] = filter_amp * filter_amp;

        filter_amp = 1.0 / (1.0 +  beta_in[fft_size >> 1] / (imag1[0] * imag1[0]));
        real2[fft_size >> 1] = filter_amp * filter_amp;
    }

    for (uintptr_t i = from; i < to; i++)
    {
        filter_amp = 1.0 / (1.0 + beta_in[i] / (real1[i] * real1[i] + imag1[i] * imag1[i]));
        real2[i] = filter_amp * filter_amp;
    }

    for (uintptr_t i = (fft_size >> 1) + 1; i < fft_size; i++)
        real2[i] = real2[fft_size - i];

    for (uintptr_t i = 0; i < fft_size; i++)
        imag2[i] = 0.0;

    variable_phase_from_power_spectrum(fft_setup, filter_spectrum, fft_size, phase, true);

    // Change format to real if necessary (leave subsequent memory in place)

    if (format == SPECTRUM_REAL)
        imag2[0] = real2[fft_size >> 1];
}

// N.B. The filter spectrum will be returned in the correct format, but must be large enough to fit a full spectrum

void make_clip_filter(FFT_SETUP_D fft_setup, FFT_SPLIT_COMPLEX_D denominator_spectrum, FFT_SPLIT_COMPLEX_D filter_spectrum,
                       double *clip_min, double *clip_max, uintptr_t fft_size, t_spectrum_format format, double phase)
{
    double *real1 = denominator_spectrum.realp;
    double *imag1 = denominator_spectrum.imagp;
    double *real2 = filter_spectrum.realp;
    double *imag2 = filter_spectrum.imagp;

    uintptr_t from = format == SPECTRUM_REAL ? 1 : 0;
    uintptr_t to = format == SPECTRUM_REAL ? fft_size >> 1 : (fft_size >> 1) + 1;

    double divisor_power;
    double filter_power;

    // N.B. Power values below

    if (format == SPECTRUM_REAL)
    {
        divisor_power = (real1[0] * real1[0]);
        filter_power = divisor_power < clip_min[0] ? divisor_power / clip_min[0] : 1.0;
        filter_power = divisor_power > clip_max[0] ? divisor_power / clip_max[0] : filter_power;
        real2[0] = filter_power;

        divisor_power = (imag1[0] * imag1[0]);
        filter_power = divisor_power < clip_min[fft_size >> 1] ? divisor_power / clip_min[fft_size >> 1] : 1.0;
        filter_power = divisor_power > clip_max[fft_size >> 1] ? divisor_power / clip_max[fft_size >> 1] : filter_power;
        real2[fft_size >> 1] = filter_power;
    }

    for (uintptr_t i = from; i < to; i++)
    {
        divisor_power = (real1[i] * real1[i] + imag1[i] * imag1[i]);
        filter_power = divisor_power < clip_min[i] ? divisor_power / clip_min[i] : 1.0;
        filter_power = divisor_power > clip_max[i] ? divisor_power / clip_max[i] : filter_power;
        real2[i] = filter_power;
    }

    for (uintptr_t i = (fft_size >> 1) + 1; i < fft_size; i++)
        real2[i] = real2[fft_size - i];

    for (uintptr_t i = 0; i < fft_size; i++)
        imag2[i] = 0.0;

    variable_phase_from_power_spectrum(fft_setup, filter_spectrum, fft_size, phase, true);

    // Change format to real if necessary (leave subsequent memory in place)

    if (format == SPECTRUM_REAL)
        imag2[0] = real2[fft_size >> 1];
}

void deconvolve_regularised_variable_phase(FFT_SETUP_D fft_setup, FFT_SPLIT_COMPLEX_D spectrum_1, FFT_SPLIT_COMPLEX_D spectrum_2,
                                            FFT_SPLIT_COMPLEX_D temp_full_spectrum, double *beta_in, uintptr_t fft_size, t_spectrum_format format, double phase)
{
    make_regularisation_filter(fft_setup, spectrum_2, temp_full_spectrum, beta_in, fft_size, format, phase);
    deconvolve_with_filter (spectrum_1, spectrum_2, temp_full_spectrum, fft_size, format);
}

void deconvolve_clip_variable_phase (FFT_SETUP_D fft_setup, FFT_SPLIT_COMPLEX_D spectrum_1, FFT_SPLIT_COMPLEX_D spectrum_2, FFT_SPLIT_COMPLEX_D temp_full_spectrum,
                                     double *clip_min, double *clip_max, uintptr_t fft_size, t_spectrum_format format, double phase)
{
    make_clip_filter(fft_setup, spectrum_2, temp_full_spectrum, clip_min, clip_max, fft_size, format, phase);
    deconvolve_with_filter (spectrum_1, spectrum_2, temp_full_spectrum, fft_size, format);
}

//////////////////////////////////////////////////////////////////////////
//////////////////// High Level Deconvolution Routines ///////////////////
//////////////////////////////////////////////////////////////////////////

// N.B. The filter spectrum must be large enough to fit a full spectrum

void deconvolve_zero_phase(FFT_SPLIT_COMPLEX_D spectrum_1, FFT_SPLIT_COMPLEX_D spectrum_2, FFT_SPLIT_COMPLEX_D filter_spectrum,
                            double *filter_specifier, double *range_specifier, double filter_db_offset,
                            uintptr_t fft_size, t_spectrum_format format, t_filter_type mode, double sample_rate)
{
    make_freq_dependent_power_array(filter_spectrum.realp, filter_specifier, fft_size, sample_rate, filter_db_offset);

    switch (mode)
    {
        case FILTER_REGULARISATION:
            for (uintptr_t i = 0; i < fft_size; i++)
                filter_spectrum.imagp[i] = 0.0;
            deconvolve_regularised_zero_phase(spectrum_1, spectrum_2, filter_spectrum.realp, fft_size, format);
            break;

        case FILTER_CLIP:
            make_freq_dependent_power_array (filter_spectrum.imagp, range_specifier, fft_size, sample_rate, 0.0);

            // Convert from range to min/max

            for (uintptr_t i = 0; i < (fft_size >> 1); i++)
                filter_spectrum.imagp[i] *= filter_spectrum.realp[i];

            deconvolve_clip_zero_phase(spectrum_1, spectrum_2, filter_spectrum.realp, filter_spectrum.imagp, fft_size, format);
            break;

        case FILTER_FILTER:
            for (uintptr_t i = 0; i < fft_size; i++)
                filter_spectrum.realp[i] = sqrt(filter_spectrum.realp[i]);
            deconvolve_with_amp_filter(spectrum_1, spectrum_2, filter_spectrum.realp, fft_size, format);
            break;
    }
}

// N.B. The filter spectrum will be returned in the correct format, but must be large enough to fit a full spectrum

void make_deconvolution_filter(FFT_SETUP_D fft_setup, FFT_SPLIT_COMPLEX_D denominator_spectrum, FFT_SPLIT_COMPLEX_D filter_spectrum,
                                double *filter_specifier, double *range_specifier, double filter_db_offset, float *filter_in, uintptr_t filter_length,
                                uintptr_t fft_size, t_spectrum_format format, t_filter_type mode, double phase, double sample_rate)
{
    if (filter_in)
    {
        time_to_spectrum_float(fft_setup, filter_in, filter_length, filter_spectrum, fft_size);
        if (mode != FILTER_FILTER)
            power_spectrum(filter_spectrum, fft_size, SPECTRUM_FULL);
    }
    else
        make_freq_dependent_power_array(filter_spectrum.realp, filter_specifier, fft_size, sample_rate, filter_db_offset);

    switch (mode)
    {
        case FILTER_REGULARISATION:
            make_regularisation_filter(fft_setup, denominator_spectrum, filter_spectrum, filter_spectrum.realp, fft_size, format, phase);
            break;

        case FILTER_CLIP:
            make_freq_dependent_power_array(filter_spectrum.imagp, range_specifier, fft_size, sample_rate, 0.0);

            // Convert from range to min/max

            for (uintptr_t i = 0; i < (fft_size >> 1); i++)
                filter_spectrum.imagp[i] *= filter_spectrum.realp[i];

            make_clip_filter(fft_setup, denominator_spectrum, filter_spectrum, filter_spectrum.realp, filter_spectrum.imagp, fft_size, format, phase);
            break;

        case FILTER_FILTER:
            if (!filter_in)
            {
                for (uintptr_t i = 0; i < fft_size; i++)
                    filter_spectrum.imagp[i] = 0.0;

                variable_phase_from_power_spectrum(fft_setup, filter_spectrum, fft_size, phase, true);
            }

            // Change format to real if necessary (leave subsequent memory in place)

            if (format == SPECTRUM_REAL)
                filter_spectrum.imagp[0] = filter_spectrum.realp[fft_size >> 1];

            break;
    }
}

// N.B. The filter spectrum will be returned in the correct format, but must be large enough to fit a full spectrum

void deconvolve_variable_phase(FFT_SETUP_D fft_setup, FFT_SPLIT_COMPLEX_D spectrum_1, FFT_SPLIT_COMPLEX_D spectrum_2, FFT_SPLIT_COMPLEX_D filter_spectrum,
                                double *filter_specifier, double *range_specifier, double filter_db_offset, float *filter_in, uintptr_t filter_length,
                                uintptr_t fft_size, t_spectrum_format format, t_filter_type mode, double phase, double sample_rate)
{
    make_deconvolution_filter(fft_setup, spectrum_2, filter_spectrum, filter_specifier, range_specifier, filter_db_offset, filter_in, filter_length, fft_size, format, mode, phase, sample_rate);
    deconvolve_with_filter(spectrum_1, spectrum_2, filter_spectrum, fft_size, format);
}

// N.B. The filter spectrum will be returned in the correct format, but must be large enough to fit a full spectrum

void deconvolve(FFT_SETUP_D fft_setup, FFT_SPLIT_COMPLEX_D spectrum_1, FFT_SPLIT_COMPLEX_D spectrum_2, FFT_SPLIT_COMPLEX_D filter_spectrum,
                 double *filter_specifier, double *range_specifier, double filter_db_offset, float *filter_in, uintptr_t filter_length,
                 uintptr_t fft_size, t_spectrum_format format, t_filter_type mode, double phase, double delay, double sample_rate)
{
    if (phase == 0.0 && !filter_in)
        deconvolve_zero_phase(spectrum_1, spectrum_2, filter_spectrum, filter_specifier, range_specifier, filter_db_offset, fft_size, format, mode, sample_rate);
    else
        deconvolve_variable_phase(fft_setup, spectrum_1, spectrum_2, filter_spectrum, filter_specifier, range_specifier, filter_db_offset, filter_in, filter_length, fft_size, format, mode, phase, sample_rate);

    if (delay)
        delay_spectrum(spectrum_1, fft_size, format, delay);
}

//////////////////////////////////////////////////////////////////////////
////////////// Analytic Spikes / Delay for Modelling Delays //////////////
//////////////////////////////////////////////////////////////////////////

void spike_spectrum(FFT_SPLIT_COMPLEX_D spectrum, uintptr_t fft_size, t_spectrum_format format, double spike)
{
    long double spike_const = static_cast<long double>(2.0 * M_PI) * (static_cast<double>(fft_size) - spike) / (static_cast<double>(fft_size));
    long double phase;
    uintptr_t i;
    
    spectrum.realp[0] = 1;

    if (format == SPECTRUM_FULL)
        spectrum.imagp[0] = 0.0;
    else
        spectrum.imagp[0] = 1.0;

    for (i = 1; i < fft_size >> 1; i++)
    {
        phase = spike_const * i;
        spectrum.realp[i] = cosl(phase);
        spectrum.imagp[i] = sinl(phase);
    }

    if (format == SPECTRUM_FULL)
    {
        spectrum.realp[i] = 1.0;
        spectrum.imagp[i] = 0.0;

        for (i++; i < fft_size; i++)
        {
            spectrum.realp[i] = spectrum.realp[fft_size - i];
            spectrum.imagp[i] = -spectrum.imagp[fft_size - i];
        }
    }
}

void delay_spectrum(FFT_SPLIT_COMPLEX_D spectrum, uintptr_t fft_size, t_spectrum_format format, double delay)
{
    long double delay_const = static_cast<long double>(2.0 * M_PI) * (-delay / static_cast<double>(fft_size));
    uintptr_t i;
    
    if (!delay)
        return;

    for (i = 1; i < fft_size >> 1; i++)
    {
        long double phase = delay_const * i;

        double a = spectrum.realp[i];
        double b = spectrum.imagp[i];
        double c = cosl(phase);
        double d = sinl(phase);

        spectrum.realp[i] = (a * c) - (b * d);
        spectrum.imagp[i] = (b * c) + (a * d);
    }

    if (format == SPECTRUM_FULL)
    {
        for (i++; i < fft_size; i++)
        {
            spectrum.realp[i] = spectrum.realp[fft_size - i];
            spectrum.imagp[i] = -spectrum.imagp[fft_size - i];
        }
    }
}
