
/*
 *  TimeDomainConvolve
 *
 *    TimeDomainConvolve performs real-time zero latency time-based convolution.
 *
 *    Typically TimeDomainConvolve is suitable for use in conjunction with PartitionedConvolve for zero-latency convolution with longer impulses (time_domain_convolve use apple's vDSP and the IR length is limited to 2044 samples).
 *    Note that in fact the algorithms process correlation with reversed impulse response coeffients - which is equivalent to convolution.
 *
 *  Copyright 2012 Alex Harker. All rights reserved.
 *
 */

#include "TimeDomainConvolve.h"
#include "ConvolveSIMD.h"

#include <algorithm>

#ifdef __APPLE__
#include <Accelerate/Accelerate.h>

uintptr_t padded_length(uintptr_t length)
{
    return length;
}
#else
uintptr_t padded_length(uintptr_t length)
{
    return ((length + 15) >> 4) << 4;
}
#endif

HISSTools::TimeDomainConvolve::TimeDomainConvolve(uintptr_t offset, uintptr_t length) : mInputPosition(0), mImpulseLength(0)
{
    // Set default initial variables
    
    setOffset(offset);
    setLength(length);
    
    // Allocate impulse buffer and input bufferr
    
    mImpulseBuffer = (float *) ALIGNED_MALLOC(sizeof(float) * 2048);
    mInputBuffer = (float *) ALIGNED_MALLOC(sizeof(float) * 8192);
    
    // Zero buffers
    
    std::fill_n(mImpulseBuffer, 2048, 0.f);
    std::fill_n(mInputBuffer, 8192, 0.f);
}

HISSTools::TimeDomainConvolve::~TimeDomainConvolve()
{
    ALIGNED_FREE(mImpulseBuffer);
    ALIGNED_FREE(mInputBuffer);
}

void HISSTools::TimeDomainConvolve::setOffset(uintptr_t offset)
{
    mOffset = offset;
}

ConvolveError HISSTools::TimeDomainConvolve::setLength(uintptr_t length)
{
    mLength = std::min(length, uintptr_t(2044));
    
    return length > 2044 ? CONVOLVE_ERR_TIME_LENGTH_OUT_OF_RANGE : CONVOLVE_ERR_NONE;
}

ConvolveError HISSTools::TimeDomainConvolve::set(const float *input, uintptr_t length)
{
    mImpulseLength = 0;
    
    if (input && length > mOffset)
    {
        // Calculate impulse length
        
        mImpulseLength = std::min(length - mOffset, (mLength ? mLength : 2044));
    
        uintptr_t pad = padded_length(mImpulseLength) - mImpulseLength;
        std::fill_n(mImpulseBuffer, pad, 0.f);
        std::reverse_copy(input + mOffset, input + mOffset + mImpulseLength, mImpulseBuffer + pad);
    }
    
    reset();
    
    return (!mLength && (length - mOffset) > 2044) ? CONVOLVE_ERR_TIME_IMPULSE_TOO_LONG : CONVOLVE_ERR_NONE;
}

void HISSTools::TimeDomainConvolve::reset()
{
    mReset = true;
}

#ifdef __APPLE__
void convolve(const float *in, const float *impulse, float *output, uintptr_t N, uintptr_t L)
{
    vDSP_conv(in + 1 - L,  1, impulse, 1, output, 1, N, L);
}
#else
void convolve(const float *in, const float *impulse, float *output, uintptr_t N, uintptr_t L)
{
    constexpr int size = FloatVector::size;
    
    L = padded_length(L);
    
    const FloatVector *impulse_vector = reinterpret_cast<const FloatVector *>(impulse);
    
    for (uintptr_t i = 0; i < N; i++)
    {
        FloatVector outputAccum(0.f);
        const float *input = in - L + 1 + i - size;
        
        for (uintptr_t j = 0; j < L >> 2; j += 4)
        {
            // Load vals
            
            outputAccum += (impulse_vector[j + 0] * FloatVector::unaligned_load(input += size));
            outputAccum += (impulse_vector[j + 1] * FloatVector::unaligned_load(input += size));
            outputAccum += (impulse_vector[j + 2] * FloatVector::unaligned_load(input += size));
            outputAccum += (impulse_vector[j + 3] * FloatVector::unaligned_load(input += size));
        }
        
        *output++ = outputAccum.sum();
    }
}
#endif

bool HISSTools::TimeDomainConvolve::process(const float *in, float *out, uintptr_t numSamples)
{
    if (mReset)
    {
        std::fill_n(mInputBuffer, 8192, 0.f);
        mReset = false;
    }
    
    uintptr_t currentLoop;
    
    while ((currentLoop = (mInputPosition + numSamples) > 4096 ? (4096 - mInputPosition) : ((numSamples > 2048) ? 2048 : numSamples)))
    {
        // Copy input twice (allows us to read input out in one go)
        
        std::copy_n(in, currentLoop, mInputBuffer + mInputPosition);
        std::copy_n(in, currentLoop, mInputBuffer + mInputPosition + 4096);

        // Advance pointer
        
        mInputPosition += currentLoop;
        if (mInputPosition >= 4096)
            mInputPosition -= 4096;
        
        // Do convolution
        
        convolve(mInputBuffer + 4096 + (mInputPosition - currentLoop), mImpulseBuffer, out, currentLoop, mImpulseLength);
        
        // Updates
        
        in += currentLoop;
        out += currentLoop;
        numSamples -= currentLoop;
    }
    
    return mImpulseLength;
}
