// Semantic lift: clipboardFormats.h — Win32 clipboard formats → Qt clipboard
#pragma once

#include <QString>

// NPP clipboard format names (for plugin communication)
namespace ClipboardFormats
{
    const QString NPPDocument = QStringLiteral("NppDocument");
    const QString NppSelection = QStringLiteral("NppSelection");
    const QString NppLink = QStringLiteral("NppLink");
    const QString NppInternal = QStringLiteral("NppInternal");
}

