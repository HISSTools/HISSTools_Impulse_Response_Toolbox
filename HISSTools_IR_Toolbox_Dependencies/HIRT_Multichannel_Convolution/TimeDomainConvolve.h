
#pragma once

#include "ConvolveErrors.h"

#include <cstdint>

namespace HISSTools
{
    class TimeDomainConvolve
    {
        
    public:
        
        TimeDomainConvolve(uintptr_t offset, uintptr_t length);
        ~TimeDomainConvolve();
        
        // Non-moveable and copyable
        
        TimeDomainConvolve(TimeDomainConvolve& obj) = delete;
        TimeDomainConvolve& operator = (TimeDomainConvolve& obj) = delete;
        TimeDomainConvolve(TimeDomainConvolve&& obj) = delete;
        TimeDomainConvolve& operator = (TimeDomainConvolve&& obj) = delete;
        
        ConvolveError setLength(uintptr_t length);
        void setOffset(uintptr_t offset);
        
        ConvolveError set(const float *input, uintptr_t length);
        void reset();
        
        bool process(const float *in, float *out, uintptr_t numSamples);
        
    private:
        
        // Internal buffers
        
        float *mImpulseBuffer;
        float *mInputBuffer;
        
        uintptr_t mInputPosition;
        uintptr_t mImpulseLength;
        
        uintptr_t mOffset;
        uintptr_t mLength;
        
        // Flags
        
        bool mReset;
    };
}
