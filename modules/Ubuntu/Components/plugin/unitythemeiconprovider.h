/*
 * Copyright 2013 Canonical Ltd.
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

#ifndef UNITY_THEME_ICON_PROVIDER_H
#define UNITY_THEME_ICON_PROVIDER_H

#include <QQuickImageProvider>

class IconTheme
{
public:
    typedef QSharedPointer<class IconTheme> IconThemePointer;

    // Returns the icon theme named @name, creating it if it didn't exist yet.
    static IconThemePointer get(const QString &name);

    // Does a breadth-first search for an icon with any name in @names. Parent
    // themes are only looked at if the current theme doesn't contain any icon
    // in @names.
    QPixmap findBestIcon(const QStringList &names, const QSize &size);

private:
    enum SizeType { Fixed, Scalable, Threshold };

    struct Directory {
        QString path;
        SizeType sizeType;
        int size, minSize, maxSize, threshold;
    };

    IconTheme(const QString &name);
    SizeType sizeTypeFromString(const QString &string);
    static QPixmap loadIcon(const QString &filename, const QSize &size);
    QString lookupIconFile(const QString &dir, const QString &name);
    QPixmap lookupIcon(const QString &iconName, const QSize &size);
    QString lookupBestMatchingIcon(const QString &iconName, const QSize &size);
    QString lookupLargestIcon(const QString &iconName, int *maxSize);
    int directorySizeDistance(const Directory &dir, const QSize &size);

    QString name;
    QStringList baseDirs;
    QList<Directory> directories;
    QList<IconThemePointer> parents;
};


class UnityThemeIconProvider: public QQuickImageProvider
{
public:
    UnityThemeIconProvider();
    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);

private:
    QSharedPointer<IconTheme> theme;
};

#endif
