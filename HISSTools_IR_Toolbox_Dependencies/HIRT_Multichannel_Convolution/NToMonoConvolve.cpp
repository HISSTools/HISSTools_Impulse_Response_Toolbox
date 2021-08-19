
#include "NToMonoConvolve.h"

HISSTools::NToMonoConvolve::NToMonoConvolve(uint32_t inChans, uintptr_t maxLength, LatencyMode latency)
:  mNumInChans(inChans)
{
    for (uint32_t i = 0; i < mNumInChans; i++)
        mConvolvers.emplace_back(maxLength, latency);
}

template<typename Method, typename... Args>
ConvolveError HISSTools::NToMonoConvolve::doChannel(Method method, uint32_t inChan, Args...args)
{
    if (inChan < mNumInChans)
        return (mConvolvers[inChan].*method)(args...);
    else
        return CONVOLVE_ERR_IN_CHAN_OUT_OF_RANGE;
}

ConvolveError HISSTools::NToMonoConvolve::resize(uint32_t inChan, uintptr_t impulse_length)
{
    return doChannel(&MonoConvolve::resize, inChan, impulse_length);
}

ConvolveError HISSTools::NToMonoConvolve::set(uint32_t inChan, const float *input, uintptr_t impulse_length, bool resize)
{
    return doChannel(&MonoConvolve::set, inChan, input, impulse_length, resize);
}

ConvolveError HISSTools::NToMonoConvolve::reset(uint32_t inChan)
{
    return doChannel(&MonoConvolve::reset, inChan);
}

void HISSTools::NToMonoConvolve::process(const float * const* ins, float *out, float *temp, size_t numSamples, size_t activeInChans)
{
    // Zero output then convolve
    
    std::fill_n(out, numSamples, 0.f);
	
	for (uint32_t i = 0; i < mNumInChans && i < activeInChans ; i++)
		mConvolvers[i].process(ins[i], temp, out, numSamples, true);
}
