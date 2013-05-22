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

#ifndef ULLAYOUTS_P_H
#define ULLAYOUTS_P_H

#include "ullayouts.h"

class ULLayoutsPrivate {
    Q_DECLARE_PUBLIC(ULLayouts)
public:
    ULLayoutsPrivate(ULLayouts *qq);

    ULLayouts *q_ptr;
    QList<ULConditionalLayout*> layouts;
    QQuickItem *currentLayout;
    int currentLayoutIndex;

    // callbacks for the "layouts" QQmlListProperty of LayoutManager
    static void append_layout(QQmlListProperty<ULConditionalLayout>*, ULConditionalLayout*);
    static int count_layouts(QQmlListProperty<ULConditionalLayout>*);
    static ULConditionalLayout *at_layout(QQmlListProperty<ULConditionalLayout>*, int);
    static void clear_layouts(QQmlListProperty<ULConditionalLayout>*);

    void reLayout();
};

#endif // ULLAYOUTS_P_H
