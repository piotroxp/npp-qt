#pragma once

#include <QString>
#include <QStringList>
#include <QMetaType>

// Forward declare to avoid pulling in full scintilla headers
struct SCNotification;

class NppNotification
{
public:
    // Emulate Windows NMHDR
    struct NMHDR {
        void* hwndFrom = nullptr;  // Not used in Qt
        int idFrom = 0;
        int code = 0;
    };

    // Scintilla notification data
    NMHDR nmhdr;
    int position = 0;
    int ch = 0;
    int modifiers = 0;
    int modificationType = 0;
    QString text;
    int length = 0;
    int linesAdded = 0;
    int message = 0;
    long long wParam = 0;
    long long lParam = 0;
    int line = 0;
    int foldLevelNow = 0;
    int foldLevelPrev = 0;
    int spaceBefore = 0;
    int linesVisible = 0;
    int wrappedLine = 0;
    int wrappedLineModified = 0;
    int id = 0;
    void* userData = nullptr;

    void fromScintilla(int id, void* pSC);
    void beNotifiedOf(SCNotification* scn);
};

// Central notification hub
class NppNotificationHub : public QObject
{
    Q_OBJECT
public:
    explicit NppNotificationHub(QObject* parent = nullptr);
    void notify(SCNotification* scn);
signals:
    void nppNotification(void* scn);
};

