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

#include "unitythemeiconprovider.h"

#include <QIcon>

UnityThemeIconProvider::UnityThemeIconProvider():
  QQuickImageProvider(QQuickImageProvider::Pixmap)
{
    QIcon::setThemeName("suru");
}

QPixmap UnityThemeIconProvider::requestPixmap(const QString &id, QSize *realSize, const QSize &requestedSize)
{
    QIcon icon;

    Q_FOREACH (const QString &name, id.split(",", QString::SkipEmptyParts)) {
        icon = QIcon::fromTheme(name);
        if (!icon.isNull()) {
            if (requestedSize.isValid()) {
                *realSize =requestedSize;
                return icon.pixmap(requestedSize);
            } else {
                QList<QSize> sizes = icon.availableSizes();
                if (sizes.count() > 0 && sizes.last().isValid()) {
                    *realSize = sizes.last();
                } else {
                    *realSize = QSize(32,32);
                }
                return icon.pixmap(*realSize);
            }
            break;
        }
    }
    return QPixmap();
}
