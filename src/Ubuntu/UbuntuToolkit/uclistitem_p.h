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
#define DIVIDER_THICKNESS_DP            1
#define DEFAULT_SWIPE_THRESHOLD_GU      1.5

class QQuickFlickable;

namespace UbuntuToolkit {

class UCListItemDivider;
class UCListItemActions;
class UCListItemStyle;
class ListItemDragHandler;
class ListItemSelection;
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
    void _q_syncDragMode();
    void _q_updateExpansion(const QList<int> &indices);
    int index();
    bool canHighlight();
    void setHighlighted(bool pressed);
    void listenToRebind(bool listen);
    void lockContentItem(bool lock);
    void update();
    void snapOut();
    void swipeEvent(const QPointF &localPos, UCSwipeEvent::Status status);
    bool swipedOverThreshold(const QPointF &mousePos, const QPointF relativePos);
    void handleLeftButtonPress(QMouseEvent *event);
    void handleLeftButtonRelease(QMouseEvent *event);
    bool sendMouseEvent(QQuickItem *item, QMouseEvent *event);
    bool shouldShowContextMenu(QMouseEvent *event);
    void _q_popoverClosed();
    void showContextMenu();

    QPointer<QQuickItem> countOwner;
    QPointer<QQuickFlickable> flickable;
    QPointer<UCViewItemsAttached> parentAttached;
    QPointer<ListItemDragHandler> dragHandler;
    QBasicTimer pressAndHoldTimer;
    QPointF lastPos;
    QPointF pressedPos;
    QPointF zeroPos;
    QColor color;
    QColor highlightColor;
    QQuickItem *contentItem;
    UCListItemDivider *divider;
    UCListItemActions *leadingActions;
    UCListItemActions *trailingActions;
    UCAction *mainAction;
    UCListItemExpansion *expansion;
    ListItemSelection *selection;
    qreal xAxisMoveThresholdGU;
    Qt::MouseButton button;
    bool highlighted:1;
    bool swipeEnabled:1;
    bool contentMoved:1;
    bool swiped:1;
    bool suppressClick:1;
    bool ready:1;
    bool customColor:1;
    bool listViewKeyNavigation:1;

    // getters/setters
    QQmlListProperty<QObject> data();
    QQmlListProperty<QQuickItem> children();
    bool contentMoving() const;
    void setContentMoving(bool moved);
    void preStyleChanged() override;
    bool loadStyleItem(bool animated = true) override;
    bool dragging();
    bool dragMode();
    void setDragMode(bool draggable);
    bool isSelected();
    void setSelected(bool value);
    bool selectMode();
    void setSelectMode(bool selectable);
    UCAction *action() const;
    void setAction(UCAction *action);
    void setListViewKeyNavigation(bool value);

    virtual void postThemeChanged();
    inline UCListItemStyle *listItemStyle() const;
};
UCListItemStyle *UCListItemPrivate::listItemStyle() const
{
    return static_cast<UCListItemStyle*>(styleItem);
}

class PropertyChange;
class ListItemDragArea;
class ListViewProxy;
class UCViewItemsAttachedPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(UCViewItemsAttached)
public:
    UCViewItemsAttachedPrivate();
    ~UCViewItemsAttachedPrivate();
    void init();

    static UCViewItemsAttachedPrivate *get(UCViewItemsAttached *item)
    {
        return item ? item->d_func() : 0;
    }

    void clearFlickablesList();
    void buildFlickablesList();
    bool addSelectedItem(UCListItem *item);
    bool removeSelectedItem(UCListItem *item);
    bool isItemSelected(UCListItem *item);
    void enterDragMode();
    void leaveDragMode();
    bool isDragUpdatedConnected();
    void updateSelectedIndices(int fromIndex, int toIndex);

    // expansion
    void expand(int index, UCListItem *listItem, bool emitChangeSignal = true);
    void collapse(int index, bool emitChangeSignal = true);
    void collapseAll();
    void toggleExpansionFlags(bool enable);

    QSet<int> selectedList;
    QMap<int, QPointer<UCListItem> > expansionList;
    QList< QPointer<QQuickFlickable> > flickables;
    QPointer<UCListItem> boundItem;
    ListViewProxy *listView;
    ListItemDragArea *dragArea;
    UCViewItemsAttached::ExpansionFlags expansionFlags;
    bool selectable:1;
    bool draggable:1;
    bool ready:1;
};

UT_NAMESPACE_END

#endif // UCVIEWITEM_P_H
