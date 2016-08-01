/*
 * Copyright 2016 Canonical Ltd.
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
 * Author: Andrea Bernabei <andrea.bernabei@canonical.com>
 */

#include "ucscrollbarutils_p.h"
#include <QtQuick/private/qquickitem_p.h>

UT_NAMESPACE_BEGIN

UCScrollbarUtils::UCScrollbarUtils(QObject *parent) : QObject(parent)
{
}

bool UCScrollbarUtils::isComponentComplete(QQuickItem *item)
{
    if (item == Q_NULLPTR) {
        qDebug() << "ScrollbarUtils: null Item parameter.";
        return false;
    } else {
        QQuickItemPrivate *f = QQuickItemPrivate::get(item);
        if (f == Q_NULLPTR) {
            //don't fall apart, but give a warning
            qDebug() << "ScrollbarUtils: failed to get item's pimpl.";
            return false;
        }
        return f->componentComplete;
    }
}

UT_NAMESPACE_END
