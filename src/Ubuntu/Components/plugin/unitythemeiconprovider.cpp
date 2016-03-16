/*
 * Copyright 2014 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Lars Uebernickel <lars.uebernickel@canonical.com>
 */

#include "unitythemeiconprovider.h"

#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QSvgRenderer>
#include <QPainter>
#include <QStandardPaths>
#include <QtDebug>

class IconTheme
{
public:
    typedef QSharedPointer<class IconTheme> IconThemePointer;

    // Returns the icon theme named @name, creating it if it didn't exist yet.
    static IconThemePointer get(const QString &name)
    {
        static QHash<QString, IconThemePointer> themes;

        IconThemePointer theme = themes[name];
        if (theme.isNull()) {
            theme = IconThemePointer(new IconTheme(name));
            themes[name] = theme;
        }

        return theme;
    }

    // Does a breadth-first search for an icon with any name in @names. Parent
    // themes are only looked at if the current theme doesn't contain any icon
    // in @names.
    QPixmap findBestIcon(const QStringList &names, const QSize &size, QSet<QString> *alreadySearchedThemes)
    {
        if (alreadySearchedThemes) {
            if (alreadySearchedThemes->contains(name))
                return QPixmap();
            alreadySearchedThemes->insert(name);
        }

        Q_FOREACH(const QString &name, names) {
            QPixmap pixmap = lookupIcon(name, size);
            if (!pixmap.isNull())
                return pixmap;
        }

        Q_FOREACH(IconThemePointer theme, parents) {
            QPixmap pixmap = theme->findBestIcon(names, size, alreadySearchedThemes);
            if (!pixmap.isNull())
                return pixmap;
        }

        return QPixmap();
    }

private:
    enum SizeType { Fixed, Scalable, Threshold };

    struct Directory {
        QString path;
        SizeType sizeType;
        int size, minSize, maxSize, threshold;
    };

    IconTheme(const QString &name): name(name)
    {
        QStringList paths = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);

        Q_FOREACH(const QString &path, paths) {
            QDir dir(path + "/icons/" + name);
            if (dir.exists())
                baseDirs.append(dir.absolutePath());
        }

        Q_FOREACH(const QString &baseDir, baseDirs) {
            QString filename = baseDir + "/index.theme";
            if (QFileInfo::exists(filename)) {
                QSettings settings(filename, QSettings::IniFormat);

                Q_FOREACH(const QString &path, settings.value("Icon Theme/Directories").toStringList()) {
                    Directory dir;
                    dir.path = path;
                    dir.sizeType = sizeTypeFromString(settings.value(path + "/Type", "Fixed").toString());
                    dir.size = settings.value(path + "/Size", 32).toInt();
                    dir.minSize = settings.value(path + "/MinSize", 0).toInt();
                    dir.maxSize = settings.value(path + "/MaxSize", 0).toInt();
                    dir.threshold = settings.value(path + "/Threshold", 0).toInt();
                    directories.append(dir);
                }

                Q_FOREACH(const QString &name, settings.value("Icon Theme/Inherits").toStringList()) {
                    if (name != QLatin1String("hicolor")) {
                        parents.append(IconTheme::get(name));
                    }
                }

                // there can only be one index.theme
                break;
            }
        }
    }

    SizeType sizeTypeFromString(const QString &string)
    {
        if (string == QLatin1String("Fixed"))
            return Fixed;
        if (string == QLatin1String("Scalable"))
            return Scalable;
        if (string == QLatin1String("Threshold"))
            return Threshold;
        qWarning() << "IconTheme: unknown size type '" << string << "'. Assuming 'Fixed'.";
        return Fixed;
    }

    static QPixmap loadIcon(const QString &filename, const QSize &size)
    {
        QPixmap pixmap;

        const bool anyZero = size.width() <= 0 || size.height() <= 0;
        const Qt::AspectRatioMode scaleMode = anyZero ? Qt::KeepAspectRatioByExpanding : Qt::KeepAspectRatio;

        if (filename.endsWith(QLatin1String(".png"))) {
            pixmap = QPixmap(filename);
            if (!pixmap.isNull() && !size.isNull() && (pixmap.width() != size.width() || pixmap.height() != size.height())) {
                const QSize newSize = pixmap.size().scaled(size.width(), size.height(), scaleMode);
                pixmap = pixmap.scaled(newSize);
            }
        }
        else if (filename.endsWith(QLatin1String(".svg"))) {
            QSvgRenderer renderer(filename);
            pixmap = QPixmap(renderer.defaultSize().scaled(size.width(), size.height(), scaleMode));
            pixmap.fill(Qt::transparent);
            QPainter painter(&pixmap);
            renderer.render(&painter);
            painter.end();
        }

        return pixmap;
    }

    QString lookupIconFile(const QString &dir, const QString &name)
    {
        QString png = QStringLiteral("%1/%2.png").arg(dir, name);
        QString svg = QStringLiteral("%1/%2.svg").arg(dir, name);

        Q_FOREACH(const QString &baseDir, baseDirs) {
            QString filename = baseDir + "/" + png;
            if (QFileInfo::exists(filename))
                return filename;

            filename = baseDir + "/" + svg;
            if (QFileInfo::exists(filename))
                return filename;
        }

        return QString();
    }

    QPixmap lookupIcon(const QString &iconName, const QSize &size)
    {
        const int iconSize = qMax(size.width(), size.height());
        if (iconSize > 0)
            return lookupBestMatchingIcon(iconName, size);
        else
            return lookupLargestIcon(iconName);
    }

    QPixmap lookupBestMatchingIcon(const QString &iconName, const QSize &size)
    {
        int minDistance = 10000;
        QString bestFilename;

        Q_FOREACH(const Directory &dir, directories) {
            int dist = directorySizeDistance(dir, size);
            if (dist >= minDistance)
                continue;

            QString filename = lookupIconFile(dir.path, iconName);
            if (!filename.isNull()) {
                minDistance = dist;
                bestFilename = filename;

                // bail out early if we can't get a better size match
                if (minDistance == 0)
                    break;
            }
        }

        if (!bestFilename.isNull())
            return loadIcon(bestFilename, size);

        return QPixmap();
    }

    QPixmap lookupLargestIcon(const QString &iconName)
    {
        int maxSize = 0;
        QString bestFilename;

        Q_FOREACH(const Directory &dir, directories) {
            int size = dir.sizeType == Scalable ? dir.maxSize : dir.size;
            if (size < maxSize)
                continue;

            QString filename = lookupIconFile(dir.path, iconName);
            if (!filename.isNull()) {
                maxSize = size;
                bestFilename = filename;
            }
        }

        if (!bestFilename.isNull())
            return loadIcon(bestFilename, QSize(maxSize, maxSize));

        return QPixmap();
    }

    int directorySizeDistance(const Directory &dir, const QSize &iconSize)
    {
        const int size = qMax(iconSize.width(), iconSize.height());
        switch (dir.sizeType) {
            case Fixed:
                return qAbs(size - dir.size);

            case Scalable:
                return qAbs(size - qBound(dir.minSize, size, dir.maxSize));

            case Threshold:
                return qAbs(size - qBound(dir.size - dir.threshold, size, dir.size + dir.threshold));

            default:
                return 10000;
        }
    }

    QString name;
    QStringList baseDirs;
    QList<Directory> directories;
    QList<IconThemePointer> parents;
};

UnityThemeIconProvider::UnityThemeIconProvider(const QString &themeName):
  QQuickImageProvider(QQuickImageProvider::Pixmap)
{
    theme = IconTheme::get(themeName);
}

QPixmap UnityThemeIconProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    // The hicolor theme will be searched last as per
    // https://specifications.freedesktop.org/icon-theme-spec/icon-theme-spec-latest.html
    QSet<QString> alreadySearchedThemes;
    const QStringList names = id.split(QLatin1Char(','), QString::SkipEmptyParts);
    QPixmap pixmap = theme->findBestIcon(names, requestedSize, &alreadySearchedThemes);

    if (pixmap.isNull()) {
        IconTheme::IconThemePointer theme = IconTheme::get(QStringLiteral("hicolor"));
        return theme->findBestIcon(names, requestedSize, nullptr);
    }

    *size = pixmap.size();
    return pixmap;
}
