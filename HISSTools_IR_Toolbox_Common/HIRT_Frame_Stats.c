

#include "HIRT_Frame_Stats.h"

#ifndef __APPLE__
#include <AH_Win_Complex_Math.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#endif


//////////////////////////////////////////////////////////////////////////
//////////////////////////// Create / Destroy ////////////////////////////
//////////////////////////////////////////////////////////////////////////


t_frame_stats *create_frame_stats(AH_UIntPtr max_N)
{
    t_frame_stats *stats = (t_frame_stats *)malloc(sizeof(t_frame_stats));
    
    if (stats)
    {
        stats->max_N = max_N;
        stats->current_frame = (double *) malloc(sizeof(double) * max_N);
        stats->ages = (AH_UInt32 *) malloc(sizeof(AH_UInt32) * max_N);
        
        if (stats->current_frame)
            frame_stats_reset(stats, true);
        else 
        {
            free(stats->current_frame);
            free(stats->ages);
            free(stats);
            stats = NULL;
        }
    }
    
    return stats;
}


void destroy_frame_stats(t_frame_stats *stats)
{
    if (stats)
    {
        free(stats->current_frame);
        free(stats->ages);
    }
    
    free(stats);
}


//////////////////////////////////////////////////////////////////////////
//////////////////////// Reset and Set Parameters ////////////////////////
//////////////////////////////////////////////////////////////////////////


void frame_stats_reset(t_frame_stats *stats, AH_Boolean full)
{
    double *current_frame = stats->current_frame;
    AH_UInt32 *ages = stats->ages;
    AH_UIntPtr i;
    
    stats->last_N = 0;
    stats->frames = 0;
    
    if (full == true)
    {    
        stats->mode = MODE_COPY;
        stats->max_age = 30;
        stats->alpha_u = 0.5;
        stats->alpha_d = 0.5;
        
        for (i = 0; i < stats->max_N; i++)
        {
            current_frame[i] = 0.0;
            ages[i] = 0;
        }
    }
}


void frame_stats_mode(t_frame_stats *stats, t_frame_mode mode)
{
    t_frame_mode old_mode = stats->mode;
    
    switch (mode) 
    {
        case MODE_COPY:
            stats->mode = MODE_COPY;
            break;
            
        case MODE_ACCUMULATE:
            stats->mode = MODE_ACCUMULATE;
            break;
            
        case MODE_PEAKS:
            stats->mode = MODE_PEAKS;
            break;
            
        case MODE_SMOOTH:
            stats->mode = MODE_SMOOTH;
            break;
            
        default:
            stats->mode = MODE_COPY;
            
    }
    
    if (old_mode != stats->mode)
        frame_stats_reset(stats, false);
}


void frame_stats_max_age(t_frame_stats *stats, AH_UInt32 max_age)
{    
    stats->max_age = max_age;    
}


void frame_stats_alpha(t_frame_stats *stats, double alpha_u, double alpha_d)
{    
    if (alpha_u > 1.0)
        alpha_u = 1.0;
    if (alpha_u < 0.0)
        alpha_u = 0.0;
    
    if (alpha_d > 1.0)
        alpha_d = 1.0;
    if (alpha_d < 0.0)
        alpha_d = 0.0;
    
    stats->alpha_u = alpha_u;        
    stats->alpha_d = alpha_d;    
}


//////////////////////////////////////////////////////////////////////////
///////////////////////////// Read and Write /////////////////////////////
//////////////////////////////////////////////////////////////////////////


void frame_stats_write(t_frame_stats *stats, float *in, AH_UIntPtr N)
{
    double *current_frame = stats->current_frame;
    double alpha_u = stats->alpha_u;
    double alpha_d = stats->alpha_d;
    double alpha;
    
    AH_UInt32 *ages = stats->ages;
    AH_UInt32 max_age = stats->max_age;
    AH_UIntPtr i;    
    
    if (N != stats->last_N)
        frame_stats_reset(stats, false);
    
    switch (stats->mode)
    {
        case MODE_COPY:
            
            for (i = 0; i < N; i++)
                current_frame[i] = in[i];
            stats->frames = 1;
            
            break;
            
        case MODE_ACCUMULATE:
            
            if (stats->frames)
            {
                for (i = 0; i < N; i++)
                    current_frame[i] += in[i];
            }    
            else
            {
                for (i = 0; i < N; i++)
                    current_frame[i] = in[i];
            }
            stats->frames++;
            
            break;
            
        case MODE_PEAKS:
            
            if (stats->frames)
            {
                for (i = 0; i < N; i++)
                {
                    if (++ages[i] > max_age || in[i] > current_frame[i])
                    {
                        current_frame[i] = in[i];
                        ages[i] = 0;
                    }
                }
            }    
            else
            {
                for (i = 0; i < N; i++)
                {
                    current_frame[i] = in[i];
                    ages[i] = 0;
                }
            }
            stats->frames = 1;
            
            break;
            
        case MODE_SMOOTH:
            
            if (stats->frames)
            {
                double in_val;
                double last_val;
                
                for (i = 0; i < N; i++)
                {
                    in_val = in[i];
                    last_val = current_frame[i];
                    
                    if (in_val > last_val)
                        alpha = alpha_u;
                    else 
                        alpha = alpha_d;
                    
                    current_frame[i] = last_val + alpha * (in_val - last_val);
                }
            }
            else
            {
                for (i = 0; i < N; i++)
                    current_frame[i] = in[i];
            }    
            
            stats->frames = 1;
            
            break;
            
    }
    
    stats->last_N = N;
}


void frame_stats_read(t_frame_stats *stats, float *out, AH_UIntPtr N)
{
    double *current_frame = stats->current_frame;
    double recip = 1.0 / stats->frames;
    
    AH_UIntPtr i;    
    
    if (stats->mode != MODE_ACCUMULATE)
        recip = 1.0;
    
    if (stats->frames)
    {
        for (i = 0; i < N; i++)
            out[i] = (float) (current_frame[i] * recip);
    }
    else
    {
        for (i = 0; i < N; i++)
            out[i] = 0.f;
    }
}

