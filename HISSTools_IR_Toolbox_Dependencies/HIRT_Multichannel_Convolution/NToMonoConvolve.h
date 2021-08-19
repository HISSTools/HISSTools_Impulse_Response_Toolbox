
#pragma once
	
#include "MonoConvolve.h"
#include "ConvolveErrors.h"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace HISSTools
{
    class NToMonoConvolve
    {
        
    public:
        
        NToMonoConvolve(uint32_t input_chans, uintptr_t maxLength, LatencyMode latency);
        
        ConvolveError resize(uint32_t inChan, uintptr_t impulse_length);
        ConvolveError set(uint32_t inChan, const float *input, uintptr_t impulse_length, bool resize);
        ConvolveError reset(uint32_t inChan);
        
        void process(const float * const* ins, float *out, float *temp, size_t numSamples, size_t active_in_chans);
        
    private:
        
        template<typename Method, typename... Args>
        ConvolveError doChannel(Method method, uint32_t inChan, Args...args);
        
        std::vector<MonoConvolve> mConvolvers;
        
        uint32_t mNumInChans;
    };
}
