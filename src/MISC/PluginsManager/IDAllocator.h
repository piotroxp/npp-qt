// MISC/PluginsManager/IDAllocator.h - Qt6 port of ID allocation for plugins
#pragma once

#include <cstdint>

class IDAllocator
{
public:
    IDAllocator(int32_t start, int32_t end)
        : _start(start), _end(end), _next(start) {}

    bool isInRange(int32_t id) const {
        return id >= _start && id <= _end;
    }

    bool allocate(int32_t numberRequired, int32_t* start) {
        if (_next + numberRequired - 1 > _end)
            return false;
        *start = _next;
        _next += numberRequired;
        return true;
    }

    void release(int32_t id, int32_t count) {
        Q_UNUSED(id);
        Q_UNUSED(count);
        // In this simplified version, we don't support release
    }

private:
    int32_t _start;
    int32_t _end;
    int32_t _next;
};