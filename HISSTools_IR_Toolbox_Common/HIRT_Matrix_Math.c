
#include "HIRT_Matrix_Math.h"


//////////////////////////////////////////////////////////////////////////
////////////////////// Allocation and Deallocation ///////////////////////
//////////////////////////////////////////////////////////////////////////


t_matrix_complex *matrix_alloc_complex(AH_UIntPtr m_dim, AH_UIntPtr n_dim)
{
    t_matrix_complex *matrix = malloc(sizeof(t_matrix_complex));

    if (! matrix)
        return 0;

    matrix->matrix_data = malloc(sizeof(COMPLEX_DOUBLE) * m_dim * n_dim);

    if (!matrix->matrix_data)
    {
        matrix_destroy_complex(matrix);
        return 0;
    }

    matrix->m_dim = matrix->max_m_dim = m_dim;
    matrix->n_dim = matrix->max_n_dim = n_dim;

    return matrix;
}

void matrix_destroy_complex(t_matrix_complex *matrix)
{
    free(matrix->matrix_data);
    free(matrix);
}

//////////////////////////////////////////////////////////////////////////
////////////////////////////// Change Size ///////////////////////////////
//////////////////////////////////////////////////////////////////////////


long matrix_new_size_complex(t_matrix_complex *out, AH_UIntPtr m_dim, AH_UIntPtr n_dim)
{
    if (m_dim > out->max_m_dim || n_dim > out->max_n_dim)
        return 1;

    out->m_dim = m_dim;
    out->n_dim = n_dim;

    return 0;
}


//////////////////////////////////////////////////////////////////////////
/////////////////// Multiplication (out-of-place only) ///////////////////
//////////////////////////////////////////////////////////////////////////


void matrix_multiply_complex(t_matrix_complex *out, t_matrix_complex *in1, t_matrix_complex *in2)
{
    AH_UIntPtr m_dim = in1->m_dim;
    AH_UIntPtr n_dim = in2->n_dim;
    AH_UIntPtr z_dim;
    AH_UIntPtr i, j, k;

    MATRIX_REF_COMPLEX(out)
    MATRIX_REF_COMPLEX(in1)
    MATRIX_REF_COMPLEX(in2)

    COMPLEX_DOUBLE sum;
    COMPLEX_DOUBLE temp;

    // Check Dimensions / Size Output

    if (in1->n_dim != in2->m_dim)
        return;

    if (matrix_new_size_complex(out, m_dim, n_dim))
        return;

    z_dim = in2->m_dim;

    // Dereference

    MATRIX_DEREF(out)
    MATRIX_DEREF(in1)
    MATRIX_DEREF(in2)

    // Do Multiplication

    for (i = 0; i < m_dim; i++)
    {
        for (j = 0; j < n_dim; j++)
        {
            for (sum = CSET(0, 0), k = 0; k < z_dim; k++)
            {
                temp = CMUL(MATRIX_ELEMENT(in1, i , k), MATRIX_ELEMENT(in2, k, j));
                sum = CADD(sum, temp);
            }

            MATRIX_ELEMENT(out, i, j) = sum;
        }
    }
}


//////////////////////////////////////////////////////////////////////////
///////////// Matrix Solver - Choelsky Complex Decomposition /////////////
//////////////////////////////////////////////////////////////////////////


long matrix_choelsky_decompose_complex(t_matrix_complex *out, t_matrix_complex *in)
{
    AH_UIntPtr z_dim = in->m_dim;
    AH_UIntPtr i, j, k;

    COMPLEX_DOUBLE sum;

    MATRIX_REF_COMPLEX(in)
    MATRIX_REF_COMPLEX(out)

    // Check Dimensions / Size Output

    if (in->n_dim != z_dim)
        return 1;

    if (matrix_new_size_complex(out, z_dim, z_dim))
        return 1;

    // Dereference

    MATRIX_DEREF(in);
    MATRIX_DEREF(out);

    // Zero

    for (i = 0; i < z_dim * z_dim; i++)
        out->matrix_data[i] = CSET(0, 0);

    // Decompose

    for (i = 0; i < z_dim; i++)
    {
        for (j = 0; j < i; j++)
        {
            for (sum = MATRIX_ELEMENT(in, i, j), k = 0; k < j; k++)
                sum = CSUB(sum, CMUL(MATRIX_ELEMENT(out, i, k), CONJ(MATRIX_ELEMENT(out, j, k))));

            sum = CDIV(sum, MATRIX_ELEMENT(out, j, j));

            MATRIX_ELEMENT(out, i, j) = sum;
        }

        for (sum = MATRIX_ELEMENT(in, i, i), k = 0; k < i; k++)
            sum = CSUB(sum, CMUL(MATRIX_ELEMENT(out, i, k), CONJ(MATRIX_ELEMENT(out, i, k))));

        if (CREAL(sum) <= 0.0)
            return 1;

        MATRIX_ELEMENT(out, i, i) = CSET(sqrt(CREAL(sum)), 0);
    }

    return 0;
}


void matrix_choelsky_solve_complex(t_matrix_complex *out, t_matrix_complex *decompose, t_matrix_complex *solve)
{
    AH_UIntPtr m_dim = solve->m_dim;
    AH_UIntPtr n_dim = solve->n_dim;
    AH_UIntPtr i, j, k;

    COMPLEX_DOUBLE sum;

    MATRIX_REF_COMPLEX(solve)
    MATRIX_REF_COMPLEX(decompose)
    MATRIX_REF_COMPLEX(out)

    // Check Dimensions / Size Output

    if (m_dim != decompose->m_dim || m_dim != decompose->n_dim)
        return;

    if (matrix_new_size_complex(out, m_dim, n_dim))
        return;

    MATRIX_DEREF(solve)
    MATRIX_DEREF(decompose)
    MATRIX_DEREF(out)

    for (i = 0; i < n_dim; i++)
    {
        // Solve L.y = b (forward substitution)

        for (j = 0; j < m_dim; j++)
        {
            for (sum = MATRIX_ELEMENT(solve, j, i), k = 0; k < j; k++)
                sum = CSUB(sum, CMUL(MATRIX_ELEMENT(decompose, j, k), MATRIX_ELEMENT(out, k, i)));
            MATRIX_ELEMENT(out, j, i) = CDIV(sum, MATRIX_ELEMENT(decompose, j, j));
        }

        // Solve LT.x = y (backward substitution)

        for (j = m_dim; j >= 1; j--)
        {
            for (sum = MATRIX_ELEMENT(out, j - 1, i), k = j; k < m_dim; k++)
                sum = CSUB(sum, CMUL(CONJ(MATRIX_ELEMENT(decompose, k, j - 1)), MATRIX_ELEMENT(out, k, i)));

            MATRIX_ELEMENT(out, j - 1, i) = CDIV(sum, MATRIX_ELEMENT(decompose, j - 1, j - 1));
        }
    }
}


//////////////////////////////////////////////////////////////////////////
////////////////////////// Conjugate Transpose ///////////////////////////
//////////////////////////////////////////////////////////////////////////


void matrix_conjugate_transpose_complex(t_matrix_complex *out, t_matrix_complex *in)
{
    AH_UIntPtr m_dim = in->n_dim;
    AH_UIntPtr n_dim = in->m_dim;
    AH_UIntPtr i, j;

    MATRIX_REF_COMPLEX(in)
    MATRIX_REF_COMPLEX(out)

    // Check Dimensions / Size Output

    if (matrix_new_size_complex(out, m_dim, n_dim))
        return;

    // Dereference

    MATRIX_DEREF(in)
    MATRIX_DEREF(out)

    // Prepare Matrix

    for (i = 0; i < m_dim; i++)
    {
        for (j = 0; j < n_dim; j++)
            MATRIX_ELEMENT(out, i, j) = CONJ(MATRIX_ELEMENT(in, j, i));
    }
}
