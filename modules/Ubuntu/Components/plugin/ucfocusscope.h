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
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#ifndef UCFOCUSSCOPE_H
#define UCFOCUSSCOPE_H

#include <QtQuick/QQuickItem>

class UCFocusScope : public QQuickItem
{
    Q_OBJECT
public:
    explicit UCFocusScope(QQuickItem *parent = 0);

Q_SIGNALS:

public Q_SLOTS:

};

#endif // UCFOCUSSCOPE_H
