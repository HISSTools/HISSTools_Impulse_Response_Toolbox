
#pragma once

#include "PartitionedConvolve.h"
#include "TimeDomainConvolve.h"
#include "ConvolveErrors.h"
#include "MemorySwap.h"

#include <cstdint>
#include <memory>
#include <random>
#include <vector>

enum LatencyMode
{
    kLatencyZero,
    kLatencyShort,
    kLatencyMedium,
} ;

namespace HISSTools
{
    class MonoConvolve
    {
        typedef MemorySwap<HISSTools::PartitionedConvolve>::Ptr PartPtr;
        typedef std::unique_ptr<HISSTools::PartitionedConvolve> PartUniquePtr;

    public:
        
        MonoConvolve(uintptr_t maxLength, LatencyMode latency);
        MonoConvolve(uintptr_t maxLength, bool zeroLatency, uint32_t A, uint32_t B = 0, uint32_t C = 0, uint32_t D = 0);
        
        // Moveable but not copyable
        
        MonoConvolve(MonoConvolve& obj) = delete;
        MonoConvolve& operator = (MonoConvolve& obj) = delete;
        MonoConvolve(MonoConvolve&& obj);
        MonoConvolve& operator = (MonoConvolve&& obj);
        
        void setResetOffset(intptr_t offset = -1);

        ConvolveError resize(uintptr_t length);
        ConvolveError set(const float *input, uintptr_t length, bool requestResize);
        ConvolveError reset();
        
        void process(const float *in, float *temp, float *out, uintptr_t numSamples, bool accumulate = false);
        
        void setPartitions(uintptr_t maxLength, bool zeroLatency, uint32_t A, uint32_t B = 0, uint32_t C = 0, uint32_t D = 0);

    private:

        void setResetOffset(PartPtr &part4, intptr_t offset = -1);

        size_t numSizes() { return mSizes.size(); }
        
        MemorySwap<PartitionedConvolve>::AllocFunc mAllocator;

        std::vector<uint32_t> mSizes;
        
        std::unique_ptr<TimeDomainConvolve> mTime1;
        std::unique_ptr<PartitionedConvolve> mPart1;
        std::unique_ptr<PartitionedConvolve> mPart2;
        std::unique_ptr<PartitionedConvolve> mPart3;
        
        MemorySwap<PartitionedConvolve> mPart4;
        
        uintptr_t mLength;
        
        intptr_t mPart4ResetOffset;
        bool mReset;
        
        // Random Number Generation
        
        std::default_random_engine mRandGenerator;
        std::uniform_int_distribution<uintptr_t> mRandDistribution;
    };
}
