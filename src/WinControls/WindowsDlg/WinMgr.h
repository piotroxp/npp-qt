// This file is part of Notepad++ project
// Copyright (C)2021 Don HO <don.h@free.fr>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <QString>
#include <QVector>
#include <QCoreApplication>

// WinMgr - Window manager utilities (Windows-specific functions)
// Provides Windows API wrappers needed for N++
// Currently cross-platform implementation stub (uses dummy values)
class WinMgr
{
public:
    WinMgr() = default;
    ~WinMgr() = default;

    // Clipboard operations
    static int WINMANAGER_getProcessCodeFromClipboard() {
        // On Windows: GetClipboardFormat
        // Cross-platform stub returns dummy value
        return CF_TEXT;
    }

    static int WINMANAGER_getDDB(QPixmap& hbitmap, int width, int height) {
        // On Windows: CreateCompatibleBitmap / GetDIBits
        // Cross-platform stub - create bitmap from QPixmap
        Q_UNUSED(hbitmap);
        Q_UNUSED(width);
        Q_UNUSED(height);
        return 0;
    }

    static int WINMANAGER_convertPixmap2BMP(const QPixmap& pix2convert) {
        // Convert a QPixmap to BMP format
        // Cross-platform stub: QPixmap already stores BMP data
        Q_UNUSED(pix2convert);
        return 0;
    }

    static std::vector<char> WINMANAGER_getBitmapBits(const QPixmap& hBitmap, int startScan, int maxScan) {
        // Get bits from a device-independent bitmap
        Q_UNUSED(hBitmap);
        Q_UNUSED(startScan);
        Q_UNUSED(maxScan);
        return {};
    }

    // Shell operations
    static bool SHGetKnownFolderPath(const QString& folderName, QString& outPath) {
        // Get known folder path (Windows-only feature)
        // Cross-platform stub returns false
        Q_UNUSED(folderName);
        Q_UNUSED(outPath);
        return false;
    }

    // Process priority
    static bool SetProcessPriority(int priorityClass) {
        // Set process priority (Windows-only)
        Q_UNUSED(priorityClass);
        return false;
    }

    // Locale
    static const char* getLocaleName(int lcType, const char* lpLCData, int cchData) {
        // Get locale name
        Q_UNUSED(lcType);
        Q_UNUSED(lpLCData);
        Q_UNUSED(cchData);
        return QLocale::system().name().toLatin1().constData();
    }

    // Registry
    static bool regSetValue(HKEY hKey, const QString& lpSubKey, const QString& lpValueName, int type, const unsigned char* lpData, int cbData) {
        // Cross-platform: use QSettings instead
    #ifndef _WIN32
        QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
        settings.setValue(lpSubKey + "/" + lpValueName, QString::fromUtf8(reinterpret_cast<const char*>(lpData)));
        return true;
    #else
        Q_UNUSED(hKey);
        Q_UNUSED(lpValueName);
        Q_UNUSED(type);
        Q_UNUSED(lpData);
        Q_UNUSED(cbData);
        return false;
    #endif
    }

    static QByteArray regGetValue(HKEY hKey, const QString& lpSubKey, const QString& lpValueName) {
        // Get registry value
    #ifndef _WIN32
        QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
        return settings.value(lpSubKey + "/" + lpValueName).toByteArray();
    #else
        Q_UNUSED(hKey);
        Q_UNUSED(lpSubKey);
        Q_UNUSED(lpValueName);
        return {};
    #endif
    }

    // Environment variables
    static QString getEnv(const QString& var) {
        return QProcessEnvironment::systemEnvironment().value(var);
    }

    // Shell execute
    static bool shellExec(const QString& app, const QString& args, const QString& operation, const QString& curDir) {
        // Execute a shell command
        Q_UNUSED(app);
        Q_UNUSED(args);
        Q_UNUSED(operation);
        Q_UNUSED(curDir);
        return QDesktopServices::openUrl(QUrl::fromLocalFile(app));
    }
};