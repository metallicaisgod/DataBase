///////////////////////////////////////////////////////////////////////////////
// thread.h
// ~~~~~~~~
// The thread-management library definitions.
// Supported platforms:
// - Win32 (WIN32 macro must be defined).
// - MacOS - removed by Kola
//
// Author:      Alexey Zhukov (alexz)
// Created:     06-Dec-1999   (alexz)
// Modified:    15-Feb-2000   (alexz)
//
// Copyright (C) 1999-2000 Maris Multimedia Ltd. All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////
#ifndef __thread_h__
#define __thread_h__
#pragma once

// Static link!
//#define THREAD_STATIC

//
// Import / export definitions.
//

//#ifdef THREAD_STATIC
// Static library build.
#define THREADAPI
//#else
//// Dynamic library build.
//#ifdef THREAD_EXPORTS
//#define THREADAPI  __declspec(dllexport)
//#else
//#define THREADAPI  __declspec(dllimport)
//#endif // THREAD_EXPORTS
//#endif // THREAD_STATIC


namespace CDICommon
{
    //
    // Platform-dependent types.
    //

    typedef void*               THREAD_TYPE;    // HANDLE
    typedef void*               SEMAPHORE_TYPE; // HANDLE
    typedef void*               CRITSECT_TYPE;  // CRITICAL_SECTION*
    typedef void*               EVENT_TYPE;     // HANDLE

	// Base class for thread errors.
    class GenericError
    {
    public:
        // Constructor.
        GenericError(const char* in_where,
                     const char* in_what) : m_where(in_where),
                                            m_what(in_what)
        {
        }

        // Returns the error location.
        const char* where() const { return (m_where); }

        // Returns the error description.
        const char* what() const { return (m_what); }

    private:
        const char* m_where;
        const char* m_what;
    };

    //
    // Runnable interface.
    //
    class Runnable
    {
    public:
        // Destructor.
        virtual ~Runnable() {}

        // This method will be executed in a separate thread.
        virtual void run() = 0;
    };

    //
    // The thread.
    //
    class Thread
    {
    private:
        // Disable copy semantics.
        Thread(const Thread&) {}
        Thread& operator =(const Thread&) { return (*this); }

    public:
        // Thrown when an error occured.
        class Error : public GenericError
        {
        public:
            Error(const char* in_what) : GenericError("class Thread", in_what)
            {
            }
        };

        // Thread priority.
        enum Priority { LOWEST,
                        BELOW_NORMAL,
                        NORMAL,
                        ABOVE_NORMAL,
                        HIGHEST,
                        TIME_CRITICAL };

        // Constructor. Creates the thread with normal priority and
        // suspended state. You must call the start() method to start
        // the thread execution.
        THREADAPI Thread(Runnable* in_pRunnable = 0);

        // Destructor.
        THREADAPI virtual ~Thread();

        // This method will be executed in a separate thread.
        THREADAPI virtual void run();

        // Sets the thread priority.
        THREADAPI void setPriority(Priority in_priority);

        // Gets the current thread priority.
        Priority getPriority() const { return (m_priority); }

        // Starts the thread.
        // Note: the thread can be started only once!
        THREADAPI void start();

        // Returns true if the thread is started.
        bool isStarted() const { return (m_isStarted); }

        // Returns when the thread is stopped.
        THREADAPI void waitForEnd();

        // For implementation purpose only!
        // Returns a pointer to the thread id.
        THREAD_TYPE* getThreadIdPtr() { return (&m_threadId); }
        
    protected:
        Runnable*       m_pRunnable;  // Runnable to be executed.

    private:
        bool            m_isStarted;
        Priority        m_priority;
        THREAD_TYPE     m_threadId;
    };

    //
    // Wait result.
    // The value will be returned by the waiting with timeout methods.
    //
    enum WaitResult
    {
        TIMEOUT,            // Operation was terminated by timeout.
        SUCCESS             // Operation has been completed successful.
    };

    //
    // Semaphore.
    // Note: For mutually exclusive access to a shared resources use
    // CritSect object instead of this, because it will be slightly faster.
    //
    class Semaphore
    {
    private:
        // Disable copy semantics.
        Semaphore(const Semaphore&) {}
        Semaphore& operator =(const Semaphore&) { return (*this); }

    public:
        // Thrown when an error occured.
        class Error : public GenericError
        {
        public:
            Error(const char* in_what) : GenericError("class Semaphore",
                                                      in_what)
            {
            }
        };

        // Creates new semaphore.
        // The semaphore created by this constructor will be deleted by
        // the destructor.
        // Win32 Note: If the named semaphore already exists the Error()
        // will be thrown. To open existing named semaphore use the
        // Semaphore(const char*) constructor.
        THREADAPI Semaphore(unsigned long in_threadCount = 1,
                            const char*   in_name        = 0);

        // Opens existing semaphore.
        // The semaphore will not be deleted by the destructor.
        THREADAPI Semaphore(const char* in_name);

        // Destructor.
        THREADAPI virtual ~Semaphore();

        // Waits for ownership of this object.
        // The method returns when the calling thread is granted ownership.
        THREADAPI void lock();

        // Waits for ownership of this object a time, that is less than
        // the passed value in milliseconds.
        // The method returns when the calling thread is granted ownership
        // or when the time expires. In first case return value will be
        // SUCCESS, in the second case - TIMEOUT.
        // If the time is zero the method test semaphore and returns
        // immediately. The return value will be TIMEOUT if the ownership
        // was not granted.
        THREADAPI WaitResult lock(unsigned long in_timeOut);

        // Tests semaphore and returns immediately. The return value will be
        // TIMEOUT if the ownership was not granted.
        WaitResult tryLock() { return (lock(0)); }

        // Releases ownership of this object. The thread count will be
        // increased by the in_count.
        THREADAPI void unlock(long in_count = 1);

        // Returns a name of the semaphore.
        const char* getName() const { return (m_name); }

        // For implementation purpose only!
        // Returns a pointer to semaphore id.
        SEMAPHORE_TYPE* getSemIdPtr() { return (&m_semId); }
        
    private:
        SEMAPHORE_TYPE m_semId;
        const char*    m_name;
    };

    //
    // Critical section.
    // Can be owned by only one thread at a time, enabling threads to
    // coordinate mutually exclusive access to a shared resource.
    //
    class CritSect
    {
    private:
        // Disable copy semantics.
        CritSect(const CritSect&) {}
        CritSect& operator =(const CritSect&) { return (*this); }

    public:
        // Thrown when an error occured.
        class Error : public GenericError
        {
        public:
            Error(const char* in_what) : GenericError("class CritSect",
                                                      in_what)
            {
            }
        };

        // Construction / destruction.
        THREADAPI CritSect();
        THREADAPI ~CritSect();

        // Waits for ownership of this object.
        // The method returns when the calling thread is granted ownership.
        THREADAPI void lock();

        // Releases ownership of this object.
        THREADAPI void unlock();

    private:
        CRITSECT_TYPE m_critSectId;
    };

    //
    // Locks a given object (CritSect, Semaphore or another one with
    // lock()/unlock() interface) when this object is constructed and
    // unlocks the object when this object is destroyed.
    //
    template <class T>
    class tAutoLock
    {
    private:
        // Disable copy semantics.
        tAutoLock(const tAutoLock&) {}
        tAutoLock& operator =(const tAutoLock&) { return (*this); }

    public:
        // Constructor.
        // Locks the given locker object.
        tAutoLock(T* in_lock) : m_lock(in_lock) { m_lock->lock(); }

        // Destructor.
        // Unlocks the locker.
        ~tAutoLock() { if (m_lock) m_lock->unlock(); }

        // Unlocks the locker object and detaches it from this object.
        void unlock() { if (m_lock) { m_lock->unlock(); m_lock = 0; } }

        // Locks the given locker object and attaches it to this object.
        void lock(T* in_lock) { m_lock = in_lock; m_lock->lock(); }

    private:
        T* m_lock;
    };

    //
    // Predefined auto lockers.
    //
    typedef tAutoLock<Semaphore> AutoSemaphore;
    typedef tAutoLock<CritSect>  AutoCritSect;

    //
    // Synchronized event.
    //
    class SyncEvent
    {
    private:
        // Disable copy semantics.
        SyncEvent(const SyncEvent&) {}
        SyncEvent& operator =(const SyncEvent&) { return (*this); }

    public:
        // Thrown when an error occured.
        class Error : public GenericError
        {
        public:
            Error(const char* in_what) : GenericError("class SyncEvent",
                                                      in_what)
            {
            }
        };

        // Constructor.
        // If in_initialState equals true then initial state is signaled.
        // If in_manualReset equals true then you must call the reset()
        // method to change the state to non-signaled, otherwise it will
        // be done automatically.
        // Win32 Note: If the named event already exists the Error()
        // will be thrown. To open existing named event use the
        // SyncEvent(const char*) constructor.
        THREADAPI SyncEvent(bool in_initialState = false,
                            bool in_manualReset  = false,
                            const char* in_name  = 0);

        // Opens existing event.
        // The event will not be deleted by the destructor.
        THREADAPI SyncEvent(const char* in_name);


        // Destructor.
        THREADAPI virtual ~SyncEvent();

        // Sets the state of the event to signaled (the event is occured).
        THREADAPI void set();

        // Resets the state of the event to non-signaled.
        THREADAPI void reset();

        // Waits until the state of the event will be signaled without
        // time-out.
        THREADAPI void wait();

        // Waits until the state of the event will be signaled.
        // The method returns when the event is signaled
        // or when the time expires. In the first case return value will be
        // SUCCESS, in the second case - TIMEOUT.
        // If the time is zero the method test the event and returns
        // immediately. The return value will be TIMEOUT if the event
        // is not signaled.
        THREADAPI WaitResult wait(unsigned long in_timeOut);

        // Tests event and returns immediately. The return value will be
        // TIMEOUT if the event is not signaled.
        WaitResult tryWait() { return (wait(0)); }

        // Returns a name of the event.
        const char* getName() const { return (m_name); }

    private:
        EVENT_TYPE  m_eventId;
        const char* m_name;
    };

    //
    // Utilites.
    //

    //
    // Blocks the current thread for a given number of milliseconds.
    // Returns true if the time is elapsed, false if the operation was
    // interrupted.
    //
    THREADAPI bool sleep(unsigned long in_millis);

    //
    // Causes the currently executing thread to temporarily pause and
    // allow other threads to execute.
    //
    THREADAPI void yield();
}

#endif // __thread_h__