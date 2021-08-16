
#include <ext.h>
#include <ext_obex.h>
#include <ext_obex_util.h>
#include <z_dsp.h>

#include <HIRT_Core_Functions.hpp>
#include <HIRT_Buffer_Access.hpp>
#include <HIRT_Matrix_Math.hpp>

// Define common attributes and the class name (for the common attributes file)

#define OBJ_CLASSNAME t_irnonlin
#define OBJ_USES_HIRT_WRITE_ATTR
#define OBJ_USES_HIRT_READ_ATTR

#include <HIRT_Common_Attribute_Vars.hpp>


// Object class and structure

t_class *this_class;

struct t_irnonlin
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

void *irnonlin_new(t_symbol *s, short argc, t_atom *argv);
void irnonlin_free(t_irnonlin *x);
void irnonlin_assist(t_irnonlin *x, void *b, long m, long a, char *s);

void irnonlin_nonlin(t_irnonlin *x, t_symbol *sym, long argc, t_atom *argv);
void irnonlin_nonlin_internal(t_irnonlin *x, t_symbol *sym, short argc, t_atom *argv);


//////////////////////////////////////////////////////////////////////////
/////////////////////// Main / New / Free / Assist ///////////////////////
//////////////////////////////////////////////////////////////////////////


int C74_EXPORT main()
{
    this_class = class_new("irnonlin~",
                          (method) irnonlin_new,
                          (method)irnonlin_free,
                          sizeof(t_irnonlin),
                          0L,
                          A_GIMME,
                          0);

    class_addmethod(this_class, (method)irnonlin_assist, "assist", A_CANT, 0L);

    class_addmethod(this_class, (method)irnonlin_nonlin, "convert", A_GIMME, 0L);
    class_addmethod(this_class, (method)irnonlin_nonlin, "convertto", A_GIMME, 0L);

    declare_HIRT_common_attributes(this_class);

    class_register(CLASS_BOX, this_class);

    return 0;
}


void *irnonlin_new(t_symbol *s, short argc, t_atom *argv)
{
    t_irnonlin *x = reinterpret_cast<t_irnonlin *>(object_alloc(this_class));

    x->process_done = bangout(x);

    init_HIRT_common_attributes(x);
    attr_args_process(x, argc, argv);

    return x;
}


void irnonlin_free(t_irnonlin *x)
{
    free_HIRT_common_attributes(x);
}


void irnonlin_assist(t_irnonlin *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET)
        sprintf(s,"Instructions In");
    else
        sprintf(s,"Bang on Success");
}

//////////////////////////////////////////////////////////////////////////
///////////////////////////// Matrix Routines ////////////////////////////
//////////////////////////////////////////////////////////////////////////


// These functions are used to calculate the necessary matrix for inversion in order to the multiplication coefficients for conversion

std::complex<double> m1_cpow(uintptr_t i, uintptr_t j)
{
    using complex = std::complex<double>;
    
    complex val = complex(1.0, 0.0);
    uintptr_t int_part = (i << 1) + (j >> 1);

    if (int_part & 1)
        val = complex(-1.0, 0.0);

    if (!(j & 1))
        val *= complex(0.0, 1.0);

    return val;
}


double factorial(double k)
{
    return (k <= 1) ? 1 : k * factorial(k - 1.0);
}


double binom(uintptr_t i, uintptr_t j)
{
    const double n = static_cast<double>(i);
    const double k = static_cast<double>((i - j) / 2);
    
    if (k <= 0)
        return 1;
    
    return factorial(n) / (factorial(k) * factorial(n - k));
}


// Solves the inversion of the matrix so as to determine the correct (complex) multiplication factor for each measured harmonic for the Hammerstein model

t_matrix_complex matrix_non_linear(uintptr_t size)
{
    using complex = std::complex<double>;

    t_matrix_complex mat(size, size);

    // N.B. Indices offset here to 1 (and stored correctly below)

    for (uintptr_t i = 1; i <= size; i++)
    {
        for (uintptr_t j = 1; j <= size; j++)
        {
            if ((i >= j) && ((i + j + 1) & 1))
                mat(j - 1, i - 1) = (m1_cpow(i, j) * complex(binom(i, j), 0.0)) / complex(pow(2.0, static_cast<double>(i - 1)), 0.0);
            else
                mat(j - 1, i - 1) = complex(0.0, 0.0);
        }
    }

    for (uintptr_t i = 0; i < size; i++)
    {
        // Multiply rows

        complex pivot_recip = complex(1.0, 0.0) / mat(i, i);
        mat(i, i) = complex(1.0, 0.0);

        for (uintptr_t j = 0; j < size; j++)
            mat(i, j) *= pivot_recip;

        // Subtract rows

        for (uintptr_t j = 0; j < size; j++)
        {
            if (j == i)
                continue;

            complex row_mul = mat(j, i);
            mat(j, i) = complex(0.0, 0.0);

            for (uintptr_t k = i; k < size; k++)
                mat(j, k) -= mat(i, k) * row_mul;
        }
    }

    return mat;
}


//////////////////////////////////////////////////////////////////////////
///////////////////////////// User Messages //////////////////////////////
//////////////////////////////////////////////////////////////////////////


void irnonlin_nonlin(t_irnonlin *x, t_symbol *sym, long argc, t_atom *argv)
{
    defer(x, (method) irnonlin_nonlin_internal, sym, (short) argc, argv);
}


// Converts from harmonic IRs to harmerstein model IRs

void irnonlin_nonlin_internal(t_irnonlin *x, t_symbol *sym, short argc, t_atom *argv)
{
    FFT_SPLIT_COMPLEX_D impulses[128];

    t_symbol *in_buffer_names[128];
    t_symbol *out_buffer_names[128];
    intptr_t lengths[128];

    intptr_t overall_length = 0;
    intptr_t length;
    intptr_t max_length = 0;
    
    double sample_rate = 0.0;
    bool overall_error = false;

    t_atom_long read_chan = x->read_chan - 1;
    t_atom_long write_chan = x->write_chan - 1;

    // Check buffers, storing names and lengths +  calculate total / largest length

    short num_buffers = buffer_multiple_names((t_object *) x, in_buffer_names, out_buffer_names, lengths, argc, argv, (sym == gensym("convert")), 128, &overall_length, &max_length, &sample_rate);

    if (!num_buffers)
        return;

    // Calculate fft size

    uintptr_t fft_size_log2;
    uintptr_t fft_size = calculate_fft_size(static_cast<uintptr_t>(max_length), fft_size_log2);

    // Check length of buffers for writing

    if (!x->resize)
    {
        for (short i = 0; i < num_buffers; i++)
        {
            if (static_cast<uintptr_t>(buffer_length(out_buffer_names[i])) < fft_size)
            {
                object_error((t_object *) x, "buffer %s is not long enough to complete write (no buffers altered)", out_buffer_names[i]->s_name);
                return;
            }
        }
    }

    // Allocate Resources

    temp_fft_setup fft_setup(fft_size_log2);

    temp_ptr<double> temp(fft_size * (num_buffers + 1));
    
    double *temp_buffer_d = temp.get();
    float  *temp_buffer_f = reinterpret_cast<float *>(temp_buffer_d);
    impulses[0].realp = temp.get() + fft_size;

    // Solve linear equations to generate multiplicative coeffients for harmonics

    t_matrix_complex coeff = matrix_non_linear(num_buffers);

    // Check Memory Allocations

    if (!fft_setup || !temp)
    {
        object_error((t_object *) x, "could not allocate temporary memory for processing");
        return;
    }

    // Set pointers to impulses and derference the coefficient matrix

    for (short i = 0; i < num_buffers; i++)
    {
        impulses[i].realp = impulses[0].realp + (fft_size * i);
        impulses[i].imagp = impulses[i].realp + (fft_size >> 1);
    }

    // Do Transforms In

    for (short i = 0; i < num_buffers; i++)
    {
        length = buffer_read(in_buffer_names[i], read_chan, temp_buffer_f, fft_size);
        time_to_halfspectrum_float(fft_setup, temp_buffer_f, length, impulses[i], fft_size);
    }

    // Convert

    for (short i = 0; i < num_buffers; i++)
    {
        if (!(i & 1))
        {
            // Odd harmonics (N.B. - i is offset by 1)

            // Copy first IR

            double current_coeff = coeff(i, i).real();

            for (uintptr_t j = 0; j < (fft_size >> 1); j++)
            {
                impulses[i].realp[j] *= current_coeff;
                impulses[i].imagp[j] *= current_coeff;
            }

            // Accumulate other IRs

            for (short k = i + 2; k < num_buffers; k += 2)
            {
                current_coeff = coeff(i, k).real();

                for (uintptr_t j = 0; j < (fft_size >> 1); j++)
                {
                    impulses[i].realp[j] += impulses[k].realp[j] * current_coeff;
                    impulses[i].imagp[j] += impulses[k].imagp[j] * current_coeff;
                }
            }
        }
        else
        {
            // Even harmonics (N.B. - i is offset by 1)

            // Copy first IR (multiplying by j first)

            double current_coeff = coeff(i, i).imag();

            // Zero DC / Nyquist (need to be real for real signal)

            impulses[i].realp[0] = 0.0;
            impulses[i].imagp[0] = 0.0;

            for (uintptr_t j = 1; j < (fft_size >> 1); j++)
            {
                impulses[i].realp[j] = -impulses[i].imagp[j] * current_coeff;
                impulses[i].imagp[j] =  impulses[i].realp[j] * current_coeff;
            }

            // Accumulate by other IRs (multiplying by j first)

            for (short k = i + 2; k < num_buffers; k += 2)
            {
                double current_coeff = coeff(i, k).imag();

                for (uintptr_t j = 1; j < (fft_size >> 1); j++)
                {
                    impulses[i].realp[j] += -impulses[k].imagp[j] * current_coeff;
                    impulses[i].imagp[j] +=  impulses[k].realp[j] * current_coeff;
                }
            }
        }

    }

    // Do Transforms Out

    for (short i = 0; i < num_buffers; i++)
    {
        spectrum_to_time(fft_setup, temp_buffer_d, impulses[i], fft_size, SPECTRUM_REAL);
        auto error = buffer_write((t_object *)x, out_buffer_names[i], temp_buffer_d, fft_size, write_chan, x->resize, sample_rate, 1.0);
        overall_error = error ? true : overall_error;
    }

    // Bang on success

    if (!overall_error)
        outlet_bang(x->process_done);
}
