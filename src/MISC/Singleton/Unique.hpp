// MISC/Unique.hpp - Qt6 unique pointer utilities
#pragma once

#include <memory>

// Type alias for Qt-style unique pointers
template<typename T>
using UniquePtr = std::unique_ptr<T>;

// Make unique helper (C++14 style for C++11 compatibility)
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

// Qt-style object deletion for unique_ptr
template<typename T>
struct QObjectDeleter {
    void operator()(T* ptr) {
        if (ptr) {
            ptr->deleteLater();
        }
    }
};

template<typename T>
using QObjectUniquePtr = std::unique_ptr<T, QObjectDeleter<T>>;

// Resource wrapper for RAII management
template<typename T, void(*Cleanup)(T*)>
class ResourceGuard {
public:
    ResourceGuard(T* resource = nullptr) : _resource(resource) {}
    ~ResourceGuard() { if (_resource) Cleanup(_resource); }

    T* get() const { return _resource; }
    T* release() { T* r = _resource; _resource = nullptr; return r; }
    void reset(T* r = nullptr) { if (_resource) Cleanup(_resource); _resource = r; }
    explicit operator bool() const { return _resource != nullptr; }

private:
    T* _resource = nullptr;
    ResourceGuard(const ResourceGuard&) = delete;
    ResourceGuard& operator=(const ResourceGuard&) = delete;
};