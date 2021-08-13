
#ifndef __HIRT_EXPONENTIAL_SWEEPS__
#define __HIRT_EXPONENTIAL_SWEEPS__

#include <algorithm>
#include <cstdint>
#include <cmath>

// ESS Class  

enum t_invert_mode
{
    INVERT_USER_CURVE_TO_FIXED_REFERENCE = 0,   // Output at fixed level, invert only the user amp curve
    INVERT_USER_CURVE_AND_SWEEP = 1,            // Output inverting the user amp curve and sweep but not the overall amplitude
    INVERT_ALL = 2,                             // Output inverting the user amp curve, sweep and the overall amplitude
};
    
class t_ess
{
public:
    
    t_ess(double f1, double f2, double fade_in, double fade_out, double T, double sample_rate, double amp, double *amp_curve)
    : m_fade_in(fade_in)
    , m_fade_out(fade_out)
    , m_sample_rate(sample_rate)
    , m_amp(amp)
    {
        // All times in seconds
        
        double last_db_val;
        long num_invalid = 0;
        long i;
        
        f1 /= sample_rate;
        f2 /= sample_rate;
        
        T *= sample_rate;
        
        double L = std::round(f1 * T / (log(f2 / f1))) / f1;
        
        m_K1 = 2 * M_PI * f1 * L;
        m_K2 = 1 / L;
        
        double NT = std::round(f1 * T / (log(f2 / f1))) * log(f2 / f1) / f1;
        double final_phase = std::floor(L * f1 * (exp(NT * m_K2) - 1));
        double NNT = std::ceil(log((final_phase / L / f1 + 1)) / m_K2);
        
        m_T = static_cast<uintptr_t>(NNT);
        m_lo_f_act = f1;
        m_hi_f_act = f1 * exp(NNT / L);
        m_f1 = f1;
        m_f2 = f2;
        
        // Design amplitude specifier
        
        for (i = 0; amp_curve && (i < 32); i++)
        {
            if (isinf(amp_curve[i]))
                break;
        }
        
        long num_items = i >> 1;
        
        // Start of amp curve
        
        m_amp_specifier[0] = num_items ? log(amp_curve[0] / (f1 * sample_rate)) : 0.0;
        m_amp_specifier[1] = last_db_val = num_items ? amp_curve[1] : 0.0;
        
        if (m_amp_specifier[0] > 0.0)
            m_amp_specifier[0] = 0.0;
        
        // Intermediate points
        
        for (i = 0; i < num_items; i++)
        {
            m_amp_specifier[2 * (i - num_invalid) + 2] = log(amp_curve[2 * i] / (f1 * sample_rate));
            m_amp_specifier[2 * (i - num_invalid) + 3] = amp_curve[2 * i + 1];
            
            // Sanitize values - If frequencies do not increase then ignore this pair
            
            if (m_amp_specifier[2 * i + 2] < m_amp_specifier[2 * i])
                num_invalid++;
            else
                last_db_val = m_amp_specifier[2 * (i - num_invalid) + 3];
        }
        
        num_items -= num_invalid;
        
        // Endstop
        
        m_amp_specifier[2 * num_items + 2] = HUGE_VAL;
        m_amp_specifier[2 * num_items + 3] = last_db_val;
        
        m_num_amp_specifiers = num_items;
    }

    void set_amp(double amp) { m_amp = amp; }

    uintptr_t length() const { return m_T; }
    double amp() const { return m_amp; }

    double harm_offset(uintptr_t harm) const
    {
        return m_T / log(m_hi_f_act / m_lo_f_act) * log(static_cast<double>(harm));
    }
    
    uintptr_t gen(void *out, uintptr_t start, uintptr_t N, bool double_precision)
    {
        if (double_precision)
            return gen_internal(reinterpret_cast<double *>(out), start, N);
        else
            return gen_internal(reinterpret_cast<float  *>(out), start, N);
    }
    
    uintptr_t igen(void *out, uintptr_t start, uintptr_t N, t_invert_mode inv_amp, bool double_precision)
    {
        if (double_precision)
            return igen_internal(reinterpret_cast<double *>(out), start, N, inv_amp);
        else
            return igen_internal(reinterpret_cast<float  *>(out), start, N, inv_amp);
    }
    
    uintptr_t gen(void *out, bool double_precision)
    {
        if (double_precision)
            return gen_internal(reinterpret_cast<double *>(out), 0, m_T);
        else
            return gen_internal(reinterpret_cast<float  *>(out), 0, m_T);
    }
    
    uintptr_t igen(void *out, t_invert_mode inv_amp, bool double_precision)
    {
        if (double_precision)
            return igen_internal(reinterpret_cast<double *>(out), 0, m_T, inv_amp);
        else
            return igen_internal(reinterpret_cast<float  *>(out), 0, m_T, inv_amp);
    }
    
private:

    template <typename T>
    uintptr_t gen_internal(T *out, uintptr_t start, uintptr_t N)
    {
        const double FiN = std::max(1.0, m_fade_in  * m_sample_rate * 2.0);
        const double FoN = std::max(1.0, m_fade_out * m_sample_rate * 2.0);
        
        N = constrainN(start, N);
        
        for (uintptr_t i = start, j = 0; i < start + N; i++)
        {
            // Time value, find amp curve point, amplitude and final sweep
            
            const double t = i * m_K2;
            
            for ( ; t > m_amp_specifier[j + 2]; j += 2);
            
            const double a = std::pow(10.0, curve(t, j) / 20.0) * fade(i, FiN, FoN) * m_amp;
            *out++ = static_cast<T>(a * std::sin(m_K1 * (std::exp(t) - 1.0)));
        }
        
        return N;
    }
    
    template <typename T>
    uintptr_t igen_internal(T *out, uintptr_t start, uintptr_t N, t_invert_mode inv_amp)
    {
        const double FiN = std::max(1.0, m_fade_in  * m_sample_rate * 2.0);
        const double FoN = std::max(1.0, m_fade_out * m_sample_rate * 2.0);
        
        double amp = (inv_amp == INVERT_ALL) ? m_amp : 1.0;
        double amp_const = (inv_amp == INVERT_USER_CURVE_TO_FIXED_REFERENCE) ? m_amp / exp((m_T - 1.0) * m_K2) : (4.0 * m_lo_f_act * m_K2) / amp;
        
        N = constrainN(start, N);
        
        for (uintptr_t i = start, j = 2 * m_num_amp_specifiers; i < start + N; i++)
        {
            // Time value, find amp curve point, amplitude and final sweep
            
            const uintptr_t k = m_T - i - 1;
            const double t = k * m_K2;
            
            for ( ; t < m_amp_specifier[j]; j -= 2);
            
            const double a = std::pow(10.0, -curve(t, j) / 20.0) * fade(k, FiN, FoN) * amp_const;
            const double e = std::exp(t);
            *out++ = static_cast<T>(a * e * std::sin(m_K1 * (e - 1.0)));
        }
        
        return N;
    }
    
    double fade(uintptr_t i, double FiN, double FoN)
    {
        double fade_in  = (1.0 - std::cos(M_PI * std::min(0.5, i / FiN)));
        double fade_out = (1.0 - std::cos(M_PI * std::min(0.5, (m_T - i) / FoN)));
        
        return fade_in * fade_out;
    }
    
    double curve(double time, uintptr_t pos)
    {
        double delta = (m_amp_specifier[pos + 2] - m_amp_specifier[pos]);
        double interp = delta ? ((time - m_amp_specifier[pos]) / delta) : 0.0;
        return m_amp_specifier[pos + 1] + interp * (m_amp_specifier[pos + 3] - m_amp_specifier[pos + 1]);
    }
    
    uintptr_t constrainN(uintptr_t startN, uintptr_t N)
    {
        N = (startN > m_T) ? 0 : N;
        return (startN + N > m_T) ? m_T - startN : N;
    }
    
    // Intermediates
    
    uintptr_t m_T;
    double m_K1;
    double m_K2;

    double m_lo_f_act;
    double m_hi_f_act;

    double m_f1;
    double m_f2;

    // Parameters requested

    double m_fade_in;
    double m_fade_out;

    double m_sample_rate;
    double m_amp;

    // Amplitude curve

    uintptr_t m_num_amp_specifiers;

    double m_amp_specifier[36];
};

#endif /* __HIRT_EXPONENTIAL_SWEEPS__ */
