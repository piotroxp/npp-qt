# Testing Reference

How to write, run, and debug tests in Notepad--Qt.

---

## Test Suites

Notepad--Qt uses **Qt Test** (Qt's built-in unit testing framework). Tests live in `tests/`.

| Test File | Suite | What it tests |
|-----------|-------|---------------|
| `TestEncodingDetector.cpp` | Encoding | BOM detection, UTF-8 validation, chardet fallback |
| `TestStringHelper.cpp` | String | trim, case, contains, startsWith/endsWith, escape, path helpers |
| `TestFileHelper.cpp` | File | exists, read/write, atomic write, copy, move, permissions |
| `TestLexer.cpp` | Lexer | Language detection from extension and content |
| `TestDialogs.cpp` | Dialogs | Dialog construction, show/hide, signal emission |
| `TestStatusBar.cpp` | StatusBar | Segment update, encoding display, position display |
| `startup_test.cpp` | Startup | Headless startup, window creation |
| `integration_test.cpp` | Integration | Multi-component workflows |
| `integration_editor_test.cpp` | Integration | Editor + buffer interactions |

---

## Running Tests

### Build and Run All

```bash
cd npp-qt
cmake -B tests/build -DCMAKE_BUILD_TYPE=Debug
cmake --build tests/build -j$(nproc)
ctest --test-dir tests/build --output-on-failure
```

### Run a Specific Suite

```bash
ctest --test-dir tests/build -R StringHelper --output-on-failure
ctest --test-dir tests/build -R Encoding --output-on-failure
ctest --test-dir tests/build -R Lexer --output-on-failure
```

### Run a Specific Test

```bash
# Verbose — print every test name as it runs
ctest --test-dir tests/build -V

# Run one test method
./tests/build/test_suite --testfunction test_trim_basic
```

### Run with Headless Display

Tests are automatically run with `QT_QPA_PLATFORM=offscreen` via the `CTEST_ENVIRONMENT` setting in `tests/CMakeLists.txt`. If tests fail on a machine with a display, ensure the env var is set:

```bash
QT_QPA_PLATFORM=offscreen ctest --test-dir tests/build --output-on-failure
```

---

## Test Structure

### Qt Test Conventions

Each test file defines a `QObject` subclass with `Q_OBJECT` and `private slots` for test methods:

```cpp
#include <QtTest/QtTest>

class TestStringHelper : public QObject
{
    Q_OBJECT

private slots:
    void test_toLower();
    void test_toUpper();
    void test_trim_basic();
    void test_startsWith();
};
```

Test methods **must be named `test_<name>()`** — Qt Test discovers them by this naming convention.

### Assertions

| Macro | Use |
|-------|-----|
| `QCOMPARE(a, b)` | Assert `a == b` |
| `QCOMPARE(a, b)` | Compare strings, ints, containers |
| `QVERIFY(expr)` | Assert `expr` is true |
| `QVERIFY2(expr, msg)` | Assert with failure message |
| `QFAIL(msg)` | Fail with message |
| `QSKIP(msg)` | Skip this test (e.g. feature not available) |
| `QEXPECT_FAIL(label, msg, mode)` | Expect a failure (for known bugs) |

### Example

```cpp
void TestStringHelper::test_trim_basic()
{
    QString result = StringHelper::trim("  hello  ");
    QVERIFY(result == "hello");
}

void TestStringHelper::test_startsWith()
{
    QVERIFY(StringHelper::startsWith("hello world", "hello"));
    QVERIFY(!StringHelper::startsWith("hello world", "world"));
    QVERIFY(StringHelper::startsWith("hello", ""));  // empty prefix always matches
}

void TestStringHelper::test_fileExt()
{
    QVERIFY(StringHelper::fileExt("/path/to/file.cpp") == ".cpp");
    QVERIFY(StringHelper::fileExt("file") == "");  // no extension
    QVERIFY(StringHelper::fileExt("/path/to.tar.gz") == ".gz");  // last segment
}
```

---

## Data-Driven Tests

Qt Test supports data-driven tests via `_data` methods:

```cpp
class TestEncodingDetector : public QObject
{
    Q_OBJECT

private slots:
    void test_utf8BOM_data();
    void test_utf8BOM();

    void test_utf8Validation_data();
    void test_utf8Validation();
};

// The _data method populates test data
void TestEncodingDetector::test_utf8BOM_data()
{
    QTest::addColumn<QByteArray>("input");
    QTest::addColumn<EncodingType>("expected");

    QTest::newRow("UTF-8 BOM")     << QByteArray("\xEF\xBB\xBF""hello") << EncodingType::UTF_8_BOM;
    QTest::newRow("UTF-16 LE BOM") << QByteArray("\xFF\xFE""hi") << EncodingType::UTF_16_LE_BOM;
    QTest::newRow("No BOM")        << QByteArray("hello") << EncodingType::UTF_8;  // fallback
}

void TestEncodingDetector::test_utf8BOM()
{
    QFETCH(QByteArray, input);
    QFETCH(EncodingType, expected);

    EncodingType detected = EncodingDetector::detect(input);
    QVERIFY2(detected == expected,
             qPrintable(QString("expected %1, got %2").arg(int(expected)).arg(int(detected))));
}
```

Run data-driven tests:
```bash
./tests/build/test_encoding --verbose
# Output:
# PASS  : TestEncodingDetector::test_utf8BOM(UTF-8 BOM)
# PASS  : TestEncodingDetector::test_utf8BOM(UTF-16 LE BOM)
# PASS  : TestEncodingDetector::test_utf8BOM(No BOM)
```

---

## Test Stubs

Widget-heavy code (e.g. `ScintillaEditor`) can't be tested headlessly. Test stubs provide minimal implementations for these classes:

**File:** `tests/stubs/TestStubs.h`

```cpp
// Stub ScintillaEditor for headless tests
class MockScintillaEditor {
    QString _text;
    int _cursorLine = 0;
    int _cursorCol = 0;
    bool _dirty = false;

public:
    void setText(const QString& t) { _text = t; }
    QString text() const { return _text; }
    void setCursorPosition(int line, int col) {
        _cursorLine = line;
        _cursorCol = col;
    }
    void insertText(const QString& t) {
        _text.insert(cursorPos(), t);
    }
    bool isDirty() const { return _dirty; }
    void setDirty(bool d) { _dirty = d; }
};
```

Use `MockScintillaEditor` in tests to avoid depending on the GUI stack:

```cpp
void TestBuffer::test_dirtyTracking()
{
    MockScintillaEditor editor;
    Buffer buf(1);

    editor.setText("hello");
    buf.setDirty(editor.isDirty());
    QVERIFY(buf.isDirty());  // fails initially

    editor.setDirty(false);
    buf.setDirty(editor.isDirty());
    QVERIFY(!buf.isDirty());  // passes
}
```

---

## Writing a New Test

1. **Create the test file** in `tests/`:

```cpp
// tests/TestMyFeature.cpp
#include <QtTest/QtTest>
#include "myfeature.h"

class TestMyFeature : public QObject
{
    Q_OBJECT
private slots:
    void test_basic();
    void test_edgeCase();
    void test_data();
    void test_data();
};

void TestMyFeature::test_basic()
{
    MyClass obj;
    QVERIFY(obj.method() == expected);
}

QTEST_MAIN(TestMyFeature)
#include "TestMyFeature.moc"  // moc must be included
```

2. **Register the test** in `tests/CMakeLists.txt`:

```cmake
add_executable(test_myfeature
    TestMyFeature.cpp
    $<TARGET_PROPERTY:npp-qt-test-lib,INTERFACE_SOURCES>
)
target_link_libraries(test_myfeature PRIVATE npp-qt-test-lib Qt6::Test)
add_test(NAME myfeature COMMAND test_myfeature)
```

3. **Build and run:**

```bash
cmake --build tests/build
ctest --test-dir tests/build -R myfeature --output-on-failure
```

---

## Integration Tests

Integration tests exercise multiple components together:

```cpp
// tests/integration_editor_test.cpp
void TestIntegration::test_openFileAssignsLanguage()
{
    Application app;
    app.init();

    int bufId = app.openFile("test.cpp");
    Buffer* buf = app.getBuffer(bufId);

    QVERIFY(buf != nullptr);
    QVERIFY(buf->langType() == L_CPP);

    app.closeBuffer(bufId);
    QVERIFY(app.getBuffer(bufId) == nullptr);
}
```

**Note:** Integration tests require a running `QApplication`, so they must be run headless (`QT_QPA_PLATFORM=offscreen`).

---

## Code Coverage

To measure test coverage with `gcov`/`lcov`:

```bash
cmake -B tests/build \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS="--coverage"

cmake --build tests/build -j$(nproc)
ctest --test-dir tests/build --output-on-failure

# Generate coverage report
lcov --capture --directory tests/build --output-file coverage.info
lcov --remove coverage.info '/usr/*' --output-file coverage.filtered.info
genhtml coverage.filtered.info --output-directory coverage_html
```

---

## CI / Headless Testing

In CI (GitHub Actions, etc.), tests run with:
```bash
QT_QPA_PLATFORM=offscreen ctest --test-dir tests/build --output-on-failure
```

The `CTEST_ENVIRONMENT` variable in `tests/CMakeLists.txt` sets this automatically.

If a test fails on CI but not locally, check:
1. `QT_QPA_PLATFORM` is `offscreen`
2. `DISPLAY` is not set
3. All required fonts are available (`fc-list`)

---

## Debugging Tests

### Debug a failing test

```bash
# Run with GDB
gdb -args ./tests/build/test_stringhelper --testfunction test_trim_basic
(gdb) run
(gdb) bt  # on crash
```

### Verbose output

```bash
# Print all test names
./tests/build/test_stringhelper --verbose

# Print timing information
./tests/build/test_stringhelper --vsbenches
```

### Skip slow tests

```cpp
void TestMyFeature::test_slowOperation()
{
    if (qgetenv("CI").isEmpty()) {
        QSKIP("Skipping slow test in CI");
    }
    // ...
}
```

---

## Test Patterns

### Arrange / Act / Assert

```cpp
void TestLexer::test_detectPythonFromExtension()
{
    // Arrange
    QString filename = "script.py";

    // Act
    LangType lang = LexerConfig::detectFromExtension(filename);

    // Assert
    QVERIFY(lang == L_PYTHON);
}
```

### Fixture

Use a shared fixture for tests that need the same setup:

```cpp
class TestBuffer : public QObject
{
    Q_OBJECT
protected:
    Buffer* _buf = nullptr;

    void init() {
        _buf = new Buffer(1);
        _buf->setFilePath("/tmp/test.txt");
    }
    void cleanup() {
        delete _buf;
        _buf = nullptr;
    }

private slots:
    void test_dirtyOnCreate();
    void test_setEncoding();
    void test_save();
};
```

### Temporary Files

```cpp
void TestFileHelper::test_atomicWrite()
{
    QTemporaryDir dir;
    QString path = dir.filePath("test.txt");

    // Act
    FileHelper::atomicWrite(path, "hello");

    // Assert
    QVERIFY(FileHelper::exists(path));
    QVERIFY(FileHelper::readTextFile(path) == "hello");
}  // QTemporaryDir auto-deletes
```
