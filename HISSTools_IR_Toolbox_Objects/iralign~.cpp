
#include <ext.h>
#include <ext_obex.h>
#include <ext_obex_util.h>
#include <z_dsp.h>

#include <HIRT_Core_Functions.h>
#include <HIRT_Buffer_Access.h>

// Define common attributes and the class name (for the common attributes file)

#define OBJ_CLASSNAME t_iralign
#define OBJ_USES_HIRT_WRITE_ATTR
#define OBJ_USES_HIRT_READ_ATTR

#include <HIRT_Common_Attribute_Vars.h>


// Object class and structure

t_class *this_class;

struct t_iralign
{
    t_pxobject x_obj;

    // Attributes

    HIRT_COMMON_ATTR

    // Bang Outlet

    void *process_done;

};


// This include deals with setup of common attributes - requires the object structure to be defined

#include <HIRT_Common_Attribute_Setup.h>


// Function prototypes

void *iralign_new(t_symbol *s, short argc, t_atom *argv);
void iralign_free(t_iralign *x);
void iralign_assist(t_iralign *x, void *b, long m, long a, char *s);

AH_SIntPtr align_find_max(double *in, AH_SIntPtr length);
void align_pad(double *out_buf, double *in_buf, AH_SIntPtr pad, AH_SIntPtr length);

void iralign_align(t_iralign *x, t_symbol *sym, long argc, t_atom *argv);
void iralign_align_internal(t_iralign *x, t_symbol *sym, short argc, t_atom *argv);


//////////////////////////////////////////////////////////////////////////
/////////////////////// Main / New / Free / Assist ///////////////////////
//////////////////////////////////////////////////////////////////////////


int main()
{
    this_class = class_new("iralign~",
                          (method) iralign_new,
                          (method)iralign_free,
                          sizeof(t_iralign),
                          0L,
                          A_GIMME,
                          0);

    class_addmethod(this_class, (method)iralign_assist, "assist", A_CANT, 0L);

    class_addmethod(this_class, (method)iralign_align, "align", A_GIMME, 0L);
    class_addmethod(this_class, (method)iralign_align, "alignto", A_GIMME, 0L);

    declare_HIRT_common_attributes(this_class);

    class_register(CLASS_BOX, this_class);

    return 0;
}


void *iralign_new(t_symbol *s, short argc, t_atom *argv)
{
    t_iralign *x = (t_iralign *)object_alloc(this_class);

    x->process_done = bangout(x);

    init_HIRT_common_attributes(x);
    attr_args_process(x, argc, argv);

    return(x);
}


void iralign_free(t_iralign *x)
{
    free_HIRT_common_attributes(x);
}


void iralign_assist(t_iralign *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET)
        sprintf(s,"Instructions In");
    else
        sprintf(s,"Bang on Success");
}


//////////////////////////////////////////////////////////////////////////
/////////////////////////// Alignment routines ///////////////////////////
//////////////////////////////////////////////////////////////////////////


AH_SIntPtr align_find_max(double *in, AH_SIntPtr length)
{
    double max = 0.0;
    double max_test;

    AH_SIntPtr max_pos = 0;
    AH_SIntPtr i;

    // Find peak

    for (i = 0; i < length; i++)
    {
        max_test = fabs(in[i]);

        if (max_test > max)
        {
            max = max_test;
            max_pos = i;
        }
    }

    return max_pos;
}


void align_pad(double *out_buf, double *in_buf, AH_SIntPtr pad, AH_SIntPtr length)
{
    AH_SIntPtr i;

    for (i = 0; i < pad; i++)
        *out_buf++ = 0.0;

    for (i = 0; i < length; i++)
        *out_buf++ = *in_buf++;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////// User messages ///////////////////////////////
//////////////////////////////////////////////////////////////////////////


void iralign_align(t_iralign *x, t_symbol *sym, long argc, t_atom *argv)
{
    defer(x, (method) iralign_align_internal, sym, (short) argc, argv);
}


void iralign_align_internal(t_iralign *x, t_symbol *sym, short argc, t_atom *argv)
{
    t_symbol *in_buffer_names[128];
    t_symbol *out_buffer_names[128];
    double *samples[128];

    AH_SIntPtr lengths[128];
    AH_SIntPtr max_pos[128];

    t_atom_long read_chan = x->read_chan - 1;
    t_atom_long write_chan = x->write_chan - 1;
    double sample_rate = 0.0;

    double *temp_buf_d;
    float *temp_buf_f;

    AH_SIntPtr num_buffers = 0;
    AH_SIntPtr overall_length = 0;
    AH_SIntPtr max_length = 0;
    AH_SIntPtr overall_max_pos = 0;
    AH_SIntPtr offset = 0;
    AH_SIntPtr i, j;

    t_buffer_write_error error;
    AH_Boolean overall_error = false;

    // Check buffers, storing names and lengths +  calculate total / largest length

    num_buffers = buffer_multiple_names((t_object *) x, in_buffer_names, out_buffer_names, lengths, argc, argv, (sym == gensym("align")), 128, &overall_length, &max_length, &sample_rate);

    if (!num_buffers)
        return;

    // Assign Temporary Memory

    samples[0] = (double *) malloc(sizeof(double) * overall_length);
    temp_buf_d = (double *) malloc(sizeof(double) * max_length * 2);
    temp_buf_f = (float *) temp_buf_d;

    // Check temporary memory

    if (!samples[0] || !temp_buf_d)
    {
        object_error((t_object *) x, "could not allocate temporary memory for internal storage");

        free(samples[0]);
        free(temp_buf_d);

        return;
    }

    // Read in buffers

    for (i = 0; i < num_buffers; i++)
    {
        samples[i] = samples[0] + offset;
        buffer_read(in_buffer_names[i], read_chan, temp_buf_f, lengths[i]);

        for (j = 0; j < lengths[i]; j++)
             samples[i][j] = temp_buf_f[j];
        offset += lengths[i];
    }

    // Find latest max sample

    for (i = 0; i < num_buffers; i++)
    {
        max_pos[i] = align_find_max (samples[i], lengths[i]);

        if (max_pos[i] > overall_max_pos)
            overall_max_pos = max_pos[i];
    }

    // Check length of buffers for writing

    if (!x->resize)
    {
        for (i = 0; i < num_buffers; i++)
        {
            if (buffer_length(out_buffer_names[i]) < lengths[i] + overall_max_pos - max_pos[i])
            {
                object_error((t_object *) x, "buffer %s is not long enough to complete write (no buffers altered)", out_buffer_names[i]->s_name);
                return;
            }
        }
    }

    // Align and write to buffers

    for (i = 0; i < num_buffers; i++)
    {
        align_pad (temp_buf_d, samples[i], overall_max_pos - max_pos[i], lengths[i]);

        error = buffer_write((t_object *)x, out_buffer_names[i], temp_buf_d, lengths[i] + overall_max_pos - max_pos[i], write_chan, x->resize, sample_rate, 1.0);
        buffer_write_error((t_object *) x, out_buffer_names[i], error);

        if (error)
            overall_error = true;
    }

    // Free

    free(samples[0]);
    free(temp_buf_d);

    // Bang on success

    if (overall_error == false)
        outlet_bang(x->process_done);
}
