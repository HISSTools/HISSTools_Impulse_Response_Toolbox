
/*
 *  AH_Win_Complex_Math
 *
 *    This header file provides definitions or code to replace complex.h (not present under Windows using Visual Studio)
 *
 */


#ifndef _AH_WIN_COMPLEX_MATH_
#define _AH_WIN_COMPLEX_MATH_

#ifdef __APPLE__

#include <math.h>
#include <complex.h>

#define COMPLEX_DOUBLE complex double

#define CREAL(a) creal(a)
#define CIMAG(a) cimag(a)
#define CNEG(a) cm_cneg(a)
#define CABS(a) cabs(a)
#define CABS_SQ(a) cm_cabs_sq(a)
#define CARG(a) carg(a)
#define CONJ(a) conj(a)
#define CSET(a, b) (a + I * b)
#define CADD(a, b) (a + b)
#define CSUB(a, b) (a - b)
#define CMUL(a, b) (a * b)
#define CDIV(a, b) (a / b)
#define CSMUL(a, b) (a * b)
#define CSDIV(a, b) (a / b)
#define CEXP(a) cexp(a)
#define CLOG(a) clog(a)
#define CPOLAR(a, b) cm_cpolar(a, b)

#else

#include <math.h>
#include "AH_Win_Math.h"
#include <complex.h>

#define COMPLEX_DOUBLE _Dcomplex 

#define CREAL(a) creal(a)
#define CIMAG(a) cimag(a)
#define CNEG(a) cm_cneg(a)
#define CABS(a) cabs(a)
#define CABS_SQ(a) cm_cabs_sq(a)
#define CARG(a) carg(a)
#define CONJ(a) conj(a)
#define CSET(a, b) _Cbuild(a, b)
#define CADD(a, b) cm_cadd(a, b)
#define CSUB(a, b) cm_csub(a, b)
#define CMUL(a, b) _Cmulcc(a, b)
#define CDIV(a, b) cm_cdiv(a, b)
#define CSMUL(a, b) _Cmulcr(a, b)
#define CSDIV(a, b) cm_csdiv(a, b)
#define CEXP(a) cexp(a)
#define CLOG(a) clog(a)
#define CPOLAR(a, b) cm_cpolar(a, b)

static __inline COMPLEX_DOUBLE cm_cadd(COMPLEX_DOUBLE in1, COMPLEX_DOUBLE in2)
{
	double a = CREAL(in1);
	double b = CIMAG(in1);
	double c = CREAL(in2);
	double d = CIMAG(in2);

    return _Cbuild(a + c, b + d);
}

static __inline COMPLEX_DOUBLE cm_csub(COMPLEX_DOUBLE in1, COMPLEX_DOUBLE in2)
{
	double a = CREAL(in1);
	double b = CIMAG(in1);
	double c = CREAL(in2);
	double d = CIMAG(in2);

	return _Cbuild(a - c, b - d);
}


static __inline COMPLEX_DOUBLE cm_csdiv(COMPLEX_DOUBLE in1, double in2)
{
    double recip = 1.0 / in2;

    return _Cbuild(CREAL(in1) * recip, CIMAG(in1) * recip);
}

static __inline COMPLEX_DOUBLE cm_cdiv(COMPLEX_DOUBLE in1, COMPLEX_DOUBLE in2)
{
	COMPLEX_DOUBLE ret;

	double a = CREAL(in1);
	double b = CIMAG(in1);
	double c = CREAL(in2);
	double d = CIMAG(in2);
    double e = 1.0 / (c*c + d*d);

    double real = (a*c + b*d) * e;
    double imag = (b*c - a*d) * e;

    return _Cbuild(real, imag);
}

#endif

static  COMPLEX_DOUBLE cm_cneg(COMPLEX_DOUBLE in)
{
    COMPLEX_DOUBLE ret = CSET(-CREAL(in), -CIMAG(in));

    return ret;
}

static __inline double cm_cabs_sq(COMPLEX_DOUBLE in)
{
    double a = CREAL(in);
    double b = CIMAG(in);

    return (a * a) + (b * b);
}

static __inline COMPLEX_DOUBLE cm_cpolar(double a, double w)
{
    COMPLEX_DOUBLE ret = CSET(a * cos(w), a * sin(w));

    return ret;
}

#endif    /* _AH_WIN_COMPLEX_MATH_ */
