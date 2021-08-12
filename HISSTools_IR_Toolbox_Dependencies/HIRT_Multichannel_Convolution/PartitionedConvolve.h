
#pragma once

#include "../HISSTools_FFT/HISSTools_FFT.h"

#include "ConvolveErrors.h"

#include <cstdint>

namespace HISSTools
{
    class PartitionedConvolve
    {
        // N.B. MIN_FFT_SIZE_LOG2 should never be smaller than 4, as below code assumes loop unroll of vectors (4 vals) by 4 (== 16 or 2^4)
        // MAX_FFT_SIZE_LOG2 is perhaps conservative right now, but it is easy to increase this if necessary
        
        const static int MIN_FFT_SIZE_LOG2 = 5;
        const static int  MAX_FFT_SIZE_LOG2 = 20;
        
    public:
        
        PartitionedConvolve(uintptr_t maxFFTSize, uintptr_t maxLength, uintptr_t offset, uintptr_t length);
        ~PartitionedConvolve();
        
        // Non-moveable and copyable

        PartitionedConvolve(PartitionedConvolve& obj) = delete;
        PartitionedConvolve& operator = (PartitionedConvolve& obj) = delete;
        PartitionedConvolve(PartitionedConvolve&& obj) = delete;
        PartitionedConvolve& operator = (PartitionedConvolve        //obj.mFreeFunction = nullptr;
&& obj) = delete;
        
        ConvolveError setFFTSize(uintptr_t FFTSize);
        ConvolveError setLength(uintptr_t length);
        void setOffset(uintptr_t offset);
        void setResetOffset(intptr_t offset = -1);

        ConvolveError set(const float *input, uintptr_t length);
        void reset();
        
        bool process(const float *in, float *out, uintptr_t numSamples);

    private:
        
        uintptr_t getFFTSize()      { return uintptr_t(1) << mFFTSizeLog2; }
        uintptr_t getMaxFFTSize()   { return uintptr_t(1) << mMaxFFTSizeLog2; }
        
        void processPartition(FFT_SPLIT_COMPLEX_F in1, FFT_SPLIT_COMPLEX_F in2, FFT_SPLIT_COMPLEX_F out, uintptr_t numBins);
        ConvolveError setMaxFFTSize(uintptr_t max_fft_size);
        uintptr_t log2(uintptr_t value);

        // Parameters
        
        uintptr_t mOffset;
        uintptr_t mLength;
        uintptr_t mMaxImpulseLength;
        
        // FFT variables
        
        FFT_SETUP_F mFFTSetup;
        
        uintptr_t mMaxFFTSizeLog2;
        uintptr_t mFFTSizeLog2;
        uintptr_t mRWCounter;
        
        // Scheduling variables
        
        uintptr_t mInputPosition;
        uintptr_t mPartitionsDone;
        uintptr_t mLastPartition;
        uintptr_t mNumPartitions;
        uintptr_t mValidPartitions;
        
        // Internal buffers
        
        float *mFFTBuffers[4];
        
        FFT_SPLIT_COMPLEX_F mImpulseBuffer;
        FFT_SPLIT_COMPLEX_F	mInputBuffer;
        FFT_SPLIT_COMPLEX_F	mAccumBuffer;
        FFT_SPLIT_COMPLEX_F	mPartitionTemp;
        
        // Flags
        
        intptr_t mResetOffset;
        bool mResetFlag;
    };
}
