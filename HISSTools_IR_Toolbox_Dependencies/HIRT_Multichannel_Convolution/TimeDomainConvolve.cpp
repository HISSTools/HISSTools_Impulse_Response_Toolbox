
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
#include <functional>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef __APPLE__
uintptr_t padded_length(uintptr_t length)
{
    return ((length + 15) >> 4) << 4;
    // return length;
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
    
    memset(mImpulseBuffer, 0, 2048 * sizeof(float));
    memset(mInputBuffer, 0, 8192 * sizeof(float));
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
    
    return (!mLength && length > 2044) ? CONVOLVE_ERR_TIME_IMPULSE_TOO_LONG : CONVOLVE_ERR_NONE;
}

void HISSTools::TimeDomainConvolve::reset()
{
    mReset = true;
}

template <class T>
void convolve(const float *in, T *impulse, float *output, uintptr_t N, uintptr_t L)
{
    L = padded_length(L);
    
    for (uintptr_t i = 0; i < N; i++)
    {
        T outputAccum = T(0.f);
        const float *input = in - L + 1 + i - T::size;
        
        for (uintptr_t j = 0; j < L >> 2; j += 4)
        {
            // Load vals
            
            outputAccum += (impulse[j + 0] * T::unaligned_load(input += T::size));
            outputAccum += (impulse[j + 1] * T::unaligned_load(input += T::size));
            outputAccum += (impulse[j + 2] * T::unaligned_load(input += T::size));
            outputAccum += (impulse[j + 3] * T::unaligned_load(input += T::size));
        }
        
        *output++ = outputAccum.sum();
    }
}

void convolve(const float *in, float *impulse, float *output, uintptr_t N, uintptr_t L)
{
    L = padded_length(L);
    
    for (uintptr_t i = 0; i < N; i++)
    {
        float outputAccum = 0.f;
        const float *input = in - L + 1 + i;
        
        for (uintptr_t j = 0; j < L; j += 8)
        {
            // Load vals
            
            outputAccum += impulse[j+0] * *input++;
            outputAccum += impulse[j+1] * *input++;
            outputAccum += impulse[j+2] * *input++;
            outputAccum += impulse[j+3] * *input++;
            outputAccum += impulse[j+4] * *input++;
            outputAccum += impulse[j+5] * *input++;
            outputAccum += impulse[j+6] * *input++;
            outputAccum += impulse[j+7] * *input++;
        }
        
        *output++ = outputAccum;
    }
}

/*
#ifdef __APPLE__
template<>
void convolve(const float *in, float *impulse, float *output, uintptr_t N, uintptr_t L)
{
    vDSP_conv(in + 1 - L, (vDSP_Stride) 1, impulse, (vDSP_Stride) 1, output, (vDSP_Stride) 1, (vDSP_Length) N, L);
}
#endif
*/

bool HISSTools::TimeDomainConvolve::process(const float *in, float *out, uintptr_t numSamples)
{
    if (mReset)
    {
        memset(mInputBuffer, 0, 8192 * sizeof(float));
        mReset = false;
    }
    
    uintptr_t currentLoop;
    
    while ((currentLoop = (mInputPosition + numSamples) > 4096 ? (4096 - mInputPosition) : ((numSamples > 2048) ? 2048 : numSamples)))
    {
        // Copy input twice (allows us to read input out in one go)
        
        memcpy(mInputBuffer + mInputPosition, in, sizeof(float) * currentLoop);
        memcpy(mInputBuffer + 4096 + mInputPosition, in, sizeof(float) * currentLoop);
        
        // Advance pointer
        
        mInputPosition += currentLoop;
        if (mInputPosition >= 4096)
            mInputPosition -= 4096;
        
        // Do convolution
        
        if (numSamples % 4)
            convolve(mInputBuffer + 4096 + (mInputPosition - currentLoop), mImpulseBuffer, out, currentLoop, mImpulseLength);
        else
        {
#ifdef __APPLE__
            convolve(mInputBuffer + 4096 + (mInputPosition - currentLoop), (FloatVector *) mImpulseBuffer, out, currentLoop, mImpulseLength);
#else
            convolve(mInputBuffer + 4096 + (mInputPosition - currentLoop), (FloatVector *) mImpulseBuffer, out, currentLoop, mImpulseLength);
#endif
        }
        
        // Updates
        
        in += currentLoop;
        out += currentLoop;
        numSamples -= currentLoop;
    }
    
    return mImpulseLength;
}
