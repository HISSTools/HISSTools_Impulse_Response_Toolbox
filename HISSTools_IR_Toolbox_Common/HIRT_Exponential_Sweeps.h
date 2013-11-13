
#ifndef __HIRT_EXPONENTIAL_SWEEPS__
#define __HIRT_EXPONENTIAL_SWEEPS__

#include <AH_Win_Math.h>
#include <AH_Types.h>

#ifdef __cplusplus
extern "C" {
#endif
	
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////// ESS Structure  /////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


typedef struct _ess
{		
	// Internal only
	
	AH_UIntPtr T;
	double K1, K2;

	double lo_f_act;
	double hi_f_act;
	
	double f1;
	double f2;
	
	// Parameters requested
	
	double RT;
	double rf1, rf2;
	
	double fade_in;
	double fade_out;
	
	double sample_rate;
	double amp;
	
	// Amplitude curve
	
	AH_UIntPtr num_amp_specifiers;
	
	double amp_specifier[34];
	
	
} t_ess;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////// Get Length / HArm Offsets ///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static __inline AH_UIntPtr ess_get_length (t_ess *x)
{
	return x->T;
}


static __inline double ess_harm_offset(t_ess *x, AH_UIntPtr harm)
{
	return x->T / log(x->hi_f_act/x->lo_f_act) * log((double) harm);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////// Function Prototypes ///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


AH_UIntPtr ess_params(t_ess *x, double f1, double f2, double fade_in, double fade_out, double T, double sample_rate, double amp, double *amp_curve);

AH_UIntPtr ess_gen_float (t_ess *x, float *out, AH_UIntPtr startN, AH_UIntPtr N);
AH_UIntPtr ess_igen_float (t_ess *x, float *out, AH_UIntPtr startN, AH_UIntPtr N, AH_Boolean inv_amp);
AH_UIntPtr ess_gen_double (t_ess *x, double *out, AH_UIntPtr startN, AH_UIntPtr N);
AH_UIntPtr ess_igen_double (t_ess *x, double *out, AH_UIntPtr startN, AH_UIntPtr N, AH_Boolean inv_amp);

AH_UIntPtr ess_gen_block (t_ess *x, void *out, AH_UIntPtr startN, AH_UIntPtr N, AH_Boolean double_precision);
AH_UIntPtr ess_igen_block (t_ess *x, void *out, AH_UIntPtr startN, AH_UIntPtr N, AH_Boolean inv_amp, AH_Boolean double_precision);
AH_UIntPtr ess_gen (t_ess *x, void *out, AH_Boolean double_precision);
AH_UIntPtr ess_igen (t_ess *x, void *out, AH_Boolean inv_amp, AH_Boolean double_precision);

#ifdef __cplusplus
}
#endif

#endif /* __HIRT_EXPONENTIAL_SWEEPS__ */
