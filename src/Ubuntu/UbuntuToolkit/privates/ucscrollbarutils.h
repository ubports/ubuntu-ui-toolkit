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

#ifndef UCSCROLLBARUTILS_H
#define UCSCROLLBARUTILS_H

#include <QObject>
#include <ubuntutoolkitglobal.h>

class QQuickItem;

UT_NAMESPACE_BEGIN

class UBUNTUTOOLKIT_EXPORT UCScrollbarUtils : public QObject
{
    Q_OBJECT
public:
    explicit UCScrollbarUtils(QObject *parent = 0);

    Q_INVOKABLE static bool isComponentComplete(QQuickItem *item);

};

UT_NAMESPACE_END

#endif // UCSCROLLBARUTILS_H
