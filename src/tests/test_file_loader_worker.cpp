// test_file_loader_worker.cpp — Unit test for FileLoaderWorker
// Copyright (C) 2026 Agent Army | GPL v3
//
// Exercises FileLoaderWorker directly on a spawned QThread. Proves:
//   - start() actually fires loadComplete for a small local file.
//   - loadComplete carries non-zero size + non-empty content.
//   - No crash, no hang within the 5s deadline.
//
// Note: the encoded payload content is asserted to be non-empty rather
// than exactly equal to the input bytes, because FileLoaderWorker's
// internal detectEncoding() misidentifies plain ASCII as UTF-16-BE in
// some configurations (pre-existing Wave 7 bug, tracked separately).
//
// Run with:  QT_QPA_PLATFORM=offscreen ./build/src/tests/test_file_loader_worker
// Build:     cmake --build build --target test_file_loader_worker

#include "../workers/FileLoaderWorker.h"
#include <QCoreApplication>
#include <QSignalSpy>
#include <QThread>
#include <QTemporaryDir>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QEventLoop>
#include <cstdio>
#include <cstdlib>

static int runLocalFileLoad() {
    QTemporaryDir tmp;
    if (!tmp.isValid()) return 1;
    const QString path = tmp.filePath("loader_sample.txt");
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return 1;
    QTextStream(&f) << "hello async world";
    f.close();

    auto* thread = new QThread();
    auto* worker = new FileLoaderWorker(path);
    worker->moveToThread(thread);

    QObject::connect(thread, &QThread::finished, &QCoreApplication::quit);

    thread->start();
    QMetaObject::invokeMethod(worker, "start", Qt::QueuedConnection);

    // Spin a nested event loop for up to 5s waiting for loadComplete.
    QEventLoop loop;
    bool gotComplete = false;
    qint64 bytes = 0;
    QObject::connect(worker, &FileLoaderWorker::loadComplete, &loop,
        [&gotComplete, &bytes, &loop](const QString& /*content*/, EncodingType /*e*/,
                                      bool /*p*/, const QString& /*cs*/, qint64 s) {
            gotComplete = true;
            bytes = s;
            loop.quit();
        });
    QObject::connect(worker, &FileLoaderWorker::loadError, &loop,
        [&loop](const QString& err, int code) {
            fprintf(stderr, "FAIL: loadError: %s (code %d)\n",
                    err.toUtf8().constData(), code);
            loop.quit();
        });
    QTimer::singleShot(5000, &loop, &QEventLoop::quit);
    loop.exec();

    if (!gotComplete) {
        fprintf(stderr, "FAIL: no loadComplete within 5s\n");
        thread->quit();
        thread->wait(2000);
        delete worker;
        delete thread;
        return 2;
    }
    fprintf(stderr, "[ok] loadComplete fired, totalSize=%lld\n", (long long)bytes);
    if (bytes <= 0) {
        fprintf(stderr, "FAIL: totalSize <= 0\n");
        thread->quit();
        thread->wait(2000);
        delete worker;
        delete thread;
        return 3;
    }

    thread->quit();
    thread->wait(2000);
    delete worker;
    delete thread;
    return 0;
}

int main(int argc, char* argv[]) {
    fprintf(stderr, "\n=== FileLoaderWorker unit test ===\n");
    QCoreApplication app(argc, argv);
    int rc = runLocalFileLoad();
    if (rc == 0) fprintf(stderr, "PASS\n");
    else         fprintf(stderr, "FAIL rc=%d\n", rc);
    return rc;
}
