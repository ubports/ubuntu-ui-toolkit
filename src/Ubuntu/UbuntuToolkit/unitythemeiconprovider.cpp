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
#include <QImageReader>
#include <QStandardPaths>
#include <QtDebug>

UT_NAMESPACE_BEGIN

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
    QImage findBestIcon(const QStringList &names, QSize *impsize, const QSize &size, QSet<QString> *alreadySearchedThemes)
    {
        if (alreadySearchedThemes) {
            if (alreadySearchedThemes->contains(name))
                return QImage();
            alreadySearchedThemes->insert(name);
        }

        Q_FOREACH(const QString &name, names) {
            QImage image = lookupIcon(name, impsize, size);
            if (!image.isNull())
                return image;
        }

        Q_FOREACH(IconThemePointer theme, parents) {
            QImage image = theme->findBestIcon(names, impsize, size, alreadySearchedThemes);
            if (!image.isNull())
                return image;
        }

        return QImage();
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
        const QStringList paths = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);

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

    static QImage loadIcon(const QString &filename, QSize *impsize, const QSize &requestSize)
    {
        QImage image;
        QImageReader imgio(filename);

        const bool force_scale = imgio.format() == QStringLiteral("svg")
            || imgio.format() == QStringLiteral("svgz");

        if (requestSize.width() > 0 || requestSize.height() > 0) {
            QSize s = imgio.size();
            qreal ratio = 0.0;
            if (requestSize.width() > 0 && (force_scale || requestSize.width() < s.width())) {
                ratio = qreal(requestSize.width())/s.width();
            }
            if (requestSize.height() > 0 && (force_scale || requestSize.height() < s.height())) {
                qreal hr = qreal(requestSize.height())/s.height();
                if (ratio == 0.0 || hr < ratio)
                    ratio = hr;
            }
            if (ratio > 0.0) {
                s.setHeight(qRound(s.height() * ratio));
                s.setWidth(qRound(s.width() * ratio));
                imgio.setScaledSize(s);
            }
        }

        if (impsize)
            *impsize = imgio.scaledSize();

        if (imgio.read(&image)) {
            if (impsize)
                *impsize = image.size();
            return image;
        } else {
            return QImage();
        }
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

    QImage lookupIcon(const QString &iconName, QSize *impsize, const QSize &size)
    {
        const int iconSize = qMax(size.width(), size.height());
        if (iconSize > 0)
            return lookupBestMatchingIcon(iconName, impsize, size);
        else
            return lookupLargestIcon(iconName, impsize);
    }

    QImage lookupBestMatchingIcon(const QString &iconName, QSize *impsize, const QSize &size)
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
            return loadIcon(bestFilename, impsize, size);

        return QImage();
    }

    QImage lookupLargestIcon(const QString &iconName, QSize *impsize)
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
            return loadIcon(bestFilename, impsize, QSize(maxSize, maxSize));

        return QImage();
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
  QQuickImageProvider(QQuickImageProvider::Image)
{
    theme = IconTheme::get(themeName);
}

QImage UnityThemeIconProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    // The hicolor theme will be searched last as per
    // https://specifications.freedesktop.org/icon-theme-spec/icon-theme-spec-latest.html
    QSet<QString> alreadySearchedThemes;
    const QStringList names = id.split(QLatin1Char(','), QString::SkipEmptyParts);
    QImage image = theme->findBestIcon(names, size, requestedSize, &alreadySearchedThemes);

    if (image.isNull()) {
        IconTheme::IconThemePointer theme = IconTheme::get(QStringLiteral("hicolor"));
        return theme->findBestIcon(names, size, requestedSize, nullptr);
    }

    return image;
}

UT_NAMESPACE_END
