// MISC/Exception/Win32Exception.h - Qt6 port of Win32 exception handling
#pragma once

#include <exception>
#include <QString>

typedef const void* ExceptionAddress;

class Win32Exception : public std::exception
{
public:
    static void installHandler();
    static void removeHandler();
    
    virtual const char* what() const throw() override { return _event.toLocal8Bit().constData(); }
    ExceptionAddress where() const { return _location; }
    unsigned int code() const { return _code; }

protected:
    explicit Win32Exception(unsigned int code, ExceptionAddress location, const QString& event)
        : _code(code), _location(location), _event(event) {}

    static void translate(unsigned code, void* info);

private:
    QString _event;
    ExceptionAddress _location;
    unsigned int _code = 0;
};

class Win32AccessViolation : public Win32Exception
{
public:
    bool isWrite() const { return _isWrite; }
    ExceptionAddress badAddress() const { return _badAddress; }

private:
    explicit Win32AccessViolation(unsigned int code, ExceptionAddress location, bool isWrite, ExceptionAddress badAddress, const QString& event)
        : Win32Exception(code, location, event), _isWrite(isWrite), _badAddress(badAddress) {}

    bool _isWrite = false;
    ExceptionAddress _badAddress;

    friend void Win32Exception::translate(unsigned code, void* info);
};