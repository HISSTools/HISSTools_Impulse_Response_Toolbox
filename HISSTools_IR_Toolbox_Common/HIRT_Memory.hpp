
#ifndef __HIRT_MEMORY__
#define __HIRT_MEMORY__

#include <SIMDSupport.hpp>

template <typename T>
class temp_ptr
{
public:
    
    temp_ptr(size_t size) : m_ptr(size ? allocate_aligned<T>(size) : nullptr)
    {}
    
    ~temp_ptr()
    {
        release();
    }
    
    temp_ptr(const temp_ptr& a) = delete;
    temp_ptr& operator = (const temp_ptr& a) = delete;
    
    temp_ptr(temp_ptr&& a)
    {
        m_ptr = a.m_ptr;
        a.m_ptr = nullptr;
    }
    
    temp_ptr& operator = (temp_ptr&& a)
    {
        release();
        m_ptr = a.m_ptr;
        a.m_ptr = nullptr;
        
        return *this;
    }

    operator bool()                         { return m_ptr; }
    
    T *get()                                { return m_ptr; }
    const T *get() const                    { return m_ptr; }
    
    T& operator [](size_t N )               { return m_ptr[N]; }
    const T& operator [](size_t N ) const   { return m_ptr[N]; }

private:
    
    void release()
    {
        if (m_ptr)
        {
            deallocate_aligned(m_ptr);
            m_ptr = nullptr;
        }
    }
    
    T *m_ptr;
};

#endif /* __HIRT_MEMORY__ */
