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

// Qt6 port: Win32 ReadDirectoryChangesW implementation
// Original: PowerEditor/src/WinControls/ReadDirectoryChanges/ReadDirectoryChangesPrivate.h
//
// On Windows, this file uses ReadDirectoryChangesW with APC callbacks to monitor
// directory changes.  On Linux, the Qt6 DirectoryWatcher (QFileSystemWatcher)
// in ReadDirectoryChanges.h provides the same functionality using inotify/FSEvents.
// This file is retained for Windows builds; Linux builds use the Qt6 path.

#pragma once

#include <string>
#include <vector>
#if !defined(_WIN32)
# include <thread>
#endif

// Forward declaration — the actual type depends on the platform.
#if defined(_WIN32)
#include <windows.h>
using FileHandleType = HANDLE;
#else
using FileHandleType = void*;
#endif

class CReadDirectoryChanges;

namespace ReadDirectoryChangesPrivate
{

class CReadChangesServer;

///////////////////////////////////////////////////////////////////////////

// All functions in CReadChangesRequest run in the context of the worker thread.
// One instance of this object is created for each call to AddDirectory().
class CReadChangesRequest
{
public:
    // sz: directory path string
    // b: include children (subdirectory watching)
    // dw: filter flags (FILE_NOTIFY_CHANGE_*)
    // size: buffer size for notifications
    CReadChangesRequest(CReadChangesServer* pServer, const wchar_t* sz, bool b, DWORD dw, DWORD size);

    ~CReadChangesRequest();

    // Opens the directory handle via CreateFileW.
    bool openDirectory();

    // Begins an asynchronous ReadDirectoryChangesW call.
    void beginRead();

    // Backs up the notification buffer for processing.
    void backupBuffer(DWORD dwSize)
    {
        std::memcpy(m_backupBuffer.data(), m_buffer.data(), dwSize);
    }

    // Processes FILE_NOTIFY_INFORMATION records from the backup buffer.
    void processNotification();

    // Cancels pending I/O and closes the directory handle.
    void requestTermination()
    {
#if defined(_WIN32)
        ::CancelIo(m_hDirectory);
        ::CloseHandle(m_hDirectory);
        m_hDirectory = nullptr;
#endif
    }

    CReadChangesServer* m_pServer = nullptr;

protected:

#if defined(_WIN32)
    // APC callback for ReadDirectoryChangesW completion.
    static void CALLBACK notificationCompletion(
        DWORD dwErrorCode,
        DWORD dwNumberOfBytesTransfered,
        LPOVERLAPPED lpOverlapped);
#endif

    // Parameters from the caller for ReadDirectoryChangesW().
    std::wstring m_wstrDirectory;
    bool m_bIncludeChildren = false;
    DWORD m_dwFilterFlags = 0;

    // Result of calling CreateFile().
    FileHandleType m_hDirectory = nullptr;

    // Required parameter for ReadDirectoryChangesW().
    // hEvent member points to 'this' when using a completion routine.
#if defined(_WIN32)
    OVERLAPPED m_overlapped = {};
#endif

    // Data buffer for the request.
    std::vector<unsigned char> m_buffer;

    // Double buffer strategy so that we can issue a new read
    // request before we process the current buffer.
    std::vector<unsigned char> m_backupBuffer;
};

///////////////////////////////////////////////////////////////////////////

// All functions in CReadChangesServer run in the context of the worker thread.
// One instance of this object is allocated for each instance of CReadDirectoryChanges.
// Responsible for thread startup, orderly thread shutdown, and shimming
// the various C++ member functions with platform-specific functions.
class CReadChangesServer
{
public:
    explicit CReadChangesServer(CReadDirectoryChanges* pParent);

#if defined(_WIN32)
    // Thread entry point (Win32 only — uses _beginthreadex / QueueUserAPC).
    static unsigned int WINAPI threadStartProc(void* arg)
    {
        auto* pServer = static_cast<CReadChangesServer*>(arg);
        pServer->run();
        return 0;
    }

    // Called by QueueUserAPC to start orderly shutdown.
    static void CALLBACK terminateProc(ULONG_PTR arg)
    {
        auto* pServer = reinterpret_cast<CReadChangesServer*>(arg);
        pServer->requestTermination();
    }

    // Called by QueueUserAPC to add another directory.
    static void CALLBACK addDirectoryProc(ULONG_PTR arg)
    {
        auto* pRequest = reinterpret_cast<CReadChangesRequest*>(arg);
        pRequest->m_pServer->addDirectory(pRequest);
    }
#else
    // On non-Windows, the server methods are not used.
    // QFileSystemWatcher in ReadDirectoryChanges.h handles notifications.
#endif

    CReadDirectoryChanges* m_pBase = nullptr;

    // Number of outstanding ReadDirectoryChangesW requests.
    volatile long m_nOutstandingRequests = 0;

protected:

    void run()
    {
#if defined(_WIN32)
        while (m_nOutstandingRequests || !m_bTerminate)
        {
            ::SleepEx(INFINITE, true); // alertable wait
        }
#else
        // On Linux, the Qt6 DirectoryWatcher uses QFileSystemWatcher.
        // This server thread is not used — the QThread event loop handles notifications.
        while (m_nOutstandingRequests || !m_bTerminate)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
#endif
    }

    void addDirectory(CReadChangesRequest* pRequest)
    {
        if (pRequest->openDirectory())
        {
#if defined(_WIN32)
            ::InterlockedIncrement(&pRequest->m_pServer->m_nOutstandingRequests);
#endif
            m_pBlocks.push_back(pRequest);
            pRequest->beginRead();
        }
        else
        {
            delete pRequest;
        }
    }

    void requestTermination()
    {
        m_bTerminate = true;

        for (auto* pBlock : m_pBlocks)
        {
            pBlock->requestTermination();
        }

        m_pBlocks.clear();
    }

    std::vector<CReadChangesRequest*> m_pBlocks;
    bool m_bTerminate = false;
};

} // namespace ReadDirectoryChangesPrivate
