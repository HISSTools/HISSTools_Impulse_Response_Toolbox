
#include <ext.h>
#include <ext_obex.h>
#include <z_dsp.h>

#include <HIRT_Core_Functions.hpp>
#include <HIRT_Buffer_Access.hpp>

// Define common attributes and the class name (for the common attributes file)

#define OBJ_CLASSNAME t_bufreverse
#define OBJ_USES_HIRT_WRITE_ATTR
#define OBJ_USES_HIRT_READ_ATTR

#include <HIRT_Common_Attribute_Vars.hpp>

// Object class and structure

t_class *this_class;

struct t_bufreverse
{
    t_pxobject x_obj;

    // Attributes

    HIRT_COMMON_ATTR

    // Bang Outlet

    void *process_done;
};


// This include deals with setup of common attributes - requires the object structure to be defined

#include <HIRT_Common_Attribute_Setup.hpp>


// Function prototypes

void *bufreverse_new();
void bufreverse_free(t_bufreverse *x);
void bufreverse_assist(t_bufreverse *x, void *b, long m, long a, char *s);

void bufreverse_process(t_bufreverse *x, t_symbol *target, t_symbol *source);
void bufreverse_process_internal(t_bufreverse *x, t_symbol *sym, short argc, t_atom *argv);


int C74_EXPORT main()
{
    this_class = class_new("bufreverse~",
                          (method) bufreverse_new,
                          (method)bufreverse_free,
                          sizeof(t_bufreverse),
                          0L,
                          0);

    class_addmethod(this_class, (method)bufreverse_process, "process", A_SYM, A_SYM, 0L);

    class_addmethod(this_class, (method)bufreverse_assist, "assist", A_CANT, 0L);

    declare_HIRT_common_attributes(this_class);

    class_register(CLASS_BOX, this_class);

    return 0;
}


void *bufreverse_new()
{
    t_bufreverse *x = reinterpret_cast<t_bufreverse *>(object_alloc(this_class));

    x->process_done = bangout(x);

    init_HIRT_common_attributes(x);

    return x;
}


void bufreverse_free(t_bufreverse *x)
{
    free_HIRT_common_attributes(x);
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
    
    t_atom_long read_chan = x->read_chan - 1;

    // Check source buffer

    if (buffer_check((t_object *) x, source))
        return;
    
    intptr_t length = buffer_length(source);
    double sample_rate = buffer_sample_rate(source);

    // Allocate Memory

    temp_ptr<float> temp1(length);
    temp_ptr<float> temp2(length);

    // Check momory allocation

    if (!temp1 || !temp2)
    {
        object_error((t_object *)x, "could not allocate temporary memory for processing");
        return;
    }

    // Read from buffer

    buffer_read(source, read_chan, temp1.get(), length);

    // Copy in reverse to temporary storage

    for (intptr_t i = 0; i < length; i++)
         temp2[i] = temp1[length - i - 1];

    // Copy out to buffer

    auto error = buffer_write_float((t_object *)x, target, temp2.get(), length, x->write_chan - 1, x->resize, sample_rate, 1.);

    if (!error)
        outlet_bang(x->process_done);
}
