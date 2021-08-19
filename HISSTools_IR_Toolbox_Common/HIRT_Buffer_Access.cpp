
#include "HIRT_Buffer_Access.hpp"

//////////////////////////////////////////////////////////////////////////
///////////////////////////// Get Properties /////////////////////////////
//////////////////////////////////////////////////////////////////////////

double buffer_sample_rate(t_symbol *buffer)
{
    ibuffer_data data(buffer);

    return data.get_sample_rate();
}

intptr_t buffer_length(t_symbol *buffer)
{
    ibuffer_data data(buffer);

    return data.get_length();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////// Buffer Validity /////////////////////////////
//////////////////////////////////////////////////////////////////////////

// Returns 0 for a valid buffer (or zero value symbol pointer), otherwise 1

long buffer_check(t_object *x, t_symbol *buffer, t_atom_long chan)
{
    ibuffer_data data(buffer);

    // A zero symbol is considered valid

    if (!buffer)
        return 0;

    if (data.get_type() == kBufferNone)
    {
        if (!data.exists())
            object_error(x, "no buffer of name %s", buffer->s_name);
        else
            object_error(x, "buffer %s is not valid", buffer->s_name);
        return 1;
    }
    
    if (chan >= data.get_num_chans())
    {
        object_error(x, "buffer %s does not have enough channels", buffer->s_name);
        return 1;
    }

    return 0;
}

//////////////////////////////////////////////////////////////////////////
///////////////////////////// Read Routines //////////////////////////////
//////////////////////////////////////////////////////////////////////////

intptr_t buffer_read(t_symbol *buffer, long chan, float *out, intptr_t max_length)
{
    ibuffer_data data(buffer);

    if (data.get_type() == kBufferNone)
        return 0;

    if (data.get_length() > max_length || !data.get_length())
        return 0;

    chan = chan % data.get_num_chans();
    
    ibuffer_get_samps(data, out, 0, data.get_length(), chan);

    return data.get_length();
}

intptr_t buffer_read_part(t_symbol *buffer, long chan, float *out, intptr_t offset, intptr_t read_length)
{
    ibuffer_data data(buffer);

    if (data.get_type() == kBufferNone)
        return 0;

    chan = chan % data.get_num_chans();

    ibuffer_get_samps(data, out, offset, read_length, chan);

    return read_length;
}

//////////////////////////////////////////////////////////////////////////
///////////////////////////// Write Routines /////////////////////////////
//////////////////////////////////////////////////////////////////////////

void buffer_write_error(t_object *x, t_symbol *buffer, t_buffer_write_error error)
{
    if (error == BUFFER_WRITE_ERR_NOT_FOUND)
        object_error(x, "no buffer of name %s", buffer->s_name);
    
    if (error == BUFFER_WRITE_ERR_INVALID)
        object_error(x, "buffer %s is not valid", buffer->s_name);

    if (error == BUFFER_WRITE_ERR_TOO_SMALL)
        object_error(x, "not enough room in buffer %s", buffer->s_name);
}

template <typename T>
t_buffer_write_error buffer_write_base(t_object *owner, t_symbol *buffer, T *in, intptr_t write_length, long chan, long resize, double sample_rate, double mul)
{
    t_buffer_info info;
    t_atom temp_atom[2];

    t_buffer_ref *ref = buffer_ref_new(owner, buffer);
    t_object *object = buffer_ref_getobject(ref);

    if (!object)
        return BUFFER_WRITE_ERR_NOT_FOUND;
    
    if (resize)
    {
        t_atom temp_atom[2];
        atom_setlong(temp_atom, write_length);
        object_method_typed(object, gensym("sizeinsamps"), 1L, temp_atom, temp_atom + 1);
    }
    
    float *samples = buffer_locksamples(buffer_ref_getobject(ref));
    buffer_getinfo(object, &info);
    
    if (!samples)
        return BUFFER_WRITE_ERR_NOT_FOUND;
    
    if (info.b_frames < write_length)
        return BUFFER_WRITE_ERR_TOO_SMALL;
    
    chan = chan % info.b_nchans;
    
    float *write_samples = samples + chan;
    
    for (size_t i = 0; i < write_length; i++, write_samples += info.b_nchans)
        *write_samples = static_cast<float>(in[i] * mul);
    
    if (!resize)
    {
        for (size_t i = write_length; i < info.b_frames; i++, write_samples += info.b_nchans)
            *write_samples = 0.f;
    }
    
    if (sample_rate)
    {
        atom_setfloat(temp_atom, sample_rate);
        object_method_typed(object, gensym("sr"), 1L, temp_atom, temp_atom + 1);
    }
    
    buffer_setdirty(object);
    buffer_unlocksamples(object);
    object_free(ref);
    
    return BUFFER_WRITE_ERR_NONE;
}

t_buffer_write_error buffer_write(t_object *owner, t_symbol *buffer, double *in, intptr_t write_length, long chan, long resize, double sample_rate, double mul)
{
    auto error =  buffer_write_base(owner, buffer, in, write_length, chan, resize, sample_rate, mul);
    buffer_write_error(owner, buffer, error);
    return error;
}

t_buffer_write_error buffer_write_float(t_object *owner, t_symbol *buffer, float *in, intptr_t write_length, long resize, long chan, double sample_rate, float mul)
{
    auto error =  buffer_write_base(owner, buffer, in, write_length, chan, resize, sample_rate, mul);
    buffer_write_error(owner, buffer, error);
    return error;
}

//////////////////////////////////////////////////////////////////////////
/////////////////// Retrieve and Check Multiple Buffers //////////////////
//////////////////////////////////////////////////////////////////////////

short buffer_multiple_names(t_object *x, t_symbol **in_bufs, t_symbol **out_bufs, intptr_t *lengths, short argc, t_atom *argv, bool in_place, short max_bufs, intptr_t *overall_len_ret, intptr_t *max_len_ret, double *sample_rate_ret)
{
    intptr_t overall_length = 0;
    intptr_t max_length = 0;
    intptr_t new_length;
    short i;

    double sample_rate = 0.0;
    double new_sample_rate;

    if (!in_place)
    {
        if (argc % 2)
        {
            object_error(x, "target buffer with no matching input buffer");
            return 0;
        }

        argc /= 2;
    }

    if (argc > max_bufs)
        argc = max_bufs;

    if (!argc)
    {
        object_error(x, "no buffers specified");
        return 0;
    }

    for (i = 0; i < argc; i++)
    {
        if (atom_gettype(argv + i) != A_SYM)
        {
            object_error(x, "name of buffer expected, but number given");
            return 0;
        }

        if (buffer_check(x, atom_getsym(argv + i)))
            return 0;

        if (in_place)
        {
            new_length = buffer_length (atom_getsym(argv + i));
            new_sample_rate = buffer_sample_rate(atom_getsym(argv + i));

            if (buffer_check(x, atom_getsym(argv + i)))
                return 0;
        }
        else
        {
            new_length = buffer_length (atom_getsym(argv + i + argc));
            new_sample_rate = buffer_sample_rate(atom_getsym(argv + i + argc));

            if (buffer_check(x, atom_getsym(argv + i + argc)))
                return 0;
        }

        if (new_length == 0)
        {
            object_error(x, "buffer %s has zero length ", atom_getsym(argv + i)->s_name);
            return 0;
        }

        // Store name and length

        out_bufs[i] = atom_getsym(argv + i);
        lengths[i] = new_length;

        if (in_place)
            in_bufs[i] = atom_getsym(argv + i);
        else
            in_bufs[i] = atom_getsym(argv + i + argc);

        if (new_length > max_length)
            max_length = new_length;

        overall_length += new_length;

        // Check sample rates

        if ((sample_rate != 0.0 && sample_rate != new_sample_rate) || new_sample_rate == 0.0)
            object_warn(x, "sample rates do not match for all source buffers");
        else
            sample_rate = new_sample_rate;
    }

    *overall_len_ret = overall_length;
    *max_len_ret = max_length;
    *sample_rate_ret = sample_rate;

    return argc;
}
