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
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#ifndef ULCONDITIONALLAYOUT_P_H
#define ULCONDITIONALLAYOUT_P_H

#include "ulconditionallayout.h"

class ULLayouts;
class ULConditionalLayoutPrivate {
    Q_DECLARE_PUBLIC(ULConditionalLayout)
public:
    ULConditionalLayoutPrivate(ULConditionalLayout *qq);

    ULConditionalLayout *q_ptr;
    QQmlBinding *when;
    QQmlComponent *component;
    QString name;

    ULLayouts *layouts();
};

#endif // ULCONDITIONALLAYOUT_P_H
