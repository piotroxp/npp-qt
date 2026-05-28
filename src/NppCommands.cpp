// NppCommands.cpp - Qt6 port of Notepad++ command handler
// Ported from Windows to Qt

#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include <mutex>

// Qt includes
#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QFileDialog>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QWheelEvent>
#include <QFile>
#include <QDir>
#include <QUrl>
#include <QKeyEvent>
#include <QDateTime>
#include <QLocale>
#include <QColorDialog>
#include <QFileInfo>

#include "Notepad_plus_Window.h"
#include "NppConstants.h"
#include "Notepad_plus.h"

using namespace std;

std::mutex command_mutex;

// Qt equivalents for Windows API calls

bool isShiftPressed() {
    return QApplication::keyboardModifiers() & Qt::ShiftModifier;
}

bool isCtrlPressed() {
    return QApplication::keyboardModifiers() & Qt::ControlModifier;
}

bool isAltPressed() {
    return QApplication::keyboardModifiers() & Qt::AltModifier;
}

// Open URL using Qt
bool nppOpenUrl(const QString& url) {
    return QDesktopServices::openUrl(QUrl(url));
}

// Open folder in file manager
bool nppOpenFolder(const QString& path) {
    QFileInfo fi(path);
    return QDesktopServices::openUrl(QUrl::fromLocalFile(fi.absolutePath()));
}

// Clipboard operations
void nppCopyToClipboard(const QString& text) {
    QApplication::clipboard()->setText(text);
}

QString nppPasteFromClipboard() {
    return QApplication::clipboard()->text();
}

// Date/time formatting
QString nppGetLocalizedDate(bool shortFormat = true) {
    QDateTime now = QDateTime::currentDateTime();
    QLocale locale;
    return shortFormat ? 
        locale.toString(now.date(), QLocale::ShortFormat) :
        locale.toString(now.date(), QLocale::LongFormat);
}

QString nppGetLocalizedTime(bool showSeconds = false) {
    QDateTime now = QDateTime::currentDateTime();
    QLocale locale;
    return showSeconds ?
        locale.toString(now.time(), Qt::DefaultLocaleLongDate) :
        locale.toString(now.time(), Qt::DefaultLocaleShortDate);
}

// Get current directory
QString nppGetCurrentDirectory() {
    return QFileInfo(QApplication::applicationFilePath()).absolutePath();
}

// Check if path exists
bool nppPathExists(const QString& path) {
    return QFileInfo(path).exists();
}

bool nppIsDirectory(const QString& path) {
    return QFileInfo(path).isDir();
}

bool nppIsFile(const QString& path) {
    return QFileInfo(path).isFile();
}

void Notepad_plus::macroPlayback(Macro macro, std::vector<Document>* pDocs4EndUAIn)
{
    _playingBackMacro = true;

    std::vector<Document>* pDocs4EndUA = nullptr;
    if (pDocs4EndUAIn)
    {
        pDocs4EndUA = pDocs4EndUAIn;
    }
    else
    {
        pDocs4EndUA = new std::vector<Document>;
        if (!pDocs4EndUA)
            return;
    }

    Document prevSciDoc = 0;
    for (Macro::iterator step = macro.begin(); step != macro.end(); ++step)
    {
        Document curSciDoc = _pEditView->getCurrentBuffer()->getDocument();
        if (curSciDoc != prevSciDoc)
        {
            if (std::find(pDocs4EndUA->begin(), pDocs4EndUA->end(), curSciDoc) == pDocs4EndUA->end())
            {
                _pEditView->execute(SCI_BEGINUNDOACTION);
                pDocs4EndUA->push_back(curSciDoc);
            }
            prevSciDoc = curSciDoc;
        }

        if (step->isScintillaMacro())
            step->PlayBack(_pPublicInterface, _pEditView);
        else
            _findReplaceDlg.execSavedCommand(step->_message, step->_lParameter, string2wstring(step->_sParameter, CP_UTF8));
    }

    if (!pDocs4EndUAIn)
    {
        Document invisSciDoc = _invisibleEditView.execute(SCI_GETDOCPOINTER);
        while (!pDocs4EndUA->empty())
        {
            Document doc = pDocs4EndUA->back();
            if (MainFileManager.getBufferFromDocument(doc) == BUFFER_INVALID)
            {
                // doc no longer exists
            }
            else
            {
                _invisibleEditView.execute(SCI_SETDOCPOINTER, 0, doc);
                _invisibleEditView.execute(SCI_ENDUNDOACTION);
            }
            pDocs4EndUA->pop_back();
        }
        _invisibleEditView.execute(SCI_SETDOCPOINTER, 0, invisSciDoc);

        delete pDocs4EndUA;
        pDocs4EndUA = nullptr;
    }

    _playingBackMacro = false;
}

void Notepad_plus::command(int id)
{
    switch (id)
    {
        case IDM_FILE_NEW:
            fileNew();
            break;

        case IDM_EDIT_INSERT_DATETIME_SHORT:
        case IDM_EDIT_INSERT_DATETIME_LONG:
        {
            bool shortDate = (id == IDM_EDIT_INSERT_DATETIME_SHORT);
            QString dateStr = nppGetLocalizedDate(shortDate);
            QString timeStr = nppGetLocalizedTime(false);
            
            QString dateTimeStr;
            if (NppParameters::getInstance().getNppGUI()._dateTimeReverseDefaultOrder)
                dateTimeStr = dateStr + " " + timeStr;
            else
                dateTimeStr = timeStr + " " + dateStr;
            
            _pEditView->execute(SCI_BEGINUNDOACTION);
            _pEditView->execute(SCI_REPLACESEL, 0, reinterpret_cast<LPARAM>(""));
            _pEditView->addGenericText(dateTimeStr.toStdWString().c_str());
            _pEditView->execute(SCI_ENDUNDOACTION);
        }
        break;

        case IDM_EDIT_INSERT_DATETIME_CUSTOMIZED:
        {
            QDateTime now = QDateTime::currentDateTime();
            SYSTEMTIME currentTime = {};
            currentTime.wYear = now.date().year();
            currentTime.wMonth = now.date().month();
            currentTime.wDay = now.date().day();
            currentTime.wHour = now.time().hour();
            currentTime.wMinute = now.time().minute();
            currentTime.wSecond = now.time().second();

            NppGUI& nppGUI = NppParameters::getInstance().getNppGUI();
            wstring dateTimeStr = getDateTimeStrFrom(nppGUI._dateTimeFormat, currentTime);

            _pEditView->execute(SCI_BEGINUNDOACTION);
            _pEditView->execute(SCI_REPLACESEL, 0, reinterpret_cast<LPARAM>(""));
            _pEditView->addGenericText(dateTimeStr.c_str());
            _pEditView->execute(SCI_ENDUNDOACTION);
        }
        break;

        case IDM_FILE_OPEN:
            fileOpen();
            break;

        case IDM_FILE_OPEN_FOLDER:
        {
            QString filePath = QString::fromWCharArray(_pEditView->getCurrentBuffer()->getFullPathName());
            nppOpenFolder(filePath);
            break;
        }

        case IDM_FILE_OPEN_FOLDER_CMD:
        {
            QString filePath = QString::fromWCharArray(_pEditView->getCurrentBuffer()->getFullPathName());
            nppOpenFolder(filePath);
            break;
        }

        case IDM_FILE_OPEN_CMD:
        {
            QString currentDir = QString::fromWCharArray(_pEditView->getCurrentBuffer()->getFullPathName());
            nppOpenFolder(currentDir);
        }
        break;

        case IDM_FILE_CONTAININGFOLDERASWORKSPACE:
        {
            QString currentFilePath = QString::fromWCharArray(_pEditView->getCurrentBuffer()->getFullPathName());
            QString currentDirPath = QFileInfo(currentFilePath).absolutePath();

            if (!_pFileBrowser)
                command(IDM_VIEW_FILEBROWSER);

            vector<wstring> folders;
            folders.push_back(currentDirPath.toStdWString());
            launchFileBrowser(folders, currentFilePath.toStdWString().c_str());
        }
        break;

        case IDM_FILE_OPEN_DEFAULT_VIEWER:
        {
            BufferID buf = _pEditView->getCurrentBufferID();
            QString fullPath = QString::fromWCharArray(buf->getFullPathName());
            QFileInfo fi(fullPath);
            
            if (!fi.exists()) {
                QMessageBox::information(nullptr, "Error", 
                    QString("File not found: %1").arg(fullPath));
                return;
            }
            
            nppOpenUrl(QUrl::fromLocalFile(fi.absoluteFilePath()));
        }
        break;

        case IDM_FILE_OPENFOLDERASWORKSPACE:
        {
            QString folderPath = QFileDialog::getExistingDirectory(
                nullptr, "Select Folder", QDir::homePath());
            if (folderPath.isEmpty())
                return;
                
            if (!_pFileBrowser)
            {
                vector<wstring> dummy;
                wstring emptyStr;
                launchFileBrowser(dummy, emptyStr);
                if (_pFileBrowser)
                {
                    checkMenuItem(IDM_VIEW_FILEBROWSER, true);
                    _toolBar.setCheck(IDM_VIEW_FILEBROWSER, true);
                    _pFileBrowser->setClosed(false);
                }
                else
                    return;
            }
            else
            {
                if (_pFileBrowser->isClosed())
                {
                    _pFileBrowser->display();
                    checkMenuItem(IDM_VIEW_FILEBROWSER, true);
                    _toolBar.setCheck(IDM_VIEW_FILEBROWSER, true);
                    _pFileBrowser->setClosed(false);
                }
            }
            _pFileBrowser->addRootFolder(folderPath.toStdWString());
        }
        break;

        case IDM_FILE_RELOAD:
            fileReload();
            break;

        case IDM_DOCLIST_FILESCLOSE:
        case IDM_DOCLIST_FILESCLOSEOTHERS:
            if (_pDocumentListPanel)
            {
                vector<BufferViewInfo> bufs2Close = _pDocumentListPanel->getSelectedFiles(
                    id == IDM_DOCLIST_FILESCLOSEOTHERS);
                fileCloseAllGiven(bufs2Close);
                if (id == IDM_DOCLIST_FILESCLOSEOTHERS)
                    _pDocumentListPanel->activateItem(_pEditView->getCurrentBufferID(), currentView());
            }
            break;

        case IDM_DOCLIST_COPYNAMES:
        case IDM_DOCLIST_COPYPATHS:
            if (_pDocumentListPanel)
            {
                std::vector<Buffer*> buffers;
                auto files = _pDocumentListPanel->getSelectedFiles(false);
                for (auto&& sel : files)
                    buffers.push_back(MainFileManager.getBufferByID(sel._bufID));
                buf2Clipboard(buffers, id == IDM_DOCLIST_COPYPATHS, nullptr);
            }
            break;

        case IDM_FILE_CLOSE:
            if (fileClose())
                checkDocState();
            break;

        case IDM_FILE_DELETE:
            if (fileDelete())
                checkDocState();
            break;

        case IDM_FILE_RENAME:
            fileRename();
            break;

        case IDM_FILE_CLOSEALL:
        {
            bool isSnapshotMode = NppParameters::getInstance().getNppGUI().isSnapshotMode();
            fileCloseAll(isSnapshotMode, false);
            checkDocState();
            break;
        }

        case IDM_FILE_CLOSEALL_BUT_CURRENT:
            fileCloseAllButCurrent();
            checkDocState();
            break;

        case IDM_FILE_CLOSEALL_BUT_PINNED:
            fileCloseAllButPinned();
            checkDocState();
            break;

        case IDM_FILE_CLOSEALL_TOLEFT:
            fileCloseAllToLeft();
            checkDocState();
            break;

        case IDM_FILE_CLOSEALL_TORIGHT:
            fileCloseAllToRight();
            checkDocState();
            break;

        case IDM_FILE_CLOSEALL_UNCHANGED:
            fileCloseAllUnchanged();
            checkDocState();
            break;

        case IDM_FILE_SAVE:
            fileSave();
            break;

        case IDM_FILE_SAVEALL:
            fileSaveAll();
            break;

        case IDM_FILE_SAVEAS:
            fileSaveAs();
            break;

        case IDM_FILE_SAVECOPYAS:
            fileSaveAs(BUFFER_INVALID, true);
            break;

        case IDM_FILE_LOADSESSION:
            fileLoadSession();
            break;

        case IDM_FILE_SAVESESSION:
            fileSaveSession();
            break;

        case IDM_FILE_PRINTNOW:
            filePrint(false);
            break;

        case IDM_FILE_PRINT:
            filePrint(true);
            break;

        case IDM_FILE_EXIT:
            if (QApplication::activeWindow())
                QApplication::activeWindow()->close();
            break;

        case IDM_EDIT_UNDO:
        {
            std::lock_guard<std::mutex> lock(command_mutex);
            _pEditView->execute(WM_UNDO);
            checkClipboard();
            checkUndoState();
            break;
        }

        case IDM_EDIT_REDO:
        {
            std::lock_guard<std::mutex> lock(command_mutex);
            _pEditView->execute(SCI_REDO);
            checkClipboard();
            checkUndoState();
            break;
        }
        
        case IDM_EDIT_CUT:
        {
            std::lock_guard<std::mutex> lock(command_mutex);
            QWidget* focusedWidget = QApplication::focusWidget();
            if (focusedWidget == _pEditView->getWidget())
            {
                if (_pEditView->hasSelection())
                    _pEditView->execute(WM_CUT);
                else if (NppParameters::getInstance().getSVP()._lineCopyCutWithoutSelection)
                {
                    _pEditView->execute(SCI_COPYALLOWLINE);
                    _pEditView->execute(SCI_LINEDELETE);
                }
            }
            else
            {
                QKeyEvent cutEvent(QEvent::KeyPress, Qt::Key_X, Qt::ControlModifier);
                QApplication::sendEvent(focusedWidget, &cutEvent);
            }
            checkClipboard();
        }
        break;

        case IDM_EDIT_COPY:
        {
            std::lock_guard<std::mutex> lock(command_mutex);
            QWidget* focusedWidget = QApplication::focusWidget();
            if (focusedWidget == _pEditView->getWidget())
            {
                if (_pEditView->hasSelection())
                    _pEditView->execute(WM_COPY);
                else if (NppParameters::getInstance().getSVP()._lineCopyCutWithoutSelection)
                    _pEditView->execute(SCI_COPYALLOWLINE);
            }
            else
            {
                QKeyEvent copyEvent(QEvent::KeyPress, Qt::Key_C, Qt::ControlModifier);
                QApplication::sendEvent(focusedWidget, &copyEvent);
            }
            checkClipboard();
        }
        break;

        case IDM_EDIT_COPY_LINK:
        {
            size_t startPos = 0, endPos = 0, curPos = 0;
            if (_pEditView->getIndicatorRange(URL_INDIC, &startPos, &endPos, &curPos))
            {
                _pEditView->execute(SCI_SETSEL, startPos, endPos);
                _pEditView->execute(WM_COPY);
                checkClipboard();
                _pEditView->execute(SCI_SETSEL, curPos, curPos);
            }
        }
        break;

        case IDM_EDIT_COPY_BINARY:
        case IDM_EDIT_CUT_BINARY:
        {
            size_t textLen = _pEditView->execute(SCI_GETSELTEXT, 0, 0);
            if (!textLen)
                return;

            auto pBinText = std::make_unique<char[]>(textLen + 1);
            std::fill_n(pBinText.get(), textLen + 1, '\0');
            _pEditView->getSelectedTextToMultiChar(pBinText.get(), textLen + 1);

            QClipboard* clipboard = QApplication::clipboard();
            clipboard->clear();
            clipboard->setText(QString::fromLatin1(pBinText.get(), static_cast<int>(textLen)));

            if (id == IDM_EDIT_CUT_BINARY)
                _pEditView->execute(SCI_REPLACESEL, 0, reinterpret_cast<LPARAM>(""));

            checkClipboard();
        }
        break;

        case IDM_EDIT_PASTE:
        {
            std::lock_guard<std::mutex> lock(command_mutex);
            QWidget* focusedWidget = QApplication::focusWidget();
            if (focusedWidget == _pEditView->getWidget())
            {
                size_t nbSelections = _pEditView->execute(SCI_GETSELECTIONS);
                Buffer* buf = getCurrentBuffer();
                bool isRO = buf->isReadOnly();
                LRESULT selectionMode = _pEditView->execute(SCI_GETSELECTIONMODE);
                if (nbSelections > 1 && !isRO && selectionMode == SC_SEL_STREAM)
                {
                    bool isPasteDone = _pEditView->pasteToMultiSelection();
                    if (isPasteDone)
                        return;
                }
                _pEditView->execute(SCI_PASTE);
            }
            else
            {
                QKeyEvent pasteEvent(QEvent::KeyPress, Qt::Key_V, Qt::ControlModifier);
                QApplication::sendEvent(focusedWidget, &pasteEvent);
            }
        }
        break;

        case IDM_EDIT_PASTE_BINARY:
        {
            std::lock_guard<std::mutex> lock(command_mutex);
            QClipboard* clipboard = QApplication::clipboard();
            QString text = clipboard->text();
            if (text.isEmpty())
                return;

            _pEditView->execute(SCI_REPLACESEL, 0, reinterpret_cast<LPARAM>(""));
            _pEditView->execute(SCI_ADDTEXT, text.toLatin1().length(),
                reinterpret_cast<LPARAM>(text.toLatin1().constData()));
        }
        break;

        case IDM_EDIT_REDACT_SELECTION:
        {
            _pEditView->execute(SCI_BEGINUNDOACTION);
            const int selCount = static_cast<int>(_pEditView->execute(SCI_GETSELECTIONS));
            const bool useBullet = isShiftPressed();
            const int codePage = static_cast<int>(_pEditView->execute(SCI_GETCODEPAGE));
            const bool isUnicode = (codePage == SC_CP_UTF8);

            std::string maskSymbol;
            if (isUnicode)
                maskSymbol = useBullet ? "\xE2\x97\x8F" : "\xE2\x96\x88";
            else
                maskSymbol = useBullet ? "." : "#";

            for (int i = 0; i < selCount; ++i)
            {
                WPARAM start = _pEditView->execute(SCI_GETSELECTIONNSTART, i);
                LPARAM end = _pEditView->execute(SCI_GETSELECTIONNEND, i);
                int charCount = static_cast<int>(_pEditView->execute(SCI_COUNTCHARACTERS, start, end));

                if (charCount > 0)
                {
                    std::string maskStr;
                    for (int j = 0; j < charCount; ++j)
                    {
                        auto bytePos = _pEditView->execute(SCI_POSITIONRELATIVE, start, j);
                        char aChar = static_cast<char>(_pEditView->execute(SCI_GETCHARAT, bytePos));
                        maskStr += (aChar == '\r' || aChar == '\n') ? aChar : maskSymbol[0];
                    }
                    _pEditView->execute(SCI_SETTARGETRANGE, start, end);
                    _pEditView->execute(SCI_REPLACETARGET, static_cast<WPARAM>(-1),
                        reinterpret_cast<LPARAM>(maskStr.c_str()));
                }
            }
            _pEditView->execute(SCI_ENDUNDOACTION);
        }
        break;

        case IDM_EDIT_OPENINFOLDER:
        case IDM_EDIT_OPENASFILE:
        {
            if (_pEditView->execute(SCI_GETSELECTIONS) != 1)
                return;

            QString currentWord;
            if (_pEditView->hasSelection())
                currentWord = _pEditView->getSelectedTextToQString();
            
            QString filePath = currentWord.isEmpty() ? 
                QString::fromWCharArray(_pEditView->getCurrentBuffer()->getFullPathName()) :
                currentWord;
            
            if (id == IDM_EDIT_OPENINFOLDER)
            {
                nppOpenFolder(filePath);
            }
            else
            {
                QFileInfo fi(filePath);
                if (fi.exists())
                    nppOpenUrl(QUrl::fromLocalFile(fi.absoluteFilePath()));
                else
                    QMessageBox::warning(nullptr, "File Open",
                        "The file you're trying to open doesn't exist.");
            }
        }
        break;

        case IDM_EDIT_SEARCHONINTERNET:
        {
            if (_pEditView->execute(SCI_GETSELECTIONS) != 1)
                return;

            const NppGUI & nppGui = NppParameters::getInstance().getNppGUI();
            auto selectedText = _pEditView->getSelectedTextToQString();
            QString searchUrl = QString::fromWCharArray(nppGui._searchEnginePath.c_str());
            searchUrl.replace("%SEARCH%", QString(QUrl::toPercentEncoding(selectedText)));
            nppOpenUrl(searchUrl);
        }
        break;

        case IDM_EDIT_LOCATETOLINE:
        case IDM_EDIT_GOTO:
            goToDialog();
            break;

        case IDM_EDIT_SELECT_ALL:
            _pEditView->execute(SCI_SELECTALL);
            break;

        case IDM_EDIT_SELECT_WORD:
            selectWord();
            break;

        case IDM_EDIT_SWITCHVIEW:
            switchEditView();
            break;

        case IDM_EDIT_CLONE_TO_ANOTHER_VIEW:
            transferFile(TransferClone);
            break;

        case IDM_EDIT_MOVE_TO_ANOTHER_VIEW:
            transferFile(TransferMove);
            break;

        case IDM_EDIT_SET_READONLY:
        {
            Buffer* buf = _pEditView->getCurrentBuffer();
            if (buf)
            {
                buf->setReadOnly(!buf->isReadOnly());
                checkDocState();
            }
        }
        break;

        case IDM_EDIT_CLEAR_READONLY_FLAG:
        {
            Buffer* buf = _pEditView->getCurrentBuffer();
            if (buf)
            {
                buf->setReadOnly(false);
                checkDocState();
            }
        }
        break;

        case IDM_EDIT_REVERSE_REDO:
        {
            std::lock_guard<std::mutex> lock(command_mutex);
            _pEditView->execute(SCI_REVERSEEDO);
            checkUndoState();
        }
        break;

        // View commands
        case IDM_VIEW_TOOLBAR_NONE:
        case IDM_VIEW_TOOLBAR_STANDARD:
        case IDM_VIEW_TOOLBAR_REDUCE:
        case IDM_VIEW_TOOLBAR_BIG:
        case IDM_VIEW_TOOLBAR_SMALL:
        case IDM_VIEW_TOOLBAR_SMALL_ALT:
            _toolBar.intuitiveConfigure(id);
            break;

        case IDM_VIEW_CUSTOMIZE_COLOURS:
        {
            QColorDialog::getColor(Qt::red, nullptr, "Select Color");
        }
        break;

        case IDM_VIEW_LOCK_STToolbar:
        {
            NppGUI& nppGUI = NppParameters::getInstance().getNppGUI();
            if (nppGUI._toolBarStatus == TB_STANDARD)
                nppGUI._toolBarStatus = TB_STANDARD_LOCKED;
            else if (nppGUI._toolBarStatus == TB_STANDARD_LOCKED)
                nppGUI._toolBarStatus = TB_STANDARD;
            _toolBar.intuitiveConfigure(IDM_VIEW_TOOLBAR_STANDARD);
        }
        break;

        case IDM_VIEW_REFRESHTAB:
            if (QWidget* tabBar = _pDocTab->getWidget())
                tabBar->update();
            break;

        case IDM_VIEW_HIDETABBAR:
        {
            NppGUI& nppGUI = NppParameters::getInstance().getNppGUI();
            nppGUI._tabStatus = (nppGUI._tabStatus & ~TABBAR_DYNAMIC) |
                ((nppGUI._tabStatus & TABBAR_DYNAMIC) ? 0 : TABBAR_DYNAMIC);
            _pDocTab->showTabBar();
        }
        break;

        case IDM_VIEW_DRAWTABBAR_TOPBAR:
        {
            NppGUI& nppGUI = NppParameters::getInstance().getNppGUI();
            nppGUI._tabStatus = (nppGUI._tabStatus & ~TABBAR_MULTILINE) |
                ((nppGUI._tabStatus & TABBAR_MULTILINE) ? 0 : TABBAR_MULTILINE);
            _pDocTab->drawTabBar();
        }
        break;

        case IDM_VIEW_NEWTAB:
            fileNew();
            break;

        case IDM_VIEW_CLOSETAB:
            fileClose();
            break;

        case IDM_VIEW_CLOSETAB_MRU:
            _pDocTab->activateTab(0);
            fileClose();
            break;

        case IDM_VIEW_FILEBROWSER:
        {
            if (_pFileBrowser)
            {
                _pFileBrowser->setClosed(!_pFileBrowser->isClosed());
                _pFileBrowser->display();
                checkMenuItem(IDM_VIEW_FILEBROWSER, !_pFileBrowser->isClosed());
                _toolBar.setCheck(IDM_VIEW_FILEBROWSER, !_pFileBrowser->isClosed());
            }
            else
            {
                vector<wstring> emptyList;
                launchFileBrowser(emptyList, L"");
                checkMenuItem(IDM_VIEW_FILEBROWSER, true);
                _toolBar.setCheck(IDM_VIEW_FILEBROWSER, true);
            }
        }
        break;

        case IDM_VIEW_FILEBROWSER_HIDE:
            if (_pFileBrowser)
            {
                _pFileBrowser->setClosed(true);
                _pFileBrowser->display();
                checkMenuItem(IDM_VIEW_FILEBROWSER, false);
                _toolBar.setCheck(IDM_VIEW_FILEBROWSER, false);
            }
            break;

        case IDM_VIEW_DOCMAP:
        {
            if (_pDocMap)
            {
                _pDocMap->setClosed(!_pDocMap->isClosed());
                _pDocMap->display();
                checkMenuItem(IDM_VIEW_DOCMAP, !_pDocMap->isClosed());
                _toolBar.setCheck(IDM_VIEW_DOCMAP, !_pDocMap->isClosed());
            }
            else
            {
                doOpenDocumentMap();
                checkMenuItem(IDM_VIEW_DOCMAP, true);
                _toolBar.setCheck(IDM_VIEW_DOCMAP, true);
            }
        }
        break;

        case IDM_VIEW_FUNC_LIST:
        {
            if (_pFuncList)
            {
                _pFuncList->setClosed(!_pFuncList->isClosed());
                _pFuncList->display();
                checkMenuItem(IDM_VIEW_FUNC_LIST, !_pFuncList->isClosed());
                _toolBar.setCheck(IDM_VIEW_FUNC_LIST, !_pFuncList->isClosed());
            }
            else
            {
                doFunctionList();
                checkMenuItem(IDM_VIEW_FUNC_LIST, true);
                _toolBar.setCheck(IDM_VIEW_FUNC_LIST, true);
            }
        }
        break;

        case IDM_VIEW_SYNC_EDITVIEW:
        {
            NppGUI& nppGUI = NppParameters::getInstance().getNppGUI();
            nppGUI._syncBiggerDoc = !nppGUI._syncBiggerDoc;
            if (nppGUI._syncBiggerDoc)
                docGotoAnotherView(TransferClone, false, false);
            checkMenuItem(IDM_VIEW_SYNC_EDITVIEW, nppGUI._syncBiggerDoc);
        }
        break;

        case IDM_VIEW_SYNTAX_FOLD_ENABLE:
        {
            NppGUI& nppGUI = NppParameters::getInstance().getNppGUI();
            nppGUI._enableSFNGreeting = !nppGUI._enableSFNGreeting;
            checkMenuItem(IDM_VIEW_SYNTAX_FOLD_ENABLE, nppGUI._enableSFNGreeting);
        }
        break;

        case IDM_VIEW_SYNTAX_FOLD_COLLAPSE:
            _pEditView->execute(SCI_COLLAPSEALLFOLD);
            break;

        case IDM_VIEW_SYNTAX_FOLD_EXPAND:
            _pEditView->execute(SCI_EXPANDALLFOLD);
            break;

        case IDM_VIEW_DOC_TABLE:
        {
            if (!_pDocumentListPanel)
            {
                _pDocumentListPanel = new DocumentListPanel(nullptr, _pDocTab);
                _pDocumentListPanel->init();
            }
            bool isVisible = !_pDocumentListPanel->isClosed();
            _pDocumentListPanel->setClosed(isVisible);
            _pDocumentListPanel->display();
            checkMenuItem(IDM_VIEW_DOC_TABLE, !isVisible);
        }
        break;

        case IDM_VIEW_SWITCHTO_ANOTHER_SPLIT:
        {
            if (_pMainSplitter && _pMainSplitter->isDualView())
            {
                int targetView = (currentView() == MAIN_VIEW) ? SUB_VIEW : MAIN_VIEW;
                if (viewVisible(targetView))
                    activateView(targetView);
            }
        }
        break;

        case IDM_VIEW_ALWAYSON_TOP:
        {
            NppGUI& nppGUI = NppParameters::getInstance().getNppGUI();
            nppGUI._alwaysOnTop = !nppGUI._alwaysOnTop;
            if (QWidget* w = QApplication::activeWindow())
            {
                Qt::WindowFlags flags = w->windowFlags();
                if (nppGUI._alwaysOnTop)
                    flags |= Qt::WindowStaysOnTopHint;
                else
                    flags &= ~Qt::WindowStaysOnTopHint;
                w->setWindowFlags(flags);
                w->show();
            }
            checkMenuItem(IDM_VIEW_ALWAYSON_TOP, nppGUI._alwaysOnTop);
        }
        break;

        case IDM_VIEW_FULLSCREENON:
        {
            if (QWidget* w = QApplication::activeWindow())
            {
                if (_isFullScreen)
                {
                    w->showNormal();
                    _isFullScreen = false;
                }
                else
                {
                    w->showFullScreen();
                    _isFullScreen = true;
                }
            }
        }
        break;

        case IDM_VIEW_POSTIT:
        {
            if (QWidget* w = QApplication::activeWindow())
            {
                w->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
                w->show();
            }
        }
        break;

        case IDM_VIEW_ZOOMIN:
            _pEditView->execute(SCI_ZOOMIN);
            break;

        case IDM_VIEW_ZOOMOUT:
            _pEditView->execute(SCI_ZOOMOUT);
            break;

        case IDM_VIEW_ZOOMDEFAULT:
            _pEditView->execute(SCI_SETZOOM, 0);
            break;

        case IDM_VIEW_WRAP:
        {
            NppGUI& nppGUI = NppParameters::getInstance().getNppGUI();
            nppGUI._wordWrap = !nppGUI._wordWrap;
            _pEditView->execute(SCI_SETWRAPMODE, nppGUI._wordWrap ? SC_WRAP_WORD : SC_WRAP_NONE);
            _pEditView->execute(SCI_SETWRAPSTATE, nppGUI._wordWrap);
            checkMenuItem(IDM_VIEW_WRAP, nppGUI._wordWrap);
        }
        break;

        // Tab navigation
        case IDM_TAB_1:
        case IDM_TAB_2:
        case IDM_TAB_3:
        case IDM_TAB_4:
        case IDM_TAB_5:
        case IDM_TAB_6:
        case IDM_TAB_7:
        case IDM_TAB_8:
        case IDM_TAB_9:
            _pDocTab->activateTab(id - IDM_TAB_1);
            break;

        case IDM_TAB_CLOSE:
            fileClose();
            break;

        case IDM_TAB_CLOSE_ALL:
            fileCloseAll(true, false);
            break;

        case IDM_TAB_CLOSE_ALL_BUT_ACTIVE:
            fileCloseAllButCurrent();
            break;

        case IDM_TAB_SWITCH_TO_NEXT:
            _pDocTab->activateTab(_pDocTab->currentTabIndex() + 1);
            break;

        case IDM_TAB_SWITCH_TO_PREV:
            _pDocTab->activateTab(_pDocTab->currentTabIndex() - 1);
            break;

        // Search commands
        case IDM_SEARCH_FIND:
        case IDM_SEARCH_REPLACE:
        case IDM_SEARCH_MARK:
        {
            bool isFirstTime = !_findReplaceDlg.isCreated();
            DIALOG_TYPE dlgID = FIND_DLG;
            if (id == IDM_SEARCH_REPLACE)
                dlgID = REPLACE_DLG;
            else if (id == IDM_SEARCH_MARK)
                dlgID = MARK_DLG;
            _findReplaceDlg.doDialog(dlgID, _nativeLangSpeaker.isRTL());
            _findReplaceDlg.setSearchTextWithSettings();
            setFindReplaceFolderFilter(NULL, NULL);
            if (isFirstTime)
                _nativeLangSpeaker.changeFindReplaceDlgLang(_findReplaceDlg);
        }
        break;

        case IDM_SEARCH_FINDINFILES:
            if (_pFindInFilesDlg)
                _pFindInFilesDlg->display();
            break;

        case IDM_SEARCH_FINDINCREMENT:
        {
            auto str = _pEditView->getSelectedTextToQString();
            if (!str.isEmpty())
                _incrementFindDlg.setSearchText(str, 
                    _pEditView->getCurrentBuffer()->getUnicodeMode() != uni8Bit);
            _incrementFindDlg.display();
        }
        break;

        case IDM_SEARCH_FINDNEXT:
            _findReplaceDlg.goToNextFindPosition();
            break;

        case IDM_SEARCH_FINDPREV:
            _findReplaceDlg.goToPreviousFindPosition();
            break;

        // Encoding commands
        case IDM_FORMAT_CONV2_ANSI:
        case IDM_FORMAT_CONV2_UTF_8:
        case IDM_FORMAT_CONV2_UTF_16BE:
        case IDM_FORMAT_CONV2_UTF_16LE:
        {
            int idEncoding = -1;
            Buffer* buf = _pEditView->getCurrentBuffer();
            universalEncoding um = buf->getUnicodeMode();

            switch (id)
            {
                case IDM_FORMAT_CONV2_ANSI:
                    idEncoding = IDM_FORMAT_ANSI;
                    if (um == uni8Bit)
                        return;
                    break;
                case IDM_FORMAT_CONV2_UTF_8:
                    idEncoding = IDM_FORMAT_UTF_8;
                    if (um == uni8Bit)
                        return;
                    break;
                case IDM_FORMAT_CONV2_UTF_16BE:
                    idEncoding = IDM_FORMAT_UTF_16BE;
                    if (um == uni16BE)
                        return;
                    break;
                case IDM_FORMAT_CONV2_UTF_16LE:
                    idEncoding = IDM_FORMAT_UTF_16LE;
                    if (um == uni16LE)
                        return;
                    break;
            }

            if (idEncoding != -1)
            {
                _pEditView->saveCurrentPos();
                size_t docLen = _pEditView->getCurrentDocLen();
                _pEditView->execute(SCI_COPYRANGE, 0, docLen);
                _pEditView->execute(SCI_CLEARALL);
                _pEditView->setCurrentBuffer(buf);
                buf->setUnicodeMode(id == IDM_FORMAT_CONV2_UTF_16BE ? uni16BE :
                                   id == IDM_FORMAT_CONV2_UTF_16LE ? uni16LE : uni8Bit);
                _pEditView->execute(SCI_PASTE);
                _pEditView->restoreCurrentPosPreStep();
                _pEditView->execute(SCI_EMPTYUNDOBUFFER);
                buf->setSavePointDirty(true);
            }
        }
        break;

        // Default case - call Notepad_plus function if available
        default:
            // Handle remaining commands through a stub or log
            break;
    }
}
