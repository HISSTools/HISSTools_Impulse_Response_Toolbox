
#ifndef _AH_ATOMIC_
#define _AH_ATOMIC_

// In case we are compiing for Max 4, or outside of the Max SDK we have to check fot a SDK number and provide the expected atomic macros if bit oresent or less than 5

#if (defined C74_MAX_SDK_VERSION && (C74_MAX_SDK_VERSION >= 0x0500))

// Use Max SDK macros

#include <ext_atomic.h>

#else			

// Local macros 

#ifdef __APPLE__	// Apple

#include <libkern/OSAtomic.h>

typedef int32_t t_int32_atomic; 

#define ATOMIC_INCREMENT OSAtomicIncrement32
#define ATOMIC_DECREMENT OSAtomicDecrement32
#define ATOMIC_INCREMENT_BARRIER OSAtomicIncrement32Barrier
#define ATOMIC_DECREMENT_BARRIER OSAtomicDecrement32Barrier

#else				// Windows

#include <intrin.h>

typedef volatile long t_int32_atomic;

#define ATOMIC_INCREMENT				_InterlockedIncrement
#define ATOMIC_DECREMENT				_InterlockedDecrement
#define ATOMIC_INCREMENT_BARRIER		_InterlockedIncrement
#define ATOMIC_DECREMENT_BARRIER		_InterlockedDecrement

#endif		/* __APPLE__ */
#endif		/* (C74_MAX_SDK_VERSION && (C74_MAX_SDK_VERSION >= 0x0500)) */


static __inline long Atomic_Compare_And_Swap (t_int32_atomic Comparand, t_int32_atomic Exchange, t_int32_atomic *Destination)
{
#ifdef __APPLE__
	if (OSAtomicCompareAndSwap32(Comparand, Exchange, (int32_t *) Destination))
#else
	if (InterlockedCompareExchange  (Destination, Exchange, Comparand) == Comparand)
#endif
		return 1;
	return 0;
}

static __inline long Atomic_Compare_And_Swap_Barrier (t_int32_atomic Comparand, t_int32_atomic Exchange, t_int32_atomic *Destination)
{
#ifdef __APPLE__
	if (OSAtomicCompareAndSwap32Barrier(Comparand, Exchange, (int32_t *) Destination))
#else
	if (InterlockedCompareExchange  (Destination, Exchange, Comparand) == Comparand)
#endif
		return 1;
	return 0;
}

static __inline long Atomic_Get_And_Zero (t_int32_atomic *theValue, t_int32_atomic *theOldValue)
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

#endif		/* _AH_ATOMIC_ */
