// Semantic Lift: Win32 FunctionList/functionParser → Qt6 Function Parser Implementation
// Original: PowerEditor/src/WinControls/FunctionList/functionParser.cpp
// Target: npp-qt/src/WinControls/functionParser.cpp
//
// Translates:
//   - Win32 file I/O → QFile / QXmlStreamReader
//   - BoostRegexSearch → stub (needs separate Boost lift)
//   - ScintillaEditView calls preserved (cross-platform SciCall interface)
//   - NppXml → QXmlStreamReader wrappers

#include "functionParser.h"
// ScintillaEditView (typedef → ScintillaComponent) lives in parent src/
#include "ScintillaComponent.h"

#include <QFile>
#include <QXmlStreamReader>
#include <QFileInfo>
#include <QDir>

// =============================================================================
// FunctionParsersManager::init
// Mirrors Win32 init() — sets paths and loads override map.
// =============================================================================

bool FunctionParsersManager::init(const QString& xmlDirPath,
                                   const QString& xmlInstalledPath,
                                   ScintillaEditView** ppEditView)
{
    _ppEditView = ppEditView;
    _xmlDirPath = xmlDirPath;
    _xmlDirInstalledPath = xmlInstalledPath;

    bool isOK = getOverrideMapFromXmlTree(_xmlDirPath);
    if (isOK)
        return true;
    else if (_xmlDirPath != _xmlInstalledPath && !_xmlInstalledPath.isEmpty())
        return getOverrideMapFromXmlTree(_xmlDirInstalledPath);
    else
        return false;
}

// =============================================================================
// XML loading helpers
// Mirrors NppXml loading — uses QXmlStreamReader.
// =============================================================================

// Forward declaration of language enum (from Notepad_plus_msgs.h)
// L_USER = 15, L_EXTERNAL = 95
enum LangType : int;
constexpr int L_USER = 15;
constexpr int L_EXTERNAL = 95;

// Stub: load XML content from a file path
// Full implementation would use QFile + QXmlStreamReader
static bool loadXmlFile(const QString& filePath, QString& outContent)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
    QTextStream stream(&file);
    outContent = stream.readAll();
    return true;
}

// Parse overrideMap.xml and populate _parsers array
bool FunctionParsersManager::getOverrideMapFromXmlTree(const QString& xmlDirPath)
{
    QString overrideMapPath = xmlDirPath;
    if (!overrideMapPath.endsWith(QDir::separator()))
        overrideMapPath += QDir::separator();
    overrideMapPath += QStringLiteral("overrideMap.xml");

    QString xmlContent;
    if (!loadXmlFile(overrideMapPath, xmlContent))
        return false;

    QXmlStreamReader xml(xmlContent);
    bool inAssociationMap = false;

    while (!xml.atEnd()) {
        QXmlStreamReader::TokenType token = xml.readNext();
        QStringView name = xml.name();

        if (token == QXmlStreamReader::StartElement) {
            if (name == QStringLiteral("associationMap")) {
                inAssociationMap = true;
            } else if (inAssociationMap && name == QStringLiteral("association")) {
                QString id = xml.attributes().value(QStringLiteral("id")).toString();
                QString langIdStr = xml.attributes().value(QStringLiteral("langID")).toString();
                QString userDefLang = xml.attributes().value(QStringLiteral("userDefinedLangName")).toString();

                if (!id.isEmpty()) {
                    int langID = langIdStr.toInt();
                    if (langID >= 0) {
                        // Standard language
                        if (langID <= L_EXTERNAL + nbMaxUserDefined) {
                            _parsers[langID] = std::make_unique<ParserInfo>(id);
                        }
                    } else if (!userDefLang.isEmpty()) {
                        // User-defined language
                        ++_currentUDIndex;
                        if (_currentUDIndex < L_EXTERNAL + nbMaxUserDefined) {
                            _parsers[_currentUDIndex] = std::make_unique<ParserInfo>(id, userDefLang);
                        }
                    }
                }
            }
        } else if (token == QXmlStreamReader::EndElement && name == QStringLiteral("associationMap")) {
            inAssociationMap = false;
        }
    }

    return true;
}

// =============================================================================
// getZoneParserParameters — extract class range parser params from XML attributes
// Mirrors Win32 getZonePaserParameters().
// =============================================================================

bool FunctionParsersManager::getZoneParserParameters(
    const QString& mainExpr, const QString& openSymbole,
    const QString& closeSymbole,
    const QVector<QString>& classNameExprArray,
    const QString& functionExpr,
    const QVector<QString>& functionNameExprArray,
    std::string& outMainExpr, std::string& outOpenSym,
    std::string& outCloseSym,
    std::vector<std::string>& outClassNameExpr,
    std::string& outFuncExpr,
    std::vector<std::string>& outFuncNameExpr)
{
    if (mainExpr.isEmpty())
        return false;
    outMainExpr = mainExpr.toStdString();

    if (!openSymbole.isEmpty())
        outOpenSym = openSymbole.toStdString();
    if (!closeSymbole.isEmpty())
        outCloseSym = closeSymbole.toStdString();

    for (const QString& expr : classNameExprArray) {
        if (!expr.isEmpty())
            outClassNameExpr.push_back(expr.toStdString());
    }

    if (!functionExpr.isEmpty())
        outFuncExpr = functionExpr.toStdString();

    for (const QString& expr : functionNameExprArray) {
        if (!expr.isEmpty())
            outFuncNameExpr.push_back(expr.toStdString());
    }

    return true;
}

// =============================================================================
// getUnitParserParameters — extract function unit parser params from XML
// Mirrors Win32 getUnitPaserParameters().
// =============================================================================

bool FunctionParsersManager::getUnitParserParameters(
    const QString& mainExpr,
    const QVector<QString>& functionNameExprArray,
    const QVector<QString>& classNameExprArray,
    std::string& outMainExpr,
    std::vector<std::string>& outFuncNameExpr,
    std::vector<std::string>& outClassNameExpr)
{
    if (mainExpr.isEmpty())
        return false;
    outMainExpr = mainExpr.toStdString();

    for (const QString& expr : functionNameExprArray) {
        if (!expr.isEmpty())
            outFuncNameExpr.push_back(expr.toStdString());
    }

    for (const QString& expr : classNameExprArray) {
        if (!expr.isEmpty())
            outClassNameExpr.push_back(expr.toStdString());
    }

    return true;
}

// =============================================================================
// loadFuncListFromXmlTree — parse a functionList .xml rule file
// Mirrors Win32 loadFuncListFromXmlTree().
// =============================================================================

bool FunctionParsersManager::loadFuncListFromXmlTree(const QString& xmlDirPath,
                                                      LangType lType,
                                                      const QString& overrideId,
                                                      int udlIndex)
{
    QString funcListRulePath = xmlDirPath;
    if (!funcListRulePath.endsWith(QDir::separator()))
        funcListRulePath += QDir::separator();

    int index = -1;
    if (lType == L_USER) {
        if (overrideId.isEmpty())
            return false;
        if (udlIndex == -1)
            return false;
        index = udlIndex;
        funcListRulePath += overrideId;
    } else {
        index = lType;
        if (overrideId.isEmpty()) {
            // Derive lexer name from language
            // std::wstring lexerName = ScintillaEditView::_langNameInfoArray[lType]._langName;
            funcListRulePath += overrideId.isEmpty() ? QStringLiteral("default.xml") : overrideId;
        } else {
            funcListRulePath += overrideId;
        }
    }

    if (index > _currentUDIndex)
        return false;
    if (_parsers[index] == nullptr)
        return false;

    QString xmlContent;
    if (!loadXmlFile(funcListRulePath, xmlContent))
        return false;

    // Parse the XML
    QXmlStreamReader xml(xmlContent);
    bool inParser = false;
    QString commentExpr;
    QString classRangeExpr;
    QString functionExpr;
    QString openSymbole;
    QString closeSymbole;
    QVector<QString> classNameExprArray;
    QVector<QString> functionNameExprArray;
    QString displayName;
    QString parserId;

    while (!xml.atEnd()) {
        QXmlStreamReader::TokenType token = xml.readNext();
        QStringView name = xml.name();

        if (token == QXmlStreamReader::StartElement) {
            if (name == QStringLiteral("parser")) {
                parserId = xml.attributes().value(QStringLiteral("id")).toString();
                displayName = xml.attributes().value(QStringLiteral("displayName")).toString();
                if (displayName.isEmpty())
                    displayName = parserId;
                QString cExpr = xml.attributes().value(QStringLiteral("commentExpr")).toString();
                if (!cExpr.isEmpty())
                    commentExpr = cExpr;
                inParser = true;
            } else if (inParser && name == QStringLiteral("classRange")) {
                classRangeExpr = xml.attributes().value(QStringLiteral("mainExpr")).toString();
                openSymbole = xml.attributes().value(QStringLiteral("openSymbole")).toString();
                closeSymbole = xml.attributes().value(QStringLiteral("closeSymbole")).toString();
                classNameExprArray.clear();
                functionNameExprArray.clear();
            } else if (inParser && name == QStringLiteral("function")) {
                functionExpr = xml.attributes().value(QStringLiteral("mainExpr")).toString();
            } else if (inParser && name == QStringLiteral("className")) {
                // Parse child nameExpr elements
                // (simplified: collect direct text for now)
            } else if (inParser && name == QStringLiteral("nameExpr")) {
                QString expr = xml.attributes().value(QStringLiteral("expr")).toString();
                if (!expr.isEmpty()) {
                    if (!classRangeExpr.isEmpty() || !classNameExprArray.isEmpty())
                        classNameExprArray.append(expr);
                    else
                        functionNameExprArray.append(expr);
                }
            } else if (inParser && name == QStringLiteral("functionName")) {
                // Parse child nameExpr elements
            }
        } else if (token == QXmlStreamReader::EndElement && name == QStringLiteral("parser")) {
            inParser = false;
        }
    }

    if (parserId.isEmpty())
        return false;

    // Build the appropriate parser type
    const char* idStr = parserId.toUtf8().constData();
    const char* displayStr = displayName.toUtf8().constData();

    if (!classRangeExpr.isEmpty() && !functionExpr.isEmpty()) {
        // FunctionMixParser
        std::string cMain, cOpen, cClose, cFunc;
        std::vector<std::string> cClassExpr, cFuncExpr;
        getZoneParserParameters(classRangeExpr, openSymbole, closeSymbole,
                                classNameExprArray, functionExpr, functionNameExprArray,
                                cMain, cOpen, cClose, cClassExpr, cFunc, cFuncExpr);

        std::string uMain;
        std::vector<std::string> uFuncExpr, uClassExpr;
        getUnitParserParameters(functionExpr, functionNameExprArray, {},
                                uMain, uFuncExpr, uClassExpr);

        auto funcUnitParser = std::make_unique<FunctionUnitParser>(
            idStr, displayStr,
            commentExpr.toStdString(),
            uMain, uFuncExpr, uClassExpr);

        _parsers[index]->_parser = std::make_unique<FunctionMixParser>(
            idStr, displayStr,
            commentExpr.toStdString(),
            cMain, cOpen, cClose,
            cClassExpr, cFunc, cFuncExpr,
            std::move(funcUnitParser));

    } else if (!classRangeExpr.isEmpty()) {
        // FunctionZoneParser
        std::string cMain, cOpen, cClose, cFunc;
        std::vector<std::string> cClassExpr, cFuncExpr;
        getZoneParserParameters(classRangeExpr, openSymbole, closeSymbole,
                                classNameExprArray, functionExpr, functionNameExprArray,
                                cMain, cOpen, cClose, cClassExpr, cFunc, cFuncExpr);

        _parsers[index]->_parser = std::make_unique<FunctionZoneParser>(
            idStr, displayStr,
            commentExpr.toStdString(),
            cMain, cOpen, cClose,
            cClassExpr, cFunc, cFuncExpr);

    } else if (!functionExpr.isEmpty()) {
        // FunctionUnitParser
        std::string uMain;
        std::vector<std::string> uFuncExpr, uClassExpr;
        getUnitParserParameters(functionExpr, functionNameExprArray, {},
                                uMain, uFuncExpr, uClassExpr);

        _parsers[index]->_parser = std::make_unique<FunctionUnitParser>(
            idStr, displayStr,
            commentExpr.toStdString(),
            uMain, uFuncExpr, uClassExpr);
    }

    return _parsers[index]->_parser != nullptr;
}

// =============================================================================
// getParser — find the parser for a given language association
// Mirrors Win32 getParser().
// =============================================================================

FunctionParser* FunctionParsersManager::getParser(const AssociationInfo& assoInfo)
{
    enum ParserChoice : unsigned char {
        doNothing,
        checkLangID,
        checkUserDefined
    };

    unsigned char choice = doNothing;
    if (assoInfo._langID != -1 && assoInfo._langID != L_USER)
        choice = checkLangID;
    else if (assoInfo._langID == L_USER && !assoInfo._userDefinedLangName.isEmpty())
        choice = checkUserDefined;
    else
        return nullptr;

    switch (choice) {
        case checkLangID: {
            if (_parsers[assoInfo._langID] != nullptr) {
                if (_parsers[assoInfo._langID]->_parser != nullptr)
                    return _parsers[assoInfo._langID]->_parser.get();

                // Load it
                if (loadFuncListFromXmlTree(_xmlDirPath, static_cast<LangType>(assoInfo._langID),
                                            _parsers[assoInfo._langID]->_id))
                    return _parsers[assoInfo._langID]->_parser.get();
                else if (_xmlDirPath != _xmlDirInstalledPath && !_xmlDirInstalledPath.isEmpty() &&
                         loadFuncListFromXmlTree(_xmlDirInstalledPath, static_cast<LangType>(assoInfo._langID),
                                                  _parsers[assoInfo._langID]->_id))
                    return _parsers[assoInfo._langID]->_parser.get();
            } else {
                _parsers[assoInfo._langID] = std::make_unique<ParserInfo>();
                if (loadFuncListFromXmlTree(_xmlDirPath, static_cast<LangType>(assoInfo._langID),
                                            _parsers[assoInfo._langID]->_id))
                    return _parsers[assoInfo._langID]->_parser.get();
                if (_xmlDirPath != _xmlDirInstalledPath && !_xmlDirInstalledPath.isEmpty() &&
                    loadFuncListFromXmlTree(_xmlDirInstalledPath, static_cast<LangType>(assoInfo._langID),
                                             _parsers[assoInfo._langID]->_id))
                    return _parsers[assoInfo._langID]->_parser.get();
                return nullptr;
            }
            break;
        }

        case checkUserDefined: {
            if (_currentUDIndex == L_EXTERNAL)
                return nullptr;

            for (int i = L_EXTERNAL + 1; i <= _currentUDIndex; ++i) {
                if (_parsers[i] && _parsers[i]->_userDefinedLangName == assoInfo._userDefinedLangName) {
                    if (_parsers[i]->_parser)
                        return _parsers[i]->_parser.get();

                    if (loadFuncListFromXmlTree(_xmlDirPath, static_cast<LangType>(assoInfo._langID),
                                                _parsers[i]->_id, i))
                        return _parsers[i]->_parser.get();
                    if (_xmlDirPath != _xmlDirInstalledPath && !_xmlDirInstalledPath.isEmpty() &&
                        loadFuncListFromXmlTree(_xmlDirInstalledPath, static_cast<LangType>(assoInfo._langID),
                                                 _parsers[i]->_id, i))
                        return _parsers[i]->_parser.get();
                    break;
                }
            }
            return nullptr;
        }
    }
    return nullptr;
}

// =============================================================================
// FunctionParser::funcParse — core regex-based function search
// Mirrors Win32 funcParse() — uses Scintilla regex search.
// =============================================================================

void FunctionParser::funcParse(
    std::vector<foundInfo>& foundInfos,
    size_t begin, size_t end,
    ScintillaEditView** ppEditView,
    const std::string& classStructName,
    const std::vector<std::pair<size_t, size_t>>* commentZones)
{
    if (begin >= end)
        return;
    if (_functionExpr.empty())
        return;

    if (!(*ppEditView))
        return;

    static constexpr int flags = SCFIND_REGEXP | SCFIND_POSIX | SCFIND_REGEXP_DOTMATCHESNL;

    (*ppEditView)->execute(SCI_SETSEARCHFLAGS, flags);
    intptr_t targetStart = (*ppEditView)->searchInTarget(_functionExpr, begin, end);

    while (targetStart >= 0) {
        targetStart = (*ppEditView)->execute(SCI_GETTARGETSTART);
        intptr_t targetEnd = (*ppEditView)->execute(SCI_GETTARGETEND);
        if (targetEnd > static_cast<intptr_t>(end))
            break;

        size_t foundTextLen = targetEnd - targetStart;
        if (targetStart + foundTextLen == end)
            break;

        foundInfo fi;

        if (_functionNameExprArray.empty() && _classNameExprArray.empty()) {
            auto foundData = std::string(1024, '\0');
            (*ppEditView)->getGenericText(foundData.data(), foundData.length(),
                                          static_cast<long>(targetStart),
                                          static_cast<long>(targetEnd));
            fi._data = foundData;
            fi._pos = targetStart;
        } else {
            intptr_t foundPos = -1;
            if (!_functionNameExprArray.empty()) {
                fi._data = parseSubLevel(targetStart, targetEnd,
                                          _functionNameExprArray, foundPos, ppEditView);
                fi._pos = foundPos;
            }

            if (!classStructName.empty()) {
                fi._data2 = classStructName;
                fi._pos2 = -1;
            } else if (!_classNameExprArray.empty()) {
                fi._data2 = parseSubLevel(targetStart, targetEnd,
                                           _classNameExprArray, foundPos, ppEditView);
                fi._pos2 = foundPos;
            }
        }

        if (fi._pos != -1 || fi._pos2 != -1) {
            if (commentZones != nullptr) {
                if (!isInZones(fi._pos, *commentZones) && !isInZones(fi._pos2, *commentZones))
                    foundInfos.push_back(fi);
            } else {
                foundInfos.push_back(fi);
            }
        }

        begin = targetStart + foundTextLen;
        targetStart = (*ppEditView)->searchInTarget(_functionExpr, begin, end);
    }
}

// =============================================================================
// FunctionParser::parseSubLevel — nested regex extraction
// Mirrors Win32 parseSubLevel().
// =============================================================================

std::string FunctionParser::parseSubLevel(size_t begin, size_t end,
                                           std::vector<std::string> dataToSearch,
                                           intptr_t& foundPos,
                                           ScintillaEditView** ppEditView)
{
    if (begin >= end) {
        foundPos = -1;
        return "";
    }
    if (dataToSearch.empty())
        return "";

    if (!(*ppEditView))
        return "";

    static constexpr int flags = SCFIND_REGEXP | SCFIND_POSIX | SCFIND_REGEXP_DOTMATCHESNL;

    (*ppEditView)->execute(SCI_SETSEARCHFLAGS, flags);
    intptr_t targetStart = (*ppEditView)->searchInTarget(dataToSearch[0], begin, end);

    if (targetStart < 0) {
        foundPos = -1;
        return "";
    }
    intptr_t targetEnd = (*ppEditView)->execute(SCI_GETTARGETEND);

    if (dataToSearch.size() >= 2) {
        dataToSearch.erase(dataToSearch.begin());
        return parseSubLevel(targetStart, targetEnd, dataToSearch, foundPos, ppEditView);
    }

    auto foundStr = std::string(1024, '\0');
    (*ppEditView)->getGenericText(foundStr.data(), foundStr.length(),
                                   static_cast<long>(targetStart),
                                   static_cast<long>(targetEnd));

    foundPos = targetStart;
    return foundStr;
}

// =============================================================================
// FunctionParsersManager::parse — main entry point
// Mirrors Win32 parse().
// =============================================================================

bool FunctionParsersManager::parse(std::vector<foundInfo>& foundInfos,
                                    const AssociationInfo& assoInfo)
{
    FunctionParser* fp = getParser(assoInfo);
    if (!fp)
        return false;

    size_t docLen = (*_ppEditView)->getCurrentDocLen();
    fp->parse(foundInfos, 0, docLen, _ppEditView);

    return true;
}

// =============================================================================
// FunctionZoneParser::getBodyClosePos — balanced bracket finder
// Mirrors Win32 getBodyClosePos().
// =============================================================================

size_t FunctionZoneParser::getBodyClosePos(
    size_t begin,
    const char* bodyOpenSymbol,
    const char* bodyCloseSymbol,
    const std::vector<std::pair<size_t, size_t>>& commentZones,
    ScintillaEditView** ppEditView)
{
    size_t cntOpen = 1;
    if (!(*ppEditView))
        return begin;

    size_t docLen = (*ppEditView)->getCurrentDocLen();
    if (begin >= docLen)
        return docLen;

    std::string exprToSearch = "(";
    exprToSearch += bodyOpenSymbol;
    exprToSearch += "|";
    exprToSearch += bodyCloseSymbol;
    exprToSearch += ")";

    static constexpr int flags = SCFIND_REGEXP | SCFIND_POSIX | SCFIND_REGEXP_DOTMATCHESNL;

    (*ppEditView)->execute(SCI_SETSEARCHFLAGS, flags);
    intptr_t targetStart = (*ppEditView)->searchInTarget(exprToSearch, begin, docLen);

    do {
        if (targetStart >= 0) {
            intptr_t targetEnd = (*ppEditView)->execute(SCI_GETTARGETEND);
            if (!isInZones(targetStart, commentZones)) {
                intptr_t tmpStart = (*ppEditView)->searchInTarget(bodyOpenSymbol, targetStart, targetEnd);
                if (tmpStart >= 0) {
                    ++cntOpen;
                } else {
                    --cntOpen;
                }
            }
        } else {
            cntOpen = 0;
            targetStart = begin;
        }

        if (cntOpen > 0) {
            intptr_t prevStart = targetStart;
            targetStart = (*ppEditView)->searchInTarget(exprToSearch,
                                                          prevStart + 1, docLen);
            if (targetStart < 0)
                cntOpen = 0;
        }
    } while (cntOpen > 0);

    intptr_t targetEnd = (*ppEditView)->execute(SCI_GETTARGETEND);
    return targetEnd >= 0 ? static_cast<size_t>(targetEnd) : docLen;
}

// =============================================================================
// FunctionZoneParser::classParse — find classes and parse their methods
// Mirrors Win32 classParse().
// =============================================================================

void FunctionZoneParser::classParse(
    std::vector<foundInfo>& foundInfos,
    std::vector<std::pair<size_t, size_t>>& scannedZones,
    const std::vector<std::pair<size_t, size_t>>& commentZones,
    size_t begin, size_t end,
    ScintillaEditView** ppEditView,
    const std::string& classStructName)
{
    if (begin >= end)
        return;
    if (!(*ppEditView))
        return;

    static constexpr int flags = SCFIND_REGEXP | SCFIND_POSIX | SCFIND_REGEXP_DOTMATCHESNL;

    (*ppEditView)->execute(SCI_SETSEARCHFLAGS, flags);
    intptr_t targetStart = (*ppEditView)->searchInTarget(_rangeExpr, begin, end);

    while (targetStart >= 0) {
        intptr_t targetEnd = (*ppEditView)->execute(SCI_GETTARGETEND);

        intptr_t foundPos = 0;
        std::string subLevelClassStructName = parseSubLevel(targetStart, targetEnd,
                                                              _classNameExprArray,
                                                              foundPos, ppEditView);

        if (!_openSymbole.empty() && !_closeSymbole.empty()) {
            targetEnd = getBodyClosePos(targetEnd, _openSymbole.c_str(),
                                         _closeSymbole.c_str(), commentZones, ppEditView);
        }

        if (targetEnd > static_cast<intptr_t>(end))
            break;

        scannedZones.push_back(std::pair<size_t, size_t>(
            static_cast<size_t>(targetStart), static_cast<size_t>(targetEnd)));

        size_t foundTextLen = targetEnd - targetStart;
        if (targetStart + foundTextLen == end)
            break;

        if (!isInZones(targetStart, commentZones)) {
            funcParse(foundInfos, targetStart, targetEnd, ppEditView,
                       subLevelClassStructName, &commentZones);
        }

        begin = targetStart + (targetEnd - targetStart);
        targetStart = (*ppEditView)->searchInTarget(_rangeExpr, begin, end);
    }
}

// =============================================================================
// FunctionParser::getCommentZones — find comment regions to exclude
// Mirrors Win32 getCommentZones().
// =============================================================================

void FunctionParser::getCommentZones(std::vector<std::pair<size_t, size_t>>& commentZone,
                                      size_t begin, size_t end,
                                      ScintillaEditView** ppEditView)
{
    if ((begin >= end) || _commentExpr.empty())
        return;
    if (!(*ppEditView))
        return;

    static constexpr int flags = SCFIND_REGEXP | SCFIND_POSIX | SCFIND_REGEXP_DOTMATCHESNL;

    (*ppEditView)->execute(SCI_SETSEARCHFLAGS, flags);
    intptr_t targetStart = (*ppEditView)->searchInTarget(_commentExpr, begin, end);

    while (targetStart >= 0) {
        intptr_t targetEnd = (*ppEditView)->execute(SCI_GETTARGETEND);
        if (targetEnd > static_cast<intptr_t>(end))
            break;

        commentZone.push_back(std::pair<size_t, size_t>(
            static_cast<size_t>(targetStart), static_cast<size_t>(targetEnd)));

        size_t foundTextLen = targetEnd - targetStart;
        if (targetStart + foundTextLen >= end)
            break;

        begin = targetStart + foundTextLen;
        targetStart = (*ppEditView)->searchInTarget(_commentExpr, begin, end);
    }
}

// =============================================================================
// FunctionParser::isInZones — check if position is in exclusion zones
// Mirrors Win32 isInZones().
// =============================================================================

bool FunctionParser::isInZones(size_t pos2Test,
                                const std::vector<std::pair<size_t, size_t>>& zones)
{
    for (size_t i = 0, len = zones.size(); i < len; ++i) {
        if (pos2Test >= zones[i].first && pos2Test < zones[i].second)
            return true;
    }
    return false;
}

// =============================================================================
// FunctionParser::getInvertZones — complement of zones
// Mirrors Win32 getInvertZones().
// =============================================================================

void FunctionParser::getInvertZones(std::vector<std::pair<size_t, size_t>>& destZones,
                                    const std::vector<std::pair<size_t, size_t>>& sourceZones,
                                    size_t begin, size_t end)
{
    if (sourceZones.empty()) {
        destZones.push_back(std::pair<size_t, size_t>(begin, end));
    } else {
        if (begin < sourceZones[0].first) {
            destZones.push_back(std::pair<size_t, size_t>(begin, sourceZones[0].first - 1));
        }

        size_t i = 0;
        for (size_t len = sourceZones.size() - 1; i < len; ++i) {
            size_t newBegin = sourceZones[i].second + 1;
            size_t newEnd = sourceZones[i + 1].first - 1;
            if (newBegin < newEnd)
                destZones.push_back(std::pair<size_t, size_t>(newBegin, newEnd));
        }
        size_t lastBegin = sourceZones[i].second + 1;
        if (lastBegin < end)
            destZones.push_back(std::pair<size_t, size_t>(lastBegin, end));
    }
}

// =============================================================================
// FunctionZoneParser::parse — zone-level parsing entry point
// Mirrors Win32 parse().
// =============================================================================

void FunctionZoneParser::parse(std::vector<foundInfo>& foundInfos,
                                 size_t begin, size_t end,
                                 ScintillaEditView** ppEditView,
                                 const std::string& classStructName)
{
    if (!(*ppEditView))
        return;

    std::vector<std::pair<size_t, size_t>> classZones, commentZones, nonCommentZones;
    getCommentZones(commentZones, begin, end, ppEditView);
    getInvertZones(nonCommentZones, commentZones, begin, end);

    for (size_t i = 0, len = nonCommentZones.size(); i < len; ++i) {
        classParse(foundInfos, classZones, commentZones,
                    nonCommentZones[i].first, nonCommentZones[i].second,
                    ppEditView, classStructName);
    }
}

// =============================================================================
// FunctionUnitParser::parse — standalone function parsing entry point
// Mirrors Win32 parse().
// =============================================================================

void FunctionUnitParser::parse(std::vector<foundInfo>& foundInfos,
                                size_t begin, size_t end,
                                ScintillaEditView** ppEditView,
                                const std::string& classStructName)
{
    if (!(*ppEditView))
        return;

    std::vector<std::pair<size_t, size_t>> commentZones, nonCommentZones;
    getCommentZones(commentZones, begin, end, ppEditView);
    getInvertZones(nonCommentZones, commentZones, begin, end);

    for (size_t i = 0, len = nonCommentZones.size(); i < len; ++i) {
        funcParse(foundInfos, nonCommentZones[i].first, nonCommentZones[i].second,
                   ppEditView, classStructName);
    }
}

// =============================================================================
// FunctionMixParser::parse — combined zone + unit parsing
// Mirrors Win32 parse().
// =============================================================================

void FunctionMixParser::parse(std::vector<foundInfo>& foundInfos,
                               size_t begin, size_t end,
                               ScintillaEditView** ppEditView,
                               const std::string& classStructName)
{
    if (!(*ppEditView))
        return;

    std::vector<std::pair<size_t, size_t>> commentZones, scannedZones, nonScannedZones;
    getCommentZones(commentZones, begin, end, ppEditView);

    classParse(foundInfos, scannedZones, commentZones, begin, end, ppEditView, classStructName);

    // Second level (nested classes)
    for (size_t i = 0, len = scannedZones.size(); i < len; ++i) {
        std::vector<std::pair<size_t, size_t>> temp;
        classParse(foundInfos, temp, commentZones,
                    scannedZones[i].first, scannedZones[i].second,
                    ppEditView, classStructName);
    }

    getInvertZones(nonScannedZones, scannedZones, begin, end);

    if (_funcUnitParser) {
        for (size_t i = 0, len = nonScannedZones.size(); i < len; ++i) {
            _funcUnitParser->funcParse(foundInfos,
                                        nonScannedZones[i].first,
                                        nonScannedZones[i].second,
                                        ppEditView, classStructName, &commentZones);
        }
    }
}
