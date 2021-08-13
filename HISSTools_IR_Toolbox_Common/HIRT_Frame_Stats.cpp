

#include "HIRT_Frame_Stats.hpp"

//////////////////////////////////////////////////////////////////////////
////////////////////////////// Constructor ///////////////////////////////
//////////////////////////////////////////////////////////////////////////

t_frame_stats::t_frame_stats(uintptr_t max_N)
: m_current_frame(max_N, 0.0)
, m_ages(max_N, 0.0)
, m_max_N(max_N)
{
    reset(true);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////// Reset and Set Parameters ////////////////////////
//////////////////////////////////////////////////////////////////////////

void t_frame_stats::reset(bool full)
{
    m_last_N = 0;
    m_frames = 0;

    if (full)
    {
        m_mode = MODE_COPY;
        m_max_age = 30;
        m_alpha_u = 0.5;
        m_alpha_d = 0.5;

        for (uintptr_t i = 0; i < m_max_N; i++)
        {
            m_current_frame[i] = 0.0;
            m_ages[i] = 0;
        }
    }
}

void t_frame_stats::set_mode(t_frame_mode mode)
{
    t_frame_mode old_mode = m_mode;

    switch (mode)
    {
        case MODE_COPY:         m_mode = MODE_COPY;         break;
        case MODE_ACCUMULATE:   m_mode = MODE_ACCUMULATE;   break;
        case MODE_PEAKS:        m_mode = MODE_PEAKS;        break;
        case MODE_SMOOTH:       m_mode = MODE_SMOOTH;       break;
        default:
            m_mode = MODE_COPY;

    }

    if (old_mode != m_mode)
        reset(false);
}

void t_frame_stats::set_max_age(uintptr_t max_age)
{
    m_max_age = max_age;
}

void t_frame_stats::set_alpha(double alpha_u, double alpha_d)
{
    m_alpha_u = std::max(0.0, std::min(1.0, alpha_u));
    m_alpha_d = std::max(0.0, std::min(1.0, alpha_d));
}

//////////////////////////////////////////////////////////////////////////
///////////////////////////// Read and Write /////////////////////////////
//////////////////////////////////////////////////////////////////////////

void t_frame_stats::write(float *in, uintptr_t N)
{
    if (N != m_last_N)
        reset(false);

    switch (m_mode)
    {
        case MODE_COPY:

            for (uintptr_t i = 0; i < N; i++)
                m_current_frame[i] = in[i];
            m_frames = 1;
            break;

        case MODE_ACCUMULATE:

            if (m_frames)
            {
                for (uintptr_t i = 0; i < N; i++)
                    m_current_frame[i] += in[i];
            }
            else
            {
                for (uintptr_t i = 0; i < N; i++)
                    m_current_frame[i] = in[i];
            }
            m_frames++;
            break;

        case MODE_PEAKS:

            if (m_frames)
            {
                for (uintptr_t i = 0; i < N; i++)
                {
                    if (++m_ages[i] > m_max_age || in[i] > m_current_frame[i])
                    {
                        m_current_frame[i] = in[i];
                        m_ages[i] = 0;
                    }
                }
            }
            else
            {
                for (uintptr_t i = 0; i < N; i++)
                {
                    m_current_frame[i] = in[i];
                    m_ages[i] = 0;
                }
            }
            m_frames = 1;
            break;

        case MODE_SMOOTH:

            if (m_frames)
            {
                double alpha;
                double in_val;
                double last_val;

                for (uintptr_t i = 0; i < N; i++)
                {
                    in_val = in[i];
                    last_val = m_current_frame[i];

                    if (in_val > last_val)
                        alpha = m_alpha_u;
                    else
                        alpha = m_alpha_d;

                    m_current_frame[i] = last_val + alpha * (in_val - last_val);
                }
            }
            else
            {
                for (uintptr_t i = 0; i < N; i++)
                    m_current_frame[i] = in[i];
            }
            m_frames = 1;
            break;

    }

    m_last_N = N;
}

void t_frame_stats::read(float *out, uintptr_t N)
{
    double recip = m_mode == MODE_ACCUMULATE ? 1.0 / m_frames : 1.0;

    if (m_frames)
    {
        for (uintptr_t i = 0; i < N; i++)
            out[i] = static_cast<float>(m_current_frame[i] * recip);
    }
    else
    {
        for (uintptr_t i = 0; i < N; i++)
            out[i] = 0.f;
    }
}
