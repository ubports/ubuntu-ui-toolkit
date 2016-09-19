/*
 * Copyright 2015 Canonical Ltd.
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
 * Author: Christian Dywan <christian.dywan@canonical.com>
 */

#include "rune.h"

#include <QtQuick/private/qquickimagebase_p.h>

QHash<QUrl, QSharedPointer<QTemporaryFile> > UCQQuickImageExtension::s_rewrittenSciFiles;

/*!
    \internal

    The UCQQuickImageExtension class makes sure that for images loaded via
    QQuickImageBase the right version is loaded and that they are scaled
    appropriately. It does so by overriding the QQuickImageBase's
    'source' property.
*/
UCQQuickImageExtension::UCQQuickImageExtension(QObject *parent) :
    QObject(parent),
    m_image(static_cast<QQuickImageBase*>(parent))
{
    // connect sourceChanged signal to extendedSourceChanged
    QObject::connect(m_image, &QQuickImageBase::sourceChanged,
                     this, &UCQQuickImageExtension::extendedSourceChanged);
}

QUrl UCQQuickImageExtension::source() const
{
    return m_source;
}

void UCQQuickImageExtension::setSource(const QUrl& url)
{
    if (url != m_source) {
        m_source = url;
    }
}
