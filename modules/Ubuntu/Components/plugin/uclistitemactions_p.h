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

#ifndef UCLISTITEMACTIONS_P_H
#define UCLISTITEMACTIONS_P_H

#include "uclistitemactions.h"
#include "QtCore/private/qobject_p.h"

class UCListItem;
class UCListItemActionsPrivate : public QObjectPrivate {
    Q_DECLARE_PUBLIC(UCListItemActions)
public:
    UCListItemActionsPrivate();
    ~UCListItemActionsPrivate();
    static UCListItemActionsPrivate* get(UCListItemActions *actions)
    {
        return actions ? actions->d_func() : 0;
    }

    bool dragging:1;
    QQmlComponent *delegate;
    QQuickItem *panelItem;
    QList<UCAction*> actions;
    QList<QObject*> data;
    QPointer<UCListItem> queuedItem;
    qreal optionSlotWidth;
    qreal offsetDragged;
    int optionsVisible;
    UCListItemActions::Status status;

    void _q_handlePanelDrag();
    void _q_handlePanelWidth();
    UCListItemActionsAttached *attachedObject();

    static bool connectToListItem(UCListItemActions *options, UCListItem *listItem, bool leading);
    static void disconnectFromListItem(UCListItemActions *options);
    static bool isConnectedTo(UCListItemActions *options, UCListItem *listItem);
    static void setDragging(UCListItemActions *actions, UCListItem *listItem, bool dragging);

    QQuickItem *createPanelItem();
};

#endif // UCLISTITEMACTIONS_P_H
