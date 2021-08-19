
#ifndef __HIRT_FFT_SETUP__
#define __HIRT_FFT_SETUP__

#include <HISSTools_FFT/HISSTools_FFT.h>

class temp_fft_setup
{
public:
    
    temp_fft_setup(uintptr_t max_fft_log_2)
    {
        hisstools_create_setup(&setup, max_fft_log_2);
    }
    
    ~temp_fft_setup()
    {
        hisstools_destroy_setup(setup);
        setup = nullptr;
    }
    
    temp_fft_setup(const temp_fft_setup& a) = delete;
    temp_fft_setup& operator = (const temp_fft_setup& a) = delete;
    
    operator bool()                         { return setup; }
    operator FFT_SETUP_D()                  { return setup; }
    
private:
    
    FFT_SETUP_D setup;
};

#endif /* __HIRT_FFT_SETUP__ */
