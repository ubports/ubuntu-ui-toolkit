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
    Q_PROPERTY(QQmlComponent *actionsDelegate MEMBER m_actionsDelegate NOTIFY actionsDelegateChanged)
    Q_PROPERTY(QQmlComponent *selectionDelegate MEMBER m_selectionDelegate NOTIFY selectionDelegateChanged)
    Q_PROPERTY(QQmlComponent *dragHandlerDelegate MEMBER m_dragHandlerDelegate NOTIFY dragHandlerDelegateChanged)
    Q_PROPERTY(QQuickPropertyAnimation *snapAnimation MEMBER m_snapAnimation NOTIFY snapAnimationChanged)
    Q_PROPERTY(qreal swipeOvershoot MEMBER m_swipeOvershoot NOTIFY swipeOvershootChanged)
public:
    explicit UCListItemStyle(QQuickItem *parent = 0);

Q_SIGNALS:
    void actionsDelegateChanged();
    void selectionDelegateChanged();
    void dragHandlerDelegateChanged();
    void snapAnimationChanged();
    void swipeOvershootChanged();

private:
    QQmlComponent *m_actionsDelegate;
    QQmlComponent *m_selectionDelegate;
    QQmlComponent *m_dragHandlerDelegate;
    QQuickPropertyAnimation *m_snapAnimation;
    qreal m_swipeOvershoot;

    friend class UCListItemPrivate;
    friend class UCListItemActionsPrivate;
    friend class UCListItemSnapAnimator;
};

#endif // UCLISTITEMSTYLE_H
