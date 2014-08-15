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

#ifndef UCFOCUSSCOPE_P_H
#define UCFOCUSSCOPE_P_H

#include <QtQuick/private/qquickitem_p.h>

class UCFocusScope;
class UCFocusScopePrivate : public QQuickItemPrivate
{
    Q_DECLARE_PUBLIC(UCFocusScope)
public:

    static UCFocusScopePrivate *get(UCFocusScope *item) {
        return item->d_func();
    }

    UCFocusScopePrivate();
    ~UCFocusScopePrivate();
    void init();

    void setFocusable();
    bool isParentNonFocusable();

public:
    bool activeFocusOnMousePress:1;
    bool pressed:1;
};

#endif // UCFOCUSSCOPE_P_H
