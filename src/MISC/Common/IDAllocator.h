// IDAllocator — ID range allocator for plugin commands, markers, indicators
// Stub implementation for Qt port
#pragma once

#include <QtCore/QSet>

class IDAllocator {
public:
    IDAllocator(int base, int limit) : _base(base), _limit(limit), _next(base) {}

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
