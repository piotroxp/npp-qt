// IconLoader.h -- 3-tier icon loading: file -> theme -> placeholder
// Tier 1: Load from rgba_icons/ SVG/PNG files (bundled with app)
// Tier 2: QIcon::fromTheme() (system icon theme)
// Tier 3: Empty QIcon (invisible placeholder)

#pragma once

#include <QIcon>
#include <QString>

// ---------------------------------------------------------------------------
// Icon names -- keys for the bundled icon set
// ---------------------------------------------------------------------------
namespace IconName {
    // File actions
    inline const QString newFile     = QStringLiteral("new");
    inline const QString open        = QStringLiteral("open");
    inline const QString save        = QStringLiteral("save");
    inline const QString saveAll     = QStringLiteral("saveAll");
    inline const QString close       = QStringLiteral("close");

    // Edit actions
    inline const QString cut         = QStringLiteral("cut");
    inline const QString copy        = QStringLiteral("copy");
    inline const QString paste       = QStringLiteral("paste");
    inline const QString undo        = QStringLiteral("undo");
    inline const QString redo        = QStringLiteral("redo");

    // Search actions
    inline const QString find        = QStringLiteral("find");
    inline const QString replace     = QStringLiteral("replace");

    // View actions
    inline const QString zoomIn      = QStringLiteral("zoomIn");
    inline const QString zoomOut     = QStringLiteral("zoomOut");
    inline const QString wordWrap    = QStringLiteral("wordWrap");
    inline const QString fullScreen  = QStringLiteral("fullScreen");

    // Misc
    inline const QString print       = QStringLiteral("print");
    inline const QString encoding    = QStringLiteral("encoding");
    inline const QString run         = QStringLiteral("run");
}

// ---------------------------------------------------------------------------
// IconLoader -- 3-tier icon loader
// ---------------------------------------------------------------------------
class IconLoader
{
public:
    explicit IconLoader(const QString& iconDir = QStringLiteral(":/icons"));

    // Load icon with 3-tier fallback.
    // @param iconName  Bare name (e.g. "new", "open") -- no extension.
    // @param size      Requested icon size in pixels.
    QIcon load(const QString& iconName, int size = 24) const;

    // Convenience: load with theme fallback name.
    // @param iconName    Bare name for file lookup.
    // @param themeName   Freedesktop icon name for fromTheme() fallback.
    // @param size        Requested size.
    QIcon loadWithTheme(const QString& iconName,
                        const QString& themeName,
                        int size = 24) const;

    // Reload from a different directory (e.g., after app data path resolution).
    void setIconDir(const QString& dir);

    // Return true if an icon file exists for the given name.
    bool hasIconFile(const QString& iconName) const;

private:
    mutable QString m_iconDir;
    mutable bool m_iconDirResolved = false;
    void ensureIconDirResolved() const;
};

// ---------------------------------------------------------------------------
// Global accessor (convenience singleton for MainWindow toolbar)
// ---------------------------------------------------------------------------
const IconLoader& globalIconLoader();
