
#pragma once

template <class T, class U, int vec_size>
struct SIMDVector
{
    static constexpr int size = vec_size;
    typedef T scalar_type;
    
    SIMDVector() {}
    SIMDVector(U a) : mVal(a) {}
    
    U mVal;
};

#if defined __arm__ || defined __arm64__

#include <arm_neon.h>

struct ARMFloat : public SIMDVector<float, float32x4_t, 4>
{
    ARMFloat() {}
    ARMFloat(uint32x4_t a) : SIMDVector(a) {}
    ARMFloat(float a) : SIMDVector(vdupq_n_f32(a)) {}
    
    friend ARMFloat operator + (const ARMFloat& a, const ARMFloat& b) { return vaddq_f32(a.mVal, b.mVal); }
    friend ARMFloat operator - (const ARMFloat& a, const ARMFloat& b) { return vsubq_f32(a.mVal, b.mVal); }
    friend ARMFloat operator * (const ARMFloat& a, const ARMFloat& b) { return vmulq_f32(a.mVal, b.mVal); }
    
    ARMFloat operator += (const ARMFloat& a)
    {
        *this = *this + a;
        return *this;
    }
    
    static ARMFloat unaligned_load(const float* ptr) { return vld1q_f32(ptr); }
    
    void unaligned_store(float* ptr)
    {
        vst1q_f32(ptr, mVal);
    }
    
    float sum()
    {
        float values[4];
        unaligned_store(values);
        return values[0] + values[1] + values[2] + values[3];
    }
};

typedef ARMFloat FloatVector;

#else

#ifdef __APPLE__
#include <Accelerate/Accelerate.h>
#else
#include <emmintrin.h>
#endif

struct SSEFloat : public SIMDVector<float, __m128, 4>
{
    SSEFloat() {}
    SSEFloat(__m128 a) : SIMDVector(a) {}
    SSEFloat(float a) : SIMDVector(_mm_set1_ps(a)) {}
    
    friend SSEFloat operator + (const SSEFloat& a, const SSEFloat& b) { return _mm_add_ps(a.mVal, b.mVal); }
    friend SSEFloat operator - (const SSEFloat& a, const SSEFloat& b) { return _mm_sub_ps(a.mVal, b.mVal); }
    friend SSEFloat operator * (const SSEFloat& a, const SSEFloat& b) { return _mm_mul_ps(a.mVal, b.mVal); }
    
    SSEFloat operator += (const SSEFloat& a)
    {
        *this = *this + a;
        return *this;
    }
    
    static SSEFloat unaligned_load(const float* ptr) { return _mm_loadu_ps(ptr); }
    
    void unaligned_store(float* ptr)
    {
        _mm_storeu_ps(ptr, mVal);
    }
    
    float sum()
    {
        float values[4];
        unaligned_store(values);
        return values[0] + values[1] + values[2] + values[3];
    }
};

typedef SSEFloat FloatVector;

#endif

#ifdef __APPLE__
#define ALIGNED_MALLOC malloc
#define ALIGNED_FREE free
#elif defined _WIN32 || defined _WIN64
#include <malloc.h>
#define ALIGNED_MALLOC(x)  _aligned_malloc(x, 16)
#define ALIGNED_FREE(x)  _aligned_free(x)
#else
#define ALIGNED_MALLOC(x) aligned_alloc(16, x);
#define ALIGNED_FREE free
#endif

