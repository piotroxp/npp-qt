#include "MISC/Common/Utf8.h"
#include <codecvt>
#include <locale>

std::string Utf8::encode(std::wstring_view sv) {
    try {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
        return conv.to_bytes(sv.data(), sv.data() + sv.size());
    } catch (...) {
        return {};
    }
}
