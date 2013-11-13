
/*
 *  AH_Win_Complex_Math
 *
 *	This header file provides definitions or code to replace complex.h (not present under Windows using Visual Studio)
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

typedef struct _complex_double
{
	double real;
	double imag;
	
} t_complex_double;

#define COMPLEX_DOUBLE t_complex_double

#define CREAL(a) (a).real
#define CIMAG(a) (a).imag
#define CNEG(a) cm_cneg(a)
#define CABS(a) cm_cabs(a)
#define CABS_SQ(a) cm_cabs_sq(a)
#define CARG(a) cm_carg(a)
#define CONJ(a) cm_conj(a)
#define CSET(a, b) cm_cset(a, b)
#define CADD(a, b) cm_cadd(a, b)
#define CSUB(a, b) cm_csub(a, b)
#define CMUL(a, b) cm_cmul(a, b)
#define CDIV(a, b) cm_cdiv(a, b)
#define CSMUL(a, b) cm_csmul(a, b)
#define CSDIV(a, b) cm_csdiv(a, b)
#define CEXP(a) cm_cexp(a)
#define CLOG(a) cm_clog(a)
#define CPOLAR(a, b) cm_cpolar(a, b)

static __inline t_complex_double cm_cset (double a, double b)
{
	t_complex_double ret;
	
	ret.real = a;
	ret.imag = b;
	
	return ret;
}

static __inline double cm_cabs (t_complex_double in)
{	
	double a = in.real;
	double b = in.imag;
	
	return sqrt((a * a) + (b * b));
}

static __inline double cm_carg (t_complex_double in)
{	
	double a = in.real;
	double b = in.imag;
	
	return atan2(b, a);
}

static __inline t_complex_double cm_conj (t_complex_double in)
{
	t_complex_double ret;
	
	ret.real = in.real;
	ret.imag = -in.imag;
	
	return ret;
}


static __inline t_complex_double cm_cadd (t_complex_double in1, t_complex_double in2)
{
	t_complex_double ret;
	
	double a = in1.real;
	double b = in1.imag;
	double c = in2.real;
	double d = in2.imag;
	
	ret.real = a + c;
	ret.imag = b + d;
	
	return ret;
}

static __inline t_complex_double cm_csub (t_complex_double in1, t_complex_double in2)
{
	t_complex_double ret;
	
	double a = in1.real;
	double b = in1.imag;
	double c = in2.real;
	double d = in2.imag;
	
	ret.real = a - c;
	ret.imag = b - d;
	
	return ret;
}

static __inline t_complex_double cm_cmul (t_complex_double in1, t_complex_double in2)
{
	t_complex_double ret;
	
	double a = in1.real;
	double b = in1.imag;
	double c = in2.real;
	double d = in2.imag;
	
	ret.real = a*c - b*d;
	ret.imag = a*d + b*c;
	
	return ret;
}

static __inline t_complex_double cm_csmul (t_complex_double in1, double in2)
{
	t_complex_double ret;
	
	ret.real = in1.real * in2;
	ret.imag = in1.imag * in2;
	
	return ret;
}

static __inline t_complex_double cm_csdiv (t_complex_double in1, double in2)
{
	t_complex_double ret;
	
	double recip = 1. / in2;
	
	ret.real = in1.real * recip;
	ret.imag = in1.imag * recip;
	
	return ret;
}

static __inline t_complex_double cm_cdiv (t_complex_double in1, t_complex_double in2)
{
	t_complex_double ret;
	
	double a = in1.real;
	double b = in1.imag;
	double c = in2.real;
	double d = in2.imag;
	double e = 1. / (c*c + d*d);
	
	ret.real = (a*c + b*d) * e;
	ret.imag = (a*d - b*c) * e;
	
	return ret;
}

static __inline t_complex_double cm_cexp (t_complex_double in1)
{
	t_complex_double ret;
	
	double a = exp(in1.real);
	double b = in1.imag;
	
	ret = cm_cset(a * cos(b), a * sin(b));
	
	return ret;
}

static __inline t_complex_double cm_clog (t_complex_double in1)
{
	t_complex_double ret = cm_cset(log(cm_cabs(in1)), cm_carg(in1));
	
	return ret;
}

#endif	

static __inline COMPLEX_DOUBLE cm_cneg (COMPLEX_DOUBLE in)
{	
	COMPLEX_DOUBLE ret = CSET(-CREAL(in), -CIMAG(in));
	
	return ret;
}

static __inline double cm_cabs_sq (COMPLEX_DOUBLE in)
{	
	double a = CREAL(in);
	double b = CIMAG(in);
	
	return (a * a) + (b * b);
}

static __inline COMPLEX_DOUBLE cm_cpolar (double a, double w)
{
	COMPLEX_DOUBLE ret = CSET(a * cos(w), a * sin(w));
	
	return ret;
}

#endif	/* _AH_WIN_COMPLEX_MATH_ */
