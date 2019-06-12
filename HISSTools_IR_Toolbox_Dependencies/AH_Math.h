
/*
 *  AH_Math
 *
 *    This header file provides definitions or code for math.h functions not present under Windows using Visual Studio
 *
 */

#ifndef _AH_MATH_
#define _AH_MATH_

#ifndef __APPLE__

#define _USE_MATH_DEFINES

#include <math.h>
#include <float.h>

#ifdef isnan
#undef isnan
#endif

#ifdef isinf
#undef isinf
#endif

static __inline double round(double r)
{
    return (r > 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
}

static __inline long isnan(double n)
{
    return !(n == n);
}

static __inline long isinf(double n)
{
    return n == n * 2;
}

#endif

#endif    /* _AH_MATH_ */
