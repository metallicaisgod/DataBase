///////////////////////////////////////////////////////////////////////////////
// threadutils.h
// ~~~~~~~~~~~~~~
// Contains several thread utilites.
//
// Author:      Alexey Zhukov (alexz)
// Created:     11-Jan-2000   (alexz)
// Modified:    11-May-2000   (alexz)
//
// Copyright (C) 2000 by Maris Multimedia Ltd. All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////
#ifndef __threadutils_h__
#define __threadutils_h__

#include "thread.h"

namespace CDICommon
{
    //
    // Generic object with synchronization and copy semantics.
    //
    class ThreadSafeObject
    {
    public:
        //
        // Creates new object.
        //
        ThreadSafeObject() : m_guard()
        {
        }

        //
        // Copy constructor.
        // Creates new critical section.
        //
        ThreadSafeObject(const ThreadSafeObject&) : m_guard()
        {
        }

        //
        // Locks the object.
        //
        void lock() { m_guard.lock(); }

        //
        // Unlocks the object.
        //
        void unlock() { m_guard.unlock(); }

    private:
        CritSect m_guard;
    };

    //
    // Separate thread queue (FIFO).
    // Provides the ability to put an item into one thread while processing it
    // in another.
    //
    template <class ITEM>
    class tSepThreadQueue : protected CritSect, public Thread
    {
    public:
        typedef typename ITEM QueueItem;

        // Creates the queue.
        tSepThreadQueue() : m_processEvent(false, true),// With manual reset.
                            m_done(false),
                            m_idle(0)
        {
        }

        // Destructor.
        ~tSepThreadQueue()
        {
            // Let's the thread exit safely.
            if (!m_done)
                stop();
        }

        // Stops the processing thread.
        // Note: The thread can't be restarted after this call!
        void stop()
        {
            m_done = true;
            m_processEvent.set();
            waitForEnd();
        }

        // Override this method to process an item from the queue.
        // Note: The item is removed from the queue at this point.
        virtual void processItem(QueueItem& in_item) = 0;

        // Inserts an item into the queue.
        void push_front(const QueueItem& in_item)
        {
            AutoCritSect alock(this);
            m_queue.push_front(in_item);
            m_processEvent.set();
        }

        void push_back(const QueueItem& in_item)
        {
            AutoCritSect alock(this);
            m_queue.push_back(in_item);
            m_processEvent.set();
        }

        // Runs in the processing thread.
        // You can override this method to achieve better error
        // handling behavior.
        void run()
        {
            while (!m_done)
            {
                QueueItem item = getItem();
                if (!m_done)
				{
//					AutoCritSect alock(this);
                    processItem(item);
				}
            }
        }

        // Sets the idle function.
        // The idle function will be called periodically while the queue
        // is empty. Call of this method with 0 will cancel the idle loop.
        void idle(Runnable* in_idle)
        {
            AutoCritSect alock(this);
            m_idle = in_idle;
            if (m_idle)
                m_processEvent.set();
        }

    protected:
        // Removes the last item from the queue and returns it.
        // If the queue is empty this method will block until the insert()
        // method will not be called.
        QueueItem getItem()
        {
            AutoCritSect alock(this);
            if (m_queue.empty())
            {
                // Wait/idle loop.
                bool waitIdleLoop;
                do
                {
                    waitIdleLoop = false;

                    if (!m_idle)
                    {
                        // Wait for the item.
                        alock.unlock();
                        m_processEvent.wait();
                    }
                
                    if (m_idle)
                    {
                        // Do the idle loop.
                        alock.lock(this);
                        Runnable* idlePtr = m_idle;
                        bool      isEmpty = m_queue.empty();
                        alock.unlock();
                        while (idlePtr && isEmpty)
                        {
                            idlePtr->run();

                            // Are we about to exit?
                            if (m_done)
                            {
                                QueueItem ret;
                                return (ret);
                            }

                            alock.lock(this);
                            idlePtr = m_idle;
                            isEmpty = m_queue.empty();
                            alock.unlock();
                        }

                        // If the idle function is null and the queue is empty
                        // continue the wait/idle loop.
                        alock.lock(this);
                        waitIdleLoop = !m_idle && m_queue.empty();
                        if (waitIdleLoop)
                            m_processEvent.reset();
                        else
                            alock.unlock();
                    }
                }
                while (waitIdleLoop);

                // Are we about to exit?
                if (m_done)
                {
                    QueueItem ret;
                    return (ret);
                }

                // An item was putted into the queue, so lock this and...
                alock.lock(this);
            }

            // Get the item and remove it from the queue.
            QueueItem item = m_queue.back();
            m_queue.pop_back();

            // Reset the event if the queue is empty.
            if (m_queue.empty())
                m_processEvent.reset();

            // Ok.
            return (item);
        }

        // Access to the queue.
        const std::deque<QueueItem>& getQueue() const { return (m_queue); }

        volatile bool m_done;   // true on exit (initially false).

    private:
        std::deque<QueueItem>  m_queue;
        SyncEvent              m_processEvent;
        Runnable*              m_idle;
    };
}

#endif // !__threadutils_h__