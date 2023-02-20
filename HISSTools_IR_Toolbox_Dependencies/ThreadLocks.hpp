
#ifndef THREADLOCKS_HPP
#define THREADLOCKS_HPP

#include <atomic>
#include <algorithm>
#include <chrono>
#include <thread>

#ifdef __linux__

// Linux specific definitions

namespace OS_Specific
{
    inline void thread_nano_sleep()
    {
        std::this_thread::sleep_for(std::chrono::nanoseconds(100));
    }
}

#elif defined(__APPLE__)

// OSX specific definitions

namespace OS_Specific
{
    inline void thread_nano_sleep()
    {
        std::this_thread::sleep_for(std::chrono::nanoseconds(100));
    }
}

#else

// Windows OS specific definitions

#include <windows.h>

namespace OS_Specific
{
    inline void thread_nano_sleep()
    {
        SwitchToThread();
    }
}

#endif


class thread_lock
{
    using Clock = std::chrono::steady_clock;
    
public:
    
    thread_lock() {}
    ~thread_lock() { acquire(); }
    
    // Non-copyable
    
    thread_lock(const thread_lock&) = delete;
    thread_lock& operator=(const thread_lock&) = delete;
    
    void acquire()
    {
        for (int i = 0; i < 10; i++)
            if (attempt())
                return;
        
        auto timeOut = Clock::now() + std::chrono::nanoseconds(10000);
        
        while (Clock::now() < timeOut)
            if (attempt())
                return;
        
        while (!attempt())
            OS_Specific::thread_nano_sleep();
    }
    
    bool attempt() { return !m_atomic_lock.test_and_set(); }
    void release() { m_atomic_lock.clear(); }
    
private:
    
    std::atomic_flag m_atomic_lock = ATOMIC_FLAG_INIT;
};


// A generic lock holder using RAII

template <class T, void (T::*acquire_method)(), void (T::*release_method)()>
class lock_hold
{
public:
    
    lock_hold() : m_lock(nullptr) {}
    lock_hold(thread_lock *lock) : m_lock(lock) { if (m_lock) m_lock->*acquire_method(); }
    ~lock_hold() { if (m_lock) m_lock->release(); }
    
    // Non-copyable
    
    lock_hold(const lock_hold&) = delete;
    lock_hold& operator=(const lock_hold&) = delete;
    
    void release()
    {
        if (m_lock)
        {
            (m_lock->*release_method)();
            m_lock = nullptr;
        }
    }
    
private:
    
    thread_lock *m_lock;
};

#endif /* THREADLOCKS_HPP */
