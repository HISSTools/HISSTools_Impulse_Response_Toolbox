
#include "Convolver.h"

HISSTools::Convolver::Convolver(uint32_t numIns, uint32_t numOuts, LatencyMode latency)
: mTemporaryMemory(0)
{
    numIns = numIns < 1 ? 1 : numIns;
    
    mN2M = true;
    mNumIns = numIns;
    mNumOuts = numOuts;
    
    for (uint32_t i = 0; i < numIns; i++)
        mInTemps.push_back(nullptr);
    
    for (uint32_t i = 0; i < numOuts; i++)
        mConvolvers.push_back(new NToMonoConvolve(numIns, 16384, latency));
    
    mTemp1 = nullptr;
    mTemp2 = nullptr;
}

HISSTools::Convolver::Convolver(uint32_t numIO, LatencyMode latency)
: mTemporaryMemory(0)
{
    numIO = numIO < 1 ? 1 : numIO;
    
    mN2M = false;
    mNumIns = numIO;
    mNumOuts = numIO;
    
    for (uint32_t i = 0; i < numIO; i++)
    {
        mConvolvers.push_back(new NToMonoConvolve(1, 16384, latency));
        mInTemps.push_back(nullptr);
    }
    
    mTemp1 = nullptr;
    mTemp2 = nullptr;
}

HISSTools::Convolver::~Convolver() throw()
{
    for (uint32_t i = 0; i < mNumOuts; i++)
        delete mConvolvers[i];
}

// Clear IRs

void HISSTools::Convolver::clear(bool resize)
{
    if (mN2M)
    {
        for (uint32_t i = 0; i < mNumOuts; i++)
            for (uint32_t j = 0; j < mNumIns; j++)
                clear(j, i, resize);
    }
    else
    {
        for (uint32_t i = 0; i < mNumOuts; i++)
            clear(i, i, resize);
    }
}

void HISSTools::Convolver::clear(uint32_t inChan, uint32_t outChan, bool resize)
{
    set(inChan, outChan, (float *)nullptr, 0, resize);
}

// DSP Engine Reset

void HISSTools::Convolver::reset()
{
    if (mN2M)
    {
        for (uint32_t i = 0; i < mNumOuts; i++)
            for (uint32_t j = 0; j < mNumIns; j++)
                reset(j, i);
    }
    else
    {
        for (uint32_t i = 0; i < mNumOuts; i++)
            reset(i, i);
    }
}

ConvolveError HISSTools::Convolver::reset(uint32_t inChan, uint32_t outChan)
{
    // For Parallel operation you must pass the same in/out channel
    
    if (!mN2M) inChan -= outChan;
    
    if (outChan < mNumOuts)
        return mConvolvers[outChan]->reset(inChan);
    else
        return CONVOLVE_ERR_OUT_CHAN_OUT_OF_RANGE;
}

// Resize and set IR

ConvolveError HISSTools::Convolver::resize(uint32_t inChan, uint32_t outChan, uintptr_t length)
{
    // For Parallel operation you must pass the same in/out channel
    
    if (!mN2M) inChan -= outChan;
    
    if (outChan < mNumOuts)
        return mConvolvers[outChan]->resize(inChan, length);
    else
        return CONVOLVE_ERR_IN_CHAN_OUT_OF_RANGE;
}

ConvolveError HISSTools::Convolver::set(uint32_t inChan, uint32_t outChan, const float* input, uintptr_t length, bool resize)
{
    // For Parallel operation you must pass the same in/out channel
    
    if (!mN2M) inChan -= outChan;
    
    if (outChan < mNumOuts)
        return mConvolvers[outChan]->set(inChan, input, length, resize);
    else
        return CONVOLVE_ERR_OUT_CHAN_OUT_OF_RANGE;
}

ConvolveError HISSTools::Convolver::set(uint32_t inChan, uint32_t outChan, const double* input, uintptr_t impulseLength, bool resize)
{
    std::vector<float> inputFloat(impulseLength);
    
    for (unsigned long i = 0; i < impulseLength; i++)
        inputFloat[i] = static_cast<float>(input[i]);
    
    return set(inChan, outChan, inputFloat.data(), impulseLength, resize);
}

// DSP

void HISSTools::Convolver::process(const float * const* ins, float** outs, size_t numIns, size_t numOuts, size_t numSamples)
{
    auto memPointer = mTemporaryMemory.grow((mNumIns + 2) * numSamples);
    tempSetup(memPointer.get(), memPointer.getSize());
    
    if (!memPointer.get())
        numIns = numOuts = 0;
    
    SIMDSettings settings;
    
    for (size_t i = 0; i < numOuts; i++)
    {
        const float *n2nIn[1] = { ins[i] };
        
        mConvolvers[i]->process(mN2M ? ins : n2nIn, outs[i], mTemp1, numSamples, mN2M ? numIns : 1);
    }
}

void HISSTools::Convolver::process(const double * const* ins, double** outs, size_t numIns, size_t numOuts, size_t numSamples)
{
    auto memPointer = mTemporaryMemory.grow((mNumIns + 2) * numSamples);
    tempSetup(memPointer.get(), memPointer.getSize());
    
    if (!memPointer.get())
        numIns = numOuts = 0;
    
    numIns = numIns > mNumIns ? mNumIns : numIns;
    numOuts = numOuts > mNumOuts ? mNumOuts : numOuts;
    
    for (uintptr_t i = 0; i < numIns; i++)
        for (uintptr_t j = 0; j < numSamples; j++)
            mInTemps[i][j] = static_cast<float>(ins[i][j]);
    
    SIMDSettings settings;
    
    for (uintptr_t i = 0; i < numOuts; i++)
    {
        const float *n2nIn[1] = { mInTemps[i] };

        mConvolvers[i]->process(mN2M ? mInTemps.data() : n2nIn, mTemp2, mTemp1, numSamples, mN2M ? numIns : 1);
        
        for (uintptr_t j = 0; j < numSamples; j++)
            outs[i][j] = mTemp2[j];
    }
}

void HISSTools::Convolver::tempSetup(float* memPointer, uintptr_t maxFrameSize)
{
    maxFrameSize /= (mNumIns + 2);
    mInTemps[0] = memPointer;
    
    for (uint32_t i = 1; i < mNumIns; i++)
        mInTemps[i] = mInTemps[0] + (i * maxFrameSize);
    
    mTemp1 = mInTemps[mNumIns - 1] + maxFrameSize;
    mTemp2 = mTemp1 + maxFrameSize;
}

HISSTools::Convolver::SIMDSettings::SIMDSettings()
{
#if defined(__i386__) || defined(__x86_64__)
    mOldMXCSR = _mm_getcsr();
    _mm_setcsr(mOldMXCSR | 0x8040);
#endif
}

HISSTools::Convolver::SIMDSettings::~SIMDSettings()
{
#if defined(__i386__) || defined(__x86_64__)
    _mm_setcsr(mOldMXCSR);
#endif
}
