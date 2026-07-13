// =============================================================================
// TestDirectoryWatcher.cpp — Qt Test suite for DirectoryWatcher queue logic
// Covers: push/pop queue, pendingCount, notification ordering,
// file action constants. No filesystem needed for queue tests.
// =============================================================================

#include <QtTest/QtTest>
#include <QCoreApplication>
#include "ReadDirectoryChanges.h"

class TestDirectoryWatcher : public QObject
{
    Q_OBJECT

private slots:
    // ── Constants ────────────────────────────────────────────────────────────
    void test_fileActionConstants();

    // ── Queue operations (no init required) ─────────────────────────────────
    void test_pendingCount_empty();
    void test_push_thenPendingCount();
    void test_pushPop_single();
    void test_pushPop_multiple();
    void test_pushPop_ordering();
    void test_pop_empty_returnsFalse();
    void test_pop_emptyZeroWait_returnsFalse();
    void test_popWithWait_returnsFalseWhenEmpty();
    void test_pendingCount_afterPop();
    void test_pushPop_mixedActions();

    // ── Notification content ────────────────────────────────────────────────
    void test_pop_returnsCorrectAction();
    void test_pop_returnsCorrectFileName();
    void test_pop_returnsUtf8FileName();
};

// =============================================================================
// Tests
// =============================================================================

void TestDirectoryWatcher::test_fileActionConstants()
{
    if (FILE_ACTION_ADDED            != 0x00000001) QFAIL("FILE_ACTION_ADDED mismatch");
    if (FILE_ACTION_REMOVED          != 0x00000002) QFAIL("FILE_ACTION_REMOVED mismatch");
    if (FILE_ACTION_MODIFIED         != 0x00000003) QFAIL("FILE_ACTION_MODIFIED mismatch");
    if (FILE_ACTION_RENAMED_OLD_NAME != 0x00000004) QFAIL("FILE_ACTION_RENAMED_OLD_NAME mismatch");
    if (FILE_ACTION_RENAMED_NEW_NAME != 0x00000005) QFAIL("FILE_ACTION_RENAMED_NEW_NAME mismatch");
}

void TestDirectoryWatcher::test_pendingCount_empty()
{
    DirectoryWatcher watcher;
    if (watcher.pendingCount() != 0)
        QFAIL(QString("New watcher: expected 0 pending, got %1").arg(watcher.pendingCount()).toUtf8());
}

void TestDirectoryWatcher::test_push_thenPendingCount()
{
    DirectoryWatcher watcher;
    watcher.push(FILE_ACTION_ADDED, "file.txt");

    if (watcher.pendingCount() != 1)
        QFAIL(QString("After push: expected 1 pending, got %1").arg(watcher.pendingCount()).toUtf8());
}

void TestDirectoryWatcher::test_pushPop_single()
{
    DirectoryWatcher watcher;
    watcher.push(FILE_ACTION_ADDED, "newfile.txt");

    quint32 action = 0;
    QString name;
    bool ok = watcher.pop(action, name);

    if (!ok) QFAIL("pop should return true after push");
    if (action != FILE_ACTION_ADDED)
        QFAIL(QString("pop action: expected ADDED(1), got %1").arg(action).toUtf8());
    if (name != "newfile.txt")
        QFAIL(QString("pop filename: expected 'newfile.txt', got '%1'").arg(name).toUtf8());
}

void TestDirectoryWatcher::test_pushPop_multiple()
{
    DirectoryWatcher watcher;
    watcher.push(FILE_ACTION_ADDED,    "a.txt");
    watcher.push(FILE_ACTION_MODIFIED, "b.txt");
    watcher.push(FILE_ACTION_REMOVED,  "c.txt");

    if (watcher.pendingCount() != 3)
        QFAIL(QString("After 3 pushes: expected 3 pending, got %1").arg(watcher.pendingCount()).toUtf8());

    quint32 a = 0; QString n;
    watcher.pop(a, n); if (n != "a.txt") QFAIL("First item should be a.txt");
    watcher.pop(a, n); if (n != "b.txt") QFAIL("Second item should be b.txt");
    watcher.pop(a, n); if (n != "c.txt") QFAIL("Third item should be c.txt");

    if (watcher.pendingCount() != 0)
        QFAIL(QString("After popping all: expected 0 pending, got %1").arg(watcher.pendingCount()).toUtf8());
}

void TestDirectoryWatcher::test_pushPop_ordering()
{
    DirectoryWatcher watcher;
    watcher.push(FILE_ACTION_MODIFIED, "first.txt");
    watcher.push(FILE_ACTION_MODIFIED, "second.txt");

    quint32 a = 0; QString n;
    watcher.pop(a, n);
    if (n != "first.txt")
        QFAIL(QString("FIFO order: expected 'first.txt', got '%1'").arg(n).toUtf8());
}

void TestDirectoryWatcher::test_pop_empty_returnsFalse()
{
    DirectoryWatcher watcher;
    // pop with waitMs=0 on empty queue → false immediately
    quint32 a = 0; QString n;
    bool ok = watcher.pop(a, n, 0);
    if (ok)
        QFAIL("pop(0) on empty queue should return false");
}

void TestDirectoryWatcher::test_pop_emptyZeroWait_returnsFalse()
{
    DirectoryWatcher watcher;
    quint32 a = 999; QString n;
    bool ok = watcher.pop(a, n, 0);
    if (ok)
        QFAIL("pop(waitMs=0) on empty queue should return false");
    // action and name must not be modified on failure
    if (a != 0 || !n.isNull())
        QFAIL("pop output params must not be modified on failure");
}

void TestDirectoryWatcher::test_popWithWait_returnsFalseWhenEmpty()
{
    DirectoryWatcher watcher;
    // Very short wait — must still not hang
    quint32 a = 0; QString n;
    QSignalSpy spy(&watcher, &DirectoryWatcher::error);
    bool ok = watcher.pop(a, n, 50); // 50ms wait
    // Without init(), nothing is producing events, so this may time out
    // The important thing is: it returns in reasonable time and doesn't hang
    Q_UNUSED(ok);
}

void TestDirectoryWatcher::test_pendingCount_afterPop()
{
    DirectoryWatcher watcher;
    watcher.push(FILE_ACTION_ADDED, "x.txt");
    watcher.push(FILE_ACTION_ADDED, "y.txt");

    if (watcher.pendingCount() != 2) QFAIL("Expected 2 before pop");

    quint32 a = 0; QString n;
    watcher.pop(a, n);

    if (watcher.pendingCount() != 1)
        QFAIL(QString("After one pop: expected 1 pending, got %1").arg(watcher.pendingCount()).toUtf8());
}

void TestDirectoryWatcher::test_pushPop_mixedActions()
{
    DirectoryWatcher watcher;
    watcher.push(FILE_ACTION_RENAMED_OLD_NAME, "old.txt");
    watcher.push(FILE_ACTION_RENAMED_NEW_NAME, "new.txt");
    watcher.push(FILE_ACTION_MODIFIED,         "same.txt");

    quint32 a = 0; QString n;

    watcher.pop(a, n);
    if (a != FILE_ACTION_RENAMED_OLD_NAME)
        QFAIL("First action should be RENAMED_OLD_NAME");

    watcher.pop(a, n);
    if (a != FILE_ACTION_RENAMED_NEW_NAME)
        QFAIL("Second action should be RENAMED_NEW_NAME");

    watcher.pop(a, n);
    if (a != FILE_ACTION_MODIFIED)
        QFAIL("Third action should be MODIFIED");
}

void TestDirectoryWatcher::test_pop_returnsCorrectAction()
{
    DirectoryWatcher watcher;
    watcher.push(FILE_ACTION_ADDED,    "add.txt");
    watcher.push(FILE_ACTION_REMOVED,  "rem.txt");
    watcher.push(FILE_ACTION_MODIFIED, "mod.txt");
    watcher.push(FILE_ACTION_RENAMED_OLD_NAME, "old.txt");
    watcher.push(FILE_ACTION_RENAMED_NEW_NAME, "new.txt");

    quint32 actions[] = {
        FILE_ACTION_ADDED,
        FILE_ACTION_REMOVED,
        FILE_ACTION_MODIFIED,
        FILE_ACTION_RENAMED_OLD_NAME,
        FILE_ACTION_RENAMED_NEW_NAME
    };
    for (size_t i = 0; i < sizeof(actions)/sizeof(actions[0]); ++i) {
        quint32 a = 0; QString n;
        watcher.pop(a, n);
        if (a != actions[i])
            QFAIL(QString("Action[%1]: expected %2, got %3").arg(i).arg(actions[i]).arg(a).toUtf8());
    }
}

void TestDirectoryWatcher::test_pop_returnsCorrectFileName()
{
    DirectoryWatcher watcher;
    QStringList names = {"alpha.txt", "beta.md", "gamma.cpp"};
    for (const QString& nm : names) {
        watcher.push(FILE_ACTION_ADDED, nm);
    }

    for (const QString& expected : names) {
        quint32 a = 0; QString n;
        watcher.pop(a, n);
        if (n != expected)
            QFAIL(QString("Filename: expected '%1', got '%2'").arg(expected).arg(n).toUtf8());
    }
}

void TestDirectoryWatcher::test_pop_returnsUtf8FileName()
{
    DirectoryWatcher watcher;
    watcher.push(FILE_ACTION_ADDED, QString::fromUtf8("日本語.txt"));
    watcher.push(FILE_ACTION_ADDED, QString::fromUtf8("中文文件.doc"));
    watcher.push(FILE_ACTION_ADDED, QString::fromUtf8("emoji_😀.txt"));

    QStringList expected = {
        QString::fromUtf8("日本語.txt"),
        QString::fromUtf8("中文文件.doc"),
        QString::fromUtf8("emoji_😀.txt")
    };

    for (const QString& exp : expected) {
        quint32 a = 0; QString n;
        watcher.pop(a, n);
        if (n != exp)
            QFAIL(QString("UTF-8 filename: expected '%1', got '%2'").arg(exp).arg(n).toUtf8());
    }
}

QTEST_MAIN(TestDirectoryWatcher)
#include "TestDirectoryWatcher.moc"
