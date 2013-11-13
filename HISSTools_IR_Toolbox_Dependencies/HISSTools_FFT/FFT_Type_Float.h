
#undef FFT_FUNC_NAME
#undef FFT_TYPE 
#undef FFT_SPLIT_TYPE 
#undef FFT_SETUP_TYPE 

#define FFT_FUNC_NAME(x) x##_float
#define FFT_TYPE float
#define FFT_SPLIT_TYPE SplitFloat
#define FFT_SETUP_TYPE FFTSetupFloat