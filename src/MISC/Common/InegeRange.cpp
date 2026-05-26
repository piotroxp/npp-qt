// MISC/Common/InegeRange.cpp - Qt6 port of integer range utilities
#include "InegeRange.h"

void RangeCollection::add(int start, int end)
{
    add(Range(start, end));
}

void RangeCollection::add(const Range& range)
{
    if (!range.isValid()) return;
    
    // Find insertion point
    int insertPos = 0;
    while (insertPos < _ranges.size() && _ranges[insertPos].start < range.start) {
        ++insertPos;
    }
    
    // Check for merging with previous
    if (insertPos > 0 && _ranges[insertPos - 1].end >= range.start - 1) {
        _ranges[insertPos - 1].end = qMax(_ranges[insertPos - 1].end, range.end);
        ++insertPos;
    } else {
        _ranges.insert(insertPos, range);
    }
    
    // Merge with following ranges
    while (insertPos < _ranges.size() && _ranges[insertPos - 1].end >= _ranges[insertPos].start - 1) {
        _ranges[insertPos - 1].end = qMax(_ranges[insertPos - 1].end, _ranges[insertPos].end);
        _ranges.removeAt(insertPos);
    }
    
    normalize();
}

void RangeCollection::remove(int start, int end)
{
    remove(Range(start, end));
}

void RangeCollection::remove(const Range& range)
{
    if (!range.isValid()) return;
    
    QVector<Range> newRanges;
    
    for (const Range& r : _ranges) {
        QVector<Range> diff = RangeUtil::rangeDifference(r, range);
        newRanges.append(diff);
    }
    
    _ranges = newRanges;
    normalize();
}

bool RangeCollection::contains(int value) const
{
    return findRangeIndex(value) >= 0;
}

bool RangeCollection::contains(const Range& range) const
{
    int idx = findRangeIndex(range.start);
    if (idx < 0) return false;
    return _ranges[idx].end >= range.end;
}

bool RangeCollection::overlaps(const Range& range) const
{
    for (const Range& r : _ranges) {
        if (RangeUtil::rangesOverlap(r, range))
            return true;
    }
    return false;
}

int RangeCollection::totalSize() const
{
    int total = 0;
    for (const Range& r : _ranges) {
        total += r.size();
    }
    return total;
}

void RangeCollection::merge()
{
    if (_ranges.size() <= 1) return;
    
    std::sort(_ranges.begin(), _ranges.end());
    
    QVector<Range> merged;
    Range current = _ranges[0];
    
    for (int i = 1; i < _ranges.size(); ++i) {
        if (current.end >= _ranges[i].start - 1) {
            current.end = qMax(current.end, _ranges[i].end);
        } else {
            merged.append(current);
            current = _ranges[i];
        }
    }
    merged.append(current);
    
    _ranges = merged;
}

void RangeCollection::split(int position)
{
    int idx = findRangeIndex(position);
    if (idx < 0) return;
    
    Range& r = _ranges[idx];
    if (position > r.start && position <= r.end) {
        Range before(r.start, position - 1);
        Range after(position, r.end);
        _ranges[idx] = before;
        _ranges.insert(idx + 1, after);
    }
}

int RangeCollection::findRangeIndex(int value) const
{
    // Binary search for the range containing value
    int lo = 0, hi = _ranges.size() - 1;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        if (_ranges[mid].contains(value)) {
            return mid;
        }
        if (value < _ranges[mid].start) {
            hi = mid - 1;
        } else {
            lo = mid + 1;
        }
    }
    return -1;
}

void RangeCollection::normalize()
{
    // Remove invalid ranges
    _ranges.erase(
        std::remove_if(_ranges.begin(), _ranges.end(), [](const Range& r) { return !r.isValid(); }),
        _ranges.end()
    );
    
    // Merge overlapping
    merge();
}