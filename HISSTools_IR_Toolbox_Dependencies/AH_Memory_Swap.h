
#ifndef _AH_MEMORY_SWAP_
#define _AH_MEMORY_SWAP_ 

#include <ext.h>

#include "AH_Atomic.h"
#include "AH_Types.h"
#include "AH_VectorOps.h"


// All memory assignments are aligned in order that the memory is suitable for vector ops etc.
// Swap results

typedef enum
{
	
	SWAP_NONE,
	SWAP_DONE,
	SWAP_FAILED
	
} t_swap_status;


// Alloc and free routine prototypes


typedef void *(*alloc_method) (AH_UIntPtr size, AH_UIntPtr nom_size);
typedef void (*free_method) (void *);


// Safe memory swap structure


typedef struct _safe_mem_swap
{
	t_int32_atomic lock;
	
	void *new_ptr;
	void *current_ptr;
	
	free_method new_free_method;
	free_method current_free_method;
	
	AH_UIntPtr new_size;
	AH_UIntPtr current_size;
	
	AH_UIntPtr new_free;
	AH_UIntPtr current_free;
	
} t_safe_mem_swap;


// Internal deferred routine for freeing memory

static void free_temp_mem_swap (t_object *x, t_symbol *s, short argc, t_atom *argv)
{
	free_method free_method_ptr = 0;
		
	if (argc)
		free_method_ptr = (free_method) atom_getsym(argv);
		
	if (free_method_ptr)
		free_method_ptr((void *) s);
	else 
		ALIGNED_FREE((void *)s);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Alloc / Free / Clear


// Alloc - You should only call this from the new routine to be thread-safe

static __inline long alloc_mem_swap (t_safe_mem_swap *mem_struct, AH_UIntPtr size, AH_UIntPtr nom_size)
{
	long fail = 0;
	
	mem_struct-> lock = 0;
	
	if (size)
		mem_struct->current_ptr = ALIGNED_MALLOC(size);
	else
		mem_struct->current_ptr = 0;

	if (size && mem_struct->current_ptr)
	{
		mem_struct->current_size = nom_size;
		mem_struct->current_free = 1;
		mem_struct->current_free_method = 0;
	}
	else 
	{
		mem_struct->current_size = 0;
		mem_struct->current_free = 0;
		mem_struct->current_free_method = 0;
		
		if (size) 
			fail = 1;
	}

	mem_struct->new_ptr = 0;
	mem_struct->new_size = 0;
	mem_struct->new_free = 0;
	mem_struct->new_free_method = 0;
	
	return fail;
}


// Free - frees the memory immediately which is only safe in the object free routine - use clear if you want to clear the memory safely elsewhere

static __inline void free_mem_swap (t_safe_mem_swap *mem_struct)
{
	// Spin on the lock
	
	while (!Atomic_Compare_And_Swap_Barrier(0, 1, &mem_struct-> lock));
	
	if (mem_struct->current_free)
	{
		if (mem_struct->current_free_method)
			mem_struct->current_free_method(mem_struct->current_ptr);
		else
			ALIGNED_FREE(mem_struct->current_ptr);
	}

	
	if (mem_struct->new_free)
	{
		if (mem_struct->new_free_method)
			mem_struct->new_free_method(mem_struct->new_ptr);
		else
			ALIGNED_FREE(mem_struct->new_ptr);
	}
		
	mem_struct->current_ptr = 0;
	mem_struct->current_size = 0;
	mem_struct->current_free = 0;
	mem_struct->current_free_method = 0;
	
	mem_struct->new_ptr = 0;
	mem_struct->new_size = 0;
	mem_struct->new_free = 0;
	mem_struct->new_free_method = 0;
	
	// This should never fail as this thread has the lock 
	
	Atomic_Compare_And_Swap_Barrier(1, 0, &mem_struct-> lock);
}


// Clear - this routine defers the freeing of memory so it should be thread-safe

static __inline void clear_mem_swap (t_safe_mem_swap *mem_struct)
{
	t_atom method_ptr;
	
	// Spin on the lock
	
	while (!Atomic_Compare_And_Swap_Barrier(0, 1, &mem_struct-> lock));
	
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
	
	mem_struct->current_ptr = 0;
	mem_struct->current_size = 0;
	mem_struct->current_free = 0;
	mem_struct->current_free_method = 0;
	
	mem_struct->new_ptr = 0;
	mem_struct->new_size = 0;
	mem_struct->new_free = 0;
	mem_struct->new_free_method = 0;
	
	// This should never fail as this thread has the lock 
	
	Atomic_Compare_And_Swap_Barrier(1, 0, &mem_struct-> lock);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Access and attempted swap routines - these routines are intended to pick up memory, but not allocate it


// Access - this routine will lock to get access to the memory struct and safely return the most recent useable values (either current or swap - BUT does not perform a swap)
// This routine can be used in any thread, and must be used if you are using grow_mem_swap to swap the memory anywhere in your code

static __inline void *access_mem_swap (t_safe_mem_swap *mem_struct, AH_UIntPtr *nom_size)
{
	void *return_ptr;
	
	// Spin on the lock
	
	while (!Atomic_Compare_And_Swap_Barrier(0, 1, &mem_struct-> lock));
	
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
	
	// This should never fail as this thread has the lock 
	
	Atomic_Compare_And_Swap_Barrier(1, 0, &mem_struct-> lock);
	
	return return_ptr;	
}


// Attempt - This non-blocking routine will attempt to swap the pointers but fail if the pointer is being altered
// Use this in one thread only (typically the audio thread)
// Returns 0 on no swap, 1 on swap and -1 on swap failed

static __inline t_swap_status attempt_mem_swap (t_safe_mem_swap *mem_struct)
{
	t_atom method_ptr; 
	
	t_swap_status swap = SWAP_NONE;
	
	if (Atomic_Compare_And_Swap_Barrier(0, 1, &mem_struct->lock))
	{
		if (mem_struct->new_ptr)
		{
			if (mem_struct->current_free)
			{
				atom_setsym(&method_ptr,(t_symbol *) mem_struct->current_free_method);
				defer_low(0, (method)free_temp_mem_swap, (t_symbol *) mem_struct->current_ptr, 1, &method_ptr);
			}
		
			// Swap
			
			mem_struct->current_ptr				= mem_struct->new_ptr;
			mem_struct->current_size			= mem_struct->new_size;
			mem_struct->current_free			= mem_struct->new_free;
			mem_struct->current_free_method		= mem_struct->new_free_method;
			
			// Zero
			
			mem_struct->new_ptr				= 0;
			mem_struct->new_size			= 0;
			mem_struct->new_free			= 0;
			mem_struct->new_free_method		= 0;
			
			swap = SWAP_DONE;
		}
		
		// This should never fail as this thread has the lock 
		
		Atomic_Compare_And_Swap_Barrier(1, 0, &mem_struct-> lock);
	}
	else
		swap = SWAP_FAILED;
	
	return swap;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Re-allocation routines - these routines allow threadsafe re-allocation of memory


// Schedule Swap Pointer - this routine will lock to get access to the memory struct and place a given ptr and nominal size in the new slots
// This pointer will *NOT* be freed by the mem_swap routines
// This routine should only be called from max threads

static __inline void schedule_swap_mem_swap (t_safe_mem_swap *mem_struct, void *ptr, AH_UIntPtr nom_size)
{
	t_atom method_ptr;
	
	// Spin on the lock
	
	while (!Atomic_Compare_And_Swap_Barrier(0, 1, &mem_struct-> lock));
	
	if (mem_struct->new_free)
	{
		atom_setsym(&method_ptr,(t_symbol *) mem_struct->new_free_method);
		defer_low(0, (method)free_temp_mem_swap, (t_symbol *) mem_struct->new_ptr, 1, &method_ptr);
	}
		
	mem_struct->new_ptr = ptr;
	mem_struct->new_size = nom_size;
	mem_struct->new_free = 0;
	mem_struct->new_free_method = 0;
	
	// This should never fail as this thread has the lock 
	
	Atomic_Compare_And_Swap_Barrier(1, 0, &mem_struct-> lock);
}


// Schedule Grow - this routine will lock to get access to the memory struct and allocate new memory if required, to be swapped in later
// This routine should only be called from max threads

static __inline void *schedule_grow_mem_swap (t_safe_mem_swap *mem_struct,  AH_UIntPtr size, AH_UIntPtr nom_size)
{
	t_atom method_ptr;
	
	void *return_ptr;
	
	// Spin on the lock
	
	while (!Atomic_Compare_And_Swap_Barrier(0, 1, &mem_struct-> lock));

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
		mem_struct->new_free_method = 0;
	}
	else 
	{
		if (mem_struct->new_size >= nom_size)
			return_ptr = mem_struct->new_ptr;
		else 
			return_ptr = mem_struct->current_ptr;
	}

	// This should never fail as this thread has the lock 
	
	Atomic_Compare_And_Swap_Barrier(1, 0, &mem_struct-> lock);
	
	return return_ptr;
}


// Schedule Equal - This routine will lock to get access to the memory struct and allocate new memory unless the sizes are equal, placing the memory in the new slots
// This routine should only be called from max threads

static __inline void *schedule_equal_mem_swap (t_safe_mem_swap *mem_struct,  AH_UIntPtr size, AH_UIntPtr nom_size)
{
	t_atom method_ptr;
	
	void *return_ptr; 
	
	// Spin on the lock
	
	while (!Atomic_Compare_And_Swap_Barrier(0, 1, &mem_struct-> lock));
	
	if (mem_struct->current_size == nom_size)
	{
		if (mem_struct->new_free)
		{
			atom_setsym(&method_ptr,(t_symbol *) mem_struct->new_free_method);
			defer_low(0, (method)free_temp_mem_swap, (t_symbol *) mem_struct->new_ptr, 1, &method_ptr);
		}
		
		mem_struct->new_ptr = 0;
		mem_struct->new_size = 0;
		mem_struct->new_free = 0;
		mem_struct->new_free_method = 0;

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
			mem_struct->new_free_method = 0;
		}
		else
			return_ptr = mem_struct->new_ptr;
	}
	
	// This should never fail as this thread has the lock 
	
	Atomic_Compare_And_Swap_Barrier(1, 0, &mem_struct-> lock);
	
	return return_ptr;
}


// Grow Immediate - this routine will lock to get access to the memory struct, and if necessary free the old memory and assign new memory - swapping pointers immediately
// This routine should only be called from max threads and must be used in combination with safe access (access_mem_swap)

static __inline void *grow_mem_swap (t_safe_mem_swap *mem_struct,  AH_UIntPtr size, AH_UIntPtr nom_size)
{
	t_atom method_ptr;
	
	void *return_ptr; 
	
	// Spin on the lock
	
	while (!Atomic_Compare_And_Swap_Barrier(0, 1, &mem_struct-> lock));
	
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
			mem_struct->current_free_method = 0;
		}
		else
		{
			mem_struct->current_ptr = mem_struct->new_ptr;
			mem_struct->current_size = mem_struct->new_size;
			mem_struct->current_free = mem_struct->new_free;
			mem_struct->current_free_method = mem_struct->new_free_method;
		}
	}
	
	mem_struct->new_ptr = 0;
	mem_struct->new_size = 0;
	mem_struct->new_free = 0;
	mem_struct->new_free_method = 0;
	
	return_ptr = mem_struct->current_ptr;
	
	// This should never fail as this thread has the lock 
	
	Atomic_Compare_And_Swap_Barrier(1, 0, &mem_struct-> lock);
	
	return return_ptr;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Custom allocation and reallocation functions


// Alloc Custom - you should only call this from the new routine to be thread-safe

static __inline long alloc_mem_swap_custom (t_safe_mem_swap *mem_struct, alloc_method alloc_method_ptr, free_method free_method_ptr, AH_UIntPtr size, AH_UIntPtr nom_size)
{
	long fail = 0;
	
	mem_struct-> lock = 0;
	
	if (size)
		mem_struct->current_ptr = alloc_method_ptr(size, nom_size);
	else
		mem_struct->current_ptr = 0;
	
	if (size && mem_struct->current_ptr)
	{
		mem_struct->current_size = nom_size;
		mem_struct->current_free = 1;
		mem_struct->current_free_method = free_method_ptr;
	}
	else 
	{
		mem_struct->current_size = 0;
		mem_struct->current_free = 0;
		mem_struct->current_free_method = 0;
		
		if (size) 
			fail = 1;
	}
	
	mem_struct->new_ptr = 0;
	mem_struct->new_size = 0;
	mem_struct->new_free = 0;
	mem_struct->new_free_method = 0;
	
	return fail;
}


// This routine will lock to get access to the memory struct and allocate new memory if required
// This routine should only be called from max threads

static __inline void *schedule_grow_mem_swap_custom (t_safe_mem_swap *mem_struct, alloc_method alloc_method_ptr, free_method free_method_ptr, AH_UIntPtr size, AH_UIntPtr nom_size)
{
	t_atom method_ptr;
	
	void *return_ptr;
	
	// Spin on the lock
	
	while (!Atomic_Compare_And_Swap_Barrier(0, 1, &mem_struct-> lock));
	
	if ((mem_struct->current_size < nom_size) && (mem_struct->new_size < nom_size))
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
	{
		if (mem_struct->new_size >= nom_size)
			return_ptr = mem_struct->new_ptr;
		else 
			return_ptr = mem_struct->current_ptr;
	}
	
	// This should never fail as this thread has the lock 
	
	Atomic_Compare_And_Swap_Barrier(1, 0, &mem_struct-> lock);
	
	return return_ptr;
}


// This routine will lock to get access to the memory struct and allocate new memory unless the sizes are equal
// This routine should only be called from max threads

static __inline void *schedule_equal_mem_swap_custom (t_safe_mem_swap *mem_struct, alloc_method alloc_method_ptr, free_method free_method_ptr,  AH_UIntPtr size, AH_UIntPtr nom_size)
{
	t_atom method_ptr;
	
	void *return_ptr; 
	
	// Spin on the lock
	
	while (!Atomic_Compare_And_Swap_Barrier(0, 1, &mem_struct-> lock));
	
	if (mem_struct->current_size == nom_size)
	{
		if (mem_struct->new_free)
		{
			atom_setsym(&method_ptr,(t_symbol *) mem_struct->new_free_method);
			defer_low(0, (method)free_temp_mem_swap, (t_symbol *) mem_struct->new_ptr, 1, &method_ptr);
		}

		mem_struct->new_ptr = 0;
		mem_struct->new_size = 0;
		mem_struct->new_free = 0;
		mem_struct->new_free_method = 0;
		
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
	
	// This should never fail as this thread has the lock 
	
	Atomic_Compare_And_Swap_Barrier(1, 0, &mem_struct-> lock);
	
	return return_ptr;
}


// This routine will lock to get access to the memory struct, freeing the old memory and assigning new memory
// This routine should only be called from max threads and must be used in combination with safe access (access_mem_swap)

static __inline void *grow_mem_swap_custom (t_safe_mem_swap *mem_struct, alloc_method alloc_method_ptr, free_method free_method_ptr, AH_UIntPtr size, AH_UIntPtr nom_size)
{
	t_atom method_ptr;
	
	void *return_ptr; 
	
	// Spin on the lock
	
	while (!Atomic_Compare_And_Swap_Barrier(0, 1, &mem_struct-> lock));
	
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
			
			mem_struct->current_ptr = alloc_method_ptr(size, nom_size);
			mem_struct->current_size = mem_struct->current_ptr ? nom_size : 0;
			mem_struct->current_free = 1;
			mem_struct->current_free_method = free_method_ptr;
		}
		else
		{
			mem_struct->current_ptr = mem_struct->new_ptr;
			mem_struct->current_size = mem_struct->new_size;
			mem_struct->current_free = mem_struct->new_free;
			mem_struct->current_free_method = mem_struct->new_free_method;
		}
	}
	
	mem_struct->new_ptr = 0;
	mem_struct->new_size = 0;
	mem_struct->new_free = 0;
	mem_struct->new_free_method = 0;
	
	return_ptr = mem_struct->current_ptr;
	
	// This should never fail as this thread has the lock 
	
	Atomic_Compare_And_Swap_Barrier(1, 0, &mem_struct-> lock);
	
	return return_ptr;
}




#endif		/* _AH_MEMORY_SWAP_	*/
