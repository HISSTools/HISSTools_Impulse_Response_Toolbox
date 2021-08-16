
#ifndef __HIRT_MAXIMUM_LENGTH_SEQUENCES__
#define __HIRT_MAXIMUM_LENGTH_SEQUENCES__

#include <algorithm>
#include <cstdint>

// MLS Class

template <class T>
class t_mls
{
public:
    
    t_mls(uint32_t log2_T, double amp)
    : m_lfsr(0x1u), m_amp(amp)
    {
        log2_T = std::max(1u, std::min(log2_T, 24u));
        m_T = (1u << log2_T) - 1u;
        m_order = log2_T;
        m_feedback_mask = feedback_mask(log2_T);
    }
    
    void reset() {  m_lfsr = 0x1u; }
    
    void set_amp(double amp) { m_amp = amp; }

    uint32_t length() const { return m_T; }
    uint32_t order()  const { return m_order; }
    double amp() const { return m_amp; }
    
    void gen(T *out, uintptr_t N)
    {
        T amp = (T) m_amp;
        T two_amp = amp * 2;
        
        for (uintptr_t i = 0; i < N; i++)
        {
            *out++ = ((m_lfsr & 0x1u) * two_amp) - amp;
            m_lfsr = get_next_lfsr_int();
        }
    }
    
    void gen(T *out)
    {
        gen(out, m_T);
    }
    
private:
    
    uint32_t get_next_lfsr_int()
    {
        return (m_lfsr >> 1) ^ ((uint32_t(0) - (m_lfsr & 0x1u)) & m_feedback_mask);
    }
    
    uint32_t feedback_mask(uint32_t N)
    {
        constexpr uint32_t feedback_mask_vals[] =
        {
            0x0u,
            0x0u,
            0x2u,
            0x6u,
            0xCu,
            0x14u,
            0x30u,
            0x60u,
            0xE1u,
            0x100u,
            0x240u,
            0x500u,
            0xE08u,
            0x1C80u,
            0x3802u,
            0x6000u,
            0xD008u,
            0x12000u,
            0x20400u,
            0x72000u,
            0x90000u,
            0x500000u,
            0xC00000u,
            0x420000u,
            0xE10000u
        };
        
        return feedback_mask_vals[N];
    }

    uint32_t m_feedback_mask;
    uint32_t m_lfsr;

    uint32_t m_T;
    uint32_t m_order;

    double m_amp;
};

#endif    /* __HIRT_MAXIMUM_LENGTH_SEQUENCES__ */
