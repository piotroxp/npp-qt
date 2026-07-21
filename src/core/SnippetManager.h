// SnippetManager.h — Code-snippet storage with INI persistence
// Copyright (C) 2026 Agent Army | GPL v3
//
// SnippetManager owns a name -> text map of code snippets. The text
// supports a small set of placeholder tokens that get rewritten on
// expansion:
//
//   |        -> the cursor lands here when the user is done editing.
//                There must be at most one cursor per snippet.
//   $NAME    -> a tab-stop variable (placeholder position); distinct
//                stops share the same name and are edited together.
//   $1, $2   -> numbered tab stops (advanced); $0 is the same as |.
//
// The file format is a flat INI:
//
//   [for]
//   text=for (int $VAR = 0; $VAR < n; ++$VAR) {\n    |\n}
//
//   [while]
//   text=while ($COND) {\n    |\n}
//
// SnippetManager is intentionally a plain QObject (no MOC signals yet) —
// it does not need to broadcast changes, callers poll. The constructor
// tries to load from the configured path; if no file exists yet, the
// manager starts empty and the caller may populate it programmatically.

#pragma once

#include <QHash>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>

class SnippetManager : public QObject {
    Q_OBJECT

public:
    /// One snippet, with its trigger name, body, and the offset of the
    /// cursor placeholder (`|`) within the body. -1 means no cursor.
    struct Snippet {
        QString name;
        QString text;
        int cursorOffset = -1;

        bool isValid() const { return !name.isEmpty(); }
    };

    /// A single tab-stop inside an expanded snippet (positions the
    /// editor should jump to in tab order).
    struct TabStop {
        QString name;       // "" for the default "tab through in order"
        int offset = 0;
        int length = 0;     // 0 for an insertion point
    };

    explicit SnippetManager(QObject* parent = nullptr);
    ~SnippetManager() override;

    // ── Persistence ──────────────────────────────────────────────────────
    /// Load snippets from an INI file. Returns true if the file was
    /// readable (false on missing/unreadable — but the manager remains
    /// usable in its previous state). Errors are logged via qWarning.
    bool loadFromFile(const QString& path);

    /// Write the current snippet set to an INI file. Returns true on
    /// success. Replaces the file if it exists.
    bool saveToFile(const QString& path) const;

    /// The path that loadFromFile last used (or empty if never loaded).
    QString filePath() const { return _filePath; }

    /// Set a default snippet directory and load `snippets.ini` from it
    /// if the file exists. Safe to call multiple times — uses the most
    /// recent path. Does nothing if the directory does not exist.
    void setDefaultDirectory(const QString& dir);

    /// Default search paths, in priority order: the configured default
    /// directory, then `$HOME/.config/npp-qt/snippets.ini`. Useful for
    /// diagnostics; tests should call setDefaultDirectory() instead.
    static QStringList defaultSearchPaths();

    // ── Mutation ─────────────────────────────────────────────────────────
    /// Insert or replace a snippet. The cursor placeholder `|` is parsed
    /// out of `text`; if absent, cursorOffset stays -1.
    void setSnippet(const QString& name, const QString& text);

    /// Remove a snippet by name. Returns true if it existed.
    bool removeSnippet(const QString& name);

    /// Remove all snippets.
    void clear();

    // ── Lookup ───────────────────────────────────────────────────────────
    bool hasSnippet(const QString& name) const;

    /// Return the raw text of a snippet (with placeholder markers still
    /// in place). Returns an empty string if the snippet does not exist.
    QString rawText(const QString& name) const;

    /// Return the snippet's expanded form (placeholders resolved to
    /// empty strings) and the cursor offset within that expansion.
    /// Returns a Snippet with isValid()==false on a miss.
    Snippet expand(const QString& name) const;

    /// All snippet names, sorted alphabetically. The list is a snapshot;
    /// mutations to the manager do not affect previously returned lists.
    QStringList names() const;

    int count() const { return _snippets.size(); }

    // ── Expansion helpers (used by expand() and exposed for tests) ───────
    /// Strip the cursor placeholder `|` from `text` and return the
    /// resulting cursor offset. -1 if no cursor marker is present.
    static int cursorOffsetFromText(const QString& text);

    /// Replace $NAME / $1 / $2 / ... / $0 with empty strings, returning
    /// the rewritten text. $0 collapses to nothing, mirroring `|`.
    static QString stripPlaceholders(const QString& text);

    /// Collect ordered tab stops from `text`. Numbered stops ($1, $2)
    /// are returned in numeric order; named stops ($FOO) follow.
    static QVector<TabStop> collectTabStops(const QString& text);

private:
    QHash<QString, QString> _snippets;
    QString _filePath;
};
