
#ifndef __HIRT_FRAME_STATS_
#define __HIRT_FRAME_STATS_

#include <cstdint>

// Mode Enum

enum t_frame_mode
{
    MODE_COPY = 0,
    MODE_PEAKS = 1,
    MODE_SMOOTH = 2,
    MODE_ACCUMULATE = 3,
};

// Frame Stats Struct

struct t_frame_stats
{
    double *current_frame;
    uint32_t *ages;

    double alpha_u;
    double alpha_d;

    uint32_t max_age;

    uintptr_t frames;
    uintptr_t max_N;
    uintptr_t last_N;

    t_frame_mode mode;
};

// Function Prototypes

t_frame_stats *create_frame_stats(uintptr_t max_N);
void destroy_frame_stats(t_frame_stats *stats);

void frame_stats_reset(t_frame_stats *stats, bool full);
void frame_stats_mode(t_frame_stats *stats, t_frame_mode mode);
void frame_stats_max_age(t_frame_stats *stats, uint32_t max_age);
void frame_stats_alpha(t_frame_stats *stats, double alpha_u, double alpha_d);

void frame_stats_write(t_frame_stats *stats, float *in, uintptr_t N);
void frame_stats_read(t_frame_stats *stats, float *out, uintptr_t N);

#endif /*__HIRT_FRAME_STATS_ */
