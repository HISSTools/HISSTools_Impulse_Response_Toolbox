
#ifndef _AH_ATOMIC_
#define _AH_ATOMIC_

// Use Max SDK macros

#include <ext_atomic.h>

static inline long Atomic_Compare_And_Swap(t_int32_atomic Comparand, t_int32_atomic Exchange, t_int32_atomic *Destination)
{
#ifdef __APPLE__
    if (OSAtomicCompareAndSwap32(Comparand, Exchange, (int32_t *) Destination))
#else
    if (InterlockedCompareExchange  (Destination, Exchange, Comparand) == Comparand)
#endif
        return 1;
    return 0;
}

static inline long Atomic_Compare_And_Swap_Barrier(t_int32_atomic Comparand, t_int32_atomic Exchange, t_int32_atomic *Destination)
{
#ifdef __APPLE__
    if (OSAtomicCompareAndSwap32Barrier(Comparand, Exchange, (int32_t *) Destination))
#else
    if (InterlockedCompareExchange  (Destination, Exchange, Comparand) == Comparand)
#endif
        return 1;
    return 0;
}

static inline long Atomic_Get_And_Zero(t_int32_atomic *theValue, t_int32_atomic *theOldValue)
{
    t_int32_atomic compare_value;
    long exchanged;

    compare_value = *theValue;
    exchanged = Atomic_Compare_And_Swap_Barrier (compare_value, 0, theValue);

    if (exchanged)
    {
        *theOldValue = compare_value;
        return 1;
    }

    return 0;
}

#endif        /* _AH_ATOMIC_ */
