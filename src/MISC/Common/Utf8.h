// This file is part of Notepad++ project
// Stub for UTF-8 encoding helpers on non-Windows

#pragma once
#include <string>
#include <string_view>

class Utf8 final
{
public:
    static std::string encode(std::wstring_view sv);
};
