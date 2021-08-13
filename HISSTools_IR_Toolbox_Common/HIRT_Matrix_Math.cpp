
#include "HIRT_Matrix_Math.hpp"

using complex = std::complex<double>;

//////////////////////////////////////////////////////////////////////////
/////////////////////////////// Constructor //////////////////////////////
//////////////////////////////////////////////////////////////////////////

t_matrix_complex::t_matrix_complex(uintptr_t m_dim, uintptr_t n_dim)
{
    m_data.resize(m_dim * n_dim);

    m_m_dim = m_dim;
    m_n_dim = n_dim;
}

//////////////////////////////////////////////////////////////////////////
/////////////////////////// Change Size / Zero ///////////////////////////
//////////////////////////////////////////////////////////////////////////

bool t_matrix_complex::new_size(uintptr_t m_dim, uintptr_t n_dim)
{
    if (m_dim * n_dim > m_data.max_size())
        return true;

    m_m_dim = m_dim;
    m_n_dim = n_dim;

    return false;
}

void t_matrix_complex::zero()
{
    for (uintptr_t i = 0; i < m_m_dim * m_n_dim; i++)
        m_data[i] = complex(0.0, 0.0);
}
            
//////////////////////////////////////////////////////////////////////////
/////////////////// Multiplication (out-of-place only) ///////////////////
//////////////////////////////////////////////////////////////////////////

void matrix_multiply_complex(t_matrix_complex& out, t_matrix_complex& in1, t_matrix_complex& in2)
{
    uintptr_t m_dim = in1.M();
    uintptr_t n_dim = in2.N();
    uintptr_t z_dim;

    // Check Dimensions / Size Output

    if (in1.N() != in2.M())
        return;

    if (out.new_size(m_dim, n_dim))
        return;

    z_dim = in2.M();

    // Do Multiplication

    for (uintptr_t i = 0; i < m_dim; i++)
    {
        for (uintptr_t j = 0; j < n_dim; j++)
        {
            complex sum = complex(0, 0);
            
            for (uintptr_t k = 0; k < z_dim; k++)
                sum += in1(i, k) * in2(k, j);

            out(i, j) = sum;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
///////////// Matrix Solver - Choelsky Complex Decomposition /////////////
//////////////////////////////////////////////////////////////////////////

long matrix_choelsky_decompose_complex(t_matrix_complex& out, t_matrix_complex& in)
{
    uintptr_t z_dim = in.M();
    
    // Check Dimensions / Size Output

    if (in.N() != z_dim)
        return 1;

    if (out.new_size(z_dim, z_dim))
        return 1;

    // Zero

    out.zero();

    // Decompose

    for (uintptr_t i = 0; i < z_dim; i++)
    {
        for (uintptr_t j = 0; j < i; j++)
        {
            complex sum = in(i, j);
            
            for (uintptr_t k = 0; k < j; k++)
                sum -= out(i, k) * std::conj(out(j, k));

            sum /= out(j, j);

            out(i, j) = sum;
        }

        complex sum = in(i, i);
        
        for (uintptr_t j = 0; j < i; j++)
            sum -= out(i, j) * std::conj(out(i, j));

        if (sum.real() <= 0.0)
            return 1;

        out(i, i) = complex(sqrt(sum.real()), 0);
    }

    return 0;
}

void matrix_choelsky_solve_complex(t_matrix_complex& out, t_matrix_complex& decompose, t_matrix_complex& solve)
{
    uintptr_t m_dim = solve.M();
    uintptr_t n_dim = solve.N();

    // Check Dimensions / Size Output

    if (m_dim != decompose.M() || m_dim != decompose.N())
        return;

    if (out.new_size(m_dim, n_dim))
        return;

    for (uintptr_t i = 0; i < n_dim; i++)
    {
        // Solve L.y = b (forward substitution)

        for (uintptr_t j = 0; j < m_dim; j++)
        {
            complex sum = solve(j, i);
            
            for (uintptr_t k = 0; k < j; k++)
                sum -= decompose(j, k) * out(k, i);
            
            out(j, i) = sum / decompose(j, j);
        }

        // Solve LT.x = y (backward substitution)

        for (uintptr_t j = m_dim; j >= 1; j--)
        {
            complex sum = out(j - 1, i);
            
            for (uintptr_t k = j; k < m_dim; k++)
                sum -= (std::conj(decompose(k, j - 1)) * out(k, i));

            out(j - 1, i) = sum / decompose(j - 1, j - 1);
        }
    }
}

//////////////////////////////////////////////////////////////////////////
////////////////////////// Conjugate Transpose ///////////////////////////
//////////////////////////////////////////////////////////////////////////

void matrix_conjugate_transpose_complex(t_matrix_complex& out, t_matrix_complex& in)
{
    uintptr_t m_dim = in.N();
    uintptr_t n_dim = in.M();

    // Check Dimensions / Size Output

    if (out.new_size(m_dim, n_dim))
        return;

    // Prepare Matrix

    for (uintptr_t i = 0; i < m_dim; i++)
    {
        for (uintptr_t j = 0; j < n_dim; j++)
            out(i, j) = std::conj(in(j, i));
    }
}
