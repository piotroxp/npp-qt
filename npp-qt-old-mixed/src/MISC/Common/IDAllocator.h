// IDAllocator — ID range allocator for plugin commands, markers, indicators
// Stub implementation for Qt port
#pragma once

#include <QtCore/QSet>

class IDAllocator {
public:
    IDAllocator(int base, int limit) : _base(base), _limit(limit), _next(base) {}

    // Allocate a single ID
    int allocate() {
        while (_next <= _limit) {
            if (!_allocated.contains(_next)) {
                _allocated.insert(_next);
                return _next++;
            }
            ++_next;
        }
        return -1;
    }

    // Allocate `numberRequired` consecutive IDs starting from `*start`.
    // Returns the first ID; sets *start = -1 on failure.
    int allocate(int numberRequired, int* start) {
        if (numberRequired <= 0 || !start)
            return -1;

        for (int candidate = _base; candidate <= _limit; ++candidate) {
            bool ok = true;
            for (int j = 0; j < numberRequired; ++j) {
                int id = candidate + j;
                if (id > _limit || _allocated.contains(id)) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                for (int j = 0; j < numberRequired; ++j) {
                    _allocated.insert(candidate + j);
                }
                *start = candidate;
                return candidate;
            }
        }
        *start = -1;
        return -1;
    }

    bool release(int id) {
        if (_allocated.remove(id)) {
            if (id < _next) _next = id;
            return true;
        }
        return false;
    }

    bool isInRange(int id) const {
        return id >= _base && id <= _limit;
    }

    void reset() {
        _allocated.clear();
        _next = _base;
    }

private:
    int _base = 0;
    int _limit = 0;
    int _next = 0;
    QSet<int> _allocated;
};
