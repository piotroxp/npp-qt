// Date class — timestamp used in settings/history for file comparison
// Win32 original: PowerEditor/src/Parameters.h class Date, Parameters.cpp Date methods

#include "Parameters.h"
#include <QDateTime>
#include <QString>
#include <cassert>
#include <cstdio>
#include <cstring>

Date::Date(unsigned long year, unsigned long month, unsigned long day)
    : _year(year), _month(month), _day(day)
{
    assert(year > 0 && year <= 9999);
    assert(month > 0 && month <= 12);
    assert(day > 0 && day <= 31);
    assert(!(month == 2 && day > 29) &&
           !(month == 4 && day > 30) &&
           !(month == 6 && day > 30) &&
           !(month == 9 && day > 30) &&
           !(month == 11 && day > 30));
}

Date::Date(const char* dateStr)
{
    // dateStr format: YYYYMMDD (8 chars)
    assert(dateStr);
    size_t D = std::strlen(dateStr);
    if (D == 8) {
        int y = QString::fromLatin1(dateStr, 4).toInt();
        int m = QString::fromLatin1(dateStr + 4, 2).toInt();
        int d = QString::fromLatin1(dateStr + 6, 2).toInt();
        if (y > 0 && y <= 9999 && m > 0 && m <= 12 && d > 0 && d <= 31) {
            _year = static_cast<unsigned long>(y);
            _month = static_cast<unsigned long>(m);
            _day = static_cast<unsigned long>(d);
            return;
        }
    }
    now();
}

Date::Date(int nbDaysFromNow) noexcept
{
    QDateTime dt = QDateTime::currentDateTime();
    dt = dt.addDays(nbDaysFromNow);
    _year  = dt.date().year();
    _month = dt.date().month();
    _day   = dt.date().day();
}

void Date::now()
{
    QDate d = QDate::currentDate();
    _year  = d.year();
    _month = d.month();
    _day   = d.day();
}

std::string Date::toString() const
{
    char buf[16];
    std::snprintf(buf, sizeof(buf), "%04lu%02lu%02lu", _year, _month, _day);
    return buf;
}

bool Date::operator<(const Date& other) const
{
    if (_year != other._year)  return _year < other._year;
    if (_month != other._month) return _month < other._month;
    return _day < other._day;
}

bool Date::operator==(const Date& other) const
{
    return _year == other._year && _month == other._month && _day == other._day;
}

bool Date::operator<=(const Date& other) const
{
    return *this == other || *this < other;
}

bool Date::operator>(const Date& other) const
{
    return !(*this <= other);
}

bool Date::operator>=(const Date& other) const
{
    return !(*this < other);
}
