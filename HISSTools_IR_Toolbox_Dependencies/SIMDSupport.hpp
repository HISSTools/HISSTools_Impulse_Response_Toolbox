
#ifndef SIMDSUPPORT_HPP
#define SIMDSUPPORT_HPP

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <functional>

#if defined(__arm__) || defined(__arm64)
#include <arm_neon.h>
#include <memory.h>
#define SIMD_COMPILER_SUPPORT_NEON 1
#elif defined(__APPLE__) || defined(__linux__) || defined(_WIN32)
#if defined(_WIN32)
#include <malloc.h>
#include <intrin.h>
#endif
#include <emmintrin.h>
#include <immintrin.h>
#endif

// ******************** MSVC SSE Support Detection ********************* //

// MSVC doesn't ever define __SSE__ so if needed set it from other defines

#ifndef __SSE__
#if defined _M_X64 || (defined _M_IX86_FP && _M_IX86_FP > 0)
#define __SSE__ 1
#endif
#endif

// ****************** Determine SIMD Compiler Support ****************** //

#define SIMD_COMPILER_SUPPORT_SCALAR 0
#define SIMD_COMPILER_SUPPORT_VEC128 1
#define SIMD_COMPILER_SUPPORT_VEC256 2
#define SIMD_COMPILER_SUPPORT_VEC512 3

template<class T>
struct SIMDLimits
{
    static const int max_size = 1;
    static const int byte_width = sizeof(T);
};

#if defined(__AVX512F__)
#define SIMD_COMPILER_SUPPORT_LEVEL SIMD_COMPILER_SUPPORT_VEC512

template<>
struct SIMDLimits<double>
{
    static const int max_size = 8;
    static const int byte_width = 64;
};

template<>
struct SIMDLimits<float>
{
    static const int max_size = 16;
    static const int byte_width = 64;
};

#elif defined(__AVX__)
#define SIMD_COMPILER_SUPPORT_LEVEL SIMD_COMPILER_SUPPORT_VEC256

template<>
struct SIMDLimits<double>
{
    static const int max_size = 4;
    static const int byte_width = 32;
};

template<>
struct SIMDLimits<float>
{
    static const int max_size = 8;
    static const int byte_width = 32;
};

#elif defined(__SSE__) || defined(__arm__) || defined(__arm64)
#define SIMD_COMPILER_SUPPORT_LEVEL SIMD_COMPILER_SUPPORT_VEC128

#if defined (__SSE__) || defined(__arm64)
template<>
struct SIMDLimits<double>
{
    static const int max_size = 2;
    static const int byte_width = 16;
};
#endif

template<>
struct SIMDLimits<float>
{
    static const int max_size = 4;
    static const int byte_width = 16;
};

#else
#define SIMD_COMPILER_SUPPORT_LEVEL SIMD_COMPILER_SUPPORT_SCALAR
#endif

// ********************* Aligned Memory Allocation ********************* //

#ifdef __APPLE__

template <class T>
T *allocate_aligned(size_t size)
{
    return static_cast<T *>(malloc(size * sizeof(T)));
}

template <class T>
void deallocate_aligned(T *ptr)
{
    free(ptr);
}

#elif defined(__linux__)

template <class T>
T *allocate_aligned(size_t size)
{
    void *mem;
    posix_memalign(&mem, SIMDLimits<T>::byte_width, size * sizeof(T));
    return static_cast<T *>(mem);
}

template <class T>
void deallocate_aligned(T *ptr)
{
    free(ptr);
}

#else

template <class T>
T *allocate_aligned(size_t size)
{
    return static_cast<T *>(_aligned_malloc(size * sizeof(T), SIMDLimits<T>::byte_width));
}

template <class T>
void deallocate_aligned(T *ptr)
{
    _aligned_free(ptr);
}

#endif

// ******************** Basic Data Type Definitions ******************** //

template <class T, class U, int vec_size>
struct SIMDVector
{
    static const int size = vec_size;
    typedef T scalar_type;
    
    SIMDVector() {}
    SIMDVector(U a) : mVal(a) {}
    
    U mVal;
};

template <class T, int vec_size>
struct SIMDType {};

// ************* A Vector of Given Size (Made of Vectors) ************** //

template <class T, int vec_size, int final_size>
struct SizedVector
{
    using SV = SizedVector;
    using VecType = SIMDType<T, vec_size>;
    static const int size = final_size;
    static const int array_size = final_size / vec_size;
    
    SizedVector() {}
    SizedVector(const T& a) { static_iterate<>().set(*this, a); }
    SizedVector(const SizedVector *ptr) { *this = *ptr; }
    SizedVector(const T *array) { static_iterate<>().load(*this, array); }
    
    // For scalar conversions use a constructor
    
    template <class U>
    SizedVector(const SizedVector<U, 1, final_size>& vec)
    {
        static_iterate<>().set(*this, vec);
    }
    
    // Attempt to cast types directly for conversions if casts are provided
    
    template <class U>
    SizedVector(const SizedVector<U, final_size, final_size>& v)
    : SizedVector(v.mData[0])
    {}
    
    void store(T *a) const { static_iterate<>().store(a, *this); }

    friend SV operator + (const SV& a, const SV& b) { return op(a, b, std::plus<VecType>()); }
    friend SV operator - (const SV& a, const SV& b) { return op(a, b, std::minus<VecType>()); }
    friend SV operator * (const SV& a, const SV& b) { return op(a, b, std::multiplies<VecType>()); }
    friend SV operator / (const SV& a, const SV& b) { return op(a, b, std::divides<VecType>()); }
    
    SV& operator += (const SV& b) { return (*this = *this + b); }
    SV& operator -= (const SV& b) { return (*this = *this - b); }
    SV& operator *= (const SV& b) { return (*this = *this * b); }
    SV& operator /= (const SV& b) { return (*this = *this / b); }
    
    friend SV min(const SV& a, const SV& b) { return op(a, b, std::min<VecType>()); }
    friend SV max(const SV& a, const SV& b) { return op(a, b, std::max<VecType>()); }
    
    friend SV operator == (const SV& a, const SV& b) { return op(a, b, std::equal_to<VecType>()); }
    friend SV operator != (const SV& a, const SV& b) { return op(a, b, std::not_equal_to<VecType>()); }
    friend SV operator > (const SV& a, const SV& b) { return op(a, b, std::greater<VecType>()); }
    friend SV operator < (const SV& a, const SV& b) { return op(a, b, std::less<VecType>()); }
    friend SV operator >= (const SV& a, const SV& b) { return op(a, b, std::greater_equal<VecType>()); }
    friend SV operator <= (const SV& a, const SV& b) { return op(a, b, std::less_equal<VecType>()); }
    
    VecType mData[array_size];
    
private:
    
    // Helpers
    
    // This template allows static loops
    
    template <int First = 0, int Last = array_size>
    struct static_iterate
    {
        template <typename Fn>
        void operator()(SV &result, const SV& a, const SV& b, Fn const& fn) const
        {
            result.mData[First] = fn(a.mData[First], b.mData[First]);
            static_iterate<First + 1, Last>()(result, a, b, fn);
        }
        
        void load(SV &vector, const T *array)
        {
            vector.mData[First] = VecType(array + First * vec_size);
            static_iterate<First + 1, Last>().load(vector, array);
        }
        
        void store(T *array, const SV& vector)
        {
            vector.mData[First].store(array + First * vec_size);
            static_iterate<First + 1, Last>().store(array, vector);
        }
        
        void set(SV &vector, const T& a)
        {
            vector.mData[First] = a;
            static_iterate<First + 1, Last>().set(vector, a);
        }
        
        template <class U>
        void set(SV &vector, const SizedVector<U, 1, final_size>& a)
        {
            vector.mData[First] = a.mData[First];
            static_iterate<First + 1, Last>().set(vector, a);
        }
    };
    
    // This specialisation avoids infinite recursion
    
    template <int N>
    struct static_iterate<N, N>
    {
        template <typename Fn>
        void operator()(SV &result, const SV& a, const SV& b, Fn const& fn) const {}
        
        void load(SV &vector, const T *array) {}
        void store(T *array, const SV& vector) {}
        void set(SV &vector, const T& a) {}
        
        template <class U>
        void set(SV &vector, const SizedVector<U, 1, final_size>& a) {}
    };
    
    // Op template
    
    template <typename Op>
    friend SV op(const SV& a, const SV& b, Op op)
    {
        SV result;
        static_iterate<>()(result, a, b, op);
        return result;
    }
};

// ************** Platform-Agnostic Data Type Definitions ************** //

template<>
struct SIMDType<double, 1>
{
    static const int size = 1;
    typedef double scalar_type;
    
    SIMDType() {}
    SIMDType(double a) : mVal(a) {}
    SIMDType(const double* a) { mVal = *a; }
    
    void store(double *a) const { *a = mVal; }
    
    friend SIMDType operator + (const SIMDType& a, const SIMDType& b) { return a.mVal + b.mVal; }
    friend SIMDType operator - (const SIMDType& a, const SIMDType& b) { return a.mVal - b.mVal; }
    friend SIMDType operator * (const SIMDType& a, const SIMDType& b) { return a.mVal * b.mVal; }
    friend SIMDType operator / (const SIMDType& a, const SIMDType& b) { return a.mVal / b.mVal; }
    
    SIMDType& operator += (const SIMDType& b) { return (*this = *this + b); }
    SIMDType& operator -= (const SIMDType& b) { return (*this = *this - b); }
    SIMDType& operator *= (const SIMDType& b) { return (*this = *this * b); }
    SIMDType& operator /= (const SIMDType& b) { return (*this = *this / b); }
    
    friend SIMDType sqrt(const SIMDType& a) { return sqrt(a.mVal); }
    
    friend SIMDType round(const SIMDType& a) { return round(a.mVal); }
    friend SIMDType trunc(const SIMDType& a) { return trunc(a.mVal); }
    
    friend SIMDType min(const SIMDType& a, const SIMDType& b) { return std::min(a.mVal, b.mVal); }
    friend SIMDType max(const SIMDType& a, const SIMDType& b) { return std::max(a.mVal, b.mVal); }
    friend SIMDType sel(const SIMDType& a, const SIMDType& b, const SIMDType& c) { return c.mVal ? b.mVal : a.mVal; }
    
    friend SIMDType operator == (const SIMDType& a, const SIMDType& b) { return a.mVal == b.mVal; }
    friend SIMDType operator != (const SIMDType& a, const SIMDType& b) { return a.mVal != b.mVal; }
    friend SIMDType operator > (const SIMDType& a, const SIMDType& b) { return a.mVal > b.mVal; }
    friend SIMDType operator < (const SIMDType& a, const SIMDType& b) { return a.mVal < b.mVal; }
    friend SIMDType operator >= (const SIMDType& a, const SIMDType& b) { return a.mVal >= b.mVal; }
    friend SIMDType operator <= (const SIMDType& a, const SIMDType& b) { return a.mVal <= b.mVal; }
    
    double mVal;
};

template<>
struct SIMDType<float, 1>
{
    static const int size = 1;
    typedef float scalar_type;
    
    SIMDType() {}
    SIMDType(float a) : mVal(a) {}
    SIMDType(const float* a) { mVal = *a; }
    
    SIMDType(const SIMDType<double, 1>& a) : mVal(static_cast<float>(a.mVal)) {}
    
    void store(float *a) const { *a = mVal; }
    
    friend SIMDType operator + (const SIMDType& a, const SIMDType& b) { return a.mVal + b.mVal; }
    friend SIMDType operator - (const SIMDType& a, const SIMDType& b) { return a.mVal - b.mVal; }
    friend SIMDType operator * (const SIMDType& a, const SIMDType& b) { return a.mVal * b.mVal; }
    friend SIMDType operator / (const SIMDType& a, const SIMDType& b) { return a.mVal / b.mVal; }
    
    SIMDType& operator += (const SIMDType& b) { return (*this = *this + b); }
    SIMDType& operator -= (const SIMDType& b) { return (*this = *this - b); }
    SIMDType& operator *= (const SIMDType& b) { return (*this = *this * b); }
    SIMDType& operator /= (const SIMDType& b) { return (*this = *this / b); }
    
    friend SIMDType sqrt(const SIMDType& a) { return sqrtf(a.mVal); }
    
    friend SIMDType round(const SIMDType& a) { return roundf(a.mVal); }
    friend SIMDType trunc(const SIMDType& a) { return truncf(a.mVal); }
    
    friend SIMDType min(const SIMDType& a, const SIMDType& b) { return std::min(a.mVal, b.mVal); }
    friend SIMDType max(const SIMDType& a, const SIMDType& b) { return std::max(a.mVal, b.mVal); }
    friend SIMDType sel(const SIMDType& a, const SIMDType& b, const SIMDType& c) { return c.mVal ? b.mVal : a.mVal; }
    
    friend SIMDType operator == (const SIMDType& a, const SIMDType& b) { return a.mVal == b.mVal; }
    friend SIMDType operator != (const SIMDType& a, const SIMDType& b) { return a.mVal != b.mVal; }
    friend SIMDType operator > (const SIMDType& a, const SIMDType& b) { return a.mVal > b.mVal; }
    friend SIMDType operator < (const SIMDType& a, const SIMDType& b) { return a.mVal < b.mVal; }
    friend SIMDType operator >= (const SIMDType& a, const SIMDType& b) { return a.mVal >= b.mVal; }
    friend SIMDType operator <= (const SIMDType& a, const SIMDType& b) { return a.mVal <= b.mVal; }
    
    operator SIMDType<double, 1>() { return static_cast<double>(mVal); }
    
    float mVal;
};

template<>
struct SIMDType<float, 2>
{
    static const int size = 1;
    typedef float scalar_type;
    
    SIMDType() {}
    
    SIMDType(float a)
    {
        mVals[0] = a;
        mVals[1] = a;
    }
    
    SIMDType(float a, float b)
    {
        mVals[0] = a;
        mVals[1] = b;
    }
    
    SIMDType(const float* a)
    {
        mVals[0] = a[0];
        mVals[1] = a[1];
    }
    
    void store(float *a) const
    {
        a[0] = mVals[0];
        a[1] = mVals[1];
    }
    
    // N.B. - no ops
    
    float mVals[2];
};

// ************** Platform-Specific Data Type Definitions ************** //

// ************************ 128-bit SIMD Types ************************* //

#if (SIMD_COMPILER_SUPPORT_LEVEL >= SIMD_COMPILER_SUPPORT_VEC128)

#ifdef SIMD_COMPILER_SUPPORT_NEON /* Neon Intrinsics */

#if defined(__arm64)
template<>
struct SIMDType<double, 2> : public SIMDVector<double, float64x2_t, 2>
{
private:
    
    template <int64x2_t Op(int64x2_t, int64x2_t)>
    friend SIMDType bitwise(const SIMDType& a, const SIMDType& b)
    {
        return vreinterpretq_s64_f64(Op(vreinterpretq_s64_f64(a.mVal), vreinterpretq_s64_f64(b.mVal)));
    }
    
    friend float64x2_t neq(float64x2_t a, float64x2_t b)
    {
        return vreinterpretq_s32_f64(vmvnq_s32(vreinterpretq_s32_f64(vceqq_f64(a, b))));
    }
    
public:
    
    SIMDType() {}
    SIMDType(const double& a) { mVal = vdupq_n_f64(a); }
    SIMDType(const double* a) { mVal = vld1q_f64(a); }
    SIMDType(float64x2_t a) : SIMDVector(a) {}
    
    SIMDType(const SIMDType<float, 2> &a)
    {
        double vals[2];
        
        vals[0] = a.mVals[0];
        vals[1] = a.mVals[1];
        
        mVal = vld1q_f64(vals);
    }
    
    void store(double *a) const { vst1q_f64(a, mVal); }
    
    friend SIMDType operator + (const SIMDType& a, const SIMDType& b) { return vaddq_f64(a.mVal, b.mVal); }
    friend SIMDType operator - (const SIMDType& a, const SIMDType& b) { return vsubq_f64(a.mVal, b.mVal); }
    friend SIMDType operator * (const SIMDType& a, const SIMDType& b) { return vmulq_f64(a.mVal, b.mVal); }
    friend SIMDType operator / (const SIMDType& a, const SIMDType& b) { return vdivq_f64(a.mVal, b.mVal); }
    
    SIMDType& operator += (const SIMDType& b) { return (*this = *this + b); }
    SIMDType& operator -= (const SIMDType& b) { return (*this = *this - b); }
    SIMDType& operator *= (const SIMDType& b) { return (*this = *this * b); }
    SIMDType& operator /= (const SIMDType& b) { return (*this = *this / b); }
    
    friend SIMDType sqrt(const SIMDType& a) { return vsqrtq_f64(a.mVal); }
    
    // N.B. - ties issue (this matches intel, but not the scalar)
    //friend SIMDType round(const SIMDType& a) { return vrndnq_f64(a.mVal, _MM_FROUND_TO_NEAREST_INT |_MM_FROUND_NO_EXC); }
    friend SIMDType trunc(const SIMDType& a) { return vrndq_f64(a.mVal); }
    
    friend SIMDType min(const SIMDType& a, const SIMDType& b) { return vminq_f64(a.mVal, b.mVal); }
    friend SIMDType max(const SIMDType& a, const SIMDType& b) { return vmaxq_f64(a.mVal, b.mVal); }
    friend SIMDType sel(const SIMDType& a, const SIMDType& b, const SIMDType& c) { return and_not(c, a) | (b & c); }
    
    // N.B. - operand swap for and_not
    friend SIMDType and_not(const SIMDType& a, const SIMDType& b) { return bitwise<vbicq_s64>(b, a); }
    friend SIMDType operator & (const SIMDType& a, const SIMDType& b) { return bitwise<vandq_s64>(a, b); }
    friend SIMDType operator | (const SIMDType& a, const SIMDType& b) { return bitwise<vorrq_s64>(a, b); }
    friend SIMDType operator ^ (const SIMDType& a, const SIMDType& b) { return bitwise<veorq_s64>(a, b); }
    
    friend SIMDType operator == (const SIMDType& a, const SIMDType& b) { return vceqq_f64(a.mVal, b.mVal); }
    friend SIMDType operator != (const SIMDType& a, const SIMDType& b) { return neq(a.mVal, b.mVal); }
    friend SIMDType operator > (const SIMDType& a, const SIMDType& b) { return vcgtq_f64(a.mVal, b.mVal); }
    friend SIMDType operator < (const SIMDType& a, const SIMDType& b) { return vcltq_f64(a.mVal, b.mVal); }
    friend SIMDType operator >= (const SIMDType& a, const SIMDType& b) { return vcgeq_f64(a.mVal, b.mVal); }
    friend SIMDType operator <= (const SIMDType& a, const SIMDType& b) { return vcleq_f64(a.mVal, b.mVal); }
    /*
    template <int y, int x>
    static SIMDType shuffle(const SIMDType& a, const SIMDType& b)
    {
        return _mm_shuffle_pd(a.mVal, b.mVal, (y<<1)|x);
    }
    */
    operator SIMDType<float, 2>()
    {
        double vals[2];
        
        store(vals);
        
        return SIMDType<float, 2>(static_cast<float>(vals[0]), static_cast<float>(vals[1]));
    }
};
#endif /* defined (__arm64) */

template<>
struct SIMDType<float, 4> : public SIMDVector<float, float32x4_t, 4>
{
private:
    
    template <int32x4_t Op(int32x4_t, int32x4_t)>
    friend SIMDType bitwise(const SIMDType& a, const SIMDType& b)
    {
        return vreinterpretq_s32_f32(Op(vreinterpretq_s32_f32(a.mVal), vreinterpretq_s32_f32(b.mVal)));
    }
    
    friend float32x4_t neq(float32x4_t a, float32x4_t b)
    {
        return vreinterpretq_s32_f32(vmvnq_s32(vreinterpretq_s32_f32(vceqq_f32(a, b))));
    }
    
public:
    
    SIMDType() {}
    SIMDType(const float& a) { mVal = vdupq_n_f32(a); }
    SIMDType(const float* a) { mVal = vld1q_f32(a); }
    SIMDType(float32x4_t a) : SIMDVector(a) {}
    
    void store(float *a) const { vst1q_f32(a, mVal); }
    
    friend SIMDType operator + (const SIMDType& a, const SIMDType& b) { return vaddq_f32(a.mVal, b.mVal); }
    friend SIMDType operator - (const SIMDType& a, const SIMDType& b) { return vsubq_f32(a.mVal, b.mVal); }
    friend SIMDType operator * (const SIMDType& a, const SIMDType& b) { return vmulq_f32(a.mVal, b.mVal); }
    friend SIMDType operator / (const SIMDType& a, const SIMDType& b) { return vdivq_f32(a.mVal, b.mVal); }
    
    SIMDType& operator += (const SIMDType& b) { return (*this = *this + b); }
    SIMDType& operator -= (const SIMDType& b) { return (*this = *this - b); }
    SIMDType& operator *= (const SIMDType& b) { return (*this = *this * b); }
    SIMDType& operator /= (const SIMDType& b) { return (*this = *this / b); }
    
    friend SIMDType sqrt(const SIMDType& a) { return vsqrtq_f32(a.mVal); }
    
    // N.B. - ties issue (this matches intel, but not the scalar)
    //friend SIMDType round(const SIMDType& a) { return vrndnq_f32(a.mVal); }
    friend SIMDType trunc(const SIMDType& a) { return vrndq_f32(a.mVal); }
    
    friend SIMDType min(const SIMDType& a, const SIMDType& b) { return vminq_f32(a.mVal, b.mVal); }
    friend SIMDType max(const SIMDType& a, const SIMDType& b) { return vmaxq_f32(a.mVal, b.mVal); }
    friend SIMDType sel(const SIMDType& a, const SIMDType& b, const SIMDType& c) { return and_not(c, a) | (b & c); }
    
    // N.B. - operand swap for and_not
    friend SIMDType and_not(const SIMDType& a, const SIMDType& b) { return bitwise<vbicq_s32>(b, a); }
    friend SIMDType operator & (const SIMDType& a, const SIMDType& b) { return bitwise<vandq_s32>(a, b); }
    friend SIMDType operator | (const SIMDType& a, const SIMDType& b) { return bitwise<vorrq_s32>(a, b); }
    friend SIMDType operator ^ (const SIMDType& a, const SIMDType& b) { return bitwise<veorq_s32>(a, b); }
    
    friend SIMDType operator == (const SIMDType& a, const SIMDType& b) { return vceqq_f32(a.mVal, b.mVal); }
    friend SIMDType operator != (const SIMDType& a, const SIMDType& b) { return neq(a.mVal, b.mVal); }
    friend SIMDType operator > (const SIMDType& a, const SIMDType& b) { return vcgtq_f32(a.mVal, b.mVal); }
    friend SIMDType operator < (const SIMDType& a, const SIMDType& b) { return vcltq_f32(a.mVal, b.mVal); }
    friend SIMDType operator >= (const SIMDType& a, const SIMDType& b) { return vcgeq_f32(a.mVal, b.mVal); }
    friend SIMDType operator <= (const SIMDType& a, const SIMDType& b) { return vcleq_f32(a.mVal, b.mVal); }
    /*
    template <int z, int y, int x, int w>
    static SIMDType shuffle(const SIMDType& a, const SIMDType& b)
    {
        return _mm_shuffle_ps(a.mVal, b.mVal, ((z<<6)|(y<<4)|(x<<2)|w));
    }*/
    
    operator SizedVector<double, 2, 4>() const
    {
        SizedVector<double, 2, 4> vec;
        
        vec.mData[0] = vcvt_f64_f32(vget_low_f32(mVal));
        vec.mData[1] = vcvt_f64_f32(vget_high_f32(mVal));
        
        return vec;
    }
};

template<>
struct SIMDType<int32_t, 4> : public SIMDVector<int32_t, int32x4_t, 4>
{
    SIMDType() {}
    SIMDType(const int32_t& a) { mVal = vdupq_n_s32(a); }
    SIMDType(const int32_t* a) { mVal = vld1q_s32(a); }
    SIMDType(int32x4_t a) : SIMDVector(a) {}
    
    void store(int32_t *a) const { vst1q_s32(a, mVal); }
    
    friend SIMDType operator + (const SIMDType& a, const SIMDType& b) { return vaddq_s32(a.mVal, b.mVal); }
    friend SIMDType operator - (const SIMDType& a, const SIMDType& b) { return vsubq_s32(a.mVal, b.mVal); }
    friend SIMDType operator * (const SIMDType& a, const SIMDType& b) { return vmulq_s32(a.mVal, b.mVal); }
    
    SIMDType& operator += (const SIMDType& b) { return (*this = *this + b); }
    SIMDType& operator -= (const SIMDType& b) { return (*this = *this - b); }
    SIMDType& operator *= (const SIMDType& b) { return (*this = *this * b); }
    
    friend SIMDType min(const SIMDType& a, const SIMDType& b) { return vminq_s32(a.mVal, b.mVal); }
    friend SIMDType max(const SIMDType& a, const SIMDType& b) { return vmaxq_s32(a.mVal, b.mVal); }
    
    operator SIMDType<float, 4>() { return SIMDType<float, 4>( vcvtq_f32_s32(mVal)); }
    /*
    operator SizedVector<double, 2, 4>() const
    {
        SizedVector<double, 2, 4> vec;
        
        vec.mData[0] = _mm_cvtepi32_pd(mVal);
        vec.mData[1] = _mm_cvtepi32_pd(_mm_shuffle_epi32(mVal, 0xE));
        
        return vec;
    }*/
};

#else /* Intel Instrinsics */

template<>
struct SIMDType<double, 2> : public SIMDVector<double, __m128d, 2>
{
    SIMDType() {}
    SIMDType(const double& a) { mVal = _mm_set1_pd(a); }
    SIMDType(const double* a) { mVal = _mm_loadu_pd(a); }
    SIMDType(__m128d a) : SIMDVector(a) {}
    
    SIMDType(const SIMDType<float, 2> &a)
    {
        double vals[2];
        
        vals[0] = a.mVals[0];
        vals[1] = a.mVals[1];
        
        mVal = _mm_loadu_pd(vals);
    }
    
    void store(double *a) const { _mm_storeu_pd(a, mVal); }
    
    friend SIMDType operator + (const SIMDType& a, const SIMDType& b) { return _mm_add_pd(a.mVal, b.mVal); }
    friend SIMDType operator - (const SIMDType& a, const SIMDType& b) { return _mm_sub_pd(a.mVal, b.mVal); }
    friend SIMDType operator * (const SIMDType& a, const SIMDType& b) { return _mm_mul_pd(a.mVal, b.mVal); }
    friend SIMDType operator / (const SIMDType& a, const SIMDType& b) { return _mm_div_pd(a.mVal, b.mVal); }
    
    SIMDType& operator += (const SIMDType& b) { return (*this = *this + b); }
    SIMDType& operator -= (const SIMDType& b) { return (*this = *this - b); }
    SIMDType& operator *= (const SIMDType& b) { return (*this = *this * b); }
    SIMDType& operator /= (const SIMDType& b) { return (*this = *this / b); }
    
    friend SIMDType sqrt(const SIMDType& a) { return _mm_sqrt_pd(a.mVal); }
    
    // N.B. - ties issue
    friend SIMDType round(const SIMDType& a) { return _mm_round_pd(a.mVal, _MM_FROUND_TO_NEAREST_INT |_MM_FROUND_NO_EXC); }
    friend SIMDType trunc(const SIMDType& a) { return _mm_round_pd(a.mVal, _MM_FROUND_TO_ZERO |_MM_FROUND_NO_EXC); }
    
    friend SIMDType min(const SIMDType& a, const SIMDType& b) { return _mm_min_pd(a.mVal, b.mVal); }
    friend SIMDType max(const SIMDType& a, const SIMDType& b) { return _mm_max_pd(a.mVal, b.mVal); }
    friend SIMDType sel(const SIMDType& a, const SIMDType& b, const SIMDType& c) { return and_not(c, a) | (b & c); }
    
    friend SIMDType and_not(const SIMDType& a, const SIMDType& b) { return _mm_andnot_pd(a.mVal, b.mVal); }
    friend SIMDType operator & (const SIMDType& a, const SIMDType& b) { return _mm_and_pd(a.mVal, b.mVal); }
    friend SIMDType operator | (const SIMDType& a, const SIMDType& b) { return _mm_or_pd(a.mVal, b.mVal); }
    friend SIMDType operator ^ (const SIMDType& a, const SIMDType& b) { return _mm_xor_pd(a.mVal, b.mVal); }
    
    friend SIMDType operator == (const SIMDType& a, const SIMDType& b) { return _mm_cmpeq_pd(a.mVal, b.mVal); }
    friend SIMDType operator != (const SIMDType& a, const SIMDType& b) { return _mm_cmpneq_pd(a.mVal, b.mVal); }
    friend SIMDType operator > (const SIMDType& a, const SIMDType& b) { return _mm_cmplt_pd(a.mVal, b.mVal); }
    friend SIMDType operator < (const SIMDType& a, const SIMDType& b) { return _mm_cmpgt_pd(a.mVal, b.mVal); }
    friend SIMDType operator >= (const SIMDType& a, const SIMDType& b) { return _mm_cmple_pd(a.mVal, b.mVal); }
    friend SIMDType operator <= (const SIMDType& a, const SIMDType& b) { return _mm_cmpge_pd(a.mVal, b.mVal); }
    
    template <int y, int x>
    static SIMDType shuffle(const SIMDType& a, const SIMDType& b)
    {
        return _mm_shuffle_pd(a.mVal, b.mVal, (y<<1)|x);
    }
    
    operator SIMDType<float, 2>()
    {
        double vals[2];
        
        store(vals);
        
        return SIMDType<float, 2>(static_cast<float>(vals[0]), static_cast<float>(vals[1]));
    }
};

template<>
struct SIMDType<float, 4> : public SIMDVector<float, __m128, 4>
{
    SIMDType() {}
    SIMDType(const float& a) { mVal = _mm_set1_ps(a); }
    SIMDType(const float* a) { mVal = _mm_loadu_ps(a); }
    SIMDType(__m128 a) : SIMDVector(a) {}
    
    void store(float *a) const { _mm_storeu_ps(a, mVal); }
    
    friend SIMDType operator + (const SIMDType& a, const SIMDType& b) { return _mm_add_ps(a.mVal, b.mVal); }
    friend SIMDType operator - (const SIMDType& a, const SIMDType& b) { return _mm_sub_ps(a.mVal, b.mVal); }
    friend SIMDType operator * (const SIMDType& a, const SIMDType& b) { return _mm_mul_ps(a.mVal, b.mVal); }
    friend SIMDType operator / (const SIMDType& a, const SIMDType& b) { return _mm_div_ps(a.mVal, b.mVal); }
    
    SIMDType& operator += (const SIMDType& b) { return (*this = *this + b); }
    SIMDType& operator -= (const SIMDType& b) { return (*this = *this - b); }
    SIMDType& operator *= (const SIMDType& b) { return (*this = *this * b); }
    SIMDType& operator /= (const SIMDType& b) { return (*this = *this / b); }
    
    friend SIMDType sqrt(const SIMDType& a) { return _mm_sqrt_ps(a.mVal); }
    
    // N.B. - ties issue
    friend SIMDType round(const SIMDType& a) { return _mm_round_ps(a.mVal, _MM_FROUND_TO_NEAREST_INT |_MM_FROUND_NO_EXC); }
    friend SIMDType trunc(const SIMDType& a) { return _mm_round_ps(a.mVal, _MM_FROUND_TO_ZERO |_MM_FROUND_NO_EXC); }
    
    friend SIMDType min(const SIMDType& a, const SIMDType& b) { return _mm_min_ps(a.mVal, b.mVal); }
    friend SIMDType max(const SIMDType& a, const SIMDType& b) { return _mm_max_ps(a.mVal, b.mVal); }
    friend SIMDType sel(const SIMDType& a, const SIMDType& b, const SIMDType& c) { return and_not(c, a) | (b & c); }
    
    friend SIMDType and_not(const SIMDType& a, const SIMDType& b) { return _mm_andnot_ps(a.mVal, b.mVal); }
    friend SIMDType operator & (const SIMDType& a, const SIMDType& b) { return _mm_and_ps(a.mVal, b.mVal); }
    friend SIMDType operator | (const SIMDType& a, const SIMDType& b) { return _mm_or_ps(a.mVal, b.mVal); }
    friend SIMDType operator ^ (const SIMDType& a, const SIMDType& b) { return _mm_xor_ps(a.mVal, b.mVal); }
    
    friend SIMDType operator == (const SIMDType& a, const SIMDType& b) { return _mm_cmpeq_ps(a.mVal, b.mVal); }
    friend SIMDType operator != (const SIMDType& a, const SIMDType& b) { return _mm_cmpneq_ps(a.mVal, b.mVal); }
    friend SIMDType operator > (const SIMDType& a, const SIMDType& b) { return _mm_cmplt_ps(a.mVal, b.mVal); }
    friend SIMDType operator < (const SIMDType& a, const SIMDType& b) { return _mm_cmpgt_ps(a.mVal, b.mVal); }
    friend SIMDType operator >= (const SIMDType& a, const SIMDType& b) { return _mm_cmple_ps(a.mVal, b.mVal); }
    friend SIMDType operator <= (const SIMDType& a, const SIMDType& b) { return _mm_cmpge_ps(a.mVal, b.mVal); }
    
    template <int z, int y, int x, int w>
    static SIMDType shuffle(const SIMDType& a, const SIMDType& b)
    {
        return _mm_shuffle_ps(a.mVal, b.mVal, ((z<<6)|(y<<4)|(x<<2)|w));
    }
    
    operator SizedVector<double, 2, 4>() const
    {
        SizedVector<double, 2, 4> vec;
        
        vec.mData[0] = _mm_cvtps_pd(mVal);
        vec.mData[1] = _mm_cvtps_pd(_mm_movehl_ps(mVal, mVal));
        
        return vec;
    }
};

template<>
struct SIMDType<int32_t, 4> : public SIMDVector<int32_t, __m128i, 4>
{
    SIMDType() {}
    SIMDType(const int32_t& a) { mVal = _mm_set1_epi32(a); }
    SIMDType(const int32_t* a) { mVal = _mm_loadu_si128(reinterpret_cast<const __m128i *>(a)); }
    SIMDType(__m128i a) : SIMDVector(a) {}
    
    void store(int32_t *a) const { _mm_storeu_si128(reinterpret_cast<__m128i *>(a), mVal); }
    
    friend SIMDType operator + (const SIMDType& a, const SIMDType& b) { return _mm_add_epi32(a.mVal, b.mVal); }
    friend SIMDType operator - (const SIMDType& a, const SIMDType& b) { return _mm_sub_epi32(a.mVal, b.mVal); }
    friend SIMDType operator * (const SIMDType& a, const SIMDType& b) { return _mm_mul_epi32(a.mVal, b.mVal); }
    
    SIMDType& operator += (const SIMDType& b) { return (*this = *this + b); }
    SIMDType& operator -= (const SIMDType& b) { return (*this = *this - b); }
    SIMDType& operator *= (const SIMDType& b) { return (*this = *this * b); }
    
    friend SIMDType min(const SIMDType& a, const SIMDType& b) { return _mm_min_epi32(a.mVal, b.mVal); }
    friend SIMDType max(const SIMDType& a, const SIMDType& b) { return _mm_max_epi32(a.mVal, b.mVal); }
    
    operator SIMDType<float, 4>() { return SIMDType<float, 4>( _mm_cvtepi32_ps(mVal)); }
    
    operator SizedVector<double, 2, 4>() const
    {
        SizedVector<double, 2, 4> vec;
        
        vec.mData[0] = _mm_cvtepi32_pd(mVal);
        vec.mData[1] = _mm_cvtepi32_pd(_mm_shuffle_epi32(mVal, 0xE));
        
        return vec;
    }
};

#endif /* SIMD_COMPILER_SUPPORT_NEON - End Intel Intrinsics */

#endif /* SIMD_COMPILER_SUPPORT_LEVEL >= SIMD_COMPILER_SUPPORT_VEC128 */

// ************************ 256-bit SIMD Types ************************* //

#if (SIMD_COMPILER_SUPPORT_LEVEL >= SIMD_COMPILER_SUPPORT_VEC256)

template<>
struct SIMDType<double, 4> : public SIMDVector<double, __m256d, 4>
{
    SIMDType() {}
    SIMDType(const double& a) { mVal = _mm256_set1_pd(a); }
    SIMDType(const double* a) { mVal = _mm256_loadu_pd(a); }
    SIMDType(__m256d a) : SIMDVector(a) {}
    
    SIMDType(const SIMDType<float, 4> &a) { mVal = _mm256_cvtps_pd(a.mVal); }
    SIMDType(const SIMDType<int32_t, 4> &a) { mVal = _mm256_cvtepi32_pd(a.mVal); }
    
    void store(double *a) const { _mm256_storeu_pd(a, mVal); }
    
    friend SIMDType operator + (const SIMDType& a, const SIMDType& b) { return _mm256_add_pd(a.mVal, b.mVal); }
    friend SIMDType operator - (const SIMDType& a, const SIMDType& b) { return _mm256_sub_pd(a.mVal, b.mVal); }
    friend SIMDType operator * (const SIMDType& a, const SIMDType& b) { return _mm256_mul_pd(a.mVal, b.mVal); }
    friend SIMDType operator / (const SIMDType& a, const SIMDType& b) { return _mm256_div_pd(a.mVal, b.mVal); }
    
    SIMDType& operator += (const SIMDType& b) { return (*this = *this + b); }
    SIMDType& operator -= (const SIMDType& b) { return (*this = *this - b); }
    SIMDType& operator *= (const SIMDType& b) { return (*this = *this * b); }
    SIMDType& operator /= (const SIMDType& b) { return (*this = *this / b); }
    
    friend SIMDType sqrt(const SIMDType& a) { return _mm256_sqrt_pd(a.mVal); }
    
    // N.B. - ties issue
    friend SIMDType round(const SIMDType& a) { return _mm256_round_pd(a.mVal, _MM_FROUND_TO_NEAREST_INT |_MM_FROUND_NO_EXC); }
    friend SIMDType trunc(const SIMDType& a) { return _mm256_round_pd(a.mVal, _MM_FROUND_TO_ZERO |_MM_FROUND_NO_EXC); }
    
    friend SIMDType min(const SIMDType& a, const SIMDType& b) { return _mm256_min_pd(a.mVal, b.mVal); }
    friend SIMDType max(const SIMDType& a, const SIMDType& b) { return _mm256_max_pd(a.mVal, b.mVal); }
    friend SIMDType sel(const SIMDType& a, const SIMDType& b, const SIMDType& c) { return and_not(c, a) | (b & c); }
    
    friend SIMDType and_not(const SIMDType& a, const SIMDType& b) { return _mm256_andnot_pd(a.mVal, b.mVal); }
    friend SIMDType operator & (const SIMDType& a, const SIMDType& b) { return _mm256_and_pd(a.mVal, b.mVal); }
    friend SIMDType operator | (const SIMDType& a, const SIMDType& b) { return _mm256_or_pd(a.mVal, b.mVal); }
    friend SIMDType operator ^ (const SIMDType& a, const SIMDType& b) { return _mm256_xor_pd(a.mVal, b.mVal); }
    
    friend SIMDType operator == (const SIMDType& a, const SIMDType& b) { return _mm256_cmp_pd(a.mVal, b.mVal, _CMP_EQ_OQ); }
    friend SIMDType operator != (const SIMDType& a, const SIMDType& b) { return _mm256_cmp_pd(a.mVal, b.mVal, _CMP_NEQ_UQ); }
    friend SIMDType operator > (const SIMDType& a, const SIMDType& b) { return _mm256_cmp_pd(a.mVal, b.mVal, _CMP_GT_OQ); }
    friend SIMDType operator < (const SIMDType& a, const SIMDType& b) { return _mm256_cmp_pd(a.mVal, b.mVal, _CMP_LT_OQ); }
    friend SIMDType operator >= (const SIMDType& a, const SIMDType& b) { return _mm256_cmp_pd(a.mVal, b.mVal, _CMP_GE_OQ); }
    friend SIMDType operator <= (const SIMDType& a, const SIMDType& b) { return _mm256_cmp_pd(a.mVal, b.mVal, _CMP_LE_OQ); }
    
    operator SIMDType<float, 4>() { return _mm256_cvtpd_ps(mVal); }
    operator SIMDType<int32_t, 4>() { return _mm256_cvtpd_epi32(mVal); }
};

template<>
struct SIMDType<float, 8> : public SIMDVector<float, __m256, 8>
{
    SIMDType() {}
    SIMDType(const float& a) { mVal = _mm256_set1_ps(a); }
    SIMDType(const float* a) { mVal = _mm256_loadu_ps(a); }
    SIMDType(__m256 a) : SIMDVector(a) {}
    
    void store(float *a) const { _mm256_storeu_ps(a, mVal); }
    
    friend SIMDType operator + (const SIMDType& a, const SIMDType& b) { return _mm256_add_ps(a.mVal, b.mVal); }
    friend SIMDType operator - (const SIMDType& a, const SIMDType& b) { return _mm256_sub_ps(a.mVal, b.mVal); }
    friend SIMDType operator * (const SIMDType& a, const SIMDType& b) { return _mm256_mul_ps(a.mVal, b.mVal); }
    friend SIMDType operator / (const SIMDType& a, const SIMDType& b) { return _mm256_div_ps(a.mVal, b.mVal); }
    
    SIMDType& operator += (const SIMDType& b) { return (*this = *this + b); }
    SIMDType& operator -= (const SIMDType& b) { return (*this = *this - b); }
    SIMDType& operator *= (const SIMDType& b) { return (*this = *this * b); }
    SIMDType& operator /= (const SIMDType& b) { return (*this = *this / b); }
    
    friend SIMDType sqrt(const SIMDType& a) { return _mm256_sqrt_ps(a.mVal); }
    
    // N.B. - ties issue
    friend SIMDType round(const SIMDType& a) { return _mm256_round_ps(a.mVal, _MM_FROUND_TO_NEAREST_INT |_MM_FROUND_NO_EXC); }
    friend SIMDType trunc(const SIMDType& a) { return _mm256_round_ps(a.mVal, _MM_FROUND_TO_ZERO |_MM_FROUND_NO_EXC); }
    
    friend SIMDType min(const SIMDType& a, const SIMDType& b) { return _mm256_min_ps(a.mVal, b.mVal); }
    friend SIMDType max(const SIMDType& a, const SIMDType& b) { return _mm256_max_ps(a.mVal, b.mVal); }
    friend SIMDType sel(const SIMDType& a, const SIMDType& b, const SIMDType& c) { return and_not(c, a) | (b & c); }
    
    friend SIMDType and_not(const SIMDType& a, const SIMDType& b) { return _mm256_andnot_ps(a.mVal, b.mVal); }
    friend SIMDType operator & (const SIMDType& a, const SIMDType& b) { return _mm256_and_ps(a.mVal, b.mVal); }
    friend SIMDType operator | (const SIMDType& a, const SIMDType& b) { return _mm256_or_ps(a.mVal, b.mVal); }
    friend SIMDType operator ^ (const SIMDType& a, const SIMDType& b) { return _mm256_xor_ps(a.mVal, b.mVal); }
    
    friend SIMDType operator == (const SIMDType& a, const SIMDType& b) { return _mm256_cmp_ps(a.mVal, b.mVal, _CMP_EQ_OQ); }
    friend SIMDType operator != (const SIMDType& a, const SIMDType& b) { return _mm256_cmp_ps(a.mVal, b.mVal, _CMP_NEQ_UQ); }
    friend SIMDType operator > (const SIMDType& a, const SIMDType& b) { return _mm256_cmp_ps(a.mVal, b.mVal, _CMP_GT_OQ); }
    friend SIMDType operator < (const SIMDType& a, const SIMDType& b) { return _mm256_cmp_ps(a.mVal, b.mVal, _CMP_LT_OQ); }
    friend SIMDType operator >= (const SIMDType& a, const SIMDType& b) { return _mm256_cmp_ps(a.mVal, b.mVal, _CMP_GE_OQ); }
    friend SIMDType operator <= (const SIMDType& a, const SIMDType& b) { return _mm256_cmp_ps(a.mVal, b.mVal, _CMP_LE_OQ); }
    
    operator SizedVector<double, 4, 8>() const
    {
        SizedVector<double, 4, 8> vec;
        
        vec.mData[0] = _mm256_cvtps_pd(_mm256_extractf128_ps(mVal, 0));
        vec.mData[1] = _mm256_cvtps_pd(_mm256_extractf128_ps(mVal, 1));
        
        return vec;
    }
};

#endif

// ************************ 512-bit SIMD Types ************************* //

#if (SIMD_COMPILER_SUPPORT_LEVEL >= SIMD_COMPILER_SUPPORT_VEC512)

template<>
struct SIMDType<double, 8> : public SIMDVector<double, __m512d, 8>
{
    SIMDType() {}
    SIMDType(const double& a) { mVal = _mm512_set1_pd(a); }
    SIMDType(const double* a) { mVal = _mm512_loadu_pd(a); }
    SIMDType(__m512d a) : SIMDVector(a) {}
    
    SIMDType(const SIMDType<float, 8> &a) { mVal = _mm512_cvtps_pd(a.mVal); }
    
    void store(double *a) const { _mm512_storeu_pd(a, mVal); }
    
    friend SIMDType operator + (const SIMDType& a, const SIMDType& b) { return _mm512_add_pd(a.mVal, b.mVal); }
    friend SIMDType operator - (const SIMDType& a, const SIMDType& b) { return _mm512_sub_pd(a.mVal, b.mVal); }
    friend SIMDType operator * (const SIMDType& a, const SIMDType& b) { return _mm512_mul_pd(a.mVal, b.mVal); }
    friend SIMDType operator / (const SIMDType& a, const SIMDType& b) { return _mm512_div_pd(a.mVal, b.mVal); }
    
    SIMDType& operator += (const SIMDType& b) { return (*this = *this + b); }
    SIMDType& operator -= (const SIMDType& b) { return (*this = *this - b); }
    SIMDType& operator *= (const SIMDType& b) { return (*this = *this * b); }
    SIMDType& operator /= (const SIMDType& b) { return (*this = *this / b); }
    
    friend SIMDType sqrt(const SIMDType& a) { return _mm512_sqrt_pd(a.mVal); }
    
    friend SIMDType min(const SIMDType& a, const SIMDType& b) { return _mm512_min_pd(a.mVal, b.mVal); }
    friend SIMDType max(const SIMDType& a, const SIMDType& b) { return _mm512_max_pd(a.mVal, b.mVal); }
    friend SIMDType sel(const SIMDType& a, const SIMDType& b, const SIMDType& c) { return and_not(c, a) | (b & c); }
    
    friend SIMDType and_not(const SIMDType& a, const SIMDType& b) { return _mm512_andnot_pd(a.mVal, b.mVal); }
    friend SIMDType operator & (const SIMDType& a, const SIMDType& b) { return _mm512_and_pd(a.mVal, b.mVal); }
    friend SIMDType operator | (const SIMDType& a, const SIMDType& b) { return _mm512_or_pd(a.mVal, b.mVal); }
    friend SIMDType operator ^ (const SIMDType& a, const SIMDType& b) { return _mm512_xor_pd(a.mVal, b.mVal); }
    
    friend SIMDType operator == (const SIMDType& a, const SIMDType& b) { return _mm512_cmp_pd_mask(a.mVal, b.mVal, _CMP_EQ_OQ); }
    friend SIMDType operator != (const SIMDType& a, const SIMDType& b) { return _mm512_cmp_pd_mask(a.mVal, b.mVal, _CMP_NEQ_UQ); }
    friend SIMDType operator > (const SIMDType& a, const SIMDType& b) { return _mm512_cmp_pd_mask(a.mVal, b.mVal, _CMP_GT_OQ); }
    friend SIMDType operator < (const SIMDType& a, const SIMDType& b) { return _mm512_cmp_pd_mask(a.mVal, b.mVal, _CMP_LT_OQ); }
    friend SIMDType operator >= (const SIMDType& a, const SIMDType& b) { return _mm512_cmp_pd_mask(a.mVal, b.mVal, _CMP_GE_OQ); }
    friend SIMDType operator <= (const SIMDType& a, const SIMDType& b) { return _mm512_cmp_pd_mask(a.mVal, b.mVal, _CMP_LE_OQ); }
    
    operator SIMDType<float, 8>() { return _mm512_cvtpd_ps(mVal); }
};

template<>
struct SIMDType<float, 16> : public SIMDVector<float, __m512, 16>
{
    SIMDType() {}
    SIMDType(const float& a) { mVal = _mm512_set1_ps(a); }
    SIMDType(const float* a) { mVal = _mm512_loadu_ps(a); }
    SIMDType(__m512 a) : SIMDVector(a) {}
    
    void store(float *a) const { _mm512_storeu_ps(a, mVal); }
    
    friend SIMDType operator + (const SIMDType& a, const SIMDType& b) { return _mm512_add_ps(a.mVal, b.mVal); }
    friend SIMDType operator - (const SIMDType& a, const SIMDType& b) { return _mm512_sub_ps(a.mVal, b.mVal); }
    friend SIMDType operator * (const SIMDType& a, const SIMDType& b) { return _mm512_mul_ps(a.mVal, b.mVal); }
    friend SIMDType operator / (const SIMDType& a, const SIMDType& b) { return _mm512_div_ps(a.mVal, b.mVal); }
    
    SIMDType& operator += (const SIMDType& b) { return (*this = *this + b); }
    SIMDType& operator -= (const SIMDType& b) { return (*this = *this - b); }
    SIMDType& operator *= (const SIMDType& b) { return (*this = *this * b); }
    SIMDType& operator /= (const SIMDType& b) { return (*this = *this / b); }
    
    friend SIMDType sqrt(const SIMDType& a) { return _mm512_sqrt_ps(a.mVal); }
    
    friend SIMDType min(const SIMDType& a, const SIMDType& b) { return _mm512_min_ps(a.mVal, b.mVal); }
    friend SIMDType max(const SIMDType& a, const SIMDType& b) { return _mm512_max_ps(a.mVal, b.mVal); }
    friend SIMDType sel(const SIMDType& a, const SIMDType& b, const SIMDType& c) { return and_not(c, a) | (b & c); }
    
    friend SIMDType and_not(const SIMDType& a, const SIMDType& b) { return _mm512_andnot_ps(a.mVal, b.mVal); }
    friend SIMDType operator & (const SIMDType& a, const SIMDType& b) { return _mm512_and_ps(a.mVal, b.mVal); }
    friend SIMDType operator | (const SIMDType& a, const SIMDType& b) { return _mm512_or_ps(a.mVal, b.mVal); }
    friend SIMDType operator ^ (const SIMDType& a, const SIMDType& b) { return _mm512_xor_ps(a.mVal, b.mVal); }
    
    friend SIMDType operator == (const SIMDType& a, const SIMDType& b) { return _mm512_cmp_ps_mask(a.mVal, b.mVal, _CMP_EQ_OQ); }
    friend SIMDType operator != (const SIMDType& a, const SIMDType& b) { return _mm512_cmp_ps_mask(a.mVal, b.mVal, _CMP_NEQ_UQ); }
    friend SIMDType operator > (const SIMDType& a, const SIMDType& b) { return _mm512_cmp_ps_mask(a.mVal, b.mVal, _CMP_GT_OQ); }
    friend SIMDType operator < (const SIMDType& a, const SIMDType& b) { return _mm512_cmp_ps_mask(a.mVal, b.mVal, _CMP_LT_OQ); }
    friend SIMDType operator >= (const SIMDType& a, const SIMDType& b) { return _mm512_cmp_ps_mask(a.mVal, b.mVal, _CMP_GE_OQ); }
    friend SIMDType operator <= (const SIMDType& a, const SIMDType& b) { return _mm512_cmp_ps_mask(a.mVal, b.mVal, _CMP_LE_OQ); }
};

#endif

// ********************** Common Functionality ********************** //

// Select Functionality for all types

template <class T, int N>
T select(const SIMDType<T, N>& a, const SIMDType<T, N>& b, const SIMDType<T, N>& mask)
{
    return (b & mask) | and_not(mask, a);
}

// Abs functionality

static inline SIMDType<double, 1> abs(const SIMDType<double, 1> a)
{
    const static uint64_t bit_mask_64 = 0x7FFFFFFFFFFFFFFFU;
    
    uint64_t temp = *(reinterpret_cast<const uint64_t *>(&a)) & bit_mask_64;
    return *(reinterpret_cast<double *>(&temp));
}

static inline SIMDType<float, 1> abs(const SIMDType<float, 1> a)
{
    const static uint32_t bit_mask_32 = 0x7FFFFFFFU;
    
    uint32_t temp = *(reinterpret_cast<const uint32_t *>(&a)) & bit_mask_32;
    return *(reinterpret_cast<float *>(&temp));
}

template <int N>
SIMDType<double, N> abs(const SIMDType<double, N> a)
{
    const static uint64_t bit_mask_64 = 0x7FFFFFFFFFFFFFFFU;
    const double bit_mask_64d = *(reinterpret_cast<const double *>(&bit_mask_64));
    
    return a & SIMDType<double, N>(bit_mask_64d);
}

template <int N>
SIMDType<float, N> abs(const SIMDType<float, N> a)
{
    const static uint32_t bit_mask_32 = 0x7FFFFFFFU;
    const float bit_mask_32f = *(reinterpret_cast<const double *>(&bit_mask_32));
    
    return a & SIMDType<float, N>(bit_mask_32f);
}

#endif
