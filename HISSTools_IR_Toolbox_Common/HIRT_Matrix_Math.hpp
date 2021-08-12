
#ifndef __HIRT_MATRIX_MATH__
#define __HIRT_MATRIX_MATH__

#ifndef __APPLE__
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#endif

#include <complex>
#include <AH_Types.h>

// Complex Matrix Structure

struct t_matrix_complex
{
    AH_UIntPtr m_dim;
    AH_UIntPtr n_dim;

    AH_UIntPtr max_m_dim;
    AH_UIntPtr max_n_dim;

    std::complex<double> *matrix_data;
};

// Matrix Defines

#define MATRIX_REF_COMPLEX(matrix) \
std::complex<double> *__##matrix##_data_ptr; \
AH_UIntPtr __##matrix##_m_dim;

#define MATRIX_DEREF(matrix) \
__##matrix##_data_ptr = matrix->matrix_data; \
__##matrix##_m_dim = matrix->m_dim;

#define MATRIX_ELEMENT(matrix, m, n) __##matrix##_data_ptr[(m) + (__##matrix##_m_dim * (n))]

// Function Prototypes

void matrix_destroy_complex(t_matrix_complex *matrix);
t_matrix_complex *matrix_alloc_complex(AH_UIntPtr m_dim, AH_UIntPtr n_dim);
long matrix_new_size_complex(t_matrix_complex *out, AH_UIntPtr m_dim, AH_UIntPtr n_dim);

void matrix_multiply_complex (t_matrix_complex *out, t_matrix_complex *in1, t_matrix_complex *in2);

long matrix_choelsky_decompose_complex(t_matrix_complex *out, t_matrix_complex *in);
void matrix_choelsky_solve_complex(t_matrix_complex *out, t_matrix_complex *decompose, t_matrix_complex *solve);
void matrix_conjugate_transpose_complex(t_matrix_complex *out, t_matrix_complex *in);

#endif    /* HIRT_MATRIX_MATH_ */
