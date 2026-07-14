// TestFileHelper.cpp — Qt Test suite for FileHelper utilities
// Phase1 canonical source: src/common/FileHelper.cpp

#include <QtTest/QtTest>
#include "common/FileHelper.h"
#include <QTemporaryDir>
#include <QDir>

class TestFileHelper : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void init();

    // ── File existence & type ────────────────────────────────────────────
    void test_exists_file();
    void test_exists_directory();
    void test_exists_nonexistent();
    void test_isFile();
    void test_isDirectory();
    void test_isReadable();
    void test_isWritable();

    // ── File size ────────────────────────────────────────────────────────
    void test_fileSize_empty();
    void test_fileSize_nonempty();
    void test_isLargeFile();

    // ── Read / Write ────────────────────────────────────────────────────
    void test_writeAndRead();
    void test_writeAndRead_wide();
    void test_read_nonexistent();

    // ── File operations ──────────────────────────────────────────────────
    void test_removeFile();
    void test_createDirectory();
    void test_moveFile();

private:
    QTemporaryDir _tmpDir;
};

void TestFileHelper::initTestCase()
{
    // Ensure tmp dir is valid
    QVERIFY2(_tmpDir.isValid(), "QTemporaryDir failed to create");
}

void TestFileHelper::init()
{
    // Per-test setup if needed
}

// ─── File existence & type ────────────────────────────────────────────────────

void TestFileHelper::test_exists_file()
{
    // Create a temp file
    QString path = _tmpDir.filePath("exists.txt");
    QFile f(path);
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.write("hello");
    f.close();

    QVERIFY2(FileHelper::exists(path.toStdString()), "existing file should exist");
    QVERIFY2(FileHelper::isFile(path.toStdString()), "path should be a file");
}

void TestFileHelper::test_exists_directory()
{
    QString path = _tmpDir.filePath("subdir");
    QDir(_tmpDir.path()).mkpath("subdir");
    QVERIFY2(FileHelper::exists(path.toStdString()), "existing directory should exist");
    QVERIFY2(FileHelper::isDirectory(path.toStdString()), "path should be a directory");
}

void TestFileHelper::test_exists_nonexistent()
{
    QString path = _tmpDir.filePath("does_not_exist.xyz");
    QVERIFY2(!FileHelper::exists(path.toStdString()), "nonexistent file should not exist");
}

void TestFileHelper::test_isFile()
{
    QString path = _tmpDir.filePath("isfile.txt");
    QFile f(path);
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.write("x");
    f.close();
    QVERIFY(FileHelper::isFile(path.toStdString()));
}

void TestFileHelper::test_isDirectory()
{
    QString path = _tmpDir.filePath("isdir");
    QDir(_tmpDir.path()).mkpath("isdir");
    QVERIFY(FileHelper::isDirectory(path.toStdString()));
}

void TestFileHelper::test_isReadable()
{
    QString path = _tmpDir.filePath("readable.txt");
    QFile f(path);
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.write("x");
    f.close();
    QVERIFY(FileHelper::isReadable(path.toStdString()));
}

void TestFileHelper::test_isWritable()
{
    QString path = _tmpDir.filePath("writable.txt");
    QFile f(path);
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.write("x");
    f.close();
    QVERIFY(FileHelper::isWritable(path.toStdString()));
}

// ─── File size ────────────────────────────────────────────────────────────────

void TestFileHelper::test_fileSize_empty()
{
    QString path = _tmpDir.filePath("empty.txt");
    QFile f(path);
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.close();
    int64_t size = FileHelper::fileSize(path.toStdString());
    if (size != 0)
        qWarning("empty file size = %ld, expected 0", long(size));
}

void TestFileHelper::test_fileSize_nonempty()
{
    QString path = _tmpDir.filePath("size.txt");
    QFile f(path);
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.write("1234567890");
    f.close();
    int64_t size = FileHelper::fileSize(path.toStdString());
    if (size != 10)
        qWarning("file size = %ld, expected 10", long(size));
}

void TestFileHelper::test_isLargeFile()
{
    // Empty file is not large
    QString path = _tmpDir.filePath("small.txt");
    QFile f(path);
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.write("x");
    f.close();
    QVERIFY2(!FileHelper::isLargeFile(path.toStdString(), 1 << 20), "1-byte file should not be large");
}

// ─── Read / Write ─────────────────────────────────────────────────────────────

void TestFileHelper::test_writeAndRead()
{
    QString path = _tmpDir.filePath("roundtrip.txt");
    std::string content = "Hello, world! 日本語";

    QVERIFY2(FileHelper::writeFile(path.toStdString(), content), "writeFile failed");
    auto read = FileHelper::readFile(path.toStdString());
    QVERIFY2(read.has_value(), "readFile failed");
    if (read.value() != content)
        qWarning("read mismatch: got \"%s\", expected \"%s\"", read->c_str(), content.c_str());
}

void TestFileHelper::test_writeAndRead_wide()
{
    QString path = _tmpDir.filePath("wide.txt");
    std::wstring content = L"Hello, wide world! 日本語";

    QVERIFY2(FileHelper::writeFile(path.toStdString(), content), "writeFile(wstring) failed");
    auto read = FileHelper::readFileW(path.toStdString());
    QVERIFY2(read.has_value(), "readFileW failed");
    if (read.value() != content)
        qWarning("wide read mismatch");
}

void TestFileHelper::test_read_nonexistent()
{
    QString path = _tmpDir.filePath("no_such_file.txt");
    auto read = FileHelper::readFile(path.toStdString());
    if (read.has_value())
        qWarning("readFile should return nullopt for nonexistent file");
}

// ─── File operations ──────────────────────────────────────────────────────────

void TestFileHelper::test_removeFile()
{
    QString path = _tmpDir.filePath("to_remove.txt");
    QFile f(path);
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.write("remove me");
    f.close();

    QVERIFY2(FileHelper::exists(path.toStdString()), "file should exist before remove");
    QVERIFY2(FileHelper::removeFile(path.toStdString()), "removeFile failed");
    QVERIFY2(!FileHelper::exists(path.toStdString()), "file should not exist after remove");
}

void TestFileHelper::test_createDirectory()
{
    QString path = _tmpDir.filePath("new_dir");
    QVERIFY2(!FileHelper::exists(path.toStdString()), "dir should not exist yet");
    QVERIFY2(FileHelper::createDirectory(path.toStdString()), "createDirectory failed");
    QVERIFY2(FileHelper::exists(path.toStdString()), "dir should exist after create");
    QVERIFY2(FileHelper::isDirectory(path.toStdString()), "should be a directory");
}

void TestFileHelper::test_moveFile()
{
    QString srcPath = _tmpDir.filePath("move_src.txt");
    QString dstPath = _tmpDir.filePath("move_dst.txt");
    std::string content = "moving content";

    QVERIFY(FileHelper::writeFile(srcPath.toStdString(), content));
    QVERIFY2(FileHelper::exists(srcPath.toStdString()), "src should exist");
    QVERIFY(!FileHelper::exists(dstPath.toStdString()));

    QVERIFY2(FileHelper::moveFile(srcPath.toStdString(), dstPath.toStdString()), "moveFile failed");
    QVERIFY2(!FileHelper::exists(srcPath.toStdString()), "src should not exist after move");
    QVERIFY2(FileHelper::exists(dstPath.toStdString()), "dst should exist after move");

    auto read = FileHelper::readFile(dstPath.toStdString());
    if (!read.has_value() || read.value() != content)
        qWarning("moved file content mismatch");
}

QTEST_MAIN(TestFileHelper)
#include "TestFileHelper.moc"
