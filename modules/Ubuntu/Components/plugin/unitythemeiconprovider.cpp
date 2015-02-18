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


IconTheme::IconThemePointer IconTheme::get(const QString &name)
{
    static QHash<QString, IconThemePointer> themes;

    IconThemePointer theme = themes[name];
    if (theme.isNull()) {
        theme = IconThemePointer(new IconTheme(name));
        themes[name] = theme;
    }

    return theme;
}

QPixmap IconTheme::findBestIcon(const QStringList &names, const QSize &size)
{
    Q_FOREACH(const QString &name, names) {
        QPixmap pixmap = lookupIcon(name, size);
        if (!pixmap.isNull())
            return pixmap;
    }

    Q_FOREACH(IconThemePointer theme, parents) {
        QPixmap pixmap = theme->findBestIcon(names, size);
        if (!pixmap.isNull())
            return pixmap;
    }

    return QPixmap();
}

IconTheme::IconTheme(const QString &name): name(name)
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

            Q_FOREACH(const QString &name, settings.value("Icon Theme/Inherits").toStringList())
                parents.append(IconTheme::get(name));

            // there can only be one index.theme
            break;
        }
    }
}

IconTheme::SizeType IconTheme::sizeTypeFromString(const QString &string)
{
    if (string == "Fixed")
        return Fixed;
    if (string == "Scalable")
        return Scalable;
    if (string == "Threshold")
        return Threshold;
    qWarning() << "IconTheme: unknown size type '" << string << "'. Assuming 'Fixed'.";
    return Fixed;
}

QPixmap IconTheme::loadIcon(const QString &filename, const QSize &size)
{
    QPixmap pixmap;

    if (filename.endsWith(".png")) {
        pixmap = QPixmap(filename);
        if (!pixmap.isNull() && !size.isNull() && (pixmap.width() != size.width() || pixmap.height() != size.height())) {
            const QSize newSize = pixmap.size().scaled(size.width(), size.height(), Qt::KeepAspectRatioByExpanding);
            pixmap = pixmap.scaled(newSize);
        }
    }
    else if (filename.endsWith(".svg")) {
        QSvgRenderer renderer(filename);
        pixmap = QPixmap(renderer.defaultSize().scaled(size.width(), size.height(), Qt::KeepAspectRatioByExpanding));
        pixmap.fill(Qt::transparent);
        QPainter painter(&pixmap);
        renderer.render(&painter);
        painter.end();
    }

    return pixmap;
}

QString IconTheme::lookupIconFile(const QString &dir, const QString &name)
{
    QString png = QString("%1/%2.png").arg(dir).arg(name);
    QString svg = QString("%1/%2.svg").arg(dir).arg(name);

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

QPixmap IconTheme::lookupIcon(const QString &iconName, const QSize &size)
{
    QString bestFilename;
    QSize sizeToLoad;

    const int iconSize = qMax(size.width(), size.height());
    if (iconSize > 0) {
        bestFilename = lookupBestMatchingIcon(iconName, size);
        sizeToLoad = size;
    } else {
        int maxSize;
        bestFilename = lookupLargestIcon(iconName, &maxSize);
        sizeToLoad = QSize(maxSize, maxSize);
    }

    if (!bestFilename.isNull())
        return loadIcon(bestFilename, sizeToLoad);

    return QPixmap();
}

QString IconTheme::lookupBestMatchingIcon(const QString &iconName, const QSize &size)
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

    return bestFilename;
}

QString IconTheme::lookupLargestIcon(const QString &iconName, int *maxSize)
{
    *maxSize = 0;
    QString bestFilename;

    Q_FOREACH(const Directory &dir, directories) {
        int size = dir.sizeType == Scalable ? dir.maxSize : dir.size;
        if (size < *maxSize)
            continue;

        QString filename = lookupIconFile(dir.path, iconName);
        if (!filename.isNull()) {
            *maxSize = size;
            bestFilename = filename;
        }
    }

    return bestFilename;
}

int IconTheme::directorySizeDistance(const Directory &dir, const QSize &size)
{
    const int iconSize = qMax(size.width(), size.height());
    switch (dir.sizeType) {
        case Fixed:
            return qAbs(iconSize - dir.size);

        case Scalable:
            return qAbs(iconSize - qBound(dir.minSize, iconSize, dir.maxSize));

        case Threshold:
            return qAbs(iconSize - qBound(dir.size - dir.threshold, iconSize, dir.size + dir.threshold));

        default:
            return 10000;
    }
}



UnityThemeIconProvider::UnityThemeIconProvider():
  QQuickImageProvider(QQuickImageProvider::Pixmap)
{
    theme = IconTheme::get("suru");
}

QPixmap UnityThemeIconProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    QPixmap pixmap = theme->findBestIcon(id.split(",", QString::SkipEmptyParts), requestedSize);
    *size = pixmap.size();
    return pixmap;
}
