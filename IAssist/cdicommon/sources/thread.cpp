///////////////////////////////////////////////////////////////////////////////
// thread.cpp
// ~~~~~~~~~~
// The thread-management library implementation.
// Supported platforms:
// - Win32 (WIN32 macro must be defined).
// - MacOS (removed by Kola)
//
// Author:      Alexey Zhukov (alexz)
// Created:     06-Dec-1999   (alexz)
// Modified:    08-Jan-2000   (alexz)
//
///////////////////////////////////////////////////////////////////////////////

#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"


#define THREADPROC_DECL     unsigned __stdcall
#define THREADPROC_RETURN   _endthreadex(0); return (0)

#include "thread.h"

//using namespace std;

namespace CDICommon
{
	
    //
    // Error strings.
    //
    const char ERRSTR_THREAD_ALLOC[]     = "Can't create thread.";
    const char ERRSTR_ILLEGAL_PRIORITY[] = "Illegal priority value.";
    const char ERRSTR_SET_PRIORITY[]     = "Can't set priority.";
    const char ERRSTR_SECOND_START[]     = "Thread can be started only once.";
    const char ERRSTR_START[]            = "Thread can not be started.";
    const char ERRSTR_WAIT_THREAD[]      = "Waiting for thread termination failed.";

    const char ERRSTR_CREATE_SEMAPHORE[] = "Can't create semaphore.";
    const char ERRSTR_EXISTS_SEMAPHORE[] = "Semaphore already exists.";
    const char ERRSTR_OPEN_SEMAPHORE[]   = "Can't open existing semaphore.";
    const char ERRSTR_LOCK_SEMAPHORE[]   = "Lock of the semaphore failed.";
    const char ERRSTR_UNLOCK_SEMAPHORE[] = "Can't unlock semaphore.";

    const char ERRSTR_CREATE_CRITSECT[]  = "Can't create critical section.";
    const char ERRSTR_LOCK_CRITSECT[]    = "Can't lock critical section.";
    const char ERRSTR_UNLOCK_CRITSECT[]  = "Can't unlock critical section.";

    const char ERRSTR_CREATE_EVENT[]     = "Can't create event.";
    const char ERRSTR_EXISTS_EVENT[]     = "Event already exists.";
    const char ERRSTR_OPEN_EVENT[]       = "Can't open existing event.";
    const char ERRSTR_SET_EVENT[]        = "Can't set event.";
    const char ERRSTR_WAIT_EVENT[]       = "Wait failed.";
    const char ERRSTR_RESET_EVENT[]      = "Can't reset event.";

    //
    // The thread procedure.
    //
    THREADPROC_DECL threadProc(void* in_param)
    {
        try
        {
            // Execute the run() method of a given Thread object.
            static_cast<Thread*>(in_param)->run();
        }
		catch (...)
        {

        }

        THREADPROC_RETURN;
    }

    //
    // Thread implementation.
    //

    //
    // Constructor.
    //
    Thread::Thread(Runnable* in_pRunnable) : m_pRunnable(in_pRunnable),
                                             m_isStarted(false),
                                             m_priority(NORMAL),
                                             m_threadId(0)
    {

        unsigned      thrdaddr;
        uintptr_t     threadId = _beginthreadex(0,
                                                0,
                                                threadProc,
                                                this,
                                                CREATE_SUSPENDED,
                                                &thrdaddr);
        if (!threadId)
            throw Error(ERRSTR_THREAD_ALLOC);
        m_threadId = reinterpret_cast<void*>(threadId); 
    }

    //
    // Destructor.
    //
    Thread::~Thread()
    {
        CloseHandle(reinterpret_cast<HANDLE>(m_threadId));
	}

    //
    // This method will be executed in a separate thread.
    //
    void Thread::run()
    {
        if (m_pRunnable)
            m_pRunnable->run();
    }

    //
    // Sets the thread priority.
    //
    void Thread::setPriority(Thread::Priority in_priority)
    {
        int priority;
        switch (in_priority)
        {
            case LOWEST:
                priority = THREAD_PRIORITY_LOWEST;
                break;

            case BELOW_NORMAL:
                priority = THREAD_PRIORITY_BELOW_NORMAL;
                break;

            case NORMAL:
                priority = THREAD_PRIORITY_NORMAL;
                break;

            case ABOVE_NORMAL:
                priority = THREAD_PRIORITY_ABOVE_NORMAL;
                break;

            case HIGHEST:
                priority = THREAD_PRIORITY_HIGHEST;
                break;

            case TIME_CRITICAL:
                priority = THREAD_PRIORITY_TIME_CRITICAL;
                break;

            default:
                throw Error(ERRSTR_ILLEGAL_PRIORITY);
        }
        if (!SetThreadPriority(reinterpret_cast<HANDLE>(m_threadId), priority))
            throw Error(ERRSTR_SET_PRIORITY);
        m_priority = in_priority;
    }

    //
    // Starts the thread.
    // Note: the thread can be started only once!
    //
    void Thread::start()
    {
        if (m_isStarted)
            throw Error(ERRSTR_SECOND_START);

        m_isStarted = true;

        if (ResumeThread(reinterpret_cast<HANDLE>(m_threadId)) == 0xffffffff)
            throw Error(ERRSTR_START);

    }

    //
    // Returns when the thread is stopped.
    //
    void Thread::waitForEnd()
    {
        switch (WaitForSingleObject(reinterpret_cast<HANDLE>(m_threadId),
                                    INFINITE))
        {
            case WAIT_ABANDONED:
            case WAIT_OBJECT_0:
            case WAIT_TIMEOUT:
                return;

            default:
                throw Error(ERRSTR_WAIT_THREAD);
        }

    }

    //
    // Semaphore implementation.
    //

    //
    // Creates new semaphore.
    // The semaphore created by this constructor will be deleted by
    // the destructor.
    // Win32 Note: If the name of semaphore already exists the constructor
    // will throw Error(). To open existing named semaphore use the
    // Semaphore(const char*) constructor.
    //
    Semaphore::Semaphore(unsigned long in_threadCount,
                         const char*   in_name) : 
	m_semId(0), 
	m_name(in_name)
    {
        SECURITY_ATTRIBUTES secAttr;
        secAttr.nLength              = sizeof(secAttr);
        secAttr.lpSecurityDescriptor = NULL; // Default for the calling process.
        secAttr.bInheritHandle       = TRUE;

		//LONG max_count = std::numeric_limits<LONG>::max();
		LONG max_count = 0x7FFFFFFF;
		HANDLE hSema = CreateSemaphore(&secAttr,
                                       in_threadCount,
									   max_count,
                                       in_name);
        if (!hSema)
            throw Error(ERRSTR_CREATE_SEMAPHORE);
        else if (GetLastError() == ERROR_ALREADY_EXISTS)
        {
            CloseHandle(hSema);
            throw Error(ERRSTR_EXISTS_SEMAPHORE);
        }

        m_semId = reinterpret_cast<void*>(hSema);
    }

    //
    // Opens an existing semaphore.
    // The semaphore will not be deleted by the destructor.
    //
    Semaphore::Semaphore(const char* in_name) : m_semId(0), m_name(in_name)
    {
        m_semId = reinterpret_cast<void*>(OpenSemaphore(SEMAPHORE_ALL_ACCESS   |
                                                       SEMAPHORE_MODIFY_STATE |
                                                       SYNCHRONIZE,
                                                       TRUE,
                                                       in_name));
        if (!m_semId)
            throw Error(ERRSTR_OPEN_SEMAPHORE);
    }

    //
    // Destructor.
    //
    Semaphore::~Semaphore()
    {
        CloseHandle(reinterpret_cast<HANDLE>(m_semId));
    }

    //
    // Waits for ownership of this object.
    // The method returns when the calling thread is granted ownership.
    //
    void Semaphore::lock()
    {
        switch (WaitForSingleObject(reinterpret_cast<HANDLE>(m_semId),
                                    INFINITE))
        {
            case WAIT_ABANDONED:
            case WAIT_OBJECT_0:
            case WAIT_TIMEOUT:
                return;

            default:
                throw Error(ERRSTR_LOCK_SEMAPHORE);
        }
    }

    //
    // Waits for ownership of this object a time, that is less than
    // the passed value in milliseconds.
    // The method returns when the calling thread is granted ownership
    // or when the time expires. In first case return value will be
    // SUCCESS, in the second case - TIMEOUT.
    // If the time is zero the method test semaphore and returns
    // immediately. The return value will be TIMEOUT if the ownership
    // was not granted.
    //
    WaitResult Semaphore::lock(unsigned long in_timeOut)
    {
        switch (WaitForSingleObject(reinterpret_cast<HANDLE>(m_semId),
                                    in_timeOut))
        {
            case WAIT_ABANDONED:
            case WAIT_OBJECT_0:
                return (SUCCESS);

            case WAIT_TIMEOUT:
                return (TIMEOUT);

            default:
                throw Error(ERRSTR_LOCK_SEMAPHORE);
        }
    }

    //
    // Releases ownership of this object. The thread count will be
    // increased by the in_count.
    //
    void Semaphore::unlock(long in_count)
    {
        if (!ReleaseSemaphore(reinterpret_cast<HANDLE>(m_semId), in_count, 0))
            throw Error(ERRSTR_UNLOCK_SEMAPHORE);
    }

    //
    // CritSect implementation.
    //

    //
    // Constructor
    //
    CritSect::CritSect() : m_critSectId(0)
    {
        CRITICAL_SECTION* pCritSect = 0;
        try
        {
            pCritSect = new CRITICAL_SECTION();
            InitializeCriticalSection(pCritSect);
            m_critSectId = pCritSect;
        }
        catch (...)     // STATUS_NO_MEMORY
        {
            delete pCritSect;
            throw Error(ERRSTR_CREATE_CRITSECT);
        }
    }

    //
    // Destructor.
    //
    CritSect::~CritSect()
    {
        DeleteCriticalSection(static_cast<CRITICAL_SECTION*>(m_critSectId));
        delete m_critSectId;
    }

    //
    // Waits for ownership of this object.
    // The method returns when the calling thread is granted ownership.
    //
    void CritSect::lock()
    {
        try
        {
            EnterCriticalSection(static_cast<CRITICAL_SECTION*>(m_critSectId));
        }
        catch (... )    // STATUS_INVALID_HANDLE
        {
            throw Error(ERRSTR_LOCK_CRITSECT);
        }
    }

    //
    // Releases ownership of this object.
    //
    void CritSect::unlock()
    {
        LeaveCriticalSection(static_cast<CRITICAL_SECTION*>(m_critSectId));
    }

    //
    // SyncEvent implementation.
    //

    //
    // Constructor.
    // If in_initialState equals true then initial state is signaled.
    // If in_manualReset equals true then you must call the reset()
    // method to change the state to non-signaled, otherwise it will
    // be done automatically.
    //
    SyncEvent::SyncEvent(bool in_initialState,
                         bool in_manualReset,
                         const char* in_name) : m_eventId(0), m_name(in_name)
    {
        SECURITY_ATTRIBUTES secAttr;
        secAttr.nLength              = sizeof(secAttr);
        secAttr.lpSecurityDescriptor = NULL; // Default for the calling process.
        secAttr.bInheritHandle       = TRUE;

        HANDLE hEvent = CreateEvent(&secAttr,
                                    in_manualReset,
                                    in_initialState,
                                    in_name);
        if (!hEvent)
            throw Error(ERRSTR_CREATE_EVENT);
        else if (GetLastError() == ERROR_ALREADY_EXISTS)
        {
            CloseHandle(hEvent);
            throw Error(ERRSTR_EXISTS_EVENT);
        }

        m_eventId = reinterpret_cast<void*>(hEvent);
    }

    //
    // Opens existing event.
    // The event will not be deleted by the destructor.
    //
    SyncEvent::SyncEvent(const char* in_name) : m_eventId(0), m_name(in_name)
    {
        m_eventId = reinterpret_cast<void*>(OpenEvent(EVENT_ALL_ACCESS   |
                                                      EVENT_MODIFY_STATE |
                                                      SYNCHRONIZE,
                                                      TRUE,
                                                      in_name));
        if (!m_eventId)
            throw Error(ERRSTR_OPEN_EVENT);
    }

    //
    // Destructor.
    //
    SyncEvent::~SyncEvent()
    {
        CloseHandle(reinterpret_cast<HANDLE>(m_eventId));
    }

    //
    // Sets the state of the event to signaled (the event is occured).
    //
    void SyncEvent::set()
    {
        if (!SetEvent(reinterpret_cast<HANDLE>(m_eventId)))
            throw Error(ERRSTR_SET_EVENT);
    }

    //
    // Waits until the state of the event will be signaled without
    // time-out.
    //
    void SyncEvent::wait()
    {
        switch (WaitForSingleObject(reinterpret_cast<HANDLE>(m_eventId),
                                    INFINITE))
        {
            case WAIT_ABANDONED:
            case WAIT_OBJECT_0:
            case WAIT_TIMEOUT:
                return;

            default:
                throw Error(ERRSTR_WAIT_EVENT);
        }
    }

    //
    // Waits until the state of the event will be signaled.
    // The method returns when the event is signaled
    // or when the time expires. In the first case return value will be
    // SUCCESS, in the second case - TIMEOUT.
    // If the time is zero the method test the event and returns
    // immediately. The return value will be TIMEOUT if the event
    // is not signaled.
    //
    WaitResult SyncEvent::wait(unsigned long in_timeOut)
    {
        switch (WaitForSingleObject(reinterpret_cast<HANDLE>(m_eventId),
                                    in_timeOut))
        {
            case WAIT_ABANDONED:
            case WAIT_OBJECT_0:
                return (SUCCESS);

            case WAIT_TIMEOUT:
                return (TIMEOUT);

            case WAIT_FAILED:
            default:
                break;
        }
        throw Error(ERRSTR_WAIT_EVENT);
    }

    //
    // Resets the state of the event to non-signaled.
    //
    void SyncEvent::reset()
    {
        if (!ResetEvent(reinterpret_cast<HANDLE>(m_eventId)))
            throw Error(ERRSTR_RESET_EVENT);
    }

    //
    // Utilites.
    //

    //
    // Blocks the current thread for a given number of milliseconds.
    // Returns true if the time is elapsed, false if the operation was
    // interrupted.
    //
    bool sleep(unsigned long in_millis)
    {
        Sleep(in_millis);
        return (true);
    }


    //
    // Causes the currently executing thread to temporarily pause and
    // allow other threads to execute.
    //
    void yield()
    {
        Sleep(0);
    }
}

