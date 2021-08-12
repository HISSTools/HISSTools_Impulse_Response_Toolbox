
#pragma once

#include "MemorySwap.h"
#include "NToMonoConvolve.h"
#include "ConvolveErrors.h"

#include <cstdint>
#include <vector>

namespace HISSTools
{
    class Convolver
    {
        struct SIMDSettings
        {
            SIMDSettings();
            ~SIMDSettings();
            
            unsigned int mOldMXCSR;
        };
        
    public:
        
        Convolver(uint32_t numIns, uint32_t numOuts, LatencyMode latency);
        Convolver(uint32_t numIO, LatencyMode latency);
        
        virtual ~Convolver() throw();
        
        // Clear IRs
        
        void clear(bool resize);
        void clear(uint32_t inChan, uint32_t outChan, bool resize);
        
        // DSP Engine Reset
        
        void reset();
        ConvolveError reset(uint32_t inChan, uint32_t outChan);
        
        // Resize and set IR
        
        ConvolveError resize(uint32_t inChan, uint32_t outChan, uintptr_t impulseLength);
        
        ConvolveError set(uint32_t inChan, uint32_t outChan, const float* input, uintptr_t length, bool resize);
        ConvolveError set(uint32_t inChan, uint32_t outChan, const double* input, uintptr_t length, bool resize);
        
        // DSP
        
        void process(const double * const* ins, double** outs, size_t numIns, size_t numOuts, size_t numSamples);
        void process(const float * const*  ins, float** outs, size_t numIns, size_t numOuts, size_t numSamples);
        
    private:
        
        void tempSetup(float* memPointer, uintptr_t maxFrameSize);
        
        // Data
        
        uint32_t mNumIns;
        uint32_t mNumOuts;
        bool mN2M;
        
        std::vector<float*> mInTemps;
        float* mTemp1;
        float* mTemp2;
        
        MemorySwap<float> mTemporaryMemory;
        
        std::vector<NToMonoConvolve*> mConvolvers;
    };
}
