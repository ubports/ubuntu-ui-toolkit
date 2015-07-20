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

#ifndef UCVIEWITEM_P_H
#define UCVIEWITEM_P_H

#include "uclistitem.h"
#include "ucstyleditembase_p.h"
#include "uclistitemstyle.h"
#include <QtCore/QPointer>
#include <QtCore/QBasicTimer>
#include <QtQuick/private/qquickrectangle_p.h>

#define IMPLICIT_LISTITEM_WIDTH_GU      40
#define IMPLICIT_LISTITEM_HEIGHT_GU     7
#define DIVIDER_THICKNESS_DP            2
#define DEFAULT_SWIPE_THRESHOLD_GU      1.5

class QQuickFlickable;
class UCListItemDivider;
class UCListItemActions;
class UCListItemStyle;
class ListItemDragHandler;
class UCListItemPrivate : public UCStyledItemBasePrivate
{
    Q_DECLARE_PUBLIC(UCListItem)
public:
    UCListItemPrivate();
    virtual ~UCListItemPrivate();
    void init();

    static inline UCListItemPrivate *get(UCListItem *that)
    {
        Q_ASSERT(that);
        return that->d_func();
    }

    void _q_themeChanged();
    bool isClickedConnected();
    bool isPressAndHoldConnected();
    void _q_relayout();
    void _q_updateSwiping();
    void setSwiped(bool swiped);
    void _q_updateSize();
    void _q_updateIndex();
    void _q_contentMoving();
    void _q_syncSelectMode();
    void _q_syncDragMode();
    int index();
    bool canHighlight(QMouseEvent *event);
    void setHighlighted(bool pressed);
    void listenToRebind(bool listen);
    void lockContentItem(bool lock);
    void update();
    void snapOut();
    void swipeEvent(const QPointF &localPos, UCSwipeEvent::Status status);

    quint16 defaultThemeVersion;
    bool highlighted:1;
    bool contentMoved:1;
    bool swiped:1;
    bool suppressClick:1;
    bool ready:1;
    bool customColor:1;
    qreal xAxisMoveThresholdGU;
    QBasicTimer pressAndHoldTimer;
    QPointF lastPos;
    QPointF pressedPos;
    QPointF zeroPos;
    QColor color;
    QColor highlightColor;
    QPointer<QQuickItem> countOwner;
    QPointer<QQuickFlickable> flickable;
    QPointer<UCViewItemsAttached> parentAttached;
    QPointer<ListItemDragHandler> dragHandler;
    QQuickItem *contentItem;
    UCListItemDivider *divider;
    UCListItemActions *leadingActions;
    UCListItemActions *trailingActions;
    UCAction *mainAction;

    // getters/setters
    QQmlListProperty<QObject> data();
    QQmlListProperty<QQuickItem> children();
    bool contentMoving() const;
    void setContentMoving(bool moved);
    virtual void preStyleChanged();
    virtual bool loadStyleItem(bool animated = true);
    bool dragging();
    bool dragMode();
    void setDragMode(bool draggable);
    bool isSelected();
    void setSelected(bool value);
    bool selectMode();
    void setSelectMode(bool selectable);
    UCAction *action() const;
    void setAction(UCAction *action);

    virtual void postThemeChanged();
    inline UCListItemStyle *listItemStyle() const;
};
UCListItemStyle *UCListItemPrivate::listItemStyle() const
{
    return static_cast<UCListItemStyle*>(styleItem);
}

class PropertyChange;
class ListItemDragArea;
class UCViewItemsAttachedPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(UCViewItemsAttached)
public:
    UCViewItemsAttachedPrivate();
    ~UCViewItemsAttachedPrivate();

    static UCViewItemsAttachedPrivate *get(UCViewItemsAttached *item)
    {
        return item ? item->d_func() : 0;
    }

    void clearFlickablesList();
    void buildFlickablesList();
    void clearChangesList();
    void buildChangesList(const QVariant &newValue);
    bool addSelectedItem(UCListItem *item);
    bool removeSelectedItem(UCListItem *item);
    bool isItemSelected(UCListItem *item);
    void enterDragMode();
    void leaveDragMode();
    bool isDragUpdatedConnected();
    void updateSelectedIndices(int fromIndex, int toIndex);

    QQuickFlickable *listView;
    ListItemDragArea *dragArea;
    bool globalDisabled:1;
    bool selectable:1;
    bool draggable:1;
    bool ready:1;
    QSet<int> selectedList;
    QList< QPointer<QQuickFlickable> > flickables;
    QList< PropertyChange* > changes;
    QPointer<UCListItem> boundItem;
    QPointer<UCListItem> disablerItem;
};

#endif // UCVIEWITEM_P_H
