// QTextCodec compatibility shim for Qt6+
// In Qt6, QTextCodec was replaced with QStringConverter

#ifndef QTEXTCODEC_COMPAT_H
#define QTEXTCODEC_COMPAT_H

#include <QtCore>
#include <QStringConverter>

// Stub QTextCodec for Qt6 
class QTextCodec {
public:
    // Stub codec - identity mapping
    static QTextCodec* codecForName(const QByteArray&) { return nullptr; }
    static QTextCodec* codecForHtml(const QByteArray&) { return nullptr; }
    static QTextCodec* latin1() { return nullptr; }
    static QTextCodec* utf8() { return nullptr; }
    
    // Dummy conversion - just returns UTF-8 compatible input (QString handles encoding)
    static QString toUnicode(const QByteArray& ba) {
        return QString::fromUtf8(ba);
    }
    
    static QByteArray fromUnicode(const QString& str) {
        return str.toUtf8();
    }
};

#endif
