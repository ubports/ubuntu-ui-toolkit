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
#include <ubuntutoolkitglobal.h>

UT_NAMESPACE_BEGIN

class UBUNTUTOOLKIT_EXPORT UnityThemeIconProvider: public QQuickImageProvider
{
public:
    UnityThemeIconProvider(const QString &themeName = QStringLiteral("suru"));
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;

private:
    QSharedPointer<class IconTheme> theme;
};

UT_NAMESPACE_END

#endif
