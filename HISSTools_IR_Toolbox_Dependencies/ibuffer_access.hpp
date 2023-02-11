
/*
 *  ibuffer_access.hpp
 *
 *  Provides an interface for accessing and reading samples from an ibuffer (or MSP buffer).
 *  You should also compile ibufffer_access.cpp in the project
 *
 *  Various kinds of interpolation are supported using SIMD calculation and 32 bit / 64 bit output.
 *
 *  All output pointers used with SIMD routines should be suitably aligned.
 *
 *  The ibuffer_data class handles acquisition of the (i)buffer~ using RAII.
 *  The main routines for accessing samples are then:
 *
 *  ibuffer_read            - read multiple samples at arbitrary positions with interpolation
 *  ibuffer_get_samp        - get a single sample
 *  ibuffer_get_samps       - get a number of consecutive samples in forward or reverse order
 *
 *  Helpers are provided for using attributes to set the interpolation type.
 *
 *  Copyright 2010-20 Alex Harker. All rights reserved.
 *
 */


#ifndef IBUFFER_ACCESS_H
#define IBUFFER_ACCESS_H

#include "ibuffer.hpp"
#include <TableReader.hpp>

enum BufferType { kBufferNone, kBufferIBuffer, kBufferMaxBuffer };

class ibuffer_data
{
    static t_symbol *ps_buffer;
    static t_symbol *ps_ibuffer;
    
public:
    
    ibuffer_data(t_symbol *name);
    ~ibuffer_data();
    
    ibuffer_data(const ibuffer_data& data) = delete;
    ibuffer_data& operator=(const ibuffer_data& data) = delete;
    
    void set_dirty();
    void set_size_in_samples(t_atom_long size);
    
    void acquire(t_symbol *name);
    void release();
    
    BufferType get_type() const         { return buffer_type; };
    void *get_samples() const           { return samples; };
    t_ptr_int get_length() const        { return length; }
    long get_num_chans() const          { return num_chans; }
    double get_sample_rate() const      { return sample_rate; }
    t_ibuffer_format get_format() const { return format; }

    bool exists() const                 { return buffer_object; }
    
private:
    
    void acquire_buffer();
    void release_buffer();

    BufferType buffer_type;

    void *samples;
    
    t_ptr_int length;
    long num_chans;
    t_ibuffer_format format;
    
    double sample_rate;
 
    t_object *buffer_object;
};

// Reading different formats

template <class T, int64_t bit_scale>
struct fetch : public table_fetcher<float>
{
    fetch(const ibuffer_data& data, long chan)
    : table_fetcher(data.get_length(), 1.0 / ((int64_t) 1 << (bit_scale - 1))), samples(((T *) data.get_samples()) + chan), num_chans(data.get_num_chans()) {}
    
    float operator()(intptr_t offset)   { return static_cast<float>(samples[offset * num_chans]); }
    double get(intptr_t offset)         { return bit_scale != 1 ? scale * operator()(offset) : operator()(offset); }
    
    T *samples;
    long num_chans;
};

template<>
struct fetch<int32_t, 24> : public table_fetcher<float>
{
    fetch(const ibuffer_data& data, long chan)
    : table_fetcher(data.get_length(), 1.0 / ((int64_t) 1 << 31)), samples(((uint8_t *) data.get_samples()) + 3 * chan), num_chans(data.get_num_chans()) {}
    
    float operator()(intptr_t offset)
    {
        return static_cast<float>((*reinterpret_cast<uint32_t *>(samples + (offset * 3 * num_chans - 1)) & 0xFFFFFF00));
    }
    double get(intptr_t offset) { return scale * operator()(offset); }
    
    uint8_t *samples;
    long num_chans;
};

typedef fetch<float, 1> fetch_float;
typedef fetch<int16_t, 16> fetch_16bit;
typedef fetch<int32_t, 24> fetch_24bit;
typedef fetch<int32_t, 32> fetch_32bit;

// Get the value of an individual sample

static inline double ibuffer_get_samp(const ibuffer_data& buffer, intptr_t offset, long chan);

// Get consecutive samples (and in reverse)

void ibuffer_get_samps(const ibuffer_data& buffer, float *out, intptr_t offset, intptr_t n_samps, long chan, bool reverse = false);
void ibuffer_get_samps(const ibuffer_data& buffer, double *out, intptr_t offset, intptr_t n_samps, long chan, bool reverse = false);

// Read with various forms of interpolation

void ibuffer_read(const ibuffer_data& buffer, double *out, const double *positions, intptr_t n_samps, long chan, double mul, InterpType interp);
void ibuffer_read(const ibuffer_data& buffer, float *out, const double *positions, intptr_t n_samps, long chan, float mul, InterpType interp);
void ibuffer_read(const ibuffer_data& buffer, float *out, const float *positions, intptr_t n_samps, long chan, float mul, InterpType interp);

// Read with various edge conditions and various forms of interpolation

void ibuffer_read_edges(const ibuffer_data& buffer, double *out, const double *positions, intptr_t n_samps, long chan, double mul, InterpType interp, EdgeMode edges, bool bound);
void ibuffer_read_edges(const ibuffer_data& buffer, float *out, const double *positions, intptr_t n_samps, long chan, float mul, InterpType interp, EdgeMode edges, bool bound);
void ibuffer_read_edges(const ibuffer_data& buffer, float *out, const float *positions, intptr_t n_samps, long chan, float mul, InterpType interp, EdgeMode edges, bool bound);

// Get individual samples

static inline double ibuffer_get_samp(const ibuffer_data& buffer, intptr_t offset, long chan)
{
    switch (buffer.get_format())
    {
        case PCM_FLOAT:     return fetch_float(buffer, chan).get(offset);
        case PCM_INT_16:    return fetch_16bit(buffer, chan).get(offset);
        case PCM_INT_24:    return fetch_24bit(buffer, chan).get(offset);
        case PCM_INT_32:    return fetch_32bit(buffer, chan).get(offset);
    }

    return 0.0;
}

// Interpolation Attributes

template <class T, InterpType defaultValue>
t_max_err ibuf_interp_attribute_set(T *x, t_attr * /* attr */, long argc, t_atom *argv)
{
    if (!argc)
    {
        x->interp_type = defaultValue;
        return MAX_ERR_NONE;
    }
    
    if (atom_gettype(argv) == A_SYM)
    {
        t_symbol *type = atom_getsym(argv);
        
        if (type == gensym("linear"))
            x->interp_type = InterpType::Linear;
        else if (type == gensym("hermite"))
            x->interp_type = InterpType::CubicHermite;
        else if (type == gensym("bspline"))
            x->interp_type = InterpType::CubicBSpline;
        else if (type == gensym("lagrange"))
            x->interp_type = InterpType::CubicLagrange;
        else
            object_error((t_object *) x, "%s: no interpolation mode %s", object_classname(x)->s_name,  type->s_name);
    }
    else
    {
        t_atom_long index = atom_getlong(argv) - 1;
        index = index < 0 ? 0 : index;
        index = index > 3 ? 3 : index;
        
        x->interp_type = static_cast<InterpType>(index);
    }
    
    return MAX_ERR_NONE;
}

template <class T>
t_max_err ibuf_interp_attribute_get(T *x, t_object */* attr */, long *argc, t_atom **argv)
{
    if (argc && argv)
    {
        char alloc;
        
        if (atom_alloc(argc, argv, &alloc))
            return MAX_ERR_GENERIC;
        
        switch (x->interp_type)
        {
            case InterpType::Linear:            atom_setsym(*argv, gensym("linear"));       break;
            case InterpType::CubicHermite:      atom_setsym(*argv, gensym("hermite"));      break;
            case InterpType::CubicBSpline:      atom_setsym(*argv, gensym("bspline"));      break;
            case InterpType::CubicLagrange:     atom_setsym(*argv, gensym("lagrange"));     break;
            
            default:
                atom_setsym(*argv, gensym("linear"));
        }
    }
    
    return MAX_ERR_NONE;
}

template <class T, InterpType defaultValue>
void add_ibuffer_interp_attribute(t_class *this_class, const char *attrname)
{
    CLASS_ATTR_LONG(this_class, attrname, 0L, T, interp_type);
    CLASS_ATTR_ENUM(this_class, attrname, 0L, "linear hermite bspline lagrange");
    CLASS_ATTR_ACCESSORS(this_class, attrname, ibuf_interp_attribute_get<T>, (ibuf_interp_attribute_set<T, defaultValue>));
    CLASS_ATTR_LABEL(this_class, attrname, 0L, "Interpolation Type");
}

#endif /* IBUFFER_ACCESS_H */
