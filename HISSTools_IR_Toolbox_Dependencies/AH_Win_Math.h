
/*
 *  AH_Win_Math
 *
 *	This header file provides definitions or code for math.h functions not present under Windows using Visual Studio
 *
 */

#ifndef _AH_WIN_MATH_
#define _AH_WIN_MATH_

#ifndef __APPLE__

#define _USE_MATH_DEFINES

#include <math.h>
#include <float.h>

#define isfinite _finite

#define LOG_2_RECIP 1.44269504088896340735992468100189213742664595415299

static __inline double round (double r) 
{
    return (r > 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
}

static __inline double log2(double n)
{
	return log(n) * LOG_2_RECIP;
}

static __inline double exp2(double n)
{
	return pow(2, n);
}

static __inline long isnan(double n)
{
	return !(n == n);
}

static __inline long isinf(double n)
{
	return n == n * 2;
}

static __inline double copysign(double a, double b)
{
	double val = fabs(a);

	return b < 0 ? -val : val;
}

static __inline double asinh (double x)
{
	return log(x + sqrt((x * x) + 1));
}

static __inline double acosh (double x)
{
	return log(x + sqrt((x * x) - 1));
}

static __inline double atanh (double x)
{
    return (log(1+x) - log(1-x))/2.0;
}

static __inline float asinhf (float x)
{
	return (float) asinh(x);
}

static __inline float acoshf (float x)
{
	return (float) acosh(x);
}

static __inline float atanhf (float x)
{
	return (float) atanh(x);
}

#ifndef FLT_MAX
#define FLT_MAX 3.402823466e+38f
#endif

#ifndef DBL_MAX
#define DBL_MAX 3.402823466e+38f
#endif

#endif	

#endif	/* _AH_WIN_MATH_ */
