
#ifndef __HIRT_BUFFER_ACCESS__
#define __HIRT_BUFFER_ACCESS__

#include <ext.h>
#include <ext_obex.h>

#include <cstdint>

#include <ibuffer_access.hpp>

// Write Error Enum

enum t_buffer_write_error
{
    BUFFER_WRITE_ERR_NONE = 0,
    BUFFER_WRITE_ERR_NOT_FOUND = 1,
    BUFFER_WRITE_ERR_INVALID = 2,
    BUFFER_WRITE_ERR_TOO_SMALL = 3
};

// Function Prototypes

double buffer_sample_rate(t_symbol *buffer);
intptr_t buffer_length(t_symbol *buffer);

long buffer_check(t_object *x, t_symbol *buffer, t_atom_long chan = 0);

intptr_t buffer_read(t_symbol *buffer, long chan, float *out, intptr_t max_length);
intptr_t buffer_read_part(t_symbol *buffer, long chan, float *out, intptr_t offset, intptr_t read_length);

t_buffer_write_error buffer_write(t_object *owner, t_symbol *buffer, double *in, intptr_t write_length, long chan, long resize, double sample_rate, double mul);
t_buffer_write_error buffer_write_float(t_object *owner, t_symbol *buffer, float *in, intptr_t write_length, long chan, long resize, double sample_rate, float mul);

short buffer_multiple_names(t_object *x, t_symbol **in_bufs, t_symbol **out_bufs, intptr_t *lengths, short argc, t_atom *argv, bool in_place, short max_bufs, intptr_t *overall_len_ret, intptr_t *max_len_ret, double *sr_ret);

#endif /* __HIRT_BUFFER_ACCESS__ */
