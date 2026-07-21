// npp-qt: Win32→Qt6 semantic lift — see SEMANTIC_LIFT_MAP.md
// Port of FunctionCallTip.cpp — function calltip display during typing
// Win32→Qt6: TCM_* → removed (Scintilla handles this natively via SCI_CALLTIP*)
#include <QByteArray>
#include <QColor>
#include <QApplication>
#include <cctype>
#include <cstring>
#include <sstream>
#include <cstdio>
#include "FunctionCallTip.h"
#include "../core/LanguageManager.h"
#include "../editor/ScintillaEditor.h"

// ─── Scintilla calltip colour constants ───────────────────────────────────────
// Scintilla uses 0-based colour indices for calltips.
// colourBG = 0, colourFG = 1, colourHL = 2 (highlight current param)
// See SCI_CALLTIPSET* messages.

FunctionCallTip::FunctionCallTip(ScintillaComponent* pEditView, QObject* parent)
    : QObject(parent)
    , _pEditView(pEditView)
{
    applyStyling();
}

void FunctionCallTip::setLanguageXML(void* xmlKeyword)
{
    Q_UNUSED(xmlKeyword);
    if (isVisible()) close();
    reset();
    _funcName.clear();
    _overloadList.clear();
    _additionalWordChar.clear();

    // Load additional word characters from the active language.
    // In production this would parse the API XML keyword definitions.
    // For now we keep defaults (underscore is always valid).
}

bool FunctionCallTip::updateCalltip(int ch, bool needShown)
{
    if (!needShown && ch != _start && ch != _param && !isVisible())
        return false;

    _curPos = _pEditView ? _pEditView->send(SCI_GETCURRENTPOS) : 0;

    if (!getCursorFunction()) {
        if (isVisible()) close();
        return false;
    }

    showCalltip();
    return true;
}

void FunctionCallTip::showNextOverload()
{
    if (!isVisible()) return;
    if (_currentNbOverloads > 0)
        _currentOverload = (_currentOverload + 1) % _currentNbOverloads;
    showCalltip();
}

void FunctionCallTip::showPrevOverload()
{
    if (!isVisible()) return;
    _currentOverload = (_currentOverload > 0) ? (_currentOverload - 1)
                                               : (_currentNbOverloads - 1);
    showCalltip();
}

void FunctionCallTip::showForFunction(const QString& funcName,
                                      const std::vector<FunctionOverload>& overloads)
{
    if (!_pEditView || overloads.empty()) return;

    _funcName = funcName.toStdString();
    _overloadList = overloads;
    _currentOverload = 0;
    _currentParam = 0;
    _currentNbOverloads = overloads.size();

    // Build return values and descriptions vectors from overload list.
    _retVals.clear();
    _descriptions.clear();
    for (const auto& ov : overloads) {
        _retVals.push_back(ov.returnType.c_str());
        _descriptions.push_back(ov.documentation.c_str());
    }

    _selfActivated = true;
    showCalltip();
}

void FunctionCallTip::highlightParam(int paramIndex)
{
    if (!_pEditView || !isVisible()) return;

    // Scintilla: highlight a parameter by setting its colour style.
    // SCI_CALLTIPSETHLT starts highlighting at the given parameter index.
    _pEditView->send(SCI_CALLTIPSETHLT, paramIndex);
    emit paramHighlightChangedSignal(paramIndex);
}

void FunctionCallTip::setForeground(const QColor& c)
{
    _fgColor = c;
    if (isVisible()) {
        _pEditView->send(SCI_CALLTIPSETFORE, 0, static_cast<int>(qRgb(c.red(), c.green(), c.blue())));
    }
}

void FunctionCallTip::setBackground(const QColor& c)
{
    _bgColor = c;
    if (isVisible()) {
        _pEditView->send(SCI_CALLTIPSETBACK, 0, static_cast<int>(qRgb(c.red(), c.green(), c.blue())));
    }
}

void FunctionCallTip::setHighlightColor(const QColor& c)
{
    _hlColor = c;
    if (isVisible()) {
        // Scintilla highlight uses a fixed colour index (2), set via
        // SCI_CALLTIPSETFOREHLT.
        _pEditView->send(SCI_CALLTIPSETFOREHLT, 0, static_cast<int>(qRgb(c.red(), c.green(), c.blue())));
    }
}

bool FunctionCallTip::isVisible() const
{
    return _pEditView && (_pEditView->send(SCI_CALLTIPACTIVE) != 0);
}

void FunctionCallTip::close()
{
    bool wasVisible = isVisible();
    if (isVisible() && _pEditView)
        _pEditView->send(SCI_CALLTIPCANCEL);

    _selfActivated = false;
    _currentOverload = 0;
    _sticky = false;

    if (wasVisible)
        onDismissed();
}

void FunctionCallTip::reset()
{
    _currentOverload = 0;
    _currentParam = 0;
    _startPos = 0;
    _overloads.clear();
    _overloadList.clear();
    _currentNbOverloads = 0;
    _retVals.clear();
    _descriptions.clear();
    _fullSignature.clear();
}

void FunctionCallTip::cleanup()
{
    close();
    reset();
    _pEditView = nullptr;
}

// ─── Styling ──────────────────────────────────────────────────────────────────

void FunctionCallTip::applyStyling()
{
    if (!_pEditView) return;

    // Set calltip appearance via Scintilla messages.
    _pEditView->send(SCI_CALLTIPSETFORE, 0, static_cast<int>(qRgb(_fgColor.red(), _fgColor.green(), _fgColor.blue())));
    _pEditView->send(SCI_CALLTIPSETBACK, 0, static_cast<int>(qRgb(_bgColor.red(),   _bgColor.green(),   _bgColor.blue())));
    _pEditView->send(SCI_CALLTIPSETFOREHLT, 0, static_cast<int>(qRgb(_hlColor.red(), _hlColor.green(), _hlColor.blue())));

    // Enable hover/click handling for calltips.
    _pEditView->send(SCI_CALLTIPSETOPTIONS, 0);
}

// ─── Cursor function detection ─────────────────────────────────────────────────

bool FunctionCallTip::getCursorFunction()
{
    if (!_pEditView) return false;

    auto line = static_cast<int>(_pEditView->send(SCI_LINEFROMPOSITION, _curPos));
    auto lineStart = static_cast<int>(_pEditView->send(SCI_POSITIONFROMLINE, line));
    auto lineEnd   = static_cast<int>(_pEditView->send(SCI_GETLINEENDPOSITION, line));
    auto len = lineEnd - lineStart + 3;

    if (len <= 2 || len >= 4096) { reset(); return false; }

    // Get the line text.
    QByteArray lineBytes(len + 1, '\0');
    _pEditView->send(SCI_GETLINE, line, reinterpret_cast<sptr_t>(lineBytes.data()));
    lineBytes.data()[len] = '\0';

    auto offset = _curPos - lineStart;
    if (offset < 2) { reset(); return false; }

    // Find the opening paren matching the function being called.
    // Scan backwards counting nesting level.
    int parenCount = 0;
    int funcEnd = -1;

    // Start scanning from the char just before cursor.
    for (int i = offset - 1; i >= 0; --i) {
        char c = lineBytes.data()[i];
        if (c == _stop) {
            ++parenCount;          // closing paren: entering deeper nest
        } else if (c == _start) {
            if (parenCount > 0) {
                --parenCount;      // closing a nested pair
            } else {
                funcEnd = i - 1;   // this is the function's opening paren
                break;
            }
        }
    }

    if (funcEnd < 0) { reset(); return false; }

    // Back up to find the start of the function name identifier.
    int funcStart = funcEnd;
    while (funcStart >= 0 &&
           (isBasicWordChar(lineBytes.data()[funcStart]) ||
            isAdditionalWordChar(lineBytes.data()[funcStart]) ||
            lineBytes.data()[funcStart] == ':' ||    // handle :: (C++ namespaces/methods)
            lineBytes.data()[funcStart] == '.'))     // handle . (C++/Python dot access)
    {
        --funcStart;
    }
    ++funcStart;

    int funcLen = funcEnd - funcStart + 1;
    if (funcLen <= 0) { reset(); return false; }

    _funcName.assign(lineBytes.data() + funcStart, static_cast<size_t>(funcLen));

    // Count commas between the function name and cursor to know which param we're on.
    _currentParam = 0;
    for (int i = funcEnd + 1; i < offset; ++i) {
        if (lineBytes.data()[i] == _param) ++_currentParam;
    }

    _startPos = lineStart + funcStart;
    return loadFunction();
}

// ─── Function lookup ──────────────────────────────────────────────────────────

// ─── Built-in function database ────────────────────────────────────────────────
// Maps function names → vector of overloads.
// In production this would be replaced by LanguageManager's API XML parser.
struct FuncEntry {
    const char* signature;   // e.g. "int foo(char* s, int n)"
    const char* returnType; // e.g. "int"
    const char* brief;      // one-line description
    const char* doc;        // detailed doc (may be empty)
};

// C/C++ standard library and common functions
static const FuncEntry kCppFunctions[] = {
    // I/O
    {"printf(const char* format, ...)", "int", "Print formatted output to stdout",
     "Print formatted data to stdout.\nformat may include escape sequences like \\n, \\t."},
    {"fprintf(FILE* stream, const char* format, ...)", "int", "Print formatted output to file stream",
     "Print formatted data to the given file stream."},
    {"scanf(const char* format, ...)", "int", "Read formatted input from stdin",
     "Reads data from stdin according to format string."},
    {"sprintf(char* buffer, const char* format, ...)", "int", "Write formatted string to buffer",
     "Write formatted output to a character string buffer."},
    {"snprintf(char* buffer, size_t n, const char* format, ...)", "int", "Write formatted string with bound check",
     "Write at most n bytes to buffer."},
    {"puts(const char* s)", "int", "Write string to stdout",
     "Writes the string s and a trailing newline to stdout."},
    {"getchar()", "int", "Read a character from stdin",
     "Returns the next character from stdin, or EOF on end-of-file/error."},
    {"getc(FILE* stream)", "int", "Read a character from file stream",
     "Returns the next character from the given file stream."},

    // String
    {"strlen(const char* s)", "size_t", "Get string length",
     "Returns the length of the string s (excluding null terminator)."},
    {"strcpy(char* dest, const char* src)", "char*", "Copy string",
     "Copies the null-terminated string src to dest. Caller must ensure dest has enough space."},
    {"strncpy(char* dest, const char* src, size_t n)", "char*", "Copy string with limit",
     "Copies at most n bytes of src to dest. Pads with null bytes if src < n."},
    {"strcat(char* dest, const char* src)", "char*", "Append string",
     "Appends src to dest, overwriting the null terminator. Caller ensures dest has room."},
    {"strcmp(const char* s1, const char* s2)", "int", "Compare strings",
     "Returns 0 if s1==s2, <0 if s1<s2, >0 if s1>s2 (lexicographically)."},
    {"strncmp(const char* s1, const char* s2, size_t n)", "int", "Compare strings with limit",
     "Compares at most n bytes of s1 and s2."},
    {"strchr(const char* s, int c)", "char*", "Find character in string",
     "Returns pointer to first occurrence of c in s, or nullptr if not found."},
    {"strstr(const char* haystack, const char* needle)", "char*", "Find substring",
     "Returns pointer to first occurrence of needle in haystack, or nullptr."},
    {"strdup(const char* s)", "char*", "Duplicate string",
     "Returns a newly allocated copy of s. Caller must free the result."},
    {"memset(void* s, int c, size_t n)", "void*", "Fill memory",
     "Fills the first n bytes of s with the constant byte c."},
    {"memcpy(void* dest, const void* src, size_t n)", "void*", "Copy memory",
     "Copies n bytes from src to dest. Regions must not overlap."},
    {"memmove(void* dest, const void* src, size_t n)", "void*", "Copy memory (overlap-safe)",
     "Copies n bytes from src to dest. Handles overlapping regions correctly."},
    {"memcmp(const void* s1, const void* s2, size_t n)", "int", "Compare memory",
     "Compares the first n bytes of s1 and s2. Returns 0 if equal."},

    // Memory
    {"malloc(size_t size)", "void*", "Allocate memory",
     "Allocates size bytes and returns a pointer. Memory is uninitialized. Free with free()."},
    {"calloc(size_t nmemb, size_t size)", "void*", "Allocate zero-initialized memory",
     "Allocates nmemb*size bytes, initialized to zero. Free with free()."},
    {"realloc(void* ptr, size_t size)", "void*", "Reallocate memory",
     "Changes the size of the memory block pointed to by ptr. May move. Free with free()."},
    {"free(void* ptr)", "void", "Free allocated memory",
     "Frees the memory space pointed to by ptr. ptr must be from malloc/calloc/realloc."},
    {"new T", "T*", "Dynamic allocation (C++)",
     "Allocates and constructs an object of type T. Use delete to free."},
    {"new T[n]", "T*", "Dynamic array allocation (C++)",
     "Allocates and constructs an array of n objects of type T. Use delete[] to free."},

    // File I/O
    {"fopen(const char* filename, const char* mode)", "FILE*", "Open file",
     "Opens the file filename with mode (r/w/a/r+/w+/a+/rb/wb/...). Returns FILE* or nullptr."},
    {"fclose(FILE* stream)", "int", "Close file",
     "Closes the given file stream. Returns 0 on success, EOF on error."},
    {"fread(void* ptr, size_t size, size_t nmemb, FILE* stream)", "size_t", "Read from file",
     "Reads nmemb elements of size bytes from stream into ptr. Returns number read."},
    {"fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream)", "size_t", "Write to file",
     "Writes nmemb elements of size bytes from ptr to stream. Returns number written."},
    {"fseek(FILE* stream, long offset, int whence)", "int", "Seek in file",
     "Sets the file position. whence: SEEK_SET(0), SEEK_CUR(1), SEEK_END(2)."},
    {"ftell(FILE* stream)", "long", "Get file position",
     "Returns the current file position, or -1 on error."},
    {"feof(FILE* stream)", "int", "Test end-of-file",
     "Returns non-zero if end-of-file indicator for stream is set."},
    {"ferror(FILE* stream)", "int", "Test file error",
     "Returns non-zero if error indicator for stream is set."},
    {"rewind(FILE* stream)", "void", "Rewind file",
     "Sets the file position to the beginning of the file."},

    // Math
    {"abs(int x)", "int", "Absolute value of int",
     "Returns the absolute value of an integer."},
    {"labs(long x)", "long", "Absolute value of long",
     "Returns the absolute value of a long integer."},
    {"fabs(double x)", "double", "Absolute value of double",
     "Returns the absolute value of a floating-point number (double)."},
    {"sqrt(double x)", "double", "Square root",
     "Returns the square root of x. x must be non-negative."},
    {"pow(double base, double exp)", "double", "Power",
     "Returns base raised to the power exp."},
    {"sin(double x)", "double", "Sine (radians)",
     "Returns the sine of x (x in radians)."},
    {"cos(double x)", "double", "Cosine (radians)",
     "Returns the cosine of x (x in radians)."},
    {"tan(double x)", "double", "Tangent (radians)",
     "Returns the tangent of x (x in radians)."},
    {"exp(double x)", "double", "Exponential",
     "Returns e raised to the power x."},
    {"log(double x)", "double", "Natural logarithm",
     "Returns the natural (base e) logarithm of x. x must be positive."},
    {"log10(double x)", "double", "Base-10 logarithm",
     "Returns the common (base 10) logarithm of x. x must be positive."},
    {"floor(double x)", "double", "Floor",
     "Returns the largest integer not greater than x."},
    {"ceil(double x)", "double", "Ceiling",
     "Returns the smallest integer not less than x."},
    {"fmod(double x, double y)", "double", "Floating-point remainder",
     "Returns the remainder of x/y."},
    {"rand()", "int", "Pseudo-random integer",
     "Returns a pseudo-random integer between 0 and RAND_MAX."},
    {"srand(unsigned int seed)", "void", "Seed random number generator",
     "Sets the starting point for the sequence of pseudo-random integers."},

    // Utilities
    {"exit(int status)", "void", "Terminate program",
     "Terminates the program normally. status is returned to the environment."},
    {"abort()", "void", "Abort program",
     "Causes abnormal program termination."},
    {"assert(expr)", "void", "Debug assertion",
     "If NDEBUG is not defined, aborts if expr is false."},
    {"sizeof(type-or-expr)", "size_t", "Size of type or object",
     "Returns the size in bytes of type or expression."},
    {"offsetof(type, member)", "size_t", "Offset of member in struct",
     "Returns the offset in bytes of member within type."},

    // C++ specific
    {"std::make_shared<T>(args...)", "std::shared_ptr<T>", "Create shared_ptr",
     "Creates a shared_ptr managing a new instance of T constructed with args."},
    {"std::make_unique<T>(args...)", "std::unique_ptr<T>", "Create unique_ptr",
     "Creates a unique_ptr managing a new instance of T constructed with args."},
    {"std::vector<T>::push_back(const T& val)", "void", "Append element to vector",
     "Appends a copy of val to the end of the vector."},
    {"std::vector<T>::at(size_t n)", "T&", "Access element with bounds check",
     "Returns a reference to the element at position n, with bounds checking."},
    {"std::string::c_str()", "const char*", "Get C-string pointer",
     "Returns a pointer to a null-terminated character array."},
    {"std::string::substr(size_t pos, size_t len)", "std::string", "Get substring",
     "Returns a newly constructed string with the substring [pos, pos+len)."},
    {"std::map<K,V>::find(const K& key)", "iterator", "Find key in map",
     "Returns an iterator to the element with key, or end() if not found."},
    {"std::make_pair(a, b)", "std::pair<T1,T2>", "Create pair",
     "Creates a pair object with values a and b."},

    // C++ iostream
    {"std::cout << value", "std::ostream&", "Stream output to stdout",
     "Inserts value into the standard output stream."},
    {"std::cin >> variable", "std::istream&", "Stream input from stdin",
     "Extracts value from standard input into variable."},
    {"std::cerr << value", "std::ostream&", "Stream output to stderr",
     "Inserts value into the standard error stream."},
    {"printf(...)", "int", "Print formatted (printf family)",
     "Prints formatted output to stdout."},

    // Qt specific (common in Qt codebases)
    {"QString::arg(const QString& a)", "QString", "Replace %1 placeholder",
     "Returns a copy of this string with the %1 placeholder replaced by a."},
    {"QString::trimmed()", "QString", "Remove leading/trailing whitespace",
     "Returns a copy of the string with leading and trailing whitespace removed."},
    {"QString::split(const QString& sep)", "QStringList", "Split string",
     "Splits the string at each occurrence of sep and returns the list of parts."},
    {"QDebug operator<<(QDebug, ...)", "QDebug", "Stream to debug output",
     "Streams values to the debug output for printing."},
};

// Python built-in functions
static const FuncEntry kPythonFunctions[] = {
    {"print(*objects, sep=' ', end='\\n', file=sys.stdout, flush=False)", "None", "Print to stdout",
     "Prints the values to the standard output (or file)."},
    {"len(s)", "int", "Get length",
     "Return the length (the number of items) of an object."},
    {"range([start,] stop[, step])", "range", "Generate integer sequence",
     "Returns a sequence of integers from start to stop-1 by step."},
    {"str(obj)", "str", "Convert to string",
     "Return a string version of obj."},
    {"int(x)", "int", "Convert to integer",
     "Convert a string or number to an integer."},
    {"float(x)", "float", "Convert to float",
     "Convert a string or number to a floating point number."},
    {"bool(x)", "bool", "Convert to bool",
     "Return True when the argument x is true, False otherwise."},
    {"list([iterable])", "list", "Create list",
     "Create a list from an iterable or another sequence."},
    {"dict(**kwargs)", "dict", "Create dictionary",
     "Create a dictionary from keyword arguments."},
    {"tuple([iterable])", "tuple", "Create tuple",
     "Create a tuple from an iterable or another sequence."},
    {"set([iterable])", "set", "Create set",
     "Create a set from an iterable or another sequence."},
    {"open(file, mode='r', ...)", "file", "Open file",
     "Open file and return a corresponding file object. mode: r/w/a/r+/w+/a+/b."},
    {"input([prompt])", "str", "Read line from stdin",
     "Read a line from stdin, stripping the trailing newline."},
    {"enumerate(iterable, start=0)", "enumerate", "Add index to iterable",
     "Return an enumerate object yielding pairs (index, value)."},
    {"zip(*iterables)", "zip", "Iterate in parallel",
     "Iterate over several iterables in parallel, yielding tuples."},
    {"map(func, *iterables)", "map", "Apply function to iterable",
     "Return an iterator that applies func to every item of iterable."},
    {"filter(func, iterable)", "filter", "Filter iterable",
     "Construct an iterator from elements of iterable for which func returns true."},
    {"sorted(iterable, key=None, reverse=False)", "list", "Return sorted list",
     "Sort items of iterable in ascending order and return the sorted list."},
    {"reversed(sequence)", "reverseiterator", "Reverse iterator",
     "Return a reverse iterator over a sequence."},
    {"sum(iterable, start=0)", "number", "Sum iterable",
     "Sums start and the items of an iterable from left to right."},
    {"min(iterable)", "value", "Return minimum",
     "Return the smallest item of an iterable or of two or more arguments."},
    {"max(iterable)", "value", "Return maximum",
     "Return the largest item of an iterable or of two or more arguments."},
    {"abs(x)", "number", "Absolute value",
     "Return the absolute value of a number."},
    {"round(number[, ndigits])", "number", "Round to precision",
     "Round a number to a given precision in decimal digits."},
    {"type(obj)", "type", "Return type of object",
     "Return the type of an object."},
    {"isinstance(obj, classinfo)", "bool", "Check instance type",
     "Return True if obj is an instance of classinfo or a subclass thereof."},
    {"hasattr(obj, name)", "bool", "Check attribute existence",
     "Return True if obj has an attribute with the given name."},
    {"getattr(obj, name[, default])", "value", "Get attribute",
     "Get the value of obj's attribute. Returns default if attribute is missing."},
    {"setattr(obj, name, value)", "None", "Set attribute",
     "Set the value of obj's attribute."},
    {"delattr(obj, name)", "None", "Delete attribute",
     "Delete the attribute named name from obj."},
    {"dir([object])", "list", "List names in scope",
     "Without arguments, return the list of names in the current local scope."},
    {"vars([object])", "dict", "Return __dict__",
     "Return the __dict__ attribute for a module, class, or object."},
    {"repr(obj)", "str", "Return repr string",
     "Return a string containing a printable representation of obj."},
    {"format(value[, format_spec])", "str", "Format value",
     "Convert a value to a formatted representation."},
    {"slice([start,] stop[, step])", "slice", "Create slice object",
     "Return a slice object representing the set of indices specified."},
    {"iter(obj)", "iterator", "Get iterator",
     "Return an iterator object based on obj."},
    {"next(iterator[, default])", "value", "Get next item",
     "Retrieve the next item from the iterator."},
    {"callable(obj)", "bool", "Check if callable",
     "Return True if the object argument appears callable."},
    {"hash(obj)", "int", "Return hash value",
     "Return the hash value of the object (if it has one)."},
    {"id(obj)", "int", "Return identity",
     "Return the identity of an object (unique integer, memory address in CPython)."},
    {"globals()", "dict", "Return global symbol table",
     "Return a dictionary representing the current global symbol table."},
    {"locals()", "dict", "Return local symbol table",
     "Return a dictionary representing the current local symbol table."},
};

// JavaScript built-in functions
static const FuncEntry kJsFunctions[] = {
    {"console.log(...args)", "void", "Log to console",
     "Outputs a message to the web console or Node.js stdout."},
    {"console.error(...args)", "void", "Log error to console",
     "Outputs an error message to the console."},
    {"console.warn(...args)", "void", "Log warning to console",
     "Outputs a warning message to the console."},
    {"console.table(data)", "void", "Display data as table",
     "Displays data as a table in the console."},
    {"JSON.parse(text)", "any", "Parse JSON string",
     "Parse a JSON string and return the resulting JavaScript value."},
    {"JSON.stringify(value, replacer, space)", "string", "Convert to JSON",
     "Return a JSON string representation of a JavaScript value."},
    {"Array.isArray(obj)", "boolean", "Check if array",
     "Returns true if obj is an array, false otherwise."},
    {"Array.from(arrLike, mapFn)", "Array", "Create array from iterable",
     "Creates a new Array instance from an array-like or iterable object."},
    {"Object.keys(obj)", "string[]", "Get object keys",
     "Returns an array of a given object's own enumerable property names."},
    {"Object.values(obj)", "any[]", "Get object values",
     "Returns an array of a given object's own enumerable property values."},
    {"Object.entries(obj)", "Array<[string,any]>", "Get object entries",
     "Returns an array of [key, value] pairs for the object's own enumerable properties."},
    {"Object.assign(target, ...sources)", "Object", "Merge objects",
     "Copies the values of all enumerable own properties from source objects to target."},
    {"Object.freeze(obj)", "Object", "Freeze object",
     "Freezes an object, preventing modifications to its properties."},
    {"Math.abs(x)", "number", "Absolute value",
     "Returns the absolute value of x."},
    {"Math.max(...values)", "number", "Maximum value",
     "Returns the largest of the given values."},
    {"Math.min(...values)", "number", "Minimum value",
     "Returns the smallest of the given values."},
    {"Math.floor(x)", "number", "Floor",
     "Returns the greatest integer less than or equal to x."},
    {"Math.ceil(x)", "number", "Ceiling",
     "Returns the smallest integer greater than or equal to x."},
    {"Math.round(x)", "number", "Round",
     "Returns the value of x rounded to the nearest integer."},
    {"Math.random()", "number", "Random 0-1",
     "Returns a pseudo-random number between 0 (inclusive) and 1 (exclusive)."},
    {"Math.sqrt(x)", "number", "Square root",
     "Returns the square root of x."},
    {"Math.pow(base, exp)", "number", "Power",
     "Returns base raised to the power of exp."},
    {"Math.sin(x)", "number", "Sine (radians)",
     "Returns the sine of x (x in radians)."},
    {"Math.cos(x)", "number", "Cosine (radians)",
     "Returns the cosine of x (x in radians)."},
    {"Math.log(x)", "number", "Natural logarithm",
     "Returns the natural logarithm of x."},
    {"Math.log10(x)", "number", "Base-10 logarithm",
     "Returns the base 10 logarithm of x."},
    {"Date.now()", "number", "Current timestamp",
     "Returns the number of milliseconds since the Unix epoch."},
    {"parseInt(string, radix)", "number", "Parse integer",
     "Parses string and returns an integer in the given radix (base)."},
    {"parseFloat(string)", "number", "Parse float",
     "Parses string and returns a floating point number."},
    {"isNaN(value)", "boolean", "Check if NaN",
     "Returns true if value is NaN (Not-a-Number)."},
    {"isFinite(value)", "boolean", "Check if finite",
     "Returns true if value is a finite number."},
    {"String(value)", "string", "Convert to string",
     "Convert value to string."},
    {"Number(value)", "number", "Convert to number",
     "Convert value to a number."},
    {"Boolean(value)", "boolean", "Convert to boolean",
     "Convert value to true or false."},
    {"Promise.resolve(value)", "Promise", "Create resolved promise",
     "Returns a Promise that is resolved with the given value."},
    {"Promise.reject(error)", "Promise", "Create rejected promise",
     "Returns a Promise that is rejected with the given reason."},
    {"Promise.all(promises)", "Promise", "Wait for all promises",
     "Returns a promise that resolves when all input promises resolve."},
    {"Promise.race(promises)", "Promise", "First promise to settle",
     "Returns a promise that resolves/rejects with the first settled promise."},
    {"fetch(url, init)", "Promise<Response>", "HTTP request",
     "Fetch a resource from the network. Returns a Promise resolving to the Response."},
    {"setTimeout(fn, delay, ...args)", "number", "Delay execution",
     "Calls fn after delay milliseconds. Returns a timeout ID for clearTimeout."},
    {"setInterval(fn, delay, ...args)", "number", "Repeated execution",
     "Calls fn every delay milliseconds. Returns an interval ID for clearInterval."},
    {"clearTimeout(id)", "void", "Cancel timeout",
     "Cancels a timeout established by setTimeout."},
    {"clearInterval(id)", "void", "Cancel interval",
     "Cancels a repeated action established by setInterval."},
    {"Object.create(proto)", "Object", "Create with prototype",
     "Creates a new object with the specified prototype and optional properties."},
    {"Reflect.get(target, propKey, receiver)", "any", "Get property",
     "Gets a property from target, with optional receiver for getters."},
    {"Reflect.set(target, propKey, value, receiver)", "boolean", "Set property",
     "Sets a property on target, returning true if successful."},
    {"Symbol(description)", "symbol", "Create unique symbol",
     "Returns a new unique Symbol value with optional description."},
};

// Helper: count commas in a signature string (approximates parameter count)
static int countParams(const char* sig) {
    int count = 0;
    int depth = 0;
    while (*sig) {
        if (*sig == '(') ++depth;
        else if (*sig == ')') --depth;
        else if (*sig == ',' && depth == 1) ++count;
        ++sig;
    }
    return count + 1;  // n commas → n+1 params
}

// Helper: look up a function by name in a table
static const FuncEntry* lookupFunc(const char* name,
                                    const FuncEntry* table,
                                    size_t tableSize) {
    for (size_t i = 0; i < tableSize; ++i) {
        const char* p = table[i].signature;
        // Compare up to the opening paren
        while (*p && *p != '(' && *p == name[i]) {
            ++p; ++name;
        }
        // Match if we reached the opening paren on both
        if ((*p == '(' || *p == ' ') && *name == '(')
            return &table[i];
        // Also try prefix matching: table entry starts with name
        if (strncmp(table[i].signature, name, strlen(name)) == 0 &&
            table[i].signature[strlen(name)] == '(')
            return &table[i];
    }
    return nullptr;
}

bool FunctionCallTip::loadFunction()
{
    reset();
    if (_funcName.empty()) return false;

    const char* name = _funcName.c_str();

    // Determine which language we're in by the function name prefix.
    // A full implementation would query LanguageManager for the current language.
    // For now, search all three databases in order (C/C++ → Python → JavaScript).
    static const size_t cppCount = sizeof(kCppFunctions) / sizeof(kCppFunctions[0]);
    static const size_t pyCount = sizeof(kPythonFunctions) / sizeof(kPythonFunctions[0]);
    static const size_t jsCount = sizeof(kJsFunctions) / sizeof(kJsFunctions[0]);

    // Try C/C++ first
    if (const FuncEntry* e = lookupFunc(name, kCppFunctions, cppCount)) {
        FunctionOverload ov;
        ov.signature = e->signature;
        ov.returnType = e->returnType;
        ov.brief = e->brief;
        ov.documentation = e->doc ? e->doc : "";
        ov.paramCount = countParams(e->signature);
        _overloadList.push_back(ov);
        _retVals.push_back(e->returnType);
        _descriptions.push_back(e->brief);
        _currentNbOverloads = 1;
        return true;
    }

    // Try Python
    if (const FuncEntry* e = lookupFunc(name, kPythonFunctions, pyCount)) {
        FunctionOverload ov;
        ov.signature = e->signature;
        ov.returnType = e->returnType;
        ov.brief = e->brief;
        ov.documentation = e->doc ? e->doc : "";
        ov.paramCount = countParams(e->signature);
        _overloadList.push_back(ov);
        _retVals.push_back(e->returnType);
        _descriptions.push_back(e->brief);
        _currentNbOverloads = 1;
        return true;
    }

    // Try JavaScript
    if (const FuncEntry* e = lookupFunc(name, kJsFunctions, jsCount)) {
        FunctionOverload ov;
        ov.signature = e->signature;
        ov.returnType = e->returnType;
        ov.brief = e->brief;
        ov.documentation = e->doc ? e->doc : "";
        ov.paramCount = countParams(e->signature);
        _overloadList.push_back(ov);
        _retVals.push_back(e->returnType);
        _descriptions.push_back(e->brief);
        _currentNbOverloads = 1;
        return true;
    }

    return false;
}

// ─── Calltip rendering ─────────────────────────────────────────────────────────

void FunctionCallTip::showCalltip()
{
    if (!_pEditView) return;

    // If we have explicit overloads (populated via showForFunction), use those.
    // Otherwise use the legacy _retVals/_descriptions vectors.
    QString tipStr;

    if (!_overloadList.empty()) {
        const auto& ov = _overloadList[_currentOverload % _overloadList.size()];

        std::ostringstream oss;
        if (_overloadList.size() > 1) {
            // Scintilla uses \001 to start a section, \002 to end it (bold/indicator).
            oss << "\001" << (_currentOverload + 1) << " of "
                << _overloadList.size() << "\002\n";
        }

        // Return type + function name (shown in bold via \001…\002).
        if (!ov.returnType.empty()) {
            oss << "\001" << ov.returnType << " \002";
        }
        oss << _funcName << "\001(" << ov.signature << ")\002";

        // Parameter highlight: colour the current parameter.
        // Scintilla uses \003 to mark the start of a highlighted section,
        // \004 for the end.
        if (_currentParam > 0 && _currentParam <= static_cast<size_t>(ov.paramCount)) {
            // Count parameters in signature to insert highlight markers.
            // A simplified approach: wrap the N-th parameter in \003…\004.
            std::string sig = ov.signature;
            size_t openParen = sig.find('(');
            if (openParen != std::string::npos) {
                std::string params = sig.substr(openParen + 1);
                // Find the N-th comma-separated parameter.
                int commasSeen = 0;
                size_t pStart = 0;
                for (size_t i = 0; i < params.size(); ++i) {
                    if (params[i] == ',') {
                        ++commasSeen;
                        if (commasSeen == static_cast<int>(_currentParam)) {
                            // End of target parameter
                            pStart = i + 1;
                            while (pStart < params.size() &&
                                   (params[pStart] == ' ' || params[pStart] == '\t'))
                                ++pStart;
                            size_t pEnd = pStart;
                            while (pEnd < params.size() &&
                                   params[pEnd] != ',' && params[pEnd] != ')')
                                ++pEnd;
                            // Insert highlight markers
                            std::string before = sig.substr(0, openParen + 1 + pStart);
                            std::string highlighted = sig.substr(pStart, pEnd - pStart);
                            std::string after = sig.substr(openParen + 1 + pEnd);
                            oss.str("");
                            oss.clear();
                            if (!ov.returnType.empty())
                                oss << "\001" << ov.returnType << " \002";
                            oss << _funcName << "\001(" << before << "\003"
                                << highlighted << "\004" << after << ")\002";
                            break;
                        }
                    }
                }
            }
        }

        if (!ov.documentation.empty())
            oss << "\n\001" << ov.documentation << "\002";

        tipStr = QString::fromLatin1(oss.str().c_str());
    } else {
        // Legacy path: use _retVals and _descriptions.
        if (_currentNbOverloads == 0) return;

        std::ostringstream oss;
        if (_currentNbOverloads > 1) {
            oss << "\001" << (_currentOverload + 1) << " of "
                << _currentNbOverloads << "\002\n";
        }
        if (!_retVals.empty() && _currentOverload < _retVals.size())
            oss << "\001" << _retVals[_currentOverload] << " \002";
        oss << _funcName << "()";
        if (_currentOverload < _descriptions.size() && _descriptions[_currentOverload][0])
            oss << "\n" << _descriptions[_currentOverload];
        tipStr = QString::fromLatin1(oss.str().c_str());
    }

    if (isVisible()) {
        _pEditView->send(SCI_CALLTIPCANCEL);
    } else {
        _startPos = _curPos;
    }

    // Show calltip at current position.
    // Scintilla will automatically position it above or below the cursor
    // depending on available space.
    const QByteArray tipUtf8 = tipStr.toUtf8();  // store — dangling pointer otherwise
    _pEditView->send(SCI_CALLTIPSHOW, _startPos,
                     reinterpret_cast<sptr_t>(tipUtf8.constData()));
    _selfActivated = true;

    // Highlight the current parameter if any.
    if (_currentParam > 0) {
        highlightParam(static_cast<int>(_currentParam));
    }
}

void FunctionCallTip::updateParamHighlight()
{
    if (!isVisible()) return;
    highlightParam(static_cast<int>(_currentParam));
}

// paramHighlightChanged is called by highlightParam() to notify listeners
void FunctionCallTip::paramHighlightChanged(int paramIndex) {
    emit paramHighlightChangedSignal(paramIndex);
}

void FunctionCallTip::onDismissed() {
    emit dismissed();
}
