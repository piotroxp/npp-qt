// Semantic Lift: Win32 FunctionList/functionParser → Qt6 Function Parser
// Original: PowerEditor/src/WinControls/FunctionList/functionParser.h
// Target: npp-qt/src/WinControls/functionParser.h

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <QString>
#include <QVector>
#include <QMap>
#include <QString>
#include <QXmlStreamReader>

// Language type constants (mirrors Win32 Notepad_plus_msgs.h LangType enum)
#include "NppConstants.h"  // defines LangType enum, L_USER=15, L_EXTERNAL=95
#include "ScintillaComponent.h"

// ScintillaComponent is the Qt6 port of ScintillaEditView
// (defined in src/ScintillaComponent.h).
// Add this typedef so parser signatures stay compatible with Win32 originals.
class ScintillaComponent;
using ScintillaEditView = ScintillaComponent;

// TreeStateNode is defined in TreeView.h (shared between tree widgets)
#include "TreeView.h"

// Helper: read current editor foreground/background from Scintilla lexer style 0.
// Used by AboutDlg to draw the chameleon icon matching the active lexer.
// NOTE: For dark-mode fallback, include NppDarkMode.h and check
//       NppDarkMode::instance().isEnabled() after calling this.
static inline QPair<QColor, QColor> getLexerColors(ScintillaEditView* sci)
{
    if (!sci) {
        // Fallback: light theme defaults
        return qMakePair(QColor(0x00, 0x00, 0x00), QColor(0xFF, 0xFF, 0xFF));
    }

    // Style 0 is the default style (lexer neutral)
    // send() returns sptr_t (64-bit); SCI_STYLEGETFORE/BACK returns a COLORREF as sptr_t
    QRgb fgRgb = static_cast<QRgb>(sci->send(SCI_STYLEGETFORE, 0));
    QRgb bgRgb = static_cast<QRgb>(sci->send(SCI_STYLEGETBACK, 0));
    QColor fg = QColor::fromRgba(fgRgb);
    QColor bg = QColor::fromRgba(bgRgb);

    // If the lexer hasn't set colors yet (black), fall back to light theme
    if (fg.rgb() == 0 && bg.rgb() == 0) {
        fg = QColor(0x00, 0x00, 0x00);
        bg = QColor(0xFF, 0xFF, 0xFF);
    }
    return qMakePair(fg, bg);
}

// =============================================================================
// foundInfo — parsed function/class info (lifted from functionParser.h)
// =============================================================================

struct foundInfo final
{
    std::string _data;     // Function name (leaf)
    std::string _data2;    // Class/namespace name (node), empty for top-level
    intptr_t _pos = -1;   // Start position in document
    intptr_t _pos2 = -1;  // Secondary position (e.g., class name)
};

// =============================================================================
// FunctionParser — base class for regex-based function parsers (lifted from functionParser.h)
// =============================================================================

class FunctionParser
{
    friend class FunctionParsersManager;

public:
    FunctionParser(const char* id, const char* displayName,
                   const std::string& commentExpr,
                   const std::string& functionExpr,
                   const std::vector<std::string>& functionNameExprArray,
                   const std::vector<std::string>& classNameExprArray) noexcept
        : _id(id), _displayName(displayName),
          _commentExpr(commentExpr),
          _functionExpr(functionExpr),
          _functionNameExprArray(functionNameExprArray),
          _classNameExprArray(classNameExprArray)
    {}

    virtual void parse(std::vector<foundInfo>& foundInfos, size_t begin, size_t end,
                       ScintillaEditView** ppEditView,
                       const std::string& classStructName = "") = 0;

    void funcParse(std::vector<foundInfo>& foundInfos, size_t begin, size_t end,
                   ScintillaEditView** ppEditView,
                   const std::string& classStructName = "",
                   const std::vector<std::pair<size_t, size_t>>* commentZones = nullptr);

    static bool isInZones(size_t pos2Test, const std::vector<std::pair<size_t, size_t>>& zones);

    virtual ~FunctionParser() = default;

protected:
    std::string _id;
    std::string _displayName;
    std::string _commentExpr;
    std::string _functionExpr;
    std::vector<std::string> _functionNameExprArray;
    std::vector<std::string> _classNameExprArray;

    void getCommentZones(std::vector<std::pair<size_t, size_t>>& commentZone,
                         size_t begin, size_t end, ScintillaEditView** ppEditView);

    void getInvertZones(std::vector<std::pair<size_t, size_t>>& destZones,
                        const std::vector<std::pair<size_t, size_t>>& sourceZones,
                        size_t begin, size_t end);

    static std::string parseSubLevel(size_t begin, size_t end,
                                     std::vector<std::string> dataToSearch,
                                     intptr_t& foundPos,
                                     ScintillaEditView** ppEditView);
};

// =============================================================================
// FunctionZoneParser — parses functions inside class/namespace zones (lifted from functionParser.h)
// =============================================================================

class FunctionZoneParser : public FunctionParser
{
public:
    FunctionZoneParser() = delete;

    FunctionZoneParser(const char* id, const char* displayName,
                       const std::string& commentExpr,
                       const std::string& rangeExpr,
                       const std::string& openSymbole,
                       const std::string& closeSymbole,
                       const std::vector<std::string>& classNameExprArray,
                       const std::string& functionExpr,
                       const std::vector<std::string>& functionNameExprArray) noexcept
        : FunctionParser(id, displayName, commentExpr, functionExpr, functionNameExprArray, classNameExprArray),
          _rangeExpr(rangeExpr),
          _openSymbole(openSymbole),
          _closeSymbole(closeSymbole)
    {}

    void parse(std::vector<foundInfo>& foundInfos, size_t begin, size_t end,
               ScintillaEditView** ppEditView,
               const std::string& classStructName = "") override;

protected:
    void classParse(std::vector<foundInfo>& foundInfos,
                    std::vector<std::pair<size_t, size_t>>& scannedZones,
                    const std::vector<std::pair<size_t, size_t>>& commentZones,
                    size_t begin, size_t end,
                    ScintillaEditView** ppEditView,
                    const std::string& classStructName = "");

private:
    std::string _rangeExpr;
    std::string _openSymbole;
    std::string _closeSymbole;

    static size_t getBodyClosePos(size_t begin, const char* bodyOpenSymbol,
                                   const char* bodyCloseSymbol,
                                   const std::vector<std::pair<size_t, size_t>>& commentZones,
                                   ScintillaEditView** ppEditView);
};

// =============================================================================
// FunctionUnitParser — parses standalone functions (lifted from functionParser.h)
// =============================================================================

class FunctionUnitParser : public FunctionParser
{
public:
    FunctionUnitParser(const char* id, const char* displayName,
                       const std::string& commentExpr,
                       const std::string& mainExpr,
                       const std::vector<std::string>& functionNameExprArray,
                       const std::vector<std::string>& classNameExprArray) noexcept
        : FunctionParser(id, displayName, commentExpr, mainExpr, functionNameExprArray, classNameExprArray)
    {}

    void parse(std::vector<foundInfo>& foundInfos, size_t begin, size_t end,
               ScintillaEditView** ppEditView,
               const std::string& classStructName = "") override;
};

// =============================================================================
// FunctionMixParser — combines zone and unit parsing (lifted from functionParser.h)
// =============================================================================

class FunctionMixParser : public FunctionZoneParser
{
public:
    FunctionMixParser(const char* id, const char* displayName,
                      const std::string& commentExpr,
                      const std::string& rangeExpr,
                      const std::string& openSymbole,
                      const std::string& closeSymbole,
                      const std::vector<std::string>& classNameExprArray,
                      const std::string& functionExpr,
                      const std::vector<std::string>& functionNameExprArray,
                      std::unique_ptr<FunctionUnitParser> funcUnitParser) noexcept
        : FunctionZoneParser(id, displayName, commentExpr, rangeExpr, openSymbole, closeSymbole,
                             classNameExprArray, functionExpr, functionNameExprArray),
          _funcUnitParser(std::move(funcUnitParser))
    {}

    void parse(std::vector<foundInfo>& foundInfos, size_t begin, size_t end,
               ScintillaEditView** ppEditView,
               const std::string& classStructName = "") override;

private:
    std::unique_ptr<FunctionUnitParser> _funcUnitParser = nullptr;
};

// =============================================================================
// AssociationInfo — language/extension association (lifted from functionParser.h)
// =============================================================================

struct AssociationInfo final
{
    int _id;
    int _langID;
    QString _ext;
    QString _userDefinedLangName;

    AssociationInfo(int id, int langID, const wchar_t* ext, const wchar_t* userDefinedLangName) noexcept
        : _id(id), _langID(langID)
    {
        if (ext)
            _ext = QString::fromWCharArray(ext);
        else
            _ext.clear();

        if (userDefinedLangName)
            _userDefinedLangName = QString::fromWCharArray(userDefinedLangName);
        else
            _userDefinedLangName.clear();
    }

    AssociationInfo(int id, int langID, const QString& ext, const QString& userDefinedLangName) noexcept
        : _id(id), _langID(langID), _ext(ext), _userDefinedLangName(userDefinedLangName)
    {}
};

inline constexpr int nbMaxUserDefined = 25;

// =============================================================================
// ParserInfo — parser configuration (lifted from functionParser.h)
// =============================================================================

struct ParserInfo
{
    QString _id;  // XML parser rule file name
    std::unique_ptr<FunctionParser> _parser = nullptr;
    QString _userDefinedLangName;

    ParserInfo() {}
    explicit ParserInfo(const QString& id) noexcept : _id(id) {}
    explicit ParserInfo(const QString& id, const QString& userDefinedLangName) noexcept
        : _id(id), _userDefinedLangName(userDefinedLangName) {}
};

// =============================================================================
// FunctionParsersManager — loads and manages parsers by language (lifted from functionParser.h)
// =============================================================================

class FunctionParsersManager final
{
public:
    bool init(const QString& xmlDirPath, const QString& xmlInstalledPath, ScintillaEditView** ppEditView);
    bool parse(std::vector<foundInfo>& foundInfos, const AssociationInfo& assoInfo);

    // For Qt: XML loading from QString paths
    bool loadFromXml(const QString& xmlContent, LangType lType, const QString& overrideId, int udlIndex = -1);

private:
    ScintillaEditView** _ppEditView = nullptr;
    QString _xmlDirPath;
    QString _xmlDirInstalledPath;

    std::unique_ptr<ParserInfo> _parsers[L_EXTERNAL + nbMaxUserDefined] = { nullptr };
    int _currentUDIndex = L_EXTERNAL;

    bool getOverrideMapFromXmlTree(const QString& xmlDirPath);
    bool loadFuncListFromXmlTree(const QString& xmlDirPath, LangType lType,
                                  const QString& overrideId, int udlIndex = -1);

    static bool getZoneParserParameters(const QString& mainExpr, const QString& openSymbole,
                                        const QString& closeSymbole,
                                        const QVector<QString>& classNameExprArray,
                                        const QString& functionExpr,
                                        const QVector<QString>& functionNameExprArray,
                                        std::string& outMainExpr, std::string& outOpenSym,
                                        std::string& outCloseSym,
                                        std::vector<std::string>& outClassNameExpr,
                                        std::string& outFuncExpr,
                                        std::vector<std::string>& outFuncNameExpr);

    static bool getUnitParserParameters(const QString& mainExpr,
                                        const QVector<QString>& functionNameExprArray,
                                        const QVector<QString>& classNameExprArray,
                                        std::string& outMainExpr,
                                        std::vector<std::string>& outFuncNameExpr,
                                        std::vector<std::string>& outClassNameExpr);

    FunctionParser* getParser(const AssociationInfo& assoInfo);
};
