

#include "zero_latency_convolve.h"


void *largest_partition_time_alloc(AH_UIntPtr size, AH_UIntPtr nom_size)
{
    return partition_convolve_new(16384, (size > 16384 ? size : 16384) - 8192, 8192, 0);
}


void *largest_partition_fft1_alloc(AH_UIntPtr size, AH_UIntPtr nom_size)
{
    return partition_convolve_new(16384, (size > 16384 ? size : 16384) - 8064, 8064, 0);
}

void *largest_partition_fft2_alloc(AH_UIntPtr size, AH_UIntPtr nom_size)
{
    return partition_convolve_new(16384, (size > 16384 ? size : 16384) - 7680, 7680, 0);
}


void largest_partition_free(void *large_part)
{
    partition_convolve_free(large_part);
}


void zero_latency_convolve_free(t_zero_latency_convolve *x)
{
    if (!x)
        return;

    time_domain_convolve_free(x->time1);
    partition_convolve_free(x->part1);
    partition_convolve_free(x->part2);
    partition_convolve_free(x->part3);
    free_memory_swap(&x->part4);
    free(x);
}


t_zero_latency_convolve *zero_latency_convolve_new(AH_UIntPtr max_length, t_convolve_latency_mode latency_mode)
{
    t_zero_latency_convolve *x = malloc(sizeof(t_zero_latency_convolve));
    long fail = 0;

    if (!x)
        return 0;

    latency_mode = latency_mode > 2 ? 2 : latency_mode;

    switch (latency_mode)
    {
        case CONVOLVE_LATENCY_ZERO:

            x->time1 = time_domain_convolve_new(0, 128);
            x->part1 = partition_convolve_new(256, 384, 128, 384);
            x->part2 = partition_convolve_new(1024, 1536, 512, 1536);
            x->part3 = partition_convolve_new(4096, 6144, 2048, 6144);
            fail = alloc_memory_swap_custom(&x->part4, largest_partition_time_alloc, largest_partition_free, max_length, max_length);

            break;

        case CONVOLVE_LATENCY_SHORT:

            x->time1 = NULL;
            x->part1 = partition_convolve_new(256, 384, 0, 384);
            x->part2 = partition_convolve_new(1024, 1536, 384, 1536);
            x->part3 = partition_convolve_new(4096, 6144, 1920, 6144);
            fail = alloc_memory_swap_custom(&x->part4, largest_partition_fft1_alloc, largest_partition_free, max_length, max_length);

            break;

        case CONVOLVE_LATENCY_MEDIUM:

            x->time1 = NULL;
            x->part1 = NULL;
            x->part2 = partition_convolve_new(1024, 1536, 0, 1536);
            x->part3 = partition_convolve_new(4096, 6144, 1536, 6144);
            fail = alloc_memory_swap_custom(&x->part4, largest_partition_fft2_alloc, largest_partition_free, max_length, max_length);

            break;

    }

    x->latency_mode = latency_mode;
    x->impulse_length = 0;

    if ((!latency_mode && !x->time1) || (latency_mode < 2 && !x->part1) || !x->part2 || !x->part3 || fail)
    {
        zero_latency_convolve_free(x);
        return 0;
    }

    return (x);
}


t_partition_convolve *zero_latency_convolve_resize(t_zero_latency_convolve *x, AH_UIntPtr impulse_length, AH_Boolean keep_lock)
{
    t_partition_convolve *return_part = NULL;
    alloc_method allocator = largest_partition_time_alloc;
    
    x->impulse_length = 0;

    switch (x->latency_mode)
    {
        case CONVOLVE_LATENCY_ZERO:         allocator = largest_partition_time_alloc;       break;
        case CONVOLVE_LATENCY_SHORT:        allocator = largest_partition_fft1_alloc;       break;
        case CONVOLVE_LATENCY_MEDIUM:       allocator = largest_partition_fft2_alloc;       break;
    }
    
    return_part = equal_memory_swap_custom(&x->part4, allocator, largest_partition_free, impulse_length, impulse_length);
    
    if (keep_lock == false)
        unlock_memory_swap(&x->part4);

    return return_part;
}


t_convolve_error zero_latency_convolve_set(t_zero_latency_convolve *x, float *input, AH_UIntPtr impulse_length, AH_Boolean resize)
{
    t_partition_convolve *part4 = 0;
    AH_UIntPtr max_impulse;

    x->impulse_length = 0;

    // Lock first to ensure that audio finishes processing before we replace

    if (resize)
    {
        part4 = zero_latency_convolve_resize(x, impulse_length, true);
        max_impulse = impulse_length;
    }
    else
        part4 = access_memory_swap(&x->part4, &max_impulse);

    if (part4)
    {
        if (x->latency_mode < 1)
            time_domain_convolve_set(x->time1, input, impulse_length);
        if (x->latency_mode < 2)
            partition_convolve_set(x->part1, input, impulse_length);
        partition_convolve_set(x->part2, input, impulse_length);
        partition_convolve_set(x->part3, input, impulse_length);
        partition_convolve_set(part4, input, impulse_length);
    }

    x->impulse_length = impulse_length;

    unlock_memory_swap(&x->part4);

    if (impulse_length && !part4)
        return CONVOLVE_ERR_MEM_UNAVAILABLE;

    if (impulse_length > max_impulse)
        return CONVOLVE_ERR_MEM_ALLOC_TOO_SMALL;

    return CONVOLVE_ERR_NONE;
}


t_convolve_error zero_latency_convolve_reset(t_zero_latency_convolve *x)
{
    AH_UIntPtr max_impulse;
    
    // Lock first to ensure that audio finishes processing before we replace
    
    t_partition_convolve *part4 = access_memory_swap(&x->part4, &max_impulse);
    
    if (part4)
    {
        if (x->latency_mode < 1)
            time_domain_convolve_reset(x->time1);
        if (x->latency_mode < 2)
            partition_convolve_reset(x->part1);
        partition_convolve_reset(x->part2);
        partition_convolve_reset(x->part3);
        partition_convolve_reset(part4);
    }
    
    unlock_memory_swap(&x->part4);
    
    return CONVOLVE_ERR_NONE;
}


void zero_latency_convolve_process_sum(float *out, float *add, AH_UIntPtr vec_size)
{
    if ((vec_size % 4) || (((AH_UIntPtr) out) % 16) || (((AH_UIntPtr) add) % 16))
    {
        for (AH_UIntPtr i = 0; i < vec_size; i++)
            *out++ += *add++;
    }
    else
    {
        vFloat *vout = (vFloat *) out;
        vFloat *vadd = (vFloat *) add;
        
        for (AH_UIntPtr i = 0; i < (vec_size >> 2); i++, vout++)
            *vout = F32_VEC_ADD_OP(*vout, *vadd++);
    }
}


void zero_latency_convolve_process(t_zero_latency_convolve *x, float *in, float *temp, float *out, AH_UIntPtr vec_size)
{
    AH_UIntPtr max_impulse = 0;
    t_partition_convolve *part4 = attempt_memory_swap(&x->part4, &max_impulse);

    // N.B. This function DOES NOT zero the output buffer as this is done elsewhere

    if (x->impulse_length && x->impulse_length <= max_impulse && part4)
    {
        if (x->latency_mode == 0)
        {
            time_domain_convolve_process(x->time1, in, temp, vec_size);
            zero_latency_convolve_process_sum(out, temp, vec_size);
        }
        if (x->latency_mode < 2)
        {
            if (partition_convolve_process(x->part1, in, temp, vec_size) == true)
                zero_latency_convolve_process_sum(out, temp, vec_size);
        }
        if (partition_convolve_process(x->part2, in, temp, vec_size) == true)
            zero_latency_convolve_process_sum(out, temp, vec_size);
        if (partition_convolve_process(x->part3, in, temp, vec_size) == true)
            zero_latency_convolve_process_sum(out, temp, vec_size);
        if (partition_convolve_process(part4, in, temp, vec_size) == true)
            zero_latency_convolve_process_sum(out, temp, vec_size);
    }

    if (part4)
        unlock_memory_swap(&x->part4);
}
