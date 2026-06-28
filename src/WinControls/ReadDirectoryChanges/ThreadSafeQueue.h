// The MIT License
//
// Copyright (c) 2010 James E Beveridge
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

// This file is part of Notepad++ project
// Copyright (C)2021 Don HO <don.h@free.fr>

// Qt6 port: thread-safe queue for directory change notifications
// Original: PowerEditor/src/WinControls/ReadDirectoryChanges/ThreadSafeQueue.h
//
// Win32 version uses a Win32 event handle for signaling.
// Qt6 version uses QWaitCondition for cross-platform signaling.

#pragma once

#include <deque>
#include <QMutex>
#include <QWaitCondition>
#include <QCoreApplication>

template <typename T>
class CThreadSafeQueue : protected std::deque<T>
{
protected:
    using Base = std::deque<T>;

public:
    CThreadSafeQueue()
    {
        // QWaitCondition requires a QMutex, not a std::mutex.
        // The condition is signaled when the queue becomes non-empty.
    }

    ~CThreadSafeQueue() = default;

    // Push an item onto the queue and signal the condition variable.
    void push(const T& c)
    {
        {
            QMutexLocker lock(&m_mutex);
            Base::push_back(c);
        }
        m_notEmpty.wakeOne();
    }

    // Move version.
    void push(T&& c)
    {
        {
            QMutexLocker lock(&m_mutex);
            Base::push_back(std::move(c));
        }
        m_notEmpty.wakeOne();
    }

    // Pop an item from the queue. Returns true if an item was retrieved.
    // Returns false if the queue was empty.
    bool pop(T& c)
    {
        QMutexLocker lock(&m_mutex);
        if (Base::empty())
        {
            return false;
        }

        c = std::move(Base::front());
        Base::pop_front();
        return true;
    }

    // Non-blocking check whether the queue is empty.
    bool empty() const
    {
        QMutexLocker lock(&m_mutex);
        return Base::empty();
    }

    // Wait for an item to become available.
    // Returns true if an item was retrieved; false on timeout.
    bool waitAndPop(T& c, int waitMs)
    {
        QMutexLocker lock(&m_mutex);
        if (Base::empty())
        {
            if (waitMs < 0)
            {
                // Infinite wait
                while (Base::empty())
                {
                    m_notEmpty.wait(&m_mutex);
                }
            }
            else
            {
                m_notEmpty.wait(&m_mutex, static_cast<unsigned long>(waitMs));
            }
        }

        if (Base::empty())
            return false;

        c = std::move(Base::front());
        Base::pop_front();
        return true;
    }

protected:
    QWaitCondition m_notEmpty;
    mutable QMutex m_mutex;
};
