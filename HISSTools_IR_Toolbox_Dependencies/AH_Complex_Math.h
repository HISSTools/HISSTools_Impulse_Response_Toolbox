
/*
 *  AH_Complex_Math
 *
 *    This header file provides definitions to unify mac/windows approaches to complex math
 *
 */


#ifndef _AH_COMPLEX_MATH_
#define _AH_COMPLEX_MATH_

#include <math.h>
#include "AH_Math.h"
#include <complex.h>

#define CREAL(a) creal(a)
#define CIMAG(a) cimag(a)
#define CABS(a) cabs(a)
#define CABS_SQ(a) cm_cabs_sq(a)
#define CARG(a) carg(a)
#define CONJ(a) conj(a)
#define CEXP(a) cexp(a)
#define CLOG(a) clog(a)
#define CPOLAR(a, b) cm_cpolar(a, b)

#ifdef __APPLE__

#define COMPLEX_DOUBLE complex double

#define CSET(a, b) ((a) + (I * (b)))
#define CADD(a, b) ((a) + (b))
#define CSUB(a, b) ((a) - (b))
#define CMUL(a, b) ((a) * (b))
#define CDIV(a, b) ((a) / (b))

#else

#define COMPLEX_DOUBLE _Dcomplex 

#define CSET(a, b) _Cbuild(a, b)
#define CADD(a, b) cm_cadd(a, b)
#define CSUB(a, b) cm_csub(a, b)
#define CMUL(a, b) _Cmulcc(a, b)
#define CDIV(a, b) cm_cdiv(a, b)

static __inline COMPLEX_DOUBLE cm_cadd(COMPLEX_DOUBLE in1, COMPLEX_DOUBLE in2)
{
	double a = CREAL(in1);
	double b = CIMAG(in1);
	double c = CREAL(in2);
	double d = CIMAG(in2);

    return CSET(a + c, b + d);
}

static __inline COMPLEX_DOUBLE cm_csub(COMPLEX_DOUBLE in1, COMPLEX_DOUBLE in2)
{
	double a = CREAL(in1);
	double b = CIMAG(in1);
	double c = CREAL(in2);
	double d = CIMAG(in2);

	return CSET(a - c, b - d);
}

static __inline COMPLEX_DOUBLE cm_cdiv(COMPLEX_DOUBLE in1, COMPLEX_DOUBLE in2)
{
	double a = CREAL(in1);
	double b = CIMAG(in1);
	double c = CREAL(in2);
	double d = CIMAG(in2);
    double e = 1.0 / (c*c + d*d);

    double real = (a*c + b*d) * e;
    double imag = (b*c - a*d) * e;

    return CSET(real, imag);
}

#endif

static __inline double cm_cabs_sq(COMPLEX_DOUBLE in)
{
    double a = CREAL(in);
    double b = CIMAG(in);

    return (a * a) + (b * b);
}

static __inline COMPLEX_DOUBLE cm_cpolar(double a, double w)
{
    return CSET(a * cos(w), a * sin(w));
}

#endif    /* _AH_WIN_COMPLEX_MATH_ */
