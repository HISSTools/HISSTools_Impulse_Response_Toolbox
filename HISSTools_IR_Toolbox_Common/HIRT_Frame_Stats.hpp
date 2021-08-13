
#ifndef __HIRT_FRAME_STATS_
#define __HIRT_FRAME_STATS_

#include <algorithm>
#include <cstdint>
#include <vector>

// Mode Enum

enum t_frame_mode
{
    MODE_COPY = 0,
    MODE_PEAKS = 1,
    MODE_SMOOTH = 2,
    MODE_ACCUMULATE = 3,
};

// Frame Stats Struct

class t_frame_stats
{
public:
        
    t_frame_stats(uintptr_t max_N);
    
    void reset(bool full);
    
    void set_mode(t_frame_mode mode);
    void set_max_age(uintptr_t max_age);
    void set_alpha(double alpha_u, double alpha_d);
    
    void write(float *in, uintptr_t N);
    void read(float *out, uintptr_t N);
    
private:
    
    std::vector<double> m_current_frame;
    std::vector<uintptr_t> m_ages;

    double m_alpha_u;
    double m_alpha_d;

    uintptr_t m_max_age;

    uintptr_t m_frames;
    uintptr_t m_max_N;
    uintptr_t m_last_N;

    t_frame_mode m_mode;
};

#endif /*__HIRT_FRAME_STATS_ */
