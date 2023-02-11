
/*
 *  ibuffer_access.cpp
 *
 *  Provides code for accessing and interpolating samplesfrom an ibuffer (or standard MSP buffer).
 *  Various kinds of interpolation are supported.
 *  All pointers used should be 16-byte aligned.
 *
 *  See the accompanying header file for more details.
 *
 *  Copyright 2010-20 Alex Harker. All rights reserved.
 *
 */

#include "ibuffer_access.hpp"

t_symbol *ibuffer_data::ps_buffer = gensym("buffer~");
t_symbol *ibuffer_data::ps_ibuffer = gensym("ibuffer~");

// IBuffer Proxy

ibuffer_data::ibuffer_data(t_symbol *name)
: buffer_type(kBufferNone), samples(nullptr), length(0), num_chans(0), format(PCM_FLOAT), sample_rate(0.0)
{
    buffer_object = name ? name->s_thing : nullptr;
    acquire_buffer();
}

ibuffer_data::~ibuffer_data()
{
    release_buffer();
}

void ibuffer_data::set_dirty()
{
    if (buffer_type == kBufferMaxBuffer)
        object_method(buffer_object, gensym("dirty"));
}

void ibuffer_data::set_size_in_samples(t_atom_long size)
{
    t_atom temp_atom[2];
    
    atom_setlong(temp_atom, size);
    
    if (buffer_type == kBufferMaxBuffer)
    {        
        release_buffer();
        object_method_typed(buffer_object, gensym("sizeinsamps"), 1, temp_atom, temp_atom + 1);
        acquire_buffer();
    }
}

void ibuffer_data::acquire(t_symbol *name)
{
    // N.B. - release (and init data using user call) before acquiring the new buffer
    
    release();
    buffer_object = name ? name->s_thing : nullptr;
    acquire_buffer();
}

void ibuffer_data::release()
{
    release_buffer();
    buffer_type = kBufferNone;
    samples = nullptr;
    length = 0;
    num_chans = 0;
    format = PCM_FLOAT;
    sample_rate = 0.0;
    buffer_object = nullptr;
}

void ibuffer_data::acquire_buffer()
{
    if (buffer_object)
    {
        if (ob_sym(buffer_object) == ps_buffer)
        {
            samples = static_cast<void *>(buffer_locksamples(buffer_object));

            if (samples)
            {
                t_buffer_info info;
                buffer_getinfo(buffer_object, &info);
                
                buffer_type = kBufferMaxBuffer;
                length = info.b_frames;
                num_chans = info.b_nchans;
                format = PCM_FLOAT;
                sample_rate = info.b_sr;
            }
        }
        
        if (ob_sym(buffer_object) == ps_ibuffer)
        {
            t_ibuffer *buffer = reinterpret_cast<t_ibuffer *>(buffer_object);
            
            ATOMIC_INCREMENT(&buffer->inuse);
            
            if (buffer->valid)
            {
                buffer_type = kBufferIBuffer;
                samples = buffer->samples;
                length = buffer->frames;
                num_chans = buffer->channels;
                format = buffer->format;
                sample_rate = buffer->sr;
            }
            else
                ATOMIC_DECREMENT(&buffer->inuse);
        }
    }
}

void ibuffer_data::release_buffer()
{
    if (buffer_type == kBufferMaxBuffer)
        buffer_unlocksamples(buffer_object);
    else if (buffer_type == kBufferIBuffer)
        ATOMIC_DECREMENT(&reinterpret_cast<t_ibuffer *>(buffer_object)->inuse);
}

// Functions

template <class T, class U>
void ibuffer_read_format(const ibuffer_data& buffer, T *out, U *positions, intptr_t n_samps, long chan, T mul, InterpType interp)
{
    switch (buffer.get_format())
    {
        case PCM_FLOAT:     table_read(fetch_float(buffer, chan), out, positions, n_samps, mul, interp);    break;
        case PCM_INT_16:    table_read(fetch_16bit(buffer, chan), out, positions, n_samps, mul, interp);    break;
        case PCM_INT_24:    table_read(fetch_24bit(buffer, chan), out, positions, n_samps, mul, interp);    break;
        case PCM_INT_32:    table_read(fetch_32bit(buffer, chan), out, positions, n_samps, mul, interp);    break;
    }
}

void ibuffer_read(const ibuffer_data& buffer, double *out, const double *positions, intptr_t n_samps, long chan, double mul, InterpType interp)
{
    ibuffer_read_format<double>(buffer, out, positions, n_samps, chan, mul, interp);
}

void ibuffer_read(const ibuffer_data& buffer, float *out, const double *positions, intptr_t n_samps, long chan, float mul, InterpType interp)
{
    ibuffer_read_format<float>(buffer, out, positions, n_samps, chan, mul, interp);
}

void ibuffer_read(const ibuffer_data& buffer, float *out, const float *positions, intptr_t n_samps, long chan, float mul, InterpType interp)
{
    ibuffer_read_format<float>(buffer, out, positions, n_samps, chan, mul, interp);
}

template <class T, class U>
void ibuffer_read_format_edges(const ibuffer_data& buffer, T *out, U *positions, intptr_t n_samps, long chan, T mul, InterpType interp, EdgeMode edges, bool bound)
{
    switch (buffer.get_format())
    {
        case PCM_FLOAT:
        {
            fetch_float fetch(buffer, chan);
            table_read_edges(fetch, out, positions, n_samps, mul, interp, edges, bound);
            break;
        }
            
        case PCM_INT_16:
        {
            fetch_16bit fetch(buffer, chan);
            table_read_edges(fetch, out, positions, n_samps, mul, interp, edges, bound);
            break;
        }
            
        case PCM_INT_24:
        {
            fetch_24bit fetch(buffer, chan);
            table_read_edges(fetch, out, positions, n_samps, mul, interp, edges, bound);
            break;
        };
            
        case PCM_INT_32:
        {
            fetch_32bit fetch(buffer, chan);
            table_read_edges(fetch, out, positions, n_samps, mul, interp, edges, bound);
            break;
        };
    }
}

void ibuffer_read_edges(const ibuffer_data& buffer, double *out, const double *positions, intptr_t n_samps, long chan, double mul, InterpType interp, EdgeMode edges, bool bound)
{
    ibuffer_read_format_edges<double>(buffer, out, positions, n_samps, chan, mul, interp, edges, bound);
}

void ibuffer_read_edges(const ibuffer_data& buffer, float *out, const double *positions, intptr_t n_samps, long chan, float mul, InterpType interp, EdgeMode edges, bool bound)
{
    ibuffer_read_format_edges<float>(buffer, out, positions, n_samps, chan, mul, interp, edges, bound);
}

void ibuffer_read_edges(const ibuffer_data& buffer, float *out, const float *positions, intptr_t n_samps, long chan, float mul, InterpType interp, EdgeMode edges, bool bound)
{
    ibuffer_read_format_edges<float>(buffer, out, positions, n_samps, chan, mul, interp, edges, bound);
}

template <class T, class Ft>
void ibuffer_get_samps_loop(Ft fetch, T *out, intptr_t offset, intptr_t n_samps, bool reverse)
{
    if (reverse)
    {
        for (intptr_t i = n_samps - 1; i >= 0; i--)
            *out++ = static_cast<T>(fetch.get(offset + i));
    }
    else
    {
        for (intptr_t i = 0; i < n_samps; i++)
            *out++ = static_cast<T>(fetch.get(offset + i));
    }
}

template <class T>
void ibuffer_get_samps(const ibuffer_data& buffer, T *out, intptr_t offset, intptr_t n_samps, long chan, bool reverse)
{
    switch(buffer.get_format())
    {
        case PCM_FLOAT:     ibuffer_get_samps_loop(fetch_float(buffer, chan), out, offset, n_samps, reverse);     break;
        case PCM_INT_16:    ibuffer_get_samps_loop(fetch_16bit(buffer, chan), out, offset, n_samps, reverse);     break;
        case PCM_INT_24:    ibuffer_get_samps_loop(fetch_24bit(buffer, chan), out, offset, n_samps, reverse);     break;
        case PCM_INT_32:    ibuffer_get_samps_loop(fetch_32bit(buffer, chan), out, offset, n_samps, reverse);     break;
    }
}

void ibuffer_get_samps(const ibuffer_data& buffer, float *out, intptr_t offset, intptr_t n_samps, long chan, bool reverse)
{
    ibuffer_get_samps<float>(buffer, out, offset, n_samps, chan, reverse);
}

void ibuffer_get_samps(const ibuffer_data& buffer, double *out, intptr_t offset, intptr_t n_samps, long chan, bool reverse)
{
    ibuffer_get_samps<double>(buffer, out, offset, n_samps, chan, reverse);
}
