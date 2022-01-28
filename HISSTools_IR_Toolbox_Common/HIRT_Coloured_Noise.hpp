
#ifndef __HIRT_COLOURED_NOISE__
#define __HIRT_COLOURED_NOISE__

#include <algorithm>
#include <cstdint>
#include <cmath>

// Mode Enum

enum t_noise_mode
{
    NOISE_MODE_WHITE = 0,
    NOISE_MODE_BROWN = 1,
    NOISE_MODE_PINK = 2,
};

// Noise Class

template <class T>
class t_noise_gen
{
    // Filter Calculator
    
    static double filter_calc(double f0, double sample_rate)
    {
        return std::sin(M_PI * 2.0 * f0 / sample_rate);
    }
    
    // White Noise Generator Class
    
    class white_noise
    {
    public:
        
        void reset()
        {
            w = x = y = 0;
            z = 4294967295u;
        }
        
        double operator()()
        {
            constexpr double UNSIGNED_INT32_TO_NORM_DOUBLE = 2.32830643653869628906e-10;
            
            uint32_t r = (x ^ (x << 20)) ^ (y ^ (y >> 11)) ^ (z ^ (z << 27)) ^ (w ^ (w >> 6));
            
            x = y;
            y = z;
            z = w;
            w = r;
            
            return (r * UNSIGNED_INT32_TO_NORM_DOUBLE * 2.0) - 1.0;
        }
        
    private:
        
        uint32_t w, x, y, z;
    };
    
    // Browning Filter Class

    class browning_filter
    {
    public:
        
        // Browning filter coefficient - N.B cf is 16Hz

        browning_filter(double sample_rate)
        : alpha(filter_calc(16.0, sample_rate))
        {}

        void reset() { prev = 0.0; }
        
        double operator()(double in)
        {
            prev = prev + (alpha * (in - prev));
            return prev;
        }
        
    private:
        
        double prev;
        double alpha;
    };
    
    // Pinking Filter Class

    class pinking_filter
    {
    public:
        
        pinking_filter(double sample_rate)
        : alpha0(filter_calc(   8.00135734209627, sample_rate))
        , alpha1(filter_calc(  46.88548507044182, sample_rate))
        , alpha2(filter_calc( 217.61558695916962, sample_rate))
        , alpha3(filter_calc( 939.80665948455472, sample_rate))
        , alpha4(filter_calc(3276.10128392439381, sample_rate))
        {}
        
        void reset()
        {
            prev = 0.0;
            b0 = b1 = b2 = b3 = b4 = b5 = b6 = 0.0;
        }
        
        double operator()(double in)
        {
            b0 = b0 + (alpha0 * ((in * 48.69991228070175) - b0));
            b1 = b1 + (alpha1 * ((in * 11.23890718562874) - b1));
            b2 = b2 + (alpha2 * ((in * 4.96296774193548) - b2));
            b3 = b3 + (alpha3 * ((in * 2.32573483146067) - b3));
            b4 = b4 + (alpha4 * ((in * 1.18433822222222) - b4));
            b5 = -0.7616 * b5 - in * 0.0168980;
            double out = (b0 + b1 + b2 + b3 + b4 + b5 + b6 + in * 0.5362);
            b6 = in * 0.115926;
            
            return out;
        }
        
    private:
        
        double prev;
        double alpha0, alpha1, alpha2, alpha3, alpha4;
        double b0, b1, b2, b3, b4, b5, b6;
    };
    
public:
    
    // N.B. Times in seconds
    
    t_noise_gen(t_noise_mode mode, double fade_in, double fade_out, double length, double sample_rate, double amp)
    : m_amp(amp)
    , m_sample_rate(sample_rate)
    , m_fade_in(fade_in)
    , m_fade_out(fade_out)
    //, m_RT(length)
    , m_T (static_cast<uintptr_t>(length * sample_rate))
    , m_mode(mode > 2 ? NOISE_MODE_WHITE : mode)
    , m_browning(sample_rate)
    , m_pinking(sample_rate)
    {
        reset();
    }

    void reset()
    {
        m_gen.reset();
        m_browning.reset();
        m_pinking.reset();
    }
    
    void set_amp(double amp) { m_amp = amp; }
    
    double amp() const { return m_amp; }
    t_noise_mode mode() const { return m_mode; }
    
    uintptr_t length() const { return m_T; }
    
    void gen(T *out, uintptr_t N)
    {
        const double FiN = std::max(1.0, m_fade_in  * m_sample_rate * 2.0);
        const double FoN = std::max(1.0, m_fade_out * m_sample_rate * 2.0);
        
        switch (m_mode)
        {
            case NOISE_MODE_WHITE:
                for (uintptr_t i = 0; i < N; i++)
                    *out++ = static_cast<T>(fade(i, FiN, FoN) * m_gen() * m_amp);
                break;
                
            case NOISE_MODE_BROWN:
                for (uintptr_t i = 0; i < N; i++)
                    *out++ = static_cast<T>(fade(i, FiN, FoN) * m_browning(m_gen()) * m_amp);
                break;
                
            case NOISE_MODE_PINK:
                for (uintptr_t i = 0; i < N; i++)
                    *out++ = static_cast<T>(fade(i, FiN, FoN) * m_pinking(m_gen()) * m_amp);
                break;
        }
    }
    
    void gen(T *out)
    {
        gen(out, m_T);
    }

    void measure(uintptr_t N, double&  max_pink, double& max_brown)
    {
        max_brown = 0.0;
        max_pink = 0.0;
        
        for (uintptr_t i = 0; i < N; i++)
            max_brown = std::max(std::fabs(m_browning(m_gen())), max_brown);
        
        reset();
        
        for (uintptr_t i = 0; i < N; i++)
            max_pink = std::max(std::fabs(m_pinking(m_gen())), max_pink);
        
        reset();
    }
    
private:
    
    double fade(uintptr_t i, double FiN, double FoN)
    {
        double fade_in  = (1.0 - std::cos(M_PI * std::min(0.5, i / FiN)));
        double fade_out = (1.0 - std::cos(M_PI * std::min(0.5, (m_T - i) / FoN)));
        
        return fade_in * fade_out;
    }
    
    double m_amp;
    double m_sample_rate;

    double m_fade_in;
    double m_fade_out;

    uintptr_t m_T;

    t_noise_mode m_mode;
    white_noise m_gen;
    browning_filter m_browning;
    pinking_filter m_pinking;
};

#endif    /* __HIRT_COLOURED_NOISE__ */
