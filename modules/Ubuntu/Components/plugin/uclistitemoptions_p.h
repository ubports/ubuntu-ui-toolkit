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

class UCListItem;
class UCListItemOptionsPrivate : public QObjectPrivate {
    Q_DECLARE_PUBLIC(UCListItemOptions)
public:
    UCListItemOptionsPrivate();
    ~UCListItemOptionsPrivate();
    static UCListItemOptionsPrivate* get(UCListItemOptions *options)
    {
        return options ? options->d_func() : 0;
    }

    bool optionsFailure:1;
    bool connected:1;
    bool leading:1;
    QQmlComponent *delegate;
    QQuickItem *panelItem;
    QList<QObject*> options;
    qreal optionSlotWidth;
    qreal offsetDragged;
    int optionsVisible;

    void _q_handlePanelDrag();
    void _q_handlePanelWidth();

    // options list property functions
    static void funcAppend(QQmlListProperty<QObject>*, QObject*);
    static int funcCount(QQmlListProperty<QObject>*);
    static QObject *funcAt(QQmlListProperty<QObject>*, int);
    static void funcClear(QQmlListProperty<QObject>*);

    static bool connectToListItem(UCListItemOptions *options, UCListItem *listItem, bool leading);
    static void disconnectFromListItem(UCListItemOptions *options);
    static bool isConnectedTo(UCListItemOptions *options, UCListItem *listItem);
    static qreal snap(UCListItemOptions *options);

    QQuickItem *createPanelItem();
};

#endif // UCLISTITEMOPTIONS_P_H
