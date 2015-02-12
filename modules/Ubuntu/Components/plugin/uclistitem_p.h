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
class UCSelectionHandler;
class UCActionPanel;
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
    inline UCListItem *item()
    {
        return q_func();
    }

    bool isClickedConnected();
    bool isPressAndHoldConnected();
    void _q_updateThemedData();
    void _q_relayout();
    void _q_updateSwiping();
    void setSwiped(bool isSwiped);
    void _q_updateSize();
    void _q_updateIndex();
    void _q_contentMoving();
    int index();
    bool canHighlight(QMouseEvent *event);
    void setHighlighted(bool pressed);
    void listenToRebind(bool listen);
    void lockContentItem(bool lock);
    void update();
    void snapOut();
    void swipeEvent(const QPointF &localPos, UCSwipeEvent::Status status);

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
    QQuickItem *contentItem;
    UCListItemDivider *divider;
    UCListItemActions *leadingActions;
    UCListItemActions *trailingActions;
    UCSelectionHandler *selectionHandler;
    UCAction *mainAction;

    // FIXME move these to StyledItemBase togehther with subtheming.
    QQmlComponent *styleComponent;
    QQmlComponent *implicitStyleComponent;
    UCListItemStyle *styleItem;

    // getters/setters
    QQmlListProperty<QObject> data();
    QQmlListProperty<QQuickItem> children();
    bool contentMoving() const;
    void setContentMoving(bool moved);
    QQmlComponent *style() const;
    void setStyle(QQmlComponent *delegate);
    void resetStyle();
    void initStyleItem();
    QQuickItem *styleInstance() const;
    bool isSelected();
    void setSelected(bool value);
    bool isSelectable();
    void _q_initializeSelectionHandler();
    UCAction *action() const;
    void setAction(UCAction *action);
};

class PropertyChange;
class UCViewItemsAttachedPrivate
{
    Q_DECLARE_PUBLIC(UCViewItemsAttached)
public:
    UCViewItemsAttachedPrivate(UCViewItemsAttached *qq);
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

    UCViewItemsAttached *q_ptr;
    bool globalDisabled:1;
    bool selectable:1;
    QSet<int> selectedList;
    QList< QPointer<QQuickFlickable> > flickables;
    QList< PropertyChange* > changes;
    QPointer<UCListItem> boundItem;
    QPointer<UCListItem> disablerItem;
};

QColor getPaletteColor(const char *profile, const char *color);

class UCHandlerBase : public QObject
{
    Q_OBJECT
public:

    explicit UCHandlerBase(UCListItem *owner = 0);
    virtual void initialize(bool animated) = 0;

protected:
    UCListItem *listItem;
    QQuickItem *panel;

    void setupPanel(QQmlComponent *component, bool animate);
};

class UCSelectionHandler : public UCHandlerBase
{
    Q_OBJECT
public:
    explicit UCSelectionHandler(UCListItem *owner = 0);

    void initialize(bool animated);

public Q_SLOTS:
    void setupSelection(bool animated = true);
};

#endif // UCVIEWITEM_P_H
