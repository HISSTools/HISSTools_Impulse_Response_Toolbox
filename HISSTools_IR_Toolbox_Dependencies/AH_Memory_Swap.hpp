
#ifndef _AH_MEMORY_SWAP_
#define _AH_MEMORY_SWAP_

#include <ext.h>

#include <ThreadLocks.hpp>

#include <stdint.h>

#ifdef __APPLE__

#define ALIGNED_MALLOC malloc
#define ALIGNED_FREE free

#elif defined _WIN32

#include <malloc.h>

#define ALIGNED_MALLOC(x)  _aligned_malloc(x, 16)
#define ALIGNED_FREE  _aligned_free

#else

#include <malloc.h>

#define ALIGNED_MALLOC(x) aligned_alloc(16, x)
#define ALIGNED_FREE free

#endif

// All memory assignments are aligned in order that the memory is suitable for vector ops etc.
// Swap results

enum t_swap_status
{

    SWAP_NONE,
    SWAP_DONE,
    SWAP_FAILED

};


// Alloc and free routine prototypes


typedef void *(*alloc_method) (uintptr_t size, uintptr_t nom_size);
typedef void (*free_method) (void *);


// Safe memory swap structure


struct t_safe_mem_swap
{
    thread_lock lock;

    void *new_ptr;
    void *current_ptr;

    free_method new_free_method;
    free_method current_free_method;

    uintptr_t new_size;
    uintptr_t current_size;

    uintptr_t new_free;
    uintptr_t current_free;
    
    t_safe_mem_swap(uintptr_t size, uintptr_t nom_size)
    : new_ptr(nullptr)
    , current_ptr(size ? ALIGNED_MALLOC(size) : nullptr)
    , new_free_method(nullptr)
    , current_free_method(nullptr)
    , new_size(0)
    , current_size(current_ptr ? nom_size : 0)
    , new_free(0)
    , current_free(current_ptr ? 1 : 0)
    {}
    
    ~t_safe_mem_swap()
    {
        lock.acquire();
        
        if (current_free)
        {
            if (current_free_method)
                current_free_method(current_ptr);
            else
                ALIGNED_FREE(current_ptr);
        }
        
        if (new_free)
        {
            if (new_free_method)
                new_free_method(new_ptr);
            else
                ALIGNED_FREE(new_ptr);
        }
        
        current_ptr = new_ptr = nullptr;
        current_size = new_size = new_free = current_free = 0;
        new_free_method = current_free_method = nullptr;
        
        lock.release();
    }
};


// Internal deferred routine for freeing memory

static void free_temp_mem_swap(t_object *x, t_symbol *s, short argc, t_atom *argv)
{
    free_method free_method_ptr = nullptr;

    if (argc)
        free_method_ptr = (free_method) atom_getsym(argv);

    if (free_method_ptr)
        free_method_ptr((void *) s);
    else
        ALIGNED_FREE((void *)s);
}


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


// Alloc / Free / Clear


// Alloc - You should only call this from the new routine to be thread-safe

static inline void alloc_mem_swap(t_safe_mem_swap *mem_struct, uintptr_t size, uintptr_t nom_size)
{
    new (mem_struct) t_safe_mem_swap(size, nom_size);
}


// Free - frees the memory immediately which is only safe in the object free routine - use clear if you want to clear the memory safely elsewhere

static inline void free_mem_swap(t_safe_mem_swap *mem_struct)
{
    mem_struct->~t_safe_mem_swap();
}


// Clear - this routine defers the freeing of memory so it should be thread-safe

static inline void clear_mem_swap(t_safe_mem_swap *mem_struct)
{
    t_atom method_ptr;

    mem_struct->lock.acquire();

    if (mem_struct->current_free)
    {
        atom_setsym(&method_ptr,(t_symbol *) mem_struct->current_free_method);
        defer_low(0, (method)free_temp_mem_swap, (t_symbol *) mem_struct->current_ptr, 1, &method_ptr);
    }

    if (mem_struct->new_free)
    {
        atom_setsym(&method_ptr,(t_symbol *) mem_struct->new_free_method);
        defer_low(0, (method)free_temp_mem_swap, (t_symbol *) mem_struct->new_ptr, 1, &method_ptr);
    }

    mem_struct->current_ptr = nullptr;
    mem_struct->current_size = 0;
    mem_struct->current_free = 0;
    mem_struct->current_free_method = nullptr;

    mem_struct->new_ptr = nullptr;
    mem_struct->new_size = 0;
    mem_struct->new_free = 0;
    mem_struct->new_free_method = nullptr;

    mem_struct->lock.release();
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


// Access and attempted swap routines - these routines are intended to pick up memory, but not allocate it


// Access - this routine will lock to get access to the memory struct and safely return the most recent useable values (either current or swap - BUT does not perform a swap)
// This routine can be used in any thread, and must be used if you are using grow_mem_swap to swap the memory anywhere in your code

static inline void *access_mem_swap(t_safe_mem_swap *mem_struct, uintptr_t *nom_size)
{
    void *return_ptr;

    mem_struct->lock.acquire();

    if (mem_struct->new_ptr)
    {
        *nom_size = mem_struct->new_size;
        return_ptr = mem_struct->new_ptr;
    }
    else
    {
        *nom_size = mem_struct->current_size;
        return_ptr = mem_struct->current_ptr;
    }

    mem_struct->lock.release();

    return return_ptr;
}


// Attempt - This non-blocking routine will attempt to swap the pointers but fail if the pointer is being altered
// Use this in one thread only (typically the audio thread)
// Returns 0 on no swap, 1 on swap and -1 on swap failed

static inline t_swap_status attempt_mem_swap(t_safe_mem_swap *mem_struct)
{
    t_atom method_ptr;

    t_swap_status swap = SWAP_NONE;

    if (mem_struct->lock.attempt())
    {
        if (mem_struct->new_ptr)
        {
            if (mem_struct->current_free)
            {
                atom_setsym(&method_ptr,(t_symbol *) mem_struct->current_free_method);
                defer_low(0, (method)free_temp_mem_swap, (t_symbol *) mem_struct->current_ptr, 1, &method_ptr);
            }

            // Swap

            mem_struct->current_ptr         = mem_struct->new_ptr;
            mem_struct->current_size        = mem_struct->new_size;
            mem_struct->current_free        = mem_struct->new_free;
            mem_struct->current_free_method = mem_struct->new_free_method;

            // Zero

            mem_struct->new_ptr         = nullptr;
            mem_struct->new_size        = 0;
            mem_struct->new_free        = 0;
            mem_struct->new_free_method = nullptr;

            swap = SWAP_DONE;
        }

        mem_struct->lock.release();
    }
    else
        swap = SWAP_FAILED;

    return swap;
}


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


// Re-allocation routines - these routines allow threadsafe re-allocation of memory


// Schedule Swap Pointer - this routine will lock to get access to the memory struct and place a given ptr and nominal size in the new slots
// This pointer will *NOT* be freed by the mem_swap routines
// This routine should only be called from max threads

static inline void schedule_swap_mem_swap(t_safe_mem_swap *mem_struct, void *ptr, uintptr_t nom_size)
{
    t_atom method_ptr;

    // Spin on the lock

    mem_struct->lock.acquire();

    if (mem_struct->new_free)
    {
        atom_setsym(&method_ptr,(t_symbol *) mem_struct->new_free_method);
        defer_low(0, (method)free_temp_mem_swap, (t_symbol *) mem_struct->new_ptr, 1, &method_ptr);
    }

    mem_struct->new_ptr = ptr;
    mem_struct->new_size = nom_size;
    mem_struct->new_free = 0;
    mem_struct->new_free_method = nullptr;

    mem_struct->lock.release();
}


// Schedule Grow - this routine will lock to get access to the memory struct and allocate new memory if required, to be swapped in later
// This routine should only be called from max threads

static inline void *schedule_grow_mem_swap(t_safe_mem_swap *mem_struct,  uintptr_t size, uintptr_t nom_size)
{
    t_atom method_ptr;

    void *return_ptr;

    // Spin on the lock

    mem_struct->lock.acquire();

    if ((mem_struct->current_size < nom_size) && (mem_struct->new_size < nom_size))
    {
        if (mem_struct->new_free)
        {
            atom_setsym(&method_ptr,(t_symbol *) mem_struct->new_free_method);
            defer_low(0, (method)free_temp_mem_swap, (t_symbol *) mem_struct->new_ptr, 1, &method_ptr);
        }

        mem_struct->new_ptr = return_ptr = ALIGNED_MALLOC(size);
        mem_struct->new_size = return_ptr ? nom_size: 0;
        mem_struct->new_free = 1;
        mem_struct->new_free_method = nullptr;
    }
    else
    {
        if (mem_struct->new_size >= nom_size)
            return_ptr = mem_struct->new_ptr;
        else
            return_ptr = mem_struct->current_ptr;
    }

    mem_struct->lock.release();

    return return_ptr;
}


// Schedule Equal - This routine will lock to get access to the memory struct and allocate new memory unless the sizes are equal, placing the memory in the new slots
// This routine should only be called from max threads

static inline void *schedule_equal_mem_swap(t_safe_mem_swap *mem_struct,  uintptr_t size, uintptr_t nom_size)
{
    t_atom method_ptr;

    void *return_ptr;

    mem_struct->lock.acquire();

    if (mem_struct->current_size == nom_size)
    {
        if (mem_struct->new_free)
        {
            atom_setsym(&method_ptr,(t_symbol *) mem_struct->new_free_method);
            defer_low(0, (method)free_temp_mem_swap, (t_symbol *) mem_struct->new_ptr, 1, &method_ptr);
        }

        mem_struct->new_ptr = nullptr;
        mem_struct->new_size = 0;
        mem_struct->new_free = 0;
        mem_struct->new_free_method = nullptr;

        return_ptr = mem_struct->current_ptr;
    }
    else
    {
        if (mem_struct->new_size != nom_size)
        {
            if (mem_struct->new_free)
            {
                atom_setsym(&method_ptr,(t_symbol *) mem_struct->new_free_method);
                defer_low(0, (method)free_temp_mem_swap, (t_symbol *) mem_struct->new_ptr, 1, &method_ptr);
            }

            mem_struct->new_ptr = return_ptr = ALIGNED_MALLOC(size);
            mem_struct->new_size = return_ptr ? nom_size: 0;
            mem_struct->new_free = 1;
            mem_struct->new_free_method = nullptr;
        }
        else
            return_ptr = mem_struct->new_ptr;
    }

    mem_struct->lock.release();

    return return_ptr;
}


// Grow Immediate - this routine will lock to get access to the memory struct, and if necessary free the old memory and assign new memory - swapping pointers immediately
// This routine should only be called from max threads and must be used in combination with safe access (access_mem_swap)

static inline void *grow_mem_swap(t_safe_mem_swap *mem_struct,  uintptr_t size, uintptr_t nom_size)
{
    t_atom method_ptr;

    void *return_ptr;

    mem_struct->lock.acquire();

    if (mem_struct->current_size < nom_size)
    {
        if (mem_struct->current_free)
        {
            atom_setsym(&method_ptr,(t_symbol *) mem_struct->current_free_method);
            defer_low(0, (method)free_temp_mem_swap, (t_symbol *) mem_struct->current_ptr, 1, &method_ptr);
        }

        if (mem_struct->new_size < nom_size)
        {
            if (mem_struct->new_free)
            {
                atom_setsym(&method_ptr,(t_symbol *) mem_struct->new_free_method);
                defer_low(0, (method)free_temp_mem_swap, (t_symbol *) mem_struct->new_ptr, 1, &method_ptr);
            }

            mem_struct->current_ptr = ALIGNED_MALLOC(size);
            mem_struct->current_size = mem_struct->current_ptr ? nom_size : 0;
            mem_struct->current_free = 1;
            mem_struct->current_free_method = nullptr;
        }
        else
        {
            mem_struct->current_ptr = mem_struct->new_ptr;
            mem_struct->current_size = mem_struct->new_size;
            mem_struct->current_free = mem_struct->new_free;
            mem_struct->current_free_method = mem_struct->new_free_method;
        }
    }

    mem_struct->new_ptr = nullptr;
    mem_struct->new_size = 0;
    mem_struct->new_free = 0;
    mem_struct->new_free_method = nullptr;

    return_ptr = mem_struct->current_ptr;

    mem_struct->lock.release();

    return return_ptr;
}


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


// Custom allocation and reallocation functions

// This routine will lock to get access to the memory struct and allocate new memory unless the sizes are equal
// This routine should only be called from max threads

static inline void *schedule_equal_mem_swap_custom (t_safe_mem_swap *mem_struct, alloc_method alloc_method_ptr, free_method free_method_ptr,  uintptr_t size, uintptr_t nom_size)
{
    t_atom method_ptr;

    void *return_ptr;

    mem_struct->lock.acquire();

    if (mem_struct->current_size == nom_size)
    {
        if (mem_struct->new_free)
        {
            atom_setsym(&method_ptr,(t_symbol *) mem_struct->new_free_method);
            defer_low(0, (method)free_temp_mem_swap, (t_symbol *) mem_struct->new_ptr, 1, &method_ptr);
        }

        mem_struct->new_ptr = nullptr;
        mem_struct->new_size = 0;
        mem_struct->new_free = 0;
        mem_struct->new_free_method = nullptr;

        return_ptr = mem_struct->current_ptr;
    }
    else
    {
        if (mem_struct->new_size != nom_size)
        {
            if (mem_struct->new_free)
            {
                atom_setsym(&method_ptr,(t_symbol *) mem_struct->new_free_method);
                defer_low(0, (method)free_temp_mem_swap, (t_symbol *) mem_struct->new_ptr, 1, &method_ptr);
            }

            mem_struct->new_ptr = return_ptr = alloc_method_ptr(size, nom_size);
            mem_struct->new_size = return_ptr ? nom_size: 0;
            mem_struct->new_free = 1;
            mem_struct->new_free_method = free_method_ptr;
        }
        else
            return_ptr = mem_struct->new_ptr;
    }

    mem_struct->lock.release();

    return return_ptr;
}

#endif        /* _AH_MEMORY_SWAP_ */
