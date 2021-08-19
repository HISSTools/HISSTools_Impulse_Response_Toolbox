
#pragma once

#include <cstdint>
#include <cstdlib>
#include <atomic>
#include <functional>

// FIX - locks around memory assignment - can be made more efficient by avoiding this at which point spinlocks will be justified....
// Follow the HISSTools C++ design for this....  use separate freeing locks so the memory is always freed in the assignment thread
// All memory assignments are aligned in order that the memory is suitable for vector ops etc.

template<class T>
class MemorySwap
{
    
public:
    
    // Alloc and free routine prototypes
    
    typedef std::function<T *(uintptr_t size)> AllocFunc;
    typedef std::function<void (T *) > FreeFunc;
    
    class Ptr
    {
        friend MemorySwap;
        
    public:
        
        Ptr(Ptr&& p)
        : mOwner(p.mOwner), mPtr(p.mPtr), mSize(p.mSize)
        {
            p.mOwner = nullptr;
            p.mPtr = nullptr;
            p.mSize = 0;
        }
        
        ~Ptr() { clear(); }
        
        void clear()
        {
            if (mOwner) mOwner->unlock();
            mOwner = nullptr;
            mPtr = nullptr;
            mSize = 0;
        }
        
        void swap(T *ptr, uintptr_t size)
        {
            update(&MemorySwap::set, ptr, size, nullptr);
        }
        
        void grow(uintptr_t size)
        {
            grow(&allocate, &deallocate, size);
        }
        
        void equal(uintptr_t size)
        {
            equal(&allocate, &deallocate, size);
        }
        
        void grow(AllocFunc allocFunction, FreeFunc freeFunction, uintptr_t size)
        {
            updateAllocateIf(allocFunction, freeFunction, size, std::greater<uintptr_t>());
        }
        
        void equal(AllocFunc allocFunction, FreeFunc freeFunction, uintptr_t size)
        {
            updateAllocateIf(allocFunction, freeFunction, size, std::not_equal_to<uintptr_t>());
        }
        
        T *get() { return mPtr; }
        uintptr_t getSize() { return mSize; }
        
    private:
        
        Ptr()
        : mOwner(nullptr), mPtr(nullptr), mSize(0)
        {}
        
        Ptr(MemorySwap *owner)
        : mOwner(owner), mPtr(mOwner ? mOwner->mPtr : nullptr), mSize(mOwner ? mOwner->mSize : 0)
        {}
        
        Ptr(const Ptr& p) = delete;
        Ptr operator = (const Ptr& p) = delete;
        
        template<typename Op>
        void updateAllocateIf(AllocFunc allocFunction, FreeFunc freeFunction, uintptr_t size, Op op)
        {
            update(&MemorySwap::allocateIfLockHeld<Op>, allocFunction, freeFunction, size, op);
        }
        
        template<typename Op, typename ...Args>
        void update(Op op, Args...args)
        {
            if (mOwner)
            {
                (mOwner->*op)(args...);
                mPtr = mOwner->mPtr;
                mSize = mOwner->mSize;
            }
        }
        
        MemorySwap *mOwner;
        T *mPtr;
        uintptr_t mSize;
    };
    
    // Constructor (standard allocation)
    
    MemorySwap(uintptr_t size)
    : mLock(false), mPtr(nullptr), mSize(0), mFreeFunction(nullptr)
    {
        if (size)
            set(allocate(size), size, &deallocate);
    }
    
    // Constructor (custom allocation)
    
    MemorySwap(AllocFunc allocFunction, FreeFunc freeFunction, uintptr_t size)
    : mLock(false), mPtr(nullptr), mSize(0), mFreeFunction(nullptr)
    {
        if (size)
            set(allocFunction(size), size, freeFunction);
    }
    
    ~MemorySwap()
    {
        clear();
    }
    
    MemorySwap(const MemorySwap&) = delete;
    MemorySwap& operator = (const MemorySwap&) = delete;

    MemorySwap(MemorySwap&& obj)
    : mLock(false), mPtr(nullptr), mSize(0), mFreeFunction(nullptr)
    {
        *this = std::move(obj);
        obj.mPtr = nullptr;
        obj.mFreeFunction = nullptr;
    }
    
    MemorySwap& operator = (MemorySwap&& obj)
    {
        clear();
        obj.lock();
        mPtr = obj.mPtr;
        mSize = obj.mSize;
        mFreeFunction = obj.mFreeFunction;
        obj.mPtr = nullptr;
        obj.mFreeFunction = nullptr;
        obj.unlock();
        
        return *this;
    }
    
    // frees the memory immediately
    
    void clear()
    {
        swap(nullptr, 0);
    }
    
    // lock to get access to the memory struct and safely return the pointer
    
    Ptr access()
    {
        lock();
        return Ptr(this);
    }
    
    // This non-blocking routine will attempt to get the pointer but fail if the pointer is being  accessed in another thread
    
    Ptr attempt()
    {
        return tryLock() ? Ptr(this) : Ptr();
    }
    
    Ptr swap(T *ptr, uintptr_t size)
    {
        lock();
        set(ptr, size, nullptr);
        return Ptr(this);
    }
    
    Ptr grow(uintptr_t size)
    {
        return grow(&allocate, &deallocate, size);
    }
    
    Ptr equal(uintptr_t size)
    {
        return equal(&allocate, &deallocate, size);
    }
    
    Ptr grow(AllocFunc allocFunction, FreeFunc freeFunction, uintptr_t size)
    {
        return allocateIf(allocFunction, freeFunction, size, std::greater<uintptr_t>());
    }
    
    Ptr equal(AllocFunc allocFunction, FreeFunc freeFunction, uintptr_t size)
    {
        return allocateIf(allocFunction, freeFunction, size, std::not_equal_to<uintptr_t>());
    }
    
private:
    
    template<typename Op>
    Ptr allocateIf(AllocFunc allocFunction, FreeFunc freeFunction, uintptr_t size, Op op)
    {
        lock();
        allocateIfLockHeld(allocFunction, freeFunction, size, op);
        return Ptr(this);
    }
    
    template<typename Op>
    void allocateIfLockHeld(AllocFunc allocFunction, FreeFunc freeFunction, uintptr_t size, Op op)
    {
        if (op(size, mSize))
            set(allocFunction(size), size, freeFunction);
    }
    
    void set(T *ptr, uintptr_t size, FreeFunc freeFunction)
    {
        if (mFreeFunction)
            mFreeFunction(mPtr);
        
        mPtr = ptr;
        mSize = ptr ? size : 0;
        mFreeFunction = freeFunction;
    }
    
    bool tryLock()
    {
        bool expected = false;
        return mLock.compare_exchange_strong(expected, true);
    }
    
    void lock()
    {
        bool expected = false;
        while (!mLock.compare_exchange_weak(expected, true));
    }
    
    void unlock()
    {
        bool expected = true;
        mLock.compare_exchange_strong(expected, false);
    }
    
#ifdef __APPLE__
    static T* allocate(size_t size)
    {
        return reinterpret_cast<T *>(malloc(size * sizeof(T)));
    }
#else
    static T* allocate(size_t size)
    {
        return reinterpret_cast<T *>(aligned_alloc(16, size * sizeof(T)));
    }
#endif

    static void deallocate(T *ptr)
    {
        free(ptr);
    }
    
    std::atomic<bool> mLock;
    
    T *mPtr;
    uintptr_t mSize;
    FreeFunc mFreeFunction;
};
