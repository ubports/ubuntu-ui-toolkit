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

#ifndef UCVIEWITEMOPTIONS_P_H
#define UCVIEWITEMOPTIONS_P_H

#include "ucviewitemoptions.h"

class UCViewItemOptionsPrivate {
    Q_DECLARE_PUBLIC(UCViewItemOptions)
public:
    UCViewItemOptionsPrivate(UCViewItemOptions* qq);

    UCViewItemOptions *q_ptr;
    QQmlComponent *delegate;
    qreal offset;
    int visibleOptions;
    QList<QObject*> options;
    QColor backgroundColor;
};

#endif // UCVIEWITEMOPTIONS_P_H
