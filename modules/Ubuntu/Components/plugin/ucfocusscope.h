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
#include "ucglobals.h"

class UCFocusScopePrivate;
class UCFocusScope : public QQuickItem
{
    Q_OBJECT
    DECLARE_PRIVATE_PROPERTY(bool, activeFocusOnMousePress, REVISION 1)
public:
    explicit UCFocusScope(QQuickItem *parent = 0);

public Q_SLOTS:
    Q_REVISION(1) void gainFocus();

protected:
    UCFocusScope(UCFocusScopePrivate &, QQuickItem *parent);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    Q_DECLARE_PRIVATE(UCFocusScope)
};

#endif // UCFOCUSSCOPE_H
