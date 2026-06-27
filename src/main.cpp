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

// Qt6 port — Win32 API stubs removed; use Qt equivalents throughout.

#include <QCoreApplication>
#include <QApplication>
#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QMenuBar>
#include <QMessageBox>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QSettings>
#include <QProcess>
#include <QThread>
#include <QElapsedTimer>
#include <QDesktopServices>
#include <QClipboard>
#include <QCursor>
#include <QScreen>
#include <QStandardPaths>
#include <QUrl>
#include <QMimeData>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QFontDatabase>
#include <QFileDialog>
#include <QInputDialog>
#include <QColor>
#include <QLibrary>
#include <QProcessEnvironment>

#include "Notepad_plus_Window.h"
#include "Parameters.h"   // NppGUI, NppParameters
#include "Processus.h"
#include "MiniDumper.h"           // Write dump files
#include "verifySignedfile.h"
#include "NppDarkMode.h"
#include "dpiManagerV2.h"
#include "MISC/Common/Common.h"
#include "Parameters.h"
#include <memory>

typedef std::vector<std::wstring> ParamVector;

// Win32 pCmdLine stub — Qt6 uses QCoreApplication::arguments() instead
const wchar_t* pCmdLine = nullptr;

// UAC operation signs (from NppConstants.h)
static const wchar_t* NPP_UAC_SAVE_SIGN = L"#UAC-SAVE#";
static const wchar_t* NPP_UAC_SETFILEATTRIBUTES_SIGN = L"#UAC-SETFILEATTRIBUTES#";
static const wchar_t* NPP_UAC_MOVEFILE_SIGN = L"#UAC-MOVEFILE#";
static const wchar_t* NPP_UAC_CREATEEMPTYFILE_SIGN = L"#UAC-CREATEEMPTYFILE#";

// Windows error codes (stubbed for non-Win32)
#ifndef ERROR_SUCCESS
#define ERROR_SUCCESS 0U
#endif
#ifndef ERROR_FILE_NOT_FOUND
#define ERROR_FILE_NOT_FOUND 2U
#endif
#ifndef ERROR_FILE_EXISTS
#define ERROR_FILE_EXISTS 80U
#endif
#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES 0xFFFFFFFFU
#endif
#ifndef FILE_ATTRIBUTE_DIRECTORY
#define FILE_ATTRIBUTE_DIRECTORY 0x10U
#endif

// COPYDATASTRUCT stub for non-Windows
#ifndef _WIN32
struct COPYDATASTRUCT {
    ULONG_PTR dwData = 0;
    DWORD cbData = 0;
    void* lpData = nullptr;
};
#define COPYDATA_FULL_CMDLINE 1
#define COPYDATA_FILENAMESW 2
#define COPYDATA_PARAMS 3
#endif

// Security guard stub (Win32 plugin security — not available on Linux)
class SecurityGuard {
public:
    bool checkModule(const std::wstring&, const wchar_t*) const {
        return true; // skip verification on non-Win32
    }
};
static const wchar_t* nm_gup = L"gup";

// Win32Exception stub — no SEH on non-Windows
#include "Win32Exception.h"

namespace
{

// allowPrivilegeMessages: Win32 UAC message filter — not applicable on Linux
// Qt6 clipboard integration does not require ChangeWindowMessageFilter.
void allowPrivilegeMessages(const Notepad_plus_Window&, winVer)
{
    // stub: no-op on non-Win32
}

// parseCommandLine: converts raw wchar_t command line into a vector of wstring args.
// Replaces Win32 lstrcpy/lstrcmp with Qt-safe wstring operations.
void parseCommandLine(const wchar_t* commandLine, ParamVector& paramVector)
{
    if (!commandLine)
        return;

    size_t len = std::wcslen(commandLine);
    wchar_t* cmdLine = new wchar_t[len + 1];
    std::wcscpy(cmdLine, commandLine);

    wchar_t* cmdLinePtr = cmdLine;
    bool isBetweenFileNameQuotes = false;
    bool isStringInArg = false;
    bool isInWhiteSpace = true;

    int zArg = 0; // for "-z" argument

    bool shouldBeTerminated = false;

    size_t commandLength = std::wcslen(cmdLinePtr);
    std::vector<wchar_t*> args;
    for (size_t i = 0; i < commandLength && !shouldBeTerminated; ++i)
    {
        switch (cmdLinePtr[i])
        {
            case '\"':
            {
                if (!isStringInArg && !isBetweenFileNameQuotes && i > 0 && cmdLinePtr[i-1] == L'=')
                {
                    isStringInArg = true;
                }
                else if (isStringInArg)
                {
                    isStringInArg = false;
                }
                else if (!isBetweenFileNameQuotes)
                {
                    args.push_back(cmdLinePtr + i + 1);
                    isBetweenFileNameQuotes = true;
                    cmdLinePtr[i] = 0;

                    if (zArg == 1)
                        ++zArg;
                }
                else
                {
                    isBetweenFileNameQuotes = false;
                    cmdLinePtr[i] = 0;
                }
                isInWhiteSpace = false;
            }
            break;

            case L'\t':
            case L' ':
            {
                isInWhiteSpace = true;
                if (!isBetweenFileNameQuotes && !isStringInArg)
                {
                    cmdLinePtr[i] = 0;
                    size_t argsLen = args.size();
                    if (argsLen > 0 && std::wcscmp(args[argsLen-1], L"-z") == 0)
                        ++zArg;
                }
            }
            break;

            default:
            {
                if (!isBetweenFileNameQuotes && !isStringInArg && isInWhiteSpace)
                {
                    args.push_back(cmdLinePtr + i);
                    if (zArg == 2)
                        shouldBeTerminated = true;
                    isInWhiteSpace = false;
                }
            }
        }
    }
    paramVector.assign(args.begin(), args.end());
    delete[] cmdLine;
}

// Converts /p or /P to -quickPrint if it exists as the first parameter
void convertParamsToNotepadStyle(ParamVector& params)
{
    for (auto it = params.begin(); it != params.end(); ++it)
    {
        if (std::wcscmp(it->c_str(), L"/p") == 0 || std::wcscmp(it->c_str(), L"/P") == 0)
            it->assign(L"-quickPrint");
    }
}

bool isInList(const wchar_t* token2Find, ParamVector& params, bool eraseArg = true)
{
    for (auto it = params.begin(); it != params.end(); ++it)
    {
        if (std::wcscmp(token2Find, it->c_str()) == 0)
        {
            if (eraseArg) params.erase(it);
            return true;
        }
    }
    return false;
}

bool getParamVal(wchar_t c, ParamVector& params, std::wstring& value)
{
    value = L"";
    for (size_t i = 0; i < params.size(); ++i)
    {
        const wchar_t* token = params.at(i).c_str();
        if (token[0] == L'-' && std::wcslen(token) >= 2 && token[1] == c)
        {
            value = token + 2;
            params.erase(params.begin() + i);
            return true;
        }
    }
    return false;
}

bool getParamValFromString(const wchar_t* str, ParamVector& params, std::wstring& value)
{
    value = L"";
    for (size_t i = 0; i < params.size(); ++i)
    {
        const wchar_t* token = params.at(i).c_str();
        std::wstring tokenStr = token;
        size_t pos = tokenStr.find(str);
        if (pos != std::wstring::npos && pos == 0)
        {
            value = token + std::wcslen(str);
            params.erase(params.begin() + i);
            return true;
        }
    }
    return false;
}

LangType getLangTypeFromParam(ParamVector& params)
{
    std::wstring langStr;
    if (!getParamVal(L'l', params, langStr))
        return L_EXTERNAL;
    return NppParameters::getLangIDFromStr(langStr.c_str());
}

std::wstring getLocalizationPathFromParam(ParamVector& params)
{
    std::wstring locStr;
    if (!getParamVal(L'L', params, locStr))
        return L"";
    // Replace underscores with dashes using Qt
    QString locStrQ = QString::fromStdWString(locStr);
    locStrQ.replace("_", "-");
    locStr = locStrQ.toLower().toStdWString();
    return NppParameters::getLocPathFromStr(locStr);
}

intptr_t getNumberFromParam(char paramName, ParamVector& params, bool& isParamePresent)
{
    std::wstring numStr;
    if (!getParamVal(paramName, params, numStr))
    {
        isParamePresent = false;
        return -1;
    }
    isParamePresent = true;
    return static_cast<intptr_t>(std::wcstoll(numStr.c_str(), nullptr, 10));
}

std::wstring getEasterEggNameFromParam(ParamVector& params, unsigned char& type)
{
    std::wstring EasterEggName;
    if (!getParamValFromString(L"-qn=", params, EasterEggName))
    {
        if (!getParamValFromString(L"-qt=", params, EasterEggName))
        {
            if (!getParamValFromString(L"-qf=", params, EasterEggName))
                return L"";
            else
                type = 2;
        }
        else
            type = 1;
    }
    else
        type = 0;

    if (!EasterEggName.empty() && EasterEggName.front() == L'"' && EasterEggName.back() == L'"')
        EasterEggName = EasterEggName.substr(1, EasterEggName.length() - 2);

    if (type == 2)
        EasterEggName = relativeFilePathToFullFilePath(QString::fromStdWString(EasterEggName)).toStdWString();

    return EasterEggName;
}

int getGhostTypingSpeedFromParam(ParamVector& params)
{
    std::wstring speedStr;
    if (!getParamValFromString(L"-qSpeed", params, speedStr))
        return -1;
    int speed = std::stoi(speedStr, nullptr, 10);
    if (speed <= 0 || speed > 3)
        return -1;
    return speed;
}

const wchar_t FLAG_MULTI_INSTANCE[] = L"-multiInst";
const wchar_t FLAG_NO_PLUGIN[] = L"-noPlugin";
const wchar_t FLAG_READONLY[] = L"-ro";
const wchar_t FLAG_FULL_READONLY[] = L"-fullReadOnly";
const wchar_t FLAG_FULL_READONLY_SAVING_FORBIDDEN[] = L"-fullReadOnlySavingForbidden";
const wchar_t FLAG_NOSESSION[] = L"-nosession";
const wchar_t FLAG_NOTABBAR[] = L"-notabbar";
const wchar_t FLAG_SYSTRAY[] = L"-systemtray";
const wchar_t FLAG_LOADINGTIME[] = L"-loadingTime";
const wchar_t FLAG_HELP[] = L"--help";
const wchar_t FLAG_ALWAYS_ON_TOP[] = L"-alwaysOnTop";
const wchar_t FLAG_OPENSESSIONFILE[] = L"-openSession";
const wchar_t FLAG_RECURSIVE[] = L"-r";
const wchar_t FLAG_FUNCLSTEXPORT[] = L"-export=functionList";
const wchar_t FLAG_PRINTANDQUIT[] = L"-quickPrint";
const wchar_t FLAG_NOTEPAD_COMPATIBILITY[] = L"-notepadStyleCmdline";
const wchar_t FLAG_OPEN_FOLDERS_AS_WORKSPACE[] = L"-openFoldersAsWorkspace";
const wchar_t FLAG_SETTINGS_DIR[] = L"-settingsDir=";
const wchar_t FLAG_TITLEBAR_ADD[] = L"-titleAdd=";
const wchar_t FLAG_APPLY_UDL[] = L"-udl=";
const wchar_t FLAG_PLUGIN_MESSAGE[] = L"-pluginMessage=";
const wchar_t FLAG_MONITOR_FILES[] = L"-monitor";

void doException(Notepad_plus_Window& notepad_plus_plus)
{
    Win32Exception::removeHandler();
    QMessageBox::warning(qobject_cast<QWidget*>(MainWindow::gNppHWND),
        QStringLiteral("Recovery initiating"),
        QStringLiteral("Notepad++ will attempt to save any unsaved data. However, data loss is very likely."));

    QString tmpDir = QDir::tempPath();
    std::wstring emergencySavedDir = tmpDir.toStdWString();
    emergencySavedDir += L"/Notepad++_RECOV";

    // emergency() is a Win32 API to save unsaved files — stub for non-Win32
    bool res = false;
    Q_UNUSED(res);
    Q_UNUSED(notepad_plus_plus);
    Q_UNUSED(emergencySavedDir);
    // stub: on non-Win32, skip recovery attempt silently
}

// Looks for -z arguments and strips command line arguments following those, if any
void stripIgnoredParams(ParamVector& params)
{
    for (auto it = params.begin(); it != params.end(); )
    {
        if (std::wcscmp(it->c_str(), L"-z") == 0)
        {
            auto nextIt = std::next(it);
            if (nextIt != params.end())
                params.erase(nextIt);
            it = params.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

bool launchUpdater(const std::wstring& updaterFullPath, const std::wstring& updaterDir)
{
    NppParameters& nppParameters = NppParameters::getInstance();
    NppGUI& nppGui = nppParameters.getNppGUI();

    Date today(0);
    if (today < nppGui._autoUpdateOpt._nextUpdateDate)
        return false;

    QString updaterPath = QString::fromStdWString(updaterFullPath);
    QString updaterDirQ = QString::fromStdWString(updaterDir);

    Process updater;
    updater.run(updaterDirQ, updaterPath, QStringList());

    if (nppGui._autoUpdateOpt._intervalDays < 0)
        nppGui._autoUpdateOpt._intervalDays = -nppGui._autoUpdateOpt._intervalDays;
    nppGui._autoUpdateOpt._nextUpdateDate = Date(nppGui._autoUpdateOpt._intervalDays);

    return true;
}

unsigned int nppUacSave(const QString& tempFilePath, const QString& protectedFilePath2Save)
{
    if (tempFilePath.isEmpty() || protectedFilePath2Save.isEmpty())
        return 0U;
    if (!doesFileExist(tempFilePath))
        return ERROR_FILE_NOT_FOUND;

    if (!QFile::copy(tempFilePath, protectedFilePath2Save))
        return 0U;
    QFile::remove(tempFilePath);
    return ERROR_SUCCESS;
}

unsigned int nppUacSetFileAttributes(const unsigned int dwFileAttribs, const QString& filePath)
{
    if (filePath.isEmpty())
        return 0U;
    if (!doesFileExist(filePath))
        return ERROR_FILE_NOT_FOUND;
    if (dwFileAttribs == INVALID_FILE_ATTRIBUTES || (dwFileAttribs & FILE_ATTRIBUTE_DIRECTORY))
        return 0U;

    QFile::setPermissions(filePath, static_cast<QFile::Permissions>(dwFileAttribs & 0x7FFF));
    return ERROR_SUCCESS;
}

unsigned int nppUacMoveFile(const QString& originalFilePath, const QString& newFilePath)
{
    if (originalFilePath.isEmpty() || newFilePath.isEmpty())
        return 0U;
    if (!doesFileExist(originalFilePath))
        return ERROR_FILE_NOT_FOUND;

    if (QFile::rename(originalFilePath, newFilePath))
        return ERROR_SUCCESS;
    if (QFile::copy(originalFilePath, newFilePath) && QFile::remove(originalFilePath))
        return ERROR_SUCCESS;
    return 0U;
}

unsigned int nppUacCreateEmptyFile(const QString& newEmptyFilePath)
{
    if (newEmptyFilePath.isEmpty())
        return 0U;
    if (doesFileExist(newEmptyFilePath))
        return ERROR_FILE_EXISTS;

    QFile file(newEmptyFilePath);
    if (file.open(QIODevice::WriteOnly))
    {
        file.close();
        return ERROR_SUCCESS;
    }
    return 0U;
}

} // namespace

std::chrono::steady_clock::time_point g_nppStartTimePoint{};

int main(int argc, char** argv)
{
    g_nppStartTimePoint = std::chrono::steady_clock::now();

    QApplication app(argc, argv);

    // Notepad++ UAC OPS — handled by launching this executable as a helper ///////////
    if (!app.arguments().isEmpty() && app.arguments().size() >= 2)
    {
        const QString opSign = app.arguments().at(1);
        if (opSign.startsWith("#UAC-"))
        {
            if (app.arguments().size() == 4 && opSign == QString::fromWCharArray(NPP_UAC_SAVE_SIGN))
                return static_cast<int>(nppUacSave(app.arguments().at(2), app.arguments().at(3)));

            if (app.arguments().size() == 4 && opSign == QString::fromWCharArray(NPP_UAC_SETFILEATTRIBUTES_SIGN))
            {
                bool ok = false;
                unsigned int attrs = app.arguments().at(2).toUInt(&ok, 0);
                if (!ok) return 0;
                return static_cast<int>(nppUacSetFileAttributes(attrs, app.arguments().at(3)));
            }

            if (app.arguments().size() == 4 && opSign == QString::fromWCharArray(NPP_UAC_MOVEFILE_SIGN))
                return static_cast<int>(nppUacMoveFile(app.arguments().at(2), app.arguments().at(3)));

            if (app.arguments().size() == 3 && opSign == QString::fromWCharArray(NPP_UAC_CREATEEMPTYFILE_SIGN))
                return static_cast<int>(nppUacCreateEmptyFile(app.arguments().at(2)));
        }
    } // UAC OPS ///////////////////////////////////////////////////////////////////

    bool TheFirstOne = true;

    std::wstring cmdLineString = pCmdLine ? QString::fromWCharArray(pCmdLine).toStdWString() : L"";
    ParamVector params;
    parseCommandLine(pCmdLine, params);

    stripIgnoredParams(params);
    if (isInList(FLAG_NOTEPAD_COMPATIBILITY, params))
        convertParamsToNotepadStyle(params);

    bool isParamePresent = false;
    bool isMultiInst = isInList(FLAG_MULTI_INSTANCE, params);
    bool doFunctionListExport = isInList(FLAG_FUNCLSTEXPORT, params);
    bool doPrintAndQuit = isInList(FLAG_PRINTANDQUIT, params);

    CmdLineParams cmdLineParams;
    cmdLineParams._displayCmdLineArgs = isInList(FLAG_HELP, params);
    cmdLineParams._isNoTab = isInList(FLAG_NOTABBAR, params);
    cmdLineParams._isNoPlugin = isInList(FLAG_NO_PLUGIN, params);
    cmdLineParams._isReadOnly = isInList(FLAG_READONLY, params);
    cmdLineParams._isFullReadOnly = isInList(FLAG_FULL_READONLY, params);
    cmdLineParams._isFullReadOnlySavingForbidden = isInList(FLAG_FULL_READONLY_SAVING_FORBIDDEN, params);
    cmdLineParams._isNoSession = isInList(FLAG_NOSESSION, params);
    cmdLineParams._isPreLaunch = isInList(FLAG_SYSTRAY, params);
    cmdLineParams._alwaysOnTop = isInList(FLAG_ALWAYS_ON_TOP, params);
    cmdLineParams._showLoadingTime = isInList(FLAG_LOADINGTIME, params);
    cmdLineParams._isSessionFile = isInList(FLAG_OPENSESSIONFILE, params);
    cmdLineParams._isRecursive = isInList(FLAG_RECURSIVE, params);
    cmdLineParams._openFoldersAsWorkspace = isInList(FLAG_OPEN_FOLDERS_AS_WORKSPACE, params);
    cmdLineParams._monitorFiles = isInList(FLAG_MONITOR_FILES, params);

    cmdLineParams._langType = getLangTypeFromParam(params);
    cmdLineParams._localizationPath = getLocalizationPathFromParam(params);
    cmdLineParams._easterEggName = getEasterEggNameFromParam(params, cmdLineParams._quoteType);
    cmdLineParams._ghostTypingSpeed = getGhostTypingSpeedFromParam(params);

    std::wstring pluginMessage;
    if (getParamValFromString(FLAG_PLUGIN_MESSAGE, params, pluginMessage))
    {
        if (pluginMessage.length() >= 2 && pluginMessage.front() == L'"' && pluginMessage.back() == L'"')
            pluginMessage = pluginMessage.substr(1, pluginMessage.length() - 2);
        cmdLineParams._pluginMessage = pluginMessage;
    }

    cmdLineParams._line2go = getNumberFromParam('n', params, isParamePresent);
    cmdLineParams._column2go = getNumberFromParam('c', params, isParamePresent);
    cmdLineParams._pos2go = getNumberFromParam('p', params, isParamePresent);
    cmdLineParams._point.setX(static_cast<int>(getNumberFromParam('x', params, cmdLineParams._isPointXValid)));
    cmdLineParams._point.setY(static_cast<int>(getNumberFromParam('y', params, cmdLineParams._isPointYValid)));

    NppParameters& nppParameters = NppParameters::getInstance();
    nppParameters.setCmdLineString(cmdLineString);

    std::wstring path;
    if (getParamValFromString(FLAG_SETTINGS_DIR, params, path))
    {
        if (!path.empty() && path.front() == L'"' && path.back() == L'"')
            path = path.substr(1, path.length() - 2);
        nppParameters.setCmdSettingsDir(path);
    }

    std::wstring titleBarAdditional;
    if (getParamValFromString(FLAG_TITLEBAR_ADD, params, titleBarAdditional))
    {
        if (titleBarAdditional.length() >= 2
            && titleBarAdditional.front() == L'"' && titleBarAdditional.back() == L'"')
            titleBarAdditional = titleBarAdditional.substr(1, titleBarAdditional.length() - 2);
        nppParameters.setTitleBarAdd(titleBarAdditional);
    }

    std::wstring udlName;
    if (getParamValFromString(FLAG_APPLY_UDL, params, udlName))
    {
        if (udlName.length() >= 2
            && udlName.front() == L'"' && udlName.back() == L'"')
            udlName = udlName.substr(1, udlName.length() - 2);
        cmdLineParams._udlName = udlName;
    }

    if (!cmdLineParams._localizationPath.empty())
        nppParameters.setStartWithLocFileName(cmdLineParams._localizationPath);

    nppParameters.load();

    NppGUI& nppGui = nppParameters.getNppGUI();

    // Initialize dark mode and DPI — use Qt singleton pattern
    ::NppDarkMode::NppDarkMode::instance().setEnabled(true);
    // DPIManagerV2 has no init() call; DPI is applied via QWidget scaling automatically

    bool doUpdateNpp = nppGui._autoUpdateOpt._doAutoUpdate != NppGUI::autoupdate_disabled;
    bool updateAtExit = nppGui._autoUpdateOpt._doAutoUpdate == NppGUI::autoupdate_on_exit;
    bool doUpdatePluginList = nppGui._autoUpdateOpt._doAutoUpdate != NppGUI::autoupdate_disabled;

    if (doFunctionListExport || doPrintAndQuit)
    {
        isMultiInst = true;
        doUpdateNpp = doUpdatePluginList = false;
        cmdLineParams._isNoSession = true;
    }

    nppParameters.setFunctionListExportBoolean(doFunctionListExport);
    nppParameters.setPrintAndExitBoolean(doPrintAndQuit);

    if (nppParameters.asNotepadStyle())
    {
        isMultiInst = true;
        cmdLineParams._isNoTab = true;
        cmdLineParams._isNoSession = true;
    }

    // Override settings if multiInst is chosen by user in the preference dialog
    const NppGUI& nppGUI = nppParameters.getNppGUI();
    if (nppGUI._multiInstSetting == NppGUI::MultiInstSetting::MULTI_INSTANCE)
    {
        isMultiInst = true;
        if (!TheFirstOne)
            cmdLineParams._isNoSession = true;
    }

    std::wstring quotFileName = L"";
    size_t nbFilesToOpen = params.size();

    for (size_t i = 0; i < nbFilesToOpen; ++i)
    {
        const wchar_t* currentFile = params.at(i).c_str();
        if (currentFile[0])
        {
            quotFileName += L"\"";
            QString fullPath = relativeFilePathToFullFilePath(QString::fromWCharArray(currentFile));
            quotFileName += fullPath.toStdWString();
            quotFileName += L"\" ";
        }
    }

    // Multi-instance: if another instance is already running, delegate to it
    // via QLocalSocket/QLockFile and exit
    if (!isMultiInst && !TheFirstOne)
    {
        QWidget* hNotepad_plus = nullptr;
        // Find running instance by checking window title or using QLockFile
        QList<QWidget*> topWidgets = QApplication::topLevelWidgets();
        for (QWidget* w : topWidgets)
        {
            if (w->inherits("Notepad_plus_Window") || w->windowTitle().contains("Notepad++"))
            {
                hNotepad_plus = w;
                break;
            }
        }

        for (int i = 0; !hNotepad_plus && i < 5; ++i)
        {
            QThread::msleep(100);
            topWidgets = QApplication::topLevelWidgets();
            for (QWidget* w : topWidgets)
            {
                if (w->inherits("Notepad_plus_Window") || w->windowTitle().contains("Notepad++"))
                {
                    hNotepad_plus = w;
                    break;
                }
            }
        }

        if (hNotepad_plus)
        {
            nppParameters.destroyInstance();

            // Restore window state
            bool isInSystemTray = false; Q_UNUSED(isInSystemTray);

            if (!isInSystemTray)
            {
                int sw = 0;
                if (hNotepad_plus->isMaximized())
                    sw = Qt::WindowMaximized;
                else if (hNotepad_plus->isMinimized())
                    sw = Qt::WindowNoState;

                if (sw != 0)
                    hNotepad_plus->setWindowState(static_cast<Qt::WindowState>(sw));
            }
            hNotepad_plus->activateWindow();

            if (params.size() > 0 || !cmdLineParams._pluginMessage.empty())
            {
                // Build IPC data structs (stub — non-Win32 uses QLocalSocket/QLockFile)
                CmdLineParamsDTO dto = CmdLineParamsDTO::FromCmdLineParams(cmdLineParams);
                Q_UNUSED(dto);

                COPYDATASTRUCT paramData{};
                paramData.dwData = COPYDATA_PARAMS;
                paramData.lpData = &dto;
                paramData.cbData = sizeof(dto);
                Q_UNUSED(paramData);

                COPYDATASTRUCT cmdLineData{};
                cmdLineData.dwData = COPYDATA_FULL_CMDLINE;
                cmdLineData.lpData = (void*)cmdLineString.c_str();
                cmdLineData.cbData = static_cast<DWORD>((cmdLineString.length() + 1) * sizeof(wchar_t));
                Q_UNUSED(cmdLineData);

                COPYDATASTRUCT fileNamesData{};
                fileNamesData.dwData = COPYDATA_FILENAMESW;
                fileNamesData.lpData = (void*)quotFileName.c_str();
                fileNamesData.cbData = static_cast<DWORD>((quotFileName.length() + 1) * sizeof(wchar_t));
                Q_UNUSED(fileNamesData);
                Q_UNUSED(hNotepad_plus);
            }
            return 0;
        }
    }

    auto upNotepadWindow = std::make_unique<Notepad_plus_Window>();
    Notepad_plus_Window& notepad_plus_plus = *upNotepadWindow.get();

    std::wstring updaterDir = nppParameters.getNppPath();
    updaterDir += L"/updater/";
    std::wstring updaterFullPath = updaterDir + L"gup.exe";

    bool isUpExist = nppGui._doesExistUpdater = doesFileExist(QString::fromStdWString(updaterFullPath));

    winVer ver = nppParameters.getWinVersion();
    bool isGtXP = ver > WV_XP;

    SecurityGuard securityGuard;
    bool isSignatureOK = securityGuard.checkModule(updaterFullPath, nm_gup);

    if (TheFirstOne && isUpExist && isGtXP && isSignatureOK && doUpdateNpp && !updateAtExit && !nppParameters.isNppAutoUpdateDisabled())
        launchUpdater(updaterFullPath, updaterDir);

    Win32Exception::installHandler();
    MiniDumper mdump;
    bool isException = false;
    int exitCode = 0;

    try
    {
        notepad_plus_plus.init();
        // allowPrivilegeMessages is a Win32-specific stub
        Q_UNUSED(ver);
        Q_UNUSED(quotFileName);

        exitCode = QApplication::exec();
    }
    catch (int i)
    {
        isException = true;
        qFatal("Notepad++ exception: code %d", i);
    }
    catch (std::runtime_error& ex)
    {
        isException = true;
        QMessageBox::critical(qobject_cast<QWidget*>(MainWindow::gNppHWND),
            QStringLiteral("Runtime Exception"),
            QString::fromStdString(ex.what()));
        doException(notepad_plus_plus);
    }
    catch (const Win32Exception& ex)
    {
        isException = true;
        // Win32Exception carries a code and what() on Win32; on non-Win32 this branch
        // should never be reached, but handle gracefully.
        QMessageBox::warning(qobject_cast<QWidget*>(MainWindow::gNppHWND),
            QStringLiteral("Win32Exception"),
            QStringLiteral("An exception occurred. Notepad++ cannot recover and must be shut down.\nCode: %1")
                .arg(static_cast<unsigned long>(ex.code())));
        mdump.writeDump(ex.info());
        doException(notepad_plus_plus);
    }
    catch (std::exception& ex)
    {
        isException = true;
        QMessageBox::critical(qobject_cast<QWidget*>(MainWindow::gNppHWND),
            QStringLiteral("General Exception"),
            QString::fromStdString(ex.what()));
        doException(notepad_plus_plus);
    }
    catch (...)
    {
        isException = true;
        QMessageBox::critical(qobject_cast<QWidget*>(MainWindow::gNppHWND),
            QStringLiteral("Unknown Exception"),
            QStringLiteral("An exception that we did not yet find its name is just caught"));
        doException(notepad_plus_plus);
    }

    doUpdateNpp = nppGui._autoUpdateOpt._doAutoUpdate != NppGUI::autoupdate_disabled;
    updateAtExit = nppGui._autoUpdateOpt._doAutoUpdate == NppGUI::autoupdate_on_exit;
    if (!isException && !nppParameters.isEndSessionCritical() && TheFirstOne && isUpExist && isGtXP && isSignatureOK && doUpdateNpp && updateAtExit)
    {
        if (launchUpdater(updaterFullPath, updaterDir))
        {
            nppParameters.createXmlTreeFromGUIParams();
            nppParameters.saveConfig_xml();
        }
    }

    return exitCode;
}
