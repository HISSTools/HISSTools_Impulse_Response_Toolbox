
/*
 *  ibuffer~ header
 *
 *	This header file provides the structure for an ibuffer, so other objects can directly access fields from a valid ibuffer pointer, as well as some useful defines.
 *	However, in most cases, the best way to retrieve data from an ibuffer is to use ibuffer_access.c, which can also be used to access standard MaxMSP buffers
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#ifndef _IBUFFER_
#define _IBUFFER_ 


#include <ext.h>
#include <z_dsp.h>
#include <buffer.h>

#include "AH_Atomic.h"
#include "AH_Types.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////// ibuffer~ object structure ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


typedef struct _ibuffer
{
    t_pxobject x_obj;
	
	t_symbol *name;			// the current name of this ibuffer~
	
	void *thebuffer;		// where the memory starts (including zero padding)
	void *samples;			// where the samples start	
	
	double sr;				// sample rate of ibuffer~
	
	AH_SIntPtr frames;		// number of frames (N.B. - could be very large under 64bit platforms - hence the pointer sized integer - signed for backwards compatibility)
	long channels;			// number of channels
	long format;			// sample format
	
	long valid;				// set to 0 when loading / unable to load
	t_int32_atomic inuse;	// count for num objects currently using this ibuffer~
	
	void *bang_out;			// bangs on load
	
} t_ibuffer;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////// Defines and enums ////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#define MASK_16_BIT			0xFFFF0000
#define MASK_24_BIT			0xFFFFFF00

#ifdef _APPLE_
#define TWO_POW_31_RECIP			0x1.0fp-31f
#define TWO_POW_31_RECIP_DOUBLE		0x1.0fp-31
#else
#define TWO_POW_31_RECIP			0.000000000465661287f
#define TWO_POW_31_RECIP_DOUBLE		0.00000000046566128730773925
#endif

t_symbol *ps_buffer;
t_symbol *ps_ibuffer;


enum {
	
	PCM_FLOAT	= 0,
	PCM_INT_16	= 2,
	PCM_INT_24	= 3,
	PCM_INT_32	= 4,
	
};

#endif		/* _IBUFFER_ */
