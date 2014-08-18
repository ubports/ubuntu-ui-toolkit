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
 */

#ifndef UCLISTITEMOPTIONS_P_H
#define UCLISTITEMOPTIONS_P_H

#include "uclistitemoptions.h"
#include "QtCore/private/qobject_p.h"

class UCListItemOptionsPrivate : public QObjectPrivate {
    Q_DECLARE_PUBLIC(UCListItemOptions)
public:
    UCListItemOptionsPrivate();
    ~UCListItemOptionsPrivate();
    static UCListItemOptionsPrivate* get(UCListItemOptions *options)
    {
        Q_ASSERT(options);
        return options->d_func();
    }

    QQmlComponent *delegate;
    QQuickItem *panel;
    QColor backgroundColor;
    QList<QObject*> options;

    bool createPanel(bool isLeading);
};

#endif // UCLISTITEMOPTIONS_P_H
