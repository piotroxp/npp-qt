// Custom QsciLexer for Rust
// Based on QsciLexerCPP with Rust-specific keyword sets
// Custom styles: RustAttribute (20), RustLifetime (21), RustMacro (22)

#include "LexerRust.h"

#include <QColor>
#include <QFont>

LexerRust::LexerRust(QObject* parent)
    : QsciLexerCPP(parent)
{
    // Set Rust keywords (set 0) — keywords
    QString rustKeywordsStr = QStringLiteral(
        "as async await break const continue crate dyn else enum extern false fn for if impl in let loop match mod move mut pub ref return self Self static struct super trait true type unsafe where while"
    );
    _rustKeywords = rustKeywordsStr.toLatin1();
    setKeywords(0, _rustKeywords.constData());

    // Set Rust types (set 1) — primitive types and std types
    QString rustTypesStr = QStringLiteral(
        "bool char f32 f64 i8 i16 i32 i64 i128 isize str String u8 u16 u32 u64 u128 usize Box Option Result Vec HashMap HashSet Arc Mutex RwLock"
    );
    _rustTypes = rustTypesStr.toLatin1();
    setKeywords(1, _rustTypes.constData());

    // Set Rust builtins (set 2) — macros and built-in functions
    QString rustBuiltinsStr = QStringLiteral(
        "println! eprintln! print! eprint! format! panic! assert! assert_eq! assert_ne! todo! unimplemented! unreachable! vec! stringify! concat! include! include_str! include_bytes! dbg!"
    );
    _rustBuiltins = rustBuiltinsStr.toLatin1();
    setKeywords(2, _rustBuiltins.constData());

    // Set Rust attributes (set 3) — attribute annotations
    QString rustAttributesStr = QStringLiteral(
        "derive Clone Copy Debug Default Eq Ord Hash PartialEq PartialOrd serde Serialize Deserialize wasm_bindgen test bench inline never_type must_use"
    );
    _rustAttributes = rustAttributesStr.toLatin1();
    setKeywords(3, _rustAttributes.constData());
}

LexerRust::~LexerRust()
{
}

const char* LexerRust::language() const
{
    return "Rust";
}

const char* LexerRust::lexer() const
{
    return "Rust";
}

QString LexerRust::description(int style) const
{
    switch (style) {
        case RustAttribute: return QStringLiteral("Attribute");
        case RustLifetime:  return QStringLiteral("Lifetime");
        case RustMacro:    return QStringLiteral("Macro");
        default:            return QsciLexerCPP::description(style);
    }
}

QColor LexerRust::defaultColor(int style) const
{
    switch (style) {
        case RustAttribute: return QColor(138, 43, 226);   // purple
        case RustLifetime:  return QColor(139, 0, 0);        // dark red
        case RustMacro:    return QColor(0, 128, 128);     // teal
        default:           return QsciLexerCPP::defaultColor(style);
    }
}

QFont LexerRust::defaultFont(int style) const
{
    QFont f = QsciLexerCPP::defaultFont(style);
    if (style == 15) {  // DocComment
        f.setItalic(true);
    }
    return f;
}
