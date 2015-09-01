/*
 * Copyright 2014-2015 Canonical Ltd.
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

class UCSwipeEvent : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QPointF to READ to)
    Q_PROPERTY(QPointF from READ from)
    Q_PROPERTY(QPointF content MEMBER m_contentPos)
    Q_PROPERTY(Status status READ status)
    Q_ENUMS(Status)
public:
    enum Status {
        Started,
        Updated,
        Finished
    };

    UCSwipeEvent(const QPointF &mousePos, const QPointF &lastPos, const QPointF &contentPos, Status status)
        : QObject(), m_mousePos(mousePos), m_lastPos(lastPos), m_contentPos(contentPos), m_status(status)
    {
    }

    QPointF to() const
    {
        return m_mousePos;
    }
    QPointF from() const
    {
        return m_lastPos;
    }
    Status status() const
    {
        return m_status;
    }

    QPointF m_mousePos;
    QPointF m_lastPos;
    QPointF m_contentPos;
    Status m_status;
};

class QQmlComponent;
class QQuickAbstractAnimation;
class QQuickPropertyAnimation;
class QQuickBehavior;
class UCListItem;
class QQuickFlickable;
class UCListItemStyle : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QQuickAbstractAnimation *snapAnimation MEMBER m_snapAnimation NOTIFY snapAnimationChanged)
    Q_PROPERTY(QQuickPropertyAnimation *dropAnimation MEMBER m_dropAnimation NOTIFY dropAnimationChanged)
    Q_PROPERTY(bool animatePanels READ animatePanels NOTIFY animatePanelsChanged)
    Q_PROPERTY(QQuickItem *dragPanel MEMBER m_dragPanel NOTIFY dragPanelChanged)
    Q_PROPERTY(int listItemIndex READ index NOTIFY listItemIndexChanged FINAL REVISION 1)
    Q_PROPERTY(QQuickFlickable *flickable READ flickable NOTIFY flickableChanged REVISION 1)
    Q_PROPERTY(bool completed READ completed NOTIFY completedChanged REVISION 1)
public:
    explicit UCListItemStyle(QQuickItem *parent = 0);

    void invokeSwipeEvent(UCSwipeEvent *event);
    void invokeRebound();
    bool animatePanels() const;
    void setAnimatePanels(bool animate);
    int index();
    QQuickFlickable *flickable();
    void updateFlickable(QQuickFlickable *flickable);
    bool completed();

Q_SIGNALS:
    void snapAnimationChanged();
    void dropAnimationChanged();
    void animatePanelsChanged();
    void dragPanelChanged();
    Q_REVISION(1) void listItemIndexChanged();
    Q_REVISION(1) void flickableChanged();
    Q_REVISION(1) void completedChanged();

public Q_SLOTS:
    void swipeEvent(UCSwipeEvent *event);
    void rebound();

protected:
    void classBegin();
    void componentComplete();

private:

    QMetaMethod m_swipeEvent;
    QMetaMethod m_rebound;
    UCListItem *m_listItem;
    QQuickAbstractAnimation *m_snapAnimation;
    QQuickPropertyAnimation *m_dropAnimation;
    QQuickItem *m_dragPanel;
    QQuickFlickable *m_flickable;
    bool m_animatePanels:1;

    friend class UCListItemPrivate;
    friend class ListItemDragArea;
    friend class ListItemDragHandler;
    friend class UCViewItemsAttachedPrivate;
};

#endif // UCLISTITEMSTYLE_H
