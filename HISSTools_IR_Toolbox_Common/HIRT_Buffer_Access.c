
#include "HIRT_Buffer_Access.h"


//////////////////////////////////////////////////////////////////////////
///////////////////////////// Get Properties /////////////////////////////
//////////////////////////////////////////////////////////////////////////


double buffer_sample_rate (t_symbol *buffer)
{
	void *ptr = ibuffer_get_ptr(buffer);
	
	if (ptr)
		return ibuffer_sample_rate(ptr);
	else 
		return 0;
}


AH_SIntPtr buffer_length (t_symbol *buffer)
{
	AH_SIntPtr length;
	long n_chans, format;
	void *samps;
	
	void *b = ibuffer_get_ptr(buffer);
	
	if (!b)
		return 0;
	
	if (!ibuffer_info(b, &samps, &length, &n_chans, &format))
		return 0;
	
	return length;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////// Buffer Validity /////////////////////////////
//////////////////////////////////////////////////////////////////////////


// Returns 0 for a valid buffer (or zero value symbol pointer), otherwise 1

long buffer_check (t_object *x, t_symbol *buffer)
{
	AH_SIntPtr length;
	long n_chans, format;
	void *samps;
	
	void *b = ibuffer_get_ptr(buffer);
	
	// A zero symbol is considered valid
	
	if (!buffer)
		return 0;
	
	if (!b)
	{
		object_error((t_object *) x, "no buffer of name %s", buffer->s_name);
		return 1;
	}
	
	if (!ibuffer_info(b, &samps, &length, &n_chans, &format))
	{
		object_error((t_object *) x, "buffer %s is not valid", buffer->s_name);
		return 1;
	}
	
	return 0;
}


//////////////////////////////////////////////////////////////////////////
///////////////////////////// Read Routines //////////////////////////////
//////////////////////////////////////////////////////////////////////////


AH_SIntPtr buffer_read (t_symbol *buffer, long chan, float *out, AH_SIntPtr max_length)
{
	AH_SIntPtr length;
	long n_chans, format;
	void *samps;
	
	void *b = ibuffer_get_ptr(buffer);
	
	if (!b)
		return 0;
	
	if (!ibuffer_info(b, &samps, &length, &n_chans, &format))
		return 0;
	
	if (length > max_length || !length)
		return 0;
	
	ibuffer_increment_inuse(b);
	ibuffer_get_samps(samps, out, 0L, length, n_chans, chan, format);
	ibuffer_decrement_inuse(b);
	
	return length;
}


AH_SIntPtr buffer_read_part (t_symbol *buffer, long chan, float *out, AH_SIntPtr offset, AH_SIntPtr read_length)
{
	AH_SIntPtr length;
	long n_chans, format;
	void *samps;
	
	void *b = ibuffer_get_ptr(buffer);
	
	if (!b)
		return 0;
	
	if (!ibuffer_info(b, &samps, &length, &n_chans, &format))
		return 0;
	
	ibuffer_increment_inuse(b);
	ibuffer_get_samps(samps, out, offset, read_length, n_chans, chan, format);
	ibuffer_decrement_inuse(b);
	
	return read_length;
}


//////////////////////////////////////////////////////////////////////////
///////////////////////////// Write Routines /////////////////////////////
//////////////////////////////////////////////////////////////////////////


void buffer_write_error (t_object *x, t_symbol *buffer, t_buffer_write_error error)
{
	if (error == BUFFER_WRITE_ERR_NOT_FOUND)
		object_error((t_object *) x, "no buffer of name %s", buffer->s_name);
	
	if (error == BUFFER_WRITE_ERR_INVALID)
		object_error((t_object *) x, "buffer %s is not valid", buffer->s_name);
	
	if (error == BUFFER_WRITE_ERR_TOO_SMALL)
		object_error((t_object *) x, "not enough room in buffer %s", buffer->s_name);
}


t_buffer_write_error buffer_write (t_symbol *buffer, double *in, AH_SIntPtr write_length, long chan, long resize, double sample_rate, double mul)
{
	AH_SIntPtr length;
	long n_chans, format;
	void *samps;
	AH_SIntPtr i;
	
	float *samples;
	
	void *b = ibuffer_get_ptr(buffer);
	
	if (!b)
		return BUFFER_WRITE_ERR_NOT_FOUND;
	
	if (ob_sym(b) != ps_buffer)
		return BUFFER_WRITE_ERR_NOT_FOUND;
	
	if (!ibuffer_info(b, &samps, &length, &n_chans, &format))
		return BUFFER_WRITE_ERR_INVALID;
	
	if (resize)
	{
		t_atom temp_atom[2];
		atom_setlong(temp_atom, write_length);	
		object_method_typed ((t_object *)b, gensym("sizeinsamps"), 1L, temp_atom, temp_atom + 1);
		ibuffer_info(b, &samps, &length, &n_chans, &format);
	}
	
	if (length < write_length)
		return BUFFER_WRITE_ERR_TOO_SMALL;

	ibuffer_increment_inuse(b);
	
	samples = (float *) samps;
	chan = chan % n_chans;
	
	for (i = 0; i < write_length; i++)
		samples[i * n_chans + chan] = (float) (in[i] * mul);
	
	if (!resize)
		for (i = write_length; i < length; i++)
			samples[i * n_chans + chan] = 0.;
	
	if (sample_rate)
	{
		t_atom temp_atom[2];
		atom_setfloat(temp_atom, sample_rate);
		object_method_typed ((t_object *)b, gensym("sr"), 1L, temp_atom, temp_atom + 1);		
	}
	
	object_method ((t_object *)b, gensym("dirty"));
	ibuffer_decrement_inuse(b);
	
	return BUFFER_WRITE_ERR_NONE;
}


t_buffer_write_error buffer_write_float (t_symbol *buffer, float *in, AH_SIntPtr write_length, long resize, long chan, double sample_rate, float mul)
{
	AH_SIntPtr length;
	long n_chans, format;
	void *samps;
	AH_SIntPtr i;
	
	float *samples;
	
	void *b = ibuffer_get_ptr(buffer);
	
	if (!b)
		return BUFFER_WRITE_ERR_NOT_FOUND;
	
	if (ob_sym(b) != ps_buffer)
		return BUFFER_WRITE_ERR_NOT_FOUND;
	
	if (!ibuffer_info(b, &samps, &length, &n_chans, &format))
		return BUFFER_WRITE_ERR_INVALID;
	
	if (resize)
	{
		t_atom temp_atom[2];
		atom_setlong(temp_atom, write_length);	
		object_method_typed ((t_object *)b, gensym("sizeinsamps"), 1L, temp_atom, temp_atom + 1);
		ibuffer_info(b, &samps, &length, &n_chans, &format);		
	}
	
	if (length < write_length)
		return BUFFER_WRITE_ERR_TOO_SMALL;
	
	ibuffer_increment_inuse(b);
	
	samples = (float *) samps;
	chan = chan % n_chans;
	
	for (i = 0; i < write_length; i++)
		samples[i * n_chans + chan] = in[i] * mul;
	
	if (!resize)
		for (i = write_length; i < length; i++)
			samples[i * n_chans + chan] = 0.;
	
	if (sample_rate)
	{
		t_atom temp_atom[2];
		atom_setfloat(temp_atom, sample_rate);
		object_method_typed ((t_object *)b, gensym("sr"), 1L, temp_atom, temp_atom + 1);		
	}
	
	object_method ((t_object *)b, gensym("dirty"));
	
	ibuffer_decrement_inuse(b);
	
	return BUFFER_WRITE_ERR_NONE;
}



//////////////////////////////////////////////////////////////////////////
/////////////////// Retrieve and Check Multiple Buffers //////////////////
//////////////////////////////////////////////////////////////////////////


short buffer_multiple_names(t_object *x, t_symbol **in_bufs, t_symbol **out_bufs, AH_SIntPtr *lengths, short argc, t_atom *argv, long in_place, short max_bufs, AH_SIntPtr *overall_len_ret, AH_SIntPtr *max_len_ret, double *sample_rate_ret)
{
	AH_SIntPtr overall_length = 0;
	AH_SIntPtr max_length = 0;
	AH_SIntPtr new_length;
	short i;
	
	double sample_rate = 0;
	double new_sample_rate;
	
	if (!in_place)
	{
		if (argc % 2)
		{
			object_error((t_object *) x, "target buffer with no matching input buffer");
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
		
		if ((sample_rate != 0 && sample_rate != new_sample_rate) || new_sample_rate == 0.)
			object_warn(x, "sample rates do not match for all source buffers");
		else 
			sample_rate = new_sample_rate;
	}
	
	*overall_len_ret = overall_length;
	*max_len_ret = max_length;
	*sample_rate_ret = sample_rate;
	
	return argc;
}

