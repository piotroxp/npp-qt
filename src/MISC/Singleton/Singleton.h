// MISC/Singleton/Singleton.h - Qt6 cross-platform singleton pattern
#pragma once

#include <QObject>
#include <QCoreApplication>

// Basic singleton template
template<typename T>
class Singleton
{
public:
    static T& getInstance() {
        static T instance;
        return instance;
    }

protected:
    Singleton() = default;
    ~Singleton() = default;

    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;
};

// Application-aware singleton (auto-deletes when app quits)
template<typename T>
class QAppSingleton : public QObject
{
public:
    static T* getInstance() {
        static T* instance = nullptr;
        if (!instance) {
            instance = new T();
            if (qApp) {
                QObject::connect(qApp, &QCoreApplication::aboutToQuit, instance, &T::deleteLater);
            }
        }
        return instance;
    }

protected:
    QAppSingleton(QObject* parent = nullptr) : QObject(parent) {}

    QAppSingleton(const QAppSingleton&) = delete;
    QAppSingleton& operator=(const QAppSingleton&) = delete;
};

// Thread-safe singleton with double-checked locking
template<typename T>
class ThreadSafeSingleton
{
public:
    static T& getInstance() {
        static T instance;
        return instance;
    }

protected:
    ThreadSafeSingleton() = default;
    ~ThreadSafeSingleton() = default;

    ThreadSafeSingleton(const ThreadSafeSingleton&) = delete;
    ThreadSafeSingleton& operator=(const ThreadSafeSingleton&) = delete;
    ThreadSafeSingleton(ThreadSafeSingleton&&) = delete;
    ThreadSafeSingleton& operator=(ThreadSafeSingleton&&) = delete;
};