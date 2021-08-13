
#ifndef __HIRT_MATRIX_MATH__
#define __HIRT_MATRIX_MATH__

#include <complex>
#include <cstdint>
#include <vector>

// Complex Matrix Structure

class t_matrix_complex
{
public:
    
    t_matrix_complex(uintptr_t m_dim, uintptr_t n_dim);
    
    bool new_size(uintptr_t m_dim, uintptr_t n_dim);
    void zero();
    
    std::complex<double>& operator()(uintptr_t m, uintptr_t n)
    {
        return m_data[m + (m_m_dim * n)];
    }
    
    const std::complex<double>& operator()(uintptr_t m, uintptr_t n) const
    {
        return m_data[m + (m_m_dim * n)];

    }
    
    uintptr_t M() const { return m_m_dim; }
    uintptr_t N() const { return m_n_dim; }
    
private:
    
    uintptr_t m_m_dim;
    uintptr_t m_n_dim;

    std::vector<std::complex<double>> m_data;
};

// Function Prototypes

void matrix_multiply_complex(t_matrix_complex& out, t_matrix_complex& in1, t_matrix_complex& in2);

long matrix_choelsky_decompose_complex(t_matrix_complex& out, t_matrix_complex& in);
void matrix_choelsky_solve_complex(t_matrix_complex& out, t_matrix_complex& decompose, t_matrix_complex& solve);
void matrix_conjugate_transpose_complex(t_matrix_complex& out, t_matrix_complex& in);

#endif    /* HIRT_MATRIX_MATH_ */
