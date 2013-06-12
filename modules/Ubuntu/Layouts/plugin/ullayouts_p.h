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
#include <QtQml/QQmlIncubator>
#include "layoutaction_p.h"

typedef QHash<QString, QQuickItem*> LaidOutItemsMap;

class ULItemLayout;
class ULLayoutsPrivate : QQmlIncubator {
    Q_DECLARE_PUBLIC(ULLayouts)
public:

    ULLayoutsPrivate(ULLayouts *qq);

    ULLayouts *q_ptr;
    QList<ULConditionalLayout*> layouts;
    ChangeList changes;
    LaidOutItemsMap itemsToLayout;
    QList<QQuickItem*> excludedFromLayout;
    QQuickItem* currentLayoutItem;
    QQuickItem* previousLayoutItem;
    int currentLayoutIndex;
    bool ready:1;

    // callbacks for the "layouts" QQmlListProperty of LayoutManager
    static void append_layout(QQmlListProperty<ULConditionalLayout>*, ULConditionalLayout*);
    static int count_layouts(QQmlListProperty<ULConditionalLayout>*);
    static ULConditionalLayout *at_layout(QQmlListProperty<ULConditionalLayout>*, int);
    static void clear_layouts(QQmlListProperty<ULConditionalLayout>*);

    void validateConditionalLayouts();
    void getLaidOutItems();
    void updateLayout();

protected: // QQmlIncubator stuff
    void setInitialState(QObject *object);
    void statusChanged(Status status);

private:
    void reLayout();
    void hideExcludedItems();
    void reparentItems();
    void reparentToItemLayout(LaidOutItemsMap &map, ULItemLayout *fragment);
};

#endif // ULLAYOUTS_P_H
