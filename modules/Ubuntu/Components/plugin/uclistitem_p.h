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

#define MIN(x, y)           ((x < y) ? x : y)
#define MAX(x, y)           ((x > y) ? x : y)
#define CLAMP(v, min, max)  (min <= max) ? MAX(min, MIN(v, max)) : MAX(max, MIN(v, min))

#define IMPLICIT_LISTITEM_WIDTH_GU      40
#define IMPLICIT_LISTITEM_HEIGHT_GU     7
#define DIVIDER_THICKNESS_DP            2
#define DEFAULT_SWIPE_THRESHOLD_GU      1.5

class QQuickFlickable;
class UCListItemDivider;
class UCListItemActions;
class UCListItemStyle;
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

    bool isClickedConnected();
    bool isPressAndHoldConnected();
    void _q_updateThemedData();
    void _q_relayout();
    void _q_updateSwiping();
    void setSwiped(bool isSwiped);
    void _q_updateSize();
    void _q_updateIndex();
    int index();
    bool canHighlight(QMouseEvent *event);
    void setHighlighted(bool pressed);
    void listenToRebind(bool listen);
    void lockContentItem(bool lock);
    void update();
    void snapOut();
    void swipeEvent(const QPointF &localPos, UCSwipeEvent::Status status);

    bool highlighted:1;
    bool swiped:1;
    bool suppressClick:1;
    bool ready:1;
    bool customColor:1;
    bool customOvershoot:1;
    bool flicked:1;
    qreal xAxisMoveThresholdGU;
    qreal overshoot;
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
    UCAction *mainAction;

    // FIXME move these to StyledItemBase togehther with subtheming.
    QQmlComponent *styleComponent;
    QQmlComponent *implicitStyleComponent;
    UCListItemStyle *styleItem;

    // getters/setters
    qreal swipeOvershoot() const;
    void setSwipeOvershoot(qreal overshoot);
    void resetSwipeOvershoot();
    QQmlListProperty<QObject> data();
    QQmlListProperty<QQuickItem> children();
    QQmlComponent *style() const;
    void setStyle(QQmlComponent *delegate);
    void resetStyle();
    void initStyleItem();
    QQuickItem *styleInstance() const;
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

    void clearFlickablesList();
    void buildFlickablesList();
    void clearChangesList();
    void buildChangesList(const QVariant &newValue);

    UCViewItemsAttached *q_ptr;
    bool globalDisabled;
    QList< QPointer<QQuickFlickable> > flickables;
    QList< PropertyChange* > changes;
    QPointer<UCListItem> boundItem;
    QPointer<UCListItem> disablerItem;
};

QColor getPaletteColor(const char *profile, const char *color);

#endif // UCVIEWITEM_P_H
