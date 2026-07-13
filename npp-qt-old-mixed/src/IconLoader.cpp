// IconLoader.cpp — 3-tier icon loading implementation
//
// Icon resolution is LAZY: the global static IconLoader singleton is constructed
// during C++ static initialization (before main()), when qApp is null.
// Therefore we defer path resolution to the first load() call, when qApp exists.

#include "IconLoader.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QLoggingCategory>
#include <QPainter>
#include <QPixmap>

// QtSvg is optional — SVG icons need it; PNG fallback works without it
#ifdef QT_SVG_LIB
#include <QSvgRenderer>
#define ICONLOADER_HAVE_SVG 1
#else
#define ICONLOADER_HAVE_SVG 0
#endif

// -----------------------------------------------------------------------
// IconLoader — lazy path resolution
// -----------------------------------------------------------------------
IconLoader::IconLoader(const QString& /*iconDir*/)
    : m_iconDirResolved(false)
{
    // Deliberately NOT resolving m_iconDir here.
    // The global s_globalLoader is constructed during static init (before main()),
    // when qApp is null. resolveIconDir() would return an empty path.
    // Resolution is deferred to the first load() call.
}

void IconLoader::ensureIconDirResolved() const
{
    if (m_iconDirResolved)
        return;
    m_iconDirResolved = true;

    // Candidate paths ordered by priority:
    // 1. Relative to binary: build/npp-qt/../rgba_icons (post-build copy)
    // 2. Relative to binary: build/rgba_icons
    // 3. Relative to current working directory: rgba_icons/
    // 4. Qt resource path: :icons
    const QStringList candidates = {
        qApp ? qApp->applicationDirPath() + QStringLiteral("/../rgba_icons") : QString(),
        qApp ? qApp->applicationDirPath() + QStringLiteral("/rgba_icons") : QString(),
        QStringLiteral("rgba_icons"),
        QStringLiteral(":/icons"),
    };

    for (const QString& candidate : candidates) {
        if (candidate.isEmpty())
            continue;
        QFileInfo fi(candidate);
        if (candidate.startsWith(QLatin1Char(':')) || fi.exists()) {
            m_iconDir = candidate;
            return;
        }
    }
    m_iconDir = QStringLiteral("rgba_icons"); // fallback
}

QIcon IconLoader::load(const QString& iconName, int size) const
{
    ensureIconDirResolved();

    // Tier 1: file-based SVG → PNG
    const QStringList extensions = {QStringLiteral(".svg"), QStringLiteral(".png")};
    for (const QString& ext : extensions) {
        QString filePath;
        if (m_iconDir.startsWith(QLatin1Char(':'))) {
            filePath = m_iconDir + QLatin1Char('/') + iconName + ext;
        } else {
            filePath = m_iconDir + QLatin1Char('/') + iconName + ext;
        }

        if (QFile::exists(filePath)) {
            if (ext == QStringLiteral(".svg") && ICONLOADER_HAVE_SVG) {
                QSvgRenderer renderer(filePath);
                if (renderer.isValid()) {
                    QPixmap pm(size, size);
                    pm.fill(Qt::transparent);
                    QPainter p(&pm);
                    renderer.render(&p);
                    return QIcon(pm);
                }
            } else if (ext == QStringLiteral(".png")) {
                QPixmap pm(filePath);
                if (!pm.isNull())
                    return QIcon(pm);
            }
        }
    }

    // Tier 2: empty (no theme fallback for bare names)
    return QIcon();
}

QIcon IconLoader::loadWithTheme(const QString& iconName,
                                const QString& themeName,
                                int size) const
{
    // Tier 1: file-based
    QIcon fileIcon = load(iconName, size);
    if (!fileIcon.isNull())
        return fileIcon;

    // Tier 2: system theme
    QIcon themeIcon = QIcon::fromTheme(themeName);
    if (!themeIcon.isNull())
        return themeIcon;

    // Tier 3: placeholder (scaled transparent pixmap with a colored dot so it's visible)
    QPixmap placeholder(size, size);
    placeholder.fill(Qt::transparent);
    {
        QPainter p(&placeholder);
        p.setRenderHint(QPainter::Antialiasing);
        p.fillRect(2, 2, size - 4, size - 4, QColor(0x7F, 0x7F, 0x7F, 0x40));
    }
    return QIcon(placeholder);
}

void IconLoader::setIconDir(const QString& dir)
{
    m_iconDir = dir;
    m_iconDirResolved = true; // explicitly set, no lazy resolution needed
}

bool IconLoader::hasIconFile(const QString& iconName) const
{
    ensureIconDirResolved();
    for (const QString& ext : {QStringLiteral(".svg"), QStringLiteral(".png")}) {
        QString path = m_iconDir + QLatin1Char('/') + iconName + ext;
        if (QFile::exists(path))
            return true;
    }
    return false;
}

// -----------------------------------------------------------------------
// Global singleton — constructed during static init (qApp is null here)
// -----------------------------------------------------------------------
static IconLoader s_globalLoader;

const IconLoader& globalIconLoader()
{
    return s_globalLoader;
}
