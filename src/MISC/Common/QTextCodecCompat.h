// QTextCodec compatibility shim for Qt6+
// Redirects to Qt6Core5Compat which provides the real QTextCodec
#ifndef QTEXTCODEC_COMPAT_H
#define QTEXTCODEC_COMPAT_H

// Qt6Core5Compat provides the real QTextCodec class — use it directly
#include <QtCore5Compat/qtextcodec.h>

#endif
