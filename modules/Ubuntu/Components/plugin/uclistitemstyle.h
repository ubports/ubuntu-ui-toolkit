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
#ifndef UCLISTITEMSTYLE_H
#define UCLISTITEMSTYLE_H

#include <QtQuick/QQuickItem>

class QQmlComponent;
class QQuickPropertyAnimation;
class UCListItemStyle : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QQmlComponent *actionsDelegate MEMBER m_actionsDelegate WRITE setActionsDelegate NOTIFY actionsDelegateChanged)
    Q_PROPERTY(QQmlComponent *selectionDelegate MEMBER m_selectionDelegate WRITE setSelectionDelegate NOTIFY selectionDelegateChanged)
    Q_PROPERTY(QQmlComponent *dragHandlerDelegate MEMBER m_dragHandlerDelegate WRITE setDragHandlerDelegate NOTIFY dragHandlerDelegateChanged)
    Q_PROPERTY(QQuickPropertyAnimation *snapAnimation MEMBER m_snapAnimation WRITE setSnapAnimation NOTIFY snapAnimationChanged)
public:
    explicit UCListItemStyle(QQuickItem *parent = 0);

    void setActionsDelegate(QQmlComponent *delegate);
    void setSelectionDelegate(QQmlComponent *delegate);
    void setDragHandlerDelegate(QQmlComponent *delegate);
    void setSnapAnimation(QQuickPropertyAnimation *animation);

Q_SIGNALS:
    void actionsDelegateChanged();
    void selectionDelegateChanged();
    void dragHandlerDelegateChanged();
    void snapAnimationChanged();

private:
    QQmlComponent *m_actionsDelegate;
    QQmlComponent *m_selectionDelegate;
    QQmlComponent *m_dragHandlerDelegate;
    QQuickPropertyAnimation *m_snapAnimation;

    friend class UCListItemActionsPrivate;
    friend class UCListItemSnapAnimator;
};

#endif // UCLISTITEMSTYLE_H
