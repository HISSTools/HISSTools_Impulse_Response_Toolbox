
#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <HIRT_Core_Functions.h>
#include <HIRT_Buffer_Access.h>

// Define common attributes and the class name (for the common attributes file)

#define OBJ_CLASSNAME t_bufreverse
#define OBJ_USES_HIRT_WRITE_ATTR
#define OBJ_USES_HIRT_READ_ATTR

#include <HIRT_Common_Attribute_Vars.h>

// Object class and structure

void *this_class;

typedef struct _bufreverse
{
    t_pxobject x_obj;

    // Attributes

    HIRT_COMMON_ATTR

    // Bang Outlet

    void *process_done;

} t_bufreverse;


// This include deals with setup of common attributes - requires the object structure to be defined

#include <HIRT_Common_Attribute_Setup.h>


// Function prototypes

void *bufreverse_new();
void bufreverse_free(t_bufreverse *x);
void bufreverse_assist(t_bufreverse *x, void *b, long m, long a, char *s);

void bufreverse_process(t_bufreverse *x, t_symbol *target, t_symbol *source);
void bufreverse_process_internal(t_bufreverse *x, t_symbol *sym, short argc, t_atom *argv);


int main(void)
{
    this_class = class_new ("bufreverse~",
                            (method) bufreverse_new,
                            (method)bufreverse_free,
                            sizeof(t_bufreverse),
                            0L,
                            0);

    class_addmethod(this_class, (method)bufreverse_process, "process", A_SYM, A_SYM, 0L);

    class_addmethod(this_class, (method)bufreverse_assist, "assist", A_CANT, 0L);

    declare_HIRT_common_attributes(this_class);

    buffer_access_init();

    return 0;
}


void *bufreverse_new()
{
    t_bufreverse *x = (t_bufreverse *)object_alloc (this_class);

    x->process_done = bangout(x);

    init_HIRT_common_attributes(x);

    return(x);
}


void bufreverse_free(t_bufreverse *x)
{
}


void bufreverse_assist(t_bufreverse *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET)
        sprintf(s,"Instructions In");
    else
        sprintf(s,"Bang on Success");
}


// Arguments are - target buffer / source buffer (can alias)

void bufreverse_process(t_bufreverse *x, t_symbol *target, t_symbol *source)
{
    t_atom args[2];

    atom_setsym(args + 0, target);
    atom_setsym(args + 1, source);

    defer(x, (method) bufreverse_process_internal, 0, 2, args);
}


void bufreverse_process_internal(t_bufreverse *x, t_symbol *sym, short argc, t_atom *argv)
{
    t_symbol *target = atom_getsym(argv++);
    t_symbol *source = atom_getsym(argv++);

    float *temp1;
    double *temp2;

    t_buffer_write_error error;

    AH_SIntPtr full_length = buffer_length(source);
    AH_SIntPtr i;

    double sample_rate = 0;
    t_atom_long read_chan = x->read_chan - 1;

    // Check source buffer

    if (buffer_check((t_object *) x, source, read_chan))
        return;
    sample_rate = buffer_sample_rate(source);

    // Allocate Memory

    temp1 = (float *) ALIGNED_MALLOC(full_length * (sizeof(double) + sizeof(float)));
    temp2 = (double *) (temp1 + full_length);

    // Check momory allocation

    if (!temp1)
    {
        object_error((t_object *)x, "could not allocate temporary memory for processing");
        free(temp1);
        return;
    }

    // Read from buffer

    buffer_read(source, read_chan, (float *) temp1, full_length);

    // Copy to double precision version

    for (i = 0; i < full_length; i++)
         temp2[i] = temp1[full_length - i - 1];

    // Copy out to buffer

    error = buffer_write(target, temp2, full_length, x->write_chan - 1, x->resize, sample_rate, 1.);
    buffer_write_error((t_object *)x, target, error);

    // Free Resources

    ALIGNED_FREE(temp1);

    if (!error)
        outlet_bang(x->process_done);
}

