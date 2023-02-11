
/*
 *  ibuffer~.hpp
 *
 *  Provides the structure for an ibuffer, so other objects can directly access fields.
 *  However, in most cases, the best method to retrieve data from an ibuffer is to use ibuffer_access.cpp.
 *  ibuffer_access.cpp can also be used to access standard Max buffers.
 *
 *  Copyright 2010-20 Alex Harker. All rights reserved.
 *
 */

#ifndef IBUFFER_H
#define IBUFFER_H

#include <ext.h>
#include <z_dsp.h>
#include <buffer.h>
#include <stdint.h>

// Format enum

// N.B. - size is for backwards compatibility

enum t_ibuffer_format : long {
    
    PCM_FLOAT    = 0,
    PCM_INT_16   = 2,
    PCM_INT_24   = 3,
    PCM_INT_32   = 4,
};

// ibuffer~ object structure

struct t_ibuffer
{
    t_pxobject x_obj;
    
    t_symbol *name;             // the current name of this ibuffer~
    
    void *memory;               // where the memory starts (including zero padding)
    void *samples;              // where the samples start
    
    double sr;                  // sample rate of ibuffer~
    
    t_ptr_int frames;           // number of frames (can be large under 64bit - signed for backwards compatibility)
    long channels;              // number of channels
    t_ibuffer_format format;    // sample format
    
    t_int32_atomic valid;       // set to 0 when loading / unable to load
    t_int32_atomic inuse;       // count for num objects currently using this ibuffer~
    
    void *bang_out;             // bangs on load
};

#endif        /* IBUFFER_H */

