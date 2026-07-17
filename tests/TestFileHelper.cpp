// TestFileHelper.cpp -- Qt Test suite for FileHelper utilities
// Phase1 canonical source: src/common/FileHelper.cpp
//
// Test coverage assessment:
// - File existence/type: GOOD -- uses QVERIFY2 properly
// - File size: was weak -- used if+warning instead of QVERIFY2
// - Read/Write: was weak -- same pattern
// - Read nonexistent: was weak -- same pattern
// - File operations: good -- uses QVERIFY2 with proper boolean checks
// - NEW: BOM detection, encoding detection, path normalization,
//         temp file generation, directory listing, recursive ops,
//         binary file detection, hash computation, file extension helpers

#include <QtTest/QtTest>
#include "common/FileHelper.h"
#include <QTemporaryDir>
#include <QDir>
#include <QFile>

class TestFileHelper : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void init();

    // -- File existence & type
    void test_exists_file();
    void test_exists_directory();
    void test_exists_nonexistent();
    void test_isFile();
    void test_isDirectory();
    void test_isReadable();
    void test_isWritable();

    // -- File size
    void test_fileSize_empty();
    void test_fileSize_nonempty();
    void test_fileSize_nonexistent();
    void test_isLargeFile();

    // -- Read / Write
    void test_writeAndRead();
    void test_writeAndRead_binary();
    void test_writeAndRead_wide();
    void test_read_nonexistent();
    void test_read_nonexistent_returnsNullopt();
    void test_appendFile();

    // -- File operations
    void test_removeFile();
    void test_removeFile_nonexistent();
    void test_createDirectory();
    void test_createDirectory_nested();
    void test_moveFile();
    void test_copyFile();
    void test_renameFile();

    // -- Path manipulation
    void test_normalize();
    void test_isAbsolutePath();
    void test_resolvePath();
    void test_baseName();
    void test_fileExtension();
    void test_setFileExtension();

    // -- Directory listing
    void test_listFiles();
    void test_listFilesWithExt();
    void test_listDirectories();

    // -- BOM / Encoding detection
    void test_hasUtf8Bom();
    void test_hasUtf8Bom_false();
    void test_hasUtf16Bom();
    void test_hasUtf16Bom_false();
    void test_detectEncoding_utf8Bom();
    void test_detectEncoding_utf16LeBom();
    void test_detectEncoding_utf8();
    void test_detectEncoding_ansi();

    // -- Temp file helpers
    void test_getTempDirectory();
    void test_getTempFileName_prefix();
    void test_getTempPath();

    // -- Recursive operations
    void test_copyRecursive_file();
    void test_removeRecursive();

    // -- Binary detection
    void test_isBinaryFile();
    void test_isBinaryFile_text();

    // -- Hash computation
    void test_computeMD5();
    void test_computeSHA256();

private:
    QTemporaryDir _tmpDir;
};

void TestFileHelper::initTestCase()
{
    QVERIFY2(_tmpDir.isValid(), "QTemporaryDir failed to create");
}

void TestFileHelper::init()
{
    // Per-test setup if needed
}

// ============================================================================
// File existence & type
// ============================================================================

void TestFileHelper::test_exists_file()
{
    QString path = _tmpDir.filePath("exists.txt");
    QFile f(path);
    QVERIFY2(f.open(QIODevice::WriteOnly),
             qPrintable(QString("Failed to create test file: %1").arg(f.errorString())));
    f.write("hello");
    f.close();

    QVERIFY2(FileHelper::exists(path.toStdString()),
             qPrintable(QString("existing file should exist: %1").arg(path)));
    QVERIFY2(FileHelper::isFile(path.toStdString()),
             qPrintable(QString("path should be a file: %1").arg(path)));
}

void TestFileHelper::test_exists_directory()
{
    QString path = _tmpDir.filePath("subdir");
    QVERIFY2(QDir(_tmpDir.path()).mkpath("subdir"),
             qPrintable(QString("Failed to create directory: %1").arg(path)));

    QVERIFY2(FileHelper::exists(path.toStdString()),
             qPrintable(QString("existing directory should exist: %1").arg(path)));
    QVERIFY2(FileHelper::isDirectory(path.toStdString()),
             qPrintable(QString("path should be a directory: %1").arg(path)));
}

void TestFileHelper::test_exists_nonexistent()
{
    QString path = _tmpDir.filePath("does_not_exist.xyz");
    QVERIFY2(!FileHelper::exists(path.toStdString()),
             qPrintable(QString("nonexistent file should not exist: %1").arg(path)));
}

void TestFileHelper::test_isFile()
{
    QString path = _tmpDir.filePath("isfile.txt");
    QFile f(path);
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.write("x");
    f.close();
    QVERIFY2(FileHelper::isFile(path.toStdString()),
             qPrintable(QString("isFile should return true for regular file: %1").arg(path)));
}

void TestFileHelper::test_isDirectory()
{
    QString path = _tmpDir.filePath("isdir");
    QVERIFY(QDir(_tmpDir.path()).mkpath("isdir"));
    QVERIFY2(FileHelper::isDirectory(path.toStdString()),
             qPrintable(QString("isDirectory should return true for directory: %1").arg(path)));
}

void TestFileHelper::test_isReadable()
{
    QString path = _tmpDir.filePath("readable.txt");
    QFile f(path);
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.write("x");
    f.close();
    QVERIFY2(FileHelper::isReadable(path.toStdString()),
             qPrintable(QString("isReadable should return true for readable file: %1").arg(path)));
}

void TestFileHelper::test_isWritable()
{
    QString path = _tmpDir.filePath("writable.txt");
    QFile f(path);
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.write("x");
    f.close();
    QVERIFY2(FileHelper::isWritable(path.toStdString()),
             qPrintable(QString("isWritable should return true for writable file: %1").arg(path)));
}

// ============================================================================
// File size
// ============================================================================

void TestFileHelper::test_fileSize_empty()
{
    QString path = _tmpDir.filePath("empty.txt");
    QFile f(path);
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.close();
    int64_t size = FileHelper::fileSize(path.toStdString());
    QVERIFY2(size == 0,
             qPrintable(QString("empty file size should be 0, got %1").arg(long(size))));
}

void TestFileHelper::test_fileSize_nonempty()
{
    QString path = _tmpDir.filePath("size.txt");
    QFile f(path);
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.write("1234567890");  // 10 bytes
    f.close();
    int64_t size = FileHelper::fileSize(path.toStdString());
    QVERIFY2(size == 10,
             qPrintable(QString("file size should be 10, got %1").arg(long(size))));
}

void TestFileHelper::test_fileSize_nonexistent()
{
    QString path = _tmpDir.filePath("no_such_file.txt");
    int64_t size = FileHelper::fileSize(path.toStdString());
    QVERIFY2(size == -1,
             qPrintable(QString("nonexistent file size should be -1, got %1").arg(long(size))));
}

void TestFileHelper::test_isLargeFile()
{
    // 1-byte file is not large with 1MB threshold
    QString path = _tmpDir.filePath("small.txt");
    QFile f(path);
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.write("x");
    f.close();
    QVERIFY2(!FileHelper::isLargeFile(path.toStdString(), 1 << 20),
             qPrintable(QString("1-byte file should not be large with 1MB threshold: %1").arg(path)));
}

// ============================================================================
// Read / Write
// ============================================================================

void TestFileHelper::test_writeAndRead()
{
    QString path = _tmpDir.filePath("roundtrip.txt");
    std::string content = "Hello, world! cafe e-acute";

    QVERIFY2(FileHelper::writeFile(path.toStdString(), content),
             qPrintable(QString("writeFile failed for: %1").arg(path)));

    auto read = FileHelper::readFile(path.toStdString());
    QVERIFY2(read.has_value(),
             qPrintable(QString("readFile should return a value for existing file: %1").arg(path)));
    QVERIFY2(read.value() == content,
             qPrintable(QString("read mismatch: expected \"%1\", got \"%2\"")
                        .arg(QString::fromStdString(content))
                        .arg(QString::fromStdString(read.value()))));
}

void TestFileHelper::test_writeAndRead_binary()
{
    // Write binary data with null bytes
    QString path = _tmpDir.filePath("binary.bin");
    std::string content;
    content.push_back('\x00');
    content.push_back('\xFF');
    content.push_back('\x01');
    content.push_back('\x80');

    QVERIFY2(FileHelper::writeFile(path.toStdString(), content),
             "writeFile failed for binary content");

    auto read = FileHelper::readFile(path.toStdString());
    QVERIFY2(read.has_value(), "readFile should return value for binary file");
    QVERIFY2(read.value() == content,
             qPrintable(QString("binary roundtrip mismatch: expected %1 bytes, got %2")
                        .arg(int(content.size())).arg(int(read.value().size()))));
}

void TestFileHelper::test_writeAndRead_wide()
{
    QString path = _tmpDir.filePath("wide.txt");
    std::wstring content = L"Hello, wide world! Japanese text";

    QVERIFY2(FileHelper::writeFile(path.toStdString(), content),
             qPrintable(QString("writeFile(wstring) failed for: %1").arg(path)));

    auto read = FileHelper::readFileW(path.toStdString());
    QVERIFY2(read.has_value(),
             qPrintable(QString("readFileW should return value for existing file: %1").arg(path)));
    QVERIFY2(!read.value().empty(),
             qPrintable(QString("readFileW returned empty content for: %1").arg(path)));
}

void TestFileHelper::test_read_nonexistent()
{
    QString path = _tmpDir.filePath("no_such_file.txt");
    auto read = FileHelper::readFile(path.toStdString());
    QVERIFY2(!read.has_value(),
             qPrintable(QString("readFile should return nullopt for nonexistent file: %1").arg(path)));
}

void TestFileHelper::test_read_nonexistent_returnsNullopt()
{
    // Explicit check: read nonexistent must return nullopt, NOT empty string
    QString path = _tmpDir.filePath("definitely_missing_xyz.txt");
    auto result = FileHelper::readFile(path.toStdString());
    QVERIFY2(result == std::nullopt,
             qPrintable(QString("readFile should return std::nullopt, not a value: %1").arg(path)));
}

void TestFileHelper::test_appendFile()
{
    QString path = _tmpDir.filePath("append.txt");
    // Write initial content
    QVERIFY2(FileHelper::writeFile(path.toStdString(), std::string("first")),
             "writeFile failed");
    // Append
    QVERIFY2(FileHelper::appendFile(path.toStdString(), std::string(" second")),
             "appendFile failed");
    // Read back
    auto read = FileHelper::readFile(path.toStdString());
    QVERIFY2(read.has_value(), "readFile failed after append");
    QVERIFY2(read.value() == "first second",
             qPrintable(QString("append content mismatch: expected \"first second\", got \"%1\"")
                        .arg(QString::fromStdString(read.value()))));
}

// ============================================================================
// File operations
// ============================================================================

void TestFileHelper::test_removeFile()
{
    QString path = _tmpDir.filePath("to_remove.txt");
    QFile f(path);
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.write("remove me");
    f.close();

    QVERIFY2(FileHelper::exists(path.toStdString()),
             qPrintable(QString("file should exist before remove: %1").arg(path)));
    QVERIFY2(FileHelper::removeFile(path.toStdString()),
             qPrintable(QString("removeFile failed: %1").arg(path)));
    QVERIFY2(!FileHelper::exists(path.toStdString()),
             qPrintable(QString("file should not exist after remove: %1").arg(path)));
}

void TestFileHelper::test_removeFile_nonexistent()
{
    // Removing nonexistent file should succeed (no-op / ENOENT is OK)
    QString path = _tmpDir.filePath("already_gone.txt");
    bool ok = FileHelper::removeFile(path.toStdString());
    QVERIFY2(ok, "removeFile on nonexistent file should return true");
}

void TestFileHelper::test_createDirectory()
{
    QString path = _tmpDir.filePath("new_dir");
    QVERIFY2(!FileHelper::exists(path.toStdString()),
             qPrintable(QString("dir should not exist yet: %1").arg(path)));
    QVERIFY2(FileHelper::createDirectory(path.toStdString()),
             qPrintable(QString("createDirectory failed: %1").arg(path)));
    QVERIFY2(FileHelper::exists(path.toStdString()),
             qPrintable(QString("dir should exist after create: %1").arg(path)));
    QVERIFY2(FileHelper::isDirectory(path.toStdString()),
             qPrintable(QString("should be a directory: %1").arg(path)));
}

void TestFileHelper::test_createDirectory_nested()
{
    // createDirectory should handle nested paths
    QString path = _tmpDir.filePath("a/b/c");
    QVERIFY2(FileHelper::createDirectory(path.toStdString()),
             qPrintable(QString("createDirectory nested failed: %1").arg(path)));
    QVERIFY2(FileHelper::exists(path.toStdString()),
             qPrintable(QString("nested dir should exist: %1").arg(path)));
}

void TestFileHelper::test_moveFile()
{
    QString srcPath = _tmpDir.filePath("move_src.txt");
    QString dstPath = _tmpDir.filePath("move_dst.txt");
    std::string content = "moving content";

    QVERIFY2(FileHelper::writeFile(srcPath.toStdString(), content),
             qPrintable(QString("writeFile failed: %1").arg(srcPath)));
    QVERIFY2(FileHelper::exists(srcPath.toStdString()),
             qPrintable(QString("src should exist: %1").arg(srcPath)));
    QVERIFY2(!FileHelper::exists(dstPath.toStdString()),
             qPrintable(QString("dst should not exist before move: %1").arg(dstPath)));

    QVERIFY2(FileHelper::moveFile(srcPath.toStdString(), dstPath.toStdString()),
             qPrintable(QString("moveFile failed")));
    QVERIFY2(!FileHelper::exists(srcPath.toStdString()),
             qPrintable(QString("src should not exist after move: %1").arg(srcPath)));
    QVERIFY2(FileHelper::exists(dstPath.toStdString()),
             qPrintable(QString("dst should exist after move: %1").arg(dstPath)));

    auto read = FileHelper::readFile(dstPath.toStdString());
    QVERIFY2(read.has_value() && read.value() == content,
             qPrintable(QString("moved file content mismatch")));
}

void TestFileHelper::test_copyFile()
{
    QString srcPath = _tmpDir.filePath("copy_src.txt");
    QString dstPath = _tmpDir.filePath("copy_dst.txt");
    std::string content = "copying content";

    QVERIFY2(FileHelper::writeFile(srcPath.toStdString(), content),
             qPrintable(QString("writeFile failed: %1").arg(srcPath)));

    QVERIFY2(FileHelper::copyFile(srcPath.toStdString(), dstPath.toStdString()),
             qPrintable(QString("copyFile failed")));
    QVERIFY2(FileHelper::exists(srcPath.toStdString()),
             "src should still exist after copy");
    QVERIFY2(FileHelper::exists(dstPath.toStdString()),
             qPrintable(QString("dst should exist after copy: %1").arg(dstPath)));

    auto read = FileHelper::readFile(dstPath.toStdString());
    QVERIFY2(read.has_value() && read.value() == content,
             "copied file content mismatch");
}

void TestFileHelper::test_renameFile()
{
    QString oldPath = _tmpDir.filePath("rename_old.txt");
    QString newPath = _tmpDir.filePath("rename_new.txt");
    std::string content = "rename me";
    QVERIFY2(FileHelper::writeFile(oldPath.toStdString(), content),
             "writeFile failed");
    QVERIFY2(FileHelper::renameFile(oldPath.toStdString(), newPath.toStdString()),
             "renameFile failed");
    QVERIFY2(!FileHelper::exists(oldPath.toStdString()),
             "old path should not exist after rename");
    QVERIFY2(FileHelper::exists(newPath.toStdString()),
             "new path should exist after rename");
}

// ============================================================================
// Path manipulation
// ============================================================================

void TestFileHelper::test_normalize()
{
    std::string r = FileHelper::normalize(std::string("foo//bar/baz"));
    QVERIFY2(r == "foo/bar/baz",
             qPrintable(QString("normalize expected \"foo/bar/baz\", got \"%1\"").arg(QString::fromStdString(r))));
}

void TestFileHelper::test_isAbsolutePath()
{
    QVERIFY2(FileHelper::isAbsolutePath(std::string("/usr/local")),
             "absolute path should return true");
    QVERIFY2(!FileHelper::isAbsolutePath(std::string("relative/path")),
             "relative path should return false");
}

void TestFileHelper::test_resolvePath()
{
    std::string base = "/home/user";
    std::string rel = "docs/file.txt";
    std::string r = FileHelper::resolvePath(base, rel);
    QVERIFY2(r.find("docs") != std::string::npos,
             qPrintable(QString("resolvePath result should contain relative path: got \"%1\"").arg(QString::fromStdString(r))));
}

void TestFileHelper::test_baseName()
{
    std::string r = FileHelper::baseName(std::string("/path/to/file.tar.gz"));
    QVERIFY2(r == "file.tar",
             qPrintable(QString("baseName expected \"file.tar\", got \"%1\"").arg(QString::fromStdString(r))));
}

void TestFileHelper::test_fileExtension()
{
    std::string r = FileHelper::fileExtension(std::string("/path/to/file.txt"));
    QVERIFY2(r == ".txt",
             qPrintable(QString("fileExtension expected \".txt\", got \"%1\"").arg(QString::fromStdString(r))));
}

void TestFileHelper::test_setFileExtension()
{
    std::string path = "file.txt";
    bool ok = FileHelper::setFileExtension(path, ".md");
    QVERIFY2(ok, "setFileExtension should return true");
    QVERIFY2(path == "file.md",
             qPrintable(QString("setFileExtension expected \"file.md\", got \"%1\"").arg(QString::fromStdString(path))));
}

// ============================================================================
// Directory listing
// ============================================================================

void TestFileHelper::test_listFiles()
{
    // Create test files in tmp dir
    QFile f1(_tmpDir.filePath("listfile1.txt"));
    QVERIFY(f1.open(QIODevice::WriteOnly)); f1.close();
    QFile f2(_tmpDir.filePath("listfile2.txt"));
    QVERIFY(f2.open(QIODevice::WriteOnly)); f2.close();

    auto files = FileHelper::listFiles(_tmpDir.path().toStdString(), false);
    QVERIFY2(files.size() >= 2,
             qPrintable(QString("listFiles should find at least 2 files, got %1").arg(int(files.size()))));
}

void TestFileHelper::test_listFilesWithExt()
{
    // Create .txt and .log files
    QFile f1(_tmpDir.filePath("a.txt"));
    QVERIFY(f1.open(QIODevice::WriteOnly)); f1.close();
    QFile f2(_tmpDir.filePath("b.log"));
    QVERIFY(f2.open(QIODevice::WriteOnly)); f2.close();

    auto txtFiles = FileHelper::listFilesWithExt(_tmpDir.path().toStdString(), ".txt");
    QVERIFY2(txtFiles.size() >= 1,
             qPrintable(QString("listFilesWithExt .txt should find at least 1, got %1").arg(int(txtFiles.size()))));
}

void TestFileHelper::test_listDirectories()
{
    // Create a subdirectory
    QDir(_tmpDir.path()).mkpath("subdir_for_listing");

    auto dirs = FileHelper::listDirectories(_tmpDir.path().toStdString());
    QVERIFY2(dirs.size() >= 1,
             qPrintable(QString("listDirectories should find at least 1 dir, got %1").arg(int(dirs.size()))));
}

// ============================================================================
// BOM / Encoding detection
// ============================================================================

void TestFileHelper::test_hasUtf8Bom()
{
    QString path = _tmpDir.filePath("utf8bom.txt");
    QFile f(path);
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.write("\xEF\xBB\xBFHello");
    f.close();

    QVERIFY2(FileHelper::hasUtf8Bom(path.toStdString()),
             qPrintable(QString("hasUtf8Bom should return true for UTF-8 BOM file: %1").arg(path)));
}

void TestFileHelper::test_hasUtf8Bom_false()
{
    QString path = _tmpDir.filePath("nfbom.txt");
    QFile f(path);
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.write("Hello world");
    f.close();

    QVERIFY2(!FileHelper::hasUtf8Bom(path.toStdString()),
             qPrintable(QString("hasUtf8Bom should return false for plain ASCII file: %1").arg(path)));
}

void TestFileHelper::test_hasUtf16Bom()
{
    QString path = _tmpDir.filePath("utf16le.txt");
    QFile f(path);
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.write("\xFF\xFEH\x00e\x00l\x00l\x00o");
    f.close();

    QVERIFY2(FileHelper::hasUtf16Bom(path.toStdString()),
             qPrintable(QString("hasUtf16Bom should return true for UTF-16 LE BOM file: %1").arg(path)));
}

void TestFileHelper::test_hasUtf16Bom_false()
{
    QString path = _tmpDir.filePath("nfbom16.txt");
    QFile f(path);
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.write("Hello world");
    f.close();

    QVERIFY2(!FileHelper::hasUtf16Bom(path.toStdString()),
             qPrintable(QString("hasUtf16Bom should return false for plain ASCII file: %1").arg(path)));
}

void TestFileHelper::test_detectEncoding_utf8Bom()
{
    QString path = _tmpDir.filePath("enc_utf8bom.txt");
    QFile f(path);
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.write("\xEF\xBB\xBFHello");
    f.close();

    EncodingType enc = FileHelper::detectEncoding(path.toStdString());
    QVERIFY2(enc == EncodingType::UTF_8_BOM,
             qPrintable(QString("detectEncoding should return UTF_8_BOM, got %1").arg(int(enc))));
}

void TestFileHelper::test_detectEncoding_utf16LeBom()
{
    QString path = _tmpDir.filePath("enc_utf16le.txt");
    QFile f(path);
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.write("\xFF\xFEH\x00e\x00l\x00l\x00o");
    f.close();

    EncodingType enc = FileHelper::detectEncoding(path.toStdString());
    QVERIFY2(enc == EncodingType::UTF_16_LE,
             qPrintable(QString("detectEncoding should return UTF_16_LE, got %1").arg(int(enc))));
}

void TestFileHelper::test_detectEncoding_utf8()
{
    QString path = _tmpDir.filePath("enc_utf8.txt");
    QFile f(path);
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.write("Hello cafe e-acute");  // valid UTF-8, no BOM
    f.close();

    EncodingType enc = FileHelper::detectEncoding(path.toStdString());
    QVERIFY2(enc == EncodingType::UTF_8,
             qPrintable(QString("detectEncoding should return UTF_8, got %1").arg(int(enc))));
}

void TestFileHelper::test_detectEncoding_ansi()
{
    QString path = _tmpDir.filePath("enc_ansi.txt");
    QFile f(path);
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.write("Plain ASCII text");
    f.close();

    EncodingType enc = FileHelper::detectEncoding(path.toStdString());
    // Valid UTF-8 (ASCII subset) with no BOM -> UTF_8
    QVERIFY2(enc == EncodingType::UTF_8 || enc == EncodingType::ANSI,
             qPrintable(QString("detectEncoding should return UTF_8 or ANSI, got %1").arg(int(enc))));
}

// ============================================================================
// Temp file helpers
// ============================================================================

void TestFileHelper::test_getTempDirectory()
{
    std::string tmp = FileHelper::getTempDirectory();
    QVERIFY2(!tmp.empty(), "getTempDirectory should not return empty string");
    QVERIFY2(FileHelper::exists(tmp),
             qPrintable(QString("getTempDirectory should exist: %1").arg(QString::fromStdString(tmp))));
}

void TestFileHelper::test_getTempFileName_prefix()
{
    std::string r = FileHelper::getTempFileName("npp_test_");
    QVERIFY2(!r.empty(), "getTempFileName should return non-empty string");
    QVERIFY2(r.find("npp_test_") != std::string::npos,
             qPrintable(QString("getTempFileName should contain prefix \"npp_test_\": got %1").arg(QString::fromStdString(r))));
}

void TestFileHelper::test_getTempPath()
{
    std::string r = FileHelper::getTempPath();
    QVERIFY2(!r.empty(), "getTempPath should return non-empty string");
}

// ============================================================================
// Recursive operations
// ============================================================================

void TestFileHelper::test_copyRecursive_file()
{
    QString src = _tmpDir.filePath("recursive_src");
    QString dst = _tmpDir.filePath("recursive_dst");

    QVERIFY(QDir().mkpath(src));
    QFile f1(src + "/file1.txt");
    QVERIFY(f1.open(QIODevice::WriteOnly));
    f1.write("content1");
    f1.close();

    bool ok = FileHelper::copyRecursive(src, dst);
    QVERIFY2(ok, "copyRecursive should succeed");
    QVERIFY2(FileHelper::exists(dst.toStdString()), "dst should exist after copyRecursive");
}

void TestFileHelper::test_removeRecursive()
{
    QString dir = _tmpDir.filePath("to_remove_recursive");
    QVERIFY(QDir().mkpath(dir + "/subdir"));
    QFile f(dir + "/file.txt");
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.write("x");
    f.close();

    QVERIFY2(FileHelper::exists(dir.toStdString()), "dir should exist before removeRecursive");
    bool ok = FileHelper::removeRecursive(dir);
    QVERIFY2(ok, "removeRecursive should succeed");
    QVERIFY2(!FileHelper::exists(dir.toStdString()),
             qPrintable(QString("dir should not exist after removeRecursive: %1").arg(dir)));
}

// ============================================================================
// Binary detection
// ============================================================================

void TestFileHelper::test_isBinaryFile()
{
    QString path = _tmpDir.filePath("binary_null.bin");
    QFile f(path);
    QVERIFY(f.open(QIODevice::WriteOnly));
    // Write null bytes
    f.write("\x00\x01\x02");
    f.close();

    bool isBinary = FileHelper::isBinaryFile(path, 8192);
    QVERIFY2(isBinary,
             qPrintable(QString("isBinaryFile should return true for null bytes: %1").arg(path)));
}

void TestFileHelper::test_isBinaryFile_text()
{
    QString path = _tmpDir.filePath("text.txt");
    QFile f(path);
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.write("Hello world\n");
    f.close();

    bool isBinary = FileHelper::isBinaryFile(path, 8192);
    QVERIFY2(!isBinary,
             qPrintable(QString("isBinaryFile should return false for text file: %1").arg(path)));
}

// ============================================================================
// Hash computation
// ============================================================================

void TestFileHelper::test_computeMD5()
{
    QString path = _tmpDir.filePath("md5test.txt");
    QFile f(path);
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.write("hello");
    f.close();

    QString hash = FileHelper::computeMD5(path);
    // MD5 of "hello" = 5d41402abc4b2a76b9719d911017c592
    QVERIFY2(!hash.isEmpty(),
             qPrintable(QString("computeMD5 should return non-empty hash: %1").arg(path)));
    QVERIFY2(hash.size() == 32,
             qPrintable(QString("MD5 hash should be 32 hex chars, got %1 (\"%2\")")
                        .arg(hash.size()).arg(hash)));
}

void TestFileHelper::test_computeSHA256()
{
    QString path = _tmpDir.filePath("sha256test.txt");
    QFile f(path);
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.write("hello");
    f.close();

    QString hash = FileHelper::computeSHA256(path);
    // SHA256 of "hello" is a known value (64 hex chars)
    QVERIFY2(!hash.isEmpty(),
             qPrintable(QString("computeSHA256 should return non-empty hash: %1").arg(path)));
    QVERIFY2(hash.size() == 64,
             qPrintable(QString("SHA256 hash should be 64 hex chars, got %1 (\"%2\")")
                        .arg(hash.size()).arg(hash)));
}

QTEST_MAIN(TestFileHelper)
#include "TestFileHelper.moc"
