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

#ifndef UCLISTITEMACTIONS_H
#define UCLISTITEMACTIONS_H

#include <QtCore/QObject>
#include "uclistitem_p.h"

class QQmlComponent;
class UCAction;
class UCListItemActionsAttached;
class UCListItemActionsPrivate;
class UCListItemActions : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlComponent *delegate READ delegate WRITE setDelegate NOTIFY delegateChanged)
    Q_PROPERTY(QQmlListProperty<UCAction> actions READ actions CONSTANT)
    Q_PROPERTY(QQuickItem *panelItem READ panelItem NOTIFY panelItemChanged)
    Q_PROPERTY(QQmlListProperty<QObject> data READ data)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_CLASSINFO("DefaultProperty", "data")
    Q_ENUMS(Status)
public:
    enum Status {
        Disconnected,
        Leading,
        Trailing
    };

    explicit UCListItemActions(QObject *parent = 0);
    ~UCListItemActions();

    static UCListItemActionsAttached *qmlAttachedProperties(QObject *owner);

    QQmlComponent *delegate() const;
    void setDelegate(QQmlComponent *delegate);
    QQmlListProperty<UCAction> actions();
    QQuickItem *panelItem() const;
    QQmlListProperty<QObject> data();
    Status status() const;

Q_SIGNALS:
    void delegateChanged();
    void panelItemChanged();
    void statusChanged();

private:
    Q_DECLARE_PRIVATE(UCListItemActions)
    Q_PRIVATE_SLOT(d_func(), void _q_handlePanelDrag())
    Q_PRIVATE_SLOT(d_func(), void _q_handlePanelWidth())
};

class UCListItemActionsAttached : public QObject
{
    Q_OBJECT
    Q_PROPERTY(UCListItemActions *container MEMBER m_container NOTIFY containerChanged)
    Q_PROPERTY(UCListItem *listItem READ listItem NOTIFY listItemChanged)
    Q_PROPERTY(int listItemIndex READ listItemIndex NOTIFY listItemIndexChanged)
    Q_PROPERTY(qreal offsetVisible READ offsetVisible NOTIFY offsetVisibleChanged)
    Q_PROPERTY(UCListItemActions::Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(bool dragging MEMBER m_dragging NOTIFY draggingChanged)
public:
    UCListItemActionsAttached(QObject *parent = 0);
    ~UCListItemActionsAttached();
    void setList(UCListItemActions *list);

    UCListItem *listItem();
    int listItemIndex();
    void setDrag(bool value);
    qreal offsetVisible();
    UCListItemActions::Status status();

    UCListItemActions *container() const
    {
        return m_container;
    }

public Q_SLOTS:
    void snapToPosition(qreal position);

Q_SIGNALS:
    void containerChanged();
    void listItemChanged();
    void listItemIndexChanged();
    void offsetVisibleChanged();
    void statusChanged();
    void draggingChanged();

private:
    UCListItemActions *m_container;
    bool m_dragging;
    friend class UCListItemAction;
};

QML_DECLARE_TYPEINFO(UCListItemActions, QML_HAS_ATTACHED_PROPERTIES)

#endif // UCLISTITEMACTIONS_H
