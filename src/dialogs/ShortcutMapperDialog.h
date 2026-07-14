// ShortcutMapperDialog.h - Keyboard shortcut configuration dialog
// Copyright (C) 2026 Agent Army | GPL v3

#pragma once

#include <QDialog>
#include <QEvent>
#include <QKeyEvent>
#include <QTableWidget>
#include <QListWidget>
#include <QDialogButtonBox>
#include <QLineEdit>

// Internal helper: captures a single key combination via a modal dialog
class KeyGrabber : public QObject {
    Q_OBJECT
public:
    explicit KeyGrabber(QLineEdit* display) : _display(display), _done(false) {}

    QString run(QDialog* parent) {
        parent->installEventFilter(this);
        parent->exec();
        parent->removeEventFilter(this);
        return _combo;
    }

signals:
    void captured(const QString& combo);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent* ke = static_cast<QKeyEvent*>(event);
            int key = ke->key();
            Qt::KeyboardModifiers mods = ke->modifiers();

            if (key == Qt::Key_Escape) {
                _combo.clear();
                _done = true;
                watched->event(event); // let dialog close
                return true;
            }

            // Build readable combo string
            QStringList parts;
            if (mods.testFlag(Qt::ShiftModifier))   parts << "Shift";
            if (mods.testFlag(Qt::ControlModifier))  parts << "Ctrl";
            if (mods.testFlag(Qt::AltModifier))      parts << "Alt";
            if (mods.testFlag(Qt::MetaModifier))     parts << "Meta";

            QString keyText = QKeySequence(key).toString();
            if (!keyText.isEmpty() && key != Qt::Key_Control &&
                key != Qt::Key_Shift && key != Qt::Key_Alt && key != Qt::Key_Meta) {
                parts << keyText;
            }

            _combo = parts.join("+");
            if (!_combo.isEmpty()) {
                _display->setText(_combo);
                _done = true;
                // Auto-close dialog when a valid combo is captured
                watched->event(event);
            }
            return true;
        }
        if (event->type() == QEvent::KeyRelease) {
            return true; // eat releases while grabbing
        }
        return QObject::eventFilter(watched, event);
    }

private:
    QLineEdit* _display;
    QString _combo;
    bool _done;
};

class ShortcutMapperDialog : public QDialog {
    Q_OBJECT
public:
    explicit ShortcutMapperDialog(QWidget* parent = nullptr);
    ~ShortcutMapperDialog() override;

private slots:
    void onCategoryChanged(const QString& category);
    void onItemDoubleClicked(int row, int col);
    void onClearShortcut();
    void onResetAll();
    void onApply();

private:
    void populateShortcuts(const QString& category);
    bool validateAndSave();
    void saveShortcuts();
    void checkAndWarnConflict(const QString& changedCmd, const QString& newShortcut);
    QString grabKeyCombo();

    QListWidget* _categoryList = nullptr;
    QTableWidget* _shortcutTable = nullptr;
    QDialogButtonBox* _buttonBox = nullptr;

    // Current category shortcuts: cmd name → shortcut string
    QMap<QString, QString> _currentShortcuts;
    QMap<QString, QString> _originalShortcuts;
};
