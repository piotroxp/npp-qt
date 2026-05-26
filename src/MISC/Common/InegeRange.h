// MISC/Common/InegeRange.h - Qt6 port of integer range utilities
#pragma once

#include <QtGlobal>
#include <QVector>
#include <algorithm>

// Represents a range of integers [start, end]
struct Range {
    int start;
    int end;
    
    Range() : start(0), end(-1) {}
    Range(int s, int e) : start(s), end(e) {}
    
    bool isValid() const { return start >= 0 && end >= start; }
    int size() const { return isValid() ? (end - start + 1) : 0; }
    bool contains(int value) const { return value >= start && value <= end; }
    bool contains(const Range& other) const { return other.start >= start && other.end <= end; }
    bool overlaps(const Range& other) const { return !(other.end < start || other.start > end); }
    
    bool operator==(const Range& other) const { return start == other.start && end == other.end; }
    bool operator!=(const Range& other) const { return !(*this == other); }
    bool operator<(const Range& other) const { return start < other.start; }
};

// Collection of non-overlapping ranges
class RangeCollection {
public:
    RangeCollection() = default;
    
    // Add a range to the collection
    void add(int start, int end);
    void add(const Range& range);
    
    // Remove a range from the collection
    void remove(int start, int end);
    void remove(const Range& range);
    
    // Query operations
    bool contains(int value) const;
    bool contains(const Range& range) const;
    bool overlaps(const Range& range) const;
    
    // Get ranges
    const QVector<Range>& getRanges() const { return _ranges; }
    QVector<Range> getRanges() { return _ranges; }
    
    // Number of ranges and total elements
    int rangeCount() const { return _ranges.size(); }
    int totalSize() const;
    
    // Clear all
    void clear() { _ranges.clear(); }
    
    // Merge adjacent ranges
    void merge();
    
    // Split at a position
    void split(int position);
    
private:
    QVector<Range> _ranges;
    
    int findRangeIndex(int value) const;
    void normalize();
};

// Utility functions for range operations
namespace RangeUtil {
    // Check if two ranges overlap
    inline bool rangesOverlap(const Range& a, const Range& b) {
        return !(b.end < a.start || b.start > a.end);
    }
    
    // Get the union of two ranges (if they overlap or are adjacent)
    inline Range rangeUnion(const Range& a, const Range& b) {
        if (!a.isValid()) return b;
        if (!b.isValid()) return a;
        return Range(qMin(a.start, b.start), qMax(a.end, b.end));
    }
    
    // Get the intersection of two ranges
    inline Range rangeIntersection(const Range& a, const Range& b) {
        if (!rangesOverlap(a, b)) return Range();
        return Range(qMax(a.start, b.start), qMin(a.end, b.end));
    }
    
    // Get the difference of two ranges (returns up to 2 ranges)
    inline QVector<Range> rangeDifference(const Range& a, const Range& b) {
        QVector<Range> result;
        if (!rangesOverlap(a, b)) {
            if (a.isValid()) result.append(a);
            return result;
        }
        
        if (b.start > a.start) {
            result.append(Range(a.start, qMin(b.start - 1, a.end)));
        }
        if (b.end < a.end) {
            result.append(Range(qMax(b.end + 1, a.start), a.end));
        }
        return result;
    }
}