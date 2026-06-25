// Semantic Lift: Win32 ImageListSet → Qt6 ImageListSet
// Original: PowerEditor/src/WinControls/ImageListSet/ImageListSet.cpp
// Target: npp-qt/src/WinControls/ImageListSet.h + .cpp

#include "ImageListSet.h"
#include "NppDarkMode.h"

#include <QApplication>
#include <QPainter>
#include <QPixmap>
#include <QBitmap>
#include <QImage>
#include <QFile>
#include <QColor>
#include <cmath>
#include <algorithm>

// -----------------------------------------------------------------------
// ImageListSet colour palette
// Mirrors the Win32 toolbar colour constants used for icon recolouring.
// These are the _toolbar_-level colours (not NppDarkMode dark palette).
// -----------------------------------------------------------------------
namespace ImageListSetColors {
    // Toolbar light / dark base colours
    static constexpr QRgb g_cDefaultMainLight      = 0xE8E8E8;
    static constexpr QRgb g_cDefaultMainDark      = 0x2D2D30;
    static constexpr QRgb g_cDefaultSecondaryLight = 0x909090;
    static constexpr QRgb g_cDefaultSecondaryDark  = 0x5A5A5A;

    // NPP accent (Windows blue, can be overridden from settings)
    static constexpr QRgb g_defaultAccentColor = 0x0078D4;

    // Invert lightness of an RGB value for dark-mode icon generation.
    // Mirrors Win32 DPIManagerV2::inverseLightness.
    inline QRgb invertLightness(QRgb c) {
        int r = qMin(255, qRed(c)   + 40);
        int g = qMin(255, qGreen(c) + 40);
        int b = qMin(255, qBlue(c)  + 40);
        return qRgb(255 - r, 255 - g, 255 - b);
    }
}; // namespace ImageListSetColors

// -----------------------------------------------------------------------
// Fluent toolbar colour helper (mirrors Win32 FluentColor enum usage)
// -----------------------------------------------------------------------
namespace FluentColorMap {
    static QRgb fromEnum(FluentColor fc, int customColor = 0, bool useMono = false) {
        using namespace ImageListSetColors;
        switch (fc) {
            case FluentColor::accent:    return g_defaultAccentColor;
            case FluentColor::red:       return 0xE81123;
            case FluentColor::green:     return 0x008B00;
            case FluentColor::blue:      return 0x0078D4;
            case FluentColor::purple:    return 0xB146C2;
            case FluentColor::cyan:      return 0x00B7C3;
            case FluentColor::olive:     return 0x498205;
            case FluentColor::yellow:    return 0xFFB900;
            case FluentColor::custom:    return customColor ? static_cast<QRgb>(customColor) : g_defaultAccentColor;
            case FluentColor::defaultColor:
                return useMono
                    ? (NppDarkMode::instance().isEnabled() ? g_cDefaultMainDark : g_cDefaultMainLight)
                    : g_defaultAccentColor;
            default:                    return g_defaultAccentColor;
        }
    }
} // namespace FluentColorMap

// -----------------------------------------------------------------------
// IconList
// -----------------------------------------------------------------------

void IconList::init(const QSize& iconSize)
{
    _iconSize = iconSize;
    _icons.clear();
}

void IconList::create(const QSize& iconSize, const QVector<int>& iconIDArray)
{
    init(iconSize);
    for (int id : iconIDArray) {
        addIcon(id, iconSize, -1, false);
    }
}

void IconList::addIcon(int iconID, const QSize& desiredSize, int failIconID, bool applyFluentColorFlag)
{
    QIcon icon;

    // Attempt to load icon from Qt resource system.
    // In Win32 NPP this used DPIManagerV2::loadIcon.
    QString resourcePath = QStringLiteral(":/icons/%1.png").arg(iconID);
    if (QFile::exists(resourcePath)) {
        icon = QIcon(resourcePath);
    } else {
        // Platform-specific fallback: try fromTheme with the numeric ID as name.
        icon = QIcon::fromTheme(QString::number(iconID));
    }

    if (icon.isNull()) {
        // Try the failIconID fallback path
        if (failIconID != -1) {
            QString failPath = QStringLiteral(":/icons/%1.png").arg(failIconID);
            icon = QIcon(failPath);
        }
        if (icon.isNull()) {
            // Generate a transparent placeholder so callers always get a valid icon.
            QPixmap pm(desiredSize);
            pm.fill(Qt::transparent);
            icon = QIcon(pm);
        }
    }

    if (applyFluentColorFlag) {
        QIcon mutableIcon = icon;
        applyFluentColor(&mutableIcon);
        _icons.push_back(std::move(mutableIcon));
    } else {
        _icons.push_back(std::move(icon));
    }
}

void IconList::addIcon(const QIcon& icon)
{
    if (!icon.isNull()) {
        _icons.push_back(icon);
    }
}

bool IconList::replaceIcon(size_t index, const QString& iconLocation)
{
    if (index >= static_cast<size_t>(_icons.size()) || iconLocation.isEmpty()) {
        return false;
    }
    QIcon newIcon(iconLocation);
    if (newIcon.isNull()) {
        return false;
    }
    _icons[static_cast<int>(index)] = std::move(newIcon);
    return true;
}

bool IconList::applyFluentColor(
    QIcon* icon,
    const QVector<std::pair<QColor, QColor>>& colorMappings,
    int tolerance)
{
    if (!icon || icon->isNull()) {
        return false;
    }

    const bool changeEverything = (colorMappings.size() == 1)
        && (colorMappings[0].first == QColor());

    const QSize sz = _iconSize.isEmpty() ? QSize(16, 16) : _iconSize;
    QPixmap pm = icon->pixmap(sz);
    if (pm.isNull()) {
        return false;
    }

    QImage img = pm.toImage();
    if (img.format() != QImage::Format_ARGB32) {
        img = img.convertToFormat(QImage::Format_ARGB32);
    }

    for (int y = 0; y < img.height(); ++y) {
        for (int x = 0; x < img.width(); ++x) {
            const QRgb pixel = img.pixel(x, y);
            if (qAlpha(pixel) == 0) {
                continue; // transparent — leave untouched
            }

            if (changeEverything) {
                const QColor& cNew = colorMappings[0].second;
                img.setPixel(x, y, qRgba(cNew.red(), cNew.green(), cNew.blue(), qAlpha(pixel)));
            } else {
                for (const auto& [cToChange, cNew] : colorMappings) {
                    if (qAbs(qRed(pixel)   - cToChange.red())   <= tolerance &&
                        qAbs(qGreen(pixel) - cToChange.green()) <= tolerance &&
                        qAbs(qBlue(pixel)  - cToChange.blue())  <= tolerance)
                    {
                        img.setPixel(x, y, qRgba(cNew.red(), cNew.green(), cNew.blue(), qAlpha(pixel)));
                        break;
                    }
                }
            }
        }
    }

    *icon = QIcon(QPixmap::fromImage(std::move(img)));
    return true;
}

bool IconList::applyFluentColor(QIcon* icon)
{
    if (!icon || icon->isNull()) {
        return false;
    }

    using namespace ImageListSetColors;

    const QRgb cMain = NppDarkMode::instance().isEnabled() ? g_cDefaultMainDark : g_cDefaultMainLight;
    const QRgb cSecondary = NppDarkMode::instance().isEnabled() ? g_cDefaultSecondaryDark : g_cDefaultSecondaryLight;

    // Win32 NPP mapped secondary colour → accent colour.
    // Since the real accent may not be available at this layer we substitute 0
    // (the caller treats 0 as "use accent") and let FluentColorMap resolve it.
    QVector<std::pair<QColor, QColor>> mappings;
    mappings.append({QColor::fromRgba(cSecondary), QColor::fromRgba(0)});
    return applyFluentColor(icon, mappings);
}

void IconList::removeAll()
{
    _icons.clear();
}

const QIcon& IconList::icon(size_t index) const
{
    static const QIcon nullIcon;
    if (index >= static_cast<size_t>(_icons.size())) {
        return nullIcon;
    }
    return _icons[static_cast<int>(index)];
}

// -----------------------------------------------------------------------
// IconLists
// -----------------------------------------------------------------------

void IconLists::init(const QVector<IconListButtonUnit>& buttonUnits,
                         const QVector<DynamicIconCmd>& cmds2add)
{
    _buttonUnits = buttonUnits;
    _moreCommands = cmds2add;
}

void IconLists::create(const QSize& iconSize)
{
    // Create all 8 icon lists (HLIST_COUNT == 8)
    for (int i = 0; i < HLIST_COUNT; ++i) {
        _iconListVector.append(IconList());
        _iconListVector[i].init(iconSize);
    }
    reInit(iconSize);
}

void IconLists::reInit(const QSize& newSize)
{
    // Clear existing lists.
    for (int i = 0; i < HLIST_COUNT; ++i) {
        _iconListVector[i].removeAll();
    }

    // Static icons from toolbar button definitions.
    for (const IconListButtonUnit& tbi : _buttonUnits) {
        if (tbi._defaultIcon != -1) {
            _iconListVector[HLIST_DEFAULT].addIcon(tbi._defaultIcon,         newSize, tbi._stdIcon, true);
            _iconListVector[HLIST_DISABLE].addIcon(tbi._grayIcon,            newSize, tbi._stdIcon, false);
            _iconListVector[HLIST_DEFAULT2].addIcon(tbi._defaultIcon2,        newSize, tbi._stdIcon, true);
            _iconListVector[HLIST_DISABLE2].addIcon(tbi._grayIcon2,           newSize, tbi._stdIcon, false);
            _iconListVector[HLIST_DEFAULT_DM].addIcon(tbi._defaultDarkModeIcon, newSize, tbi._stdIcon, true);
            _iconListVector[HLIST_DISABLE_DM].addIcon(tbi._grayDarkModeIcon,   newSize, tbi._stdIcon, false);
            _iconListVector[HLIST_DEFAULT_DM2].addIcon(tbi._defaultDarkModeIcon2, newSize, tbi._stdIcon, true);
            _iconListVector[HLIST_DISABLE_DM2].addIcon(tbi._grayDarkModeIcon2,  newSize, tbi._stdIcon, false);
        }
    }

    // Dynamic icons from plugins.
    for (const DynamicIconCmd& dyn : _moreCommands) {
        _iconListVector[HLIST_DEFAULT].addIcon(dyn._icon);
        _iconListVector[HLIST_DISABLE].addIcon(dyn._icon);
        _iconListVector[HLIST_DEFAULT2].addIcon(dyn._icon);
        _iconListVector[HLIST_DISABLE2].addIcon(dyn._icon);

        // Dark-mode list: use explicit DM icon if provided, otherwise invert the normal icon.
        QIcon dmIcon = dyn._iconDM;
        if (dmIcon.isNull()) {
            const QSize sz = newSize.isEmpty() ? QSize(16, 16) : newSize;
            QPixmap pm = dyn._icon.pixmap(sz);
            if (!pm.isNull()) {
                QImage img = pm.toImage();
                if (img.format() != QImage::Format_ARGB32) {
                    img = img.convertToFormat(QImage::Format_ARGB32);
                }
                for (int y = 0; y < img.height(); ++y) {
                    for (int x = 0; x < img.width(); ++x) {
                        const QRgb p = img.pixel(x, y);
                        if (qAlpha(p) == 0) continue;
                        const QRgb inv = ImageListSetColors::invertLightness(p);
                        img.setPixel(x, y, qRgba(qRed(inv), qGreen(inv), qBlue(inv), qAlpha(p)));
                    }
                }
                dmIcon = QIcon(QPixmap::fromImage(std::move(img)));
            }
        }

        _iconListVector[HLIST_DEFAULT_DM].addIcon(dmIcon);
        _iconListVector[HLIST_DISABLE_DM].addIcon(dmIcon);
        _iconListVector[HLIST_DEFAULT_DM2].addIcon(dmIcon);
        _iconListVector[HLIST_DISABLE_DM2].addIcon(dmIcon);
    }
}

void IconLists::destroy()
{
    for (int i = 0; i < HLIST_COUNT; ++i) {
        _iconListVector[i].removeAll();
    }
    _iconListVector.clear();
    _buttonUnits.clear();
    _moreCommands.clear();
}
