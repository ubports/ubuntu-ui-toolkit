/*
 * Copyright 2013 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Jussi Pakkanen <jussi.pakkanen@canonical.com>
*/

#ifndef THUMBNAIL_PROVIDER_H
#define THUMBNAIL_PROVIDER_H

#include <QQuickImageProvider>

class ThumbnailProvider: public QQuickImageProvider
{
private:
    Thumbnailer tn;

public:
    ThumbnailProvider();
    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);
};

#endif
