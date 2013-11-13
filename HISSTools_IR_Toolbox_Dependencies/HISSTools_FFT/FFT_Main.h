
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

long AHFFT_SSE_Exists = 0;

// Compile FFT Setup for double and single precision

#include "FFT_Type_Double.h"
#include "FFT_Setup.h"
#include "FFT_Type_Float.h"
#include "FFT_Setup.h"

// Compile SIMD Passes

#include "FFT_SIMD.h"

// Compile FFT Scalar Passes double and single precision

#include "FFT_Type_Double.h"
#include "FFT_Scalar.h"
#include "FFT_Type_Float.h"
#include "FFT_Scalar.h"

// Compile FFT Calls for double and single precision

#include "FFT_Type_Double.h"
#include "FFT_Calls.h"
#include "FFT_Type_Float.h"
#include "FFT_Calls.h"

// Compile Real FFT Calls for double and single precision

#include "FFT_Type_Double.h"
#include "FFT_Real.h"
#include "FFT_Type_Float.h"
#include "FFT_Real.h"
