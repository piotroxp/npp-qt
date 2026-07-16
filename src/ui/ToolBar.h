// ToolBar.h - Main toolbar with full Notepad++ functionality
// Copyright (C) 2026 Agent Army
// GPL v3

#pragma once

#include <QToolBar>
#include <QString>
#include <QMap>
#include <QAction>
#include <QMenu>
#include <QList>

class ToolBar : public QToolBar {
    Q_OBJECT
public:
    explicit ToolBar(QWidget* parent = nullptr);
    ~ToolBar() override;

    // Icon size management
    void setIconSize(int size);
    void setIconSizeSmall();
    void setIconSizeLarge();
    int iconSize() const { return m_iconSize; }
    
    // Style options
    enum ToolBarStyle { IconOnly, TextOnly, TextBesideIcon, TextBelowIcon };
    void setToolBarStyle(ToolBarStyle style);
    ToolBarStyle toolBarStyle() const { return m_style; }
    
    // Action management
    QAction* actionForCommand(const QString& command) const;
    
    // Enable/disable actions
    void setActionEnabled(const QString& command, bool enabled);
    void updateActionStates(const QMap<QString, bool>& states);
    
    // Zoom controls
    void zoomIn();
    void zoomOut();
    void zoomReset();
    
    // Drag handle for floating toolbar
    void setDragHandleVisible(bool visible);
    bool isDragHandleVisible() const { return m_dragHandleVisible; }

    // DPI
    void updateForDpi();

Q_SIGNALS:
    void toolBarCommand(const QString& cmd);
    void toolBarStyleChanged(ToolBarStyle style);
    void zoomChanged(int level);

public slots:
    // File actions
    void onNew();
    void onOpen();
    void onSave();
    void onSaveAs();
    void onClose();
    
    // Edit actions
    void onUndo();
    void onRedo();
    void onCut();
    void onCopy();
    void onPaste();
    void onDelete();
    void onFind();
    void onReplace();
    void onGotoLine();
    
    // Comment actions
    void onToggleComment();
    void onBlockComment();
    
    // Indent actions
    void onIndent();
    void onOutdent();
    
    // Macro actions
    void onRecordMacro();
    void onStopMacro();
    void onPlayMacro();
    
    // View/Run actions
    void onRun();
    void onPrint();
    void onWordWrap();
    void onShowAllCharacters();
    void onToggleReadOnly();
    
    // Zoom actions
    void onZoomIn();
    void onZoomOut();
    void onZoomReset();
    
    // Panel actions
    void onDocMap();
    void onFuncList();
    void onFileBrowser();
    
    // Encoding dropdown
    void onEncodingMenu();
    
    // EOL dropdown
    void onEolMenu();
    
    // Language dropdown
    void onLanguageMenu();
    
    // Customization
    void showCustomizationDialog();

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private slots:
    void onActionTriggered();
    void onEncodingAction(QAction* action);
    void onEolAction(QAction* action);
    void onLanguageAction(QAction* action);
    void onStyleAction(QAction* action);

private:
    void setupActions();
    void setupMenus();
    void createEncodingMenu();
    void createEolMenu();
    void createLanguageMenu();
    void applyDpiScaling();
    void updateZoomLevel(int delta);
    
    // Action map
    QMap<QString, QAction*> m_actions;
    
    // Menus
    QMenu* m_encodingMenu = nullptr;
    QMenu* m_eolMenu = nullptr;
    QMenu* m_languageMenu = nullptr;
    
    // State
    int m_iconSize = 16;
    ToolBarStyle m_style = TextBesideIcon;
    bool m_dragHandleVisible = true;
    int m_zoomLevel = 0;
    bool m_wordWrapEnabled = false;
    bool m_readOnlyEnabled = false;
    bool m_macroRecording = false;
    
    // New tab button
    QAction* m_newTabAction = nullptr;
    
    // Constants
    static constexpr int DefaultIconSize = 16;
    static constexpr int LargeIconSize = 24;
    static constexpr int SmallIconSize = 16;
};
