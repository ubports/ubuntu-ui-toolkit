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
#define LAYOUT_HMARGIN_GU               2
#define LAYOUT_VMARGIN_GU               0.5

class QQuickAbstractAnimation;
class QQuickBehavior;
class UCListItemSnapAnimator : public QObject
{
    Q_OBJECT
public:
    UCListItemSnapAnimator(QObject *parent = 0);
    ~UCListItemSnapAnimator();
    void init(UCListItem *listItem)
    {
        item = listItem;
    }

    bool snap(qreal to);
    void stop();

public Q_SLOTS:
    void snapOut();
    void snapIn();

    QQuickAbstractAnimation *getSnapBehavior();

private:
    bool active;
    UCListItem *item;
    QPointer<QQuickBehavior> behavior;
};

class QQuickFlickable;
class QQuickPropertyAnimation;
class UCListItemContent;
class UCListItemDivider;
class UCListItemActions;
class UCListItemSnapAnimator;
class UCListItemStyle;
class UCActionPanel;
class QQuickBehavior;
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
    void _q_rebound();
    void _q_relayout();
    void _q_removeLayoutMargin();
    void promptRebound();
    void _q_updateSize();
    void _q_updateIndex();
    int index();
    bool canHighlight(QMouseEvent *event);
    void setHighlighted(bool pressed);
    void setSwiped(bool tugged);
    void listenToRebind(bool listen);
    void lockContentItem(bool lock);
    void update();
    void clampAndMoveX(qreal &x, qreal dx);

    bool highlighted:1;
    bool contentMoved:1;
    bool swiped:1;
    bool suppressClick:1;
    bool ready:1;
    bool customColor:1;
    bool customOvershoot:1;
    bool flicked:1;
    QSet<QString> layoutMargins; // list of properties
    qreal xAxisMoveThresholdGU;
    qreal overshoot;
    QBasicTimer pressAndHoldTimer;
    QPointF lastPos;
    QPointF pressedPos;
    QColor color;
    QColor highlightColor;
    QPointer<QQuickItem> countOwner;
    QPointer<QQuickFlickable> flickable;
    QPointer<UCViewItemsAttached> parentAttached;
    QQuickItem *contentItem;
    UCListItemDivider *divider;
    UCListItemActions *leadingActions;
    UCListItemActions *trailingActions;
    UCActionPanel *leadingPanel;
    UCActionPanel *trailingPanel;
    UCListItemSnapAnimator animator;
    UCAction *defaultAction;

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
    bool contentMoving() const;
    void setContentMoving(bool moved);
    QQmlComponent *style() const;
    void setStyle(QQmlComponent *delegate);
    void resetStyle();
    void initStyleItem();
    QQuickItem *styleInstance() const;
    UCAction *action() const;
    void setAction(UCAction *action);
};

class UCListItemAttachedPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(UCListItemAttached)
public:
    UCListItemAttachedPrivate() : QObjectPrivate(), panel(0), listItem(0) {}

    static UCListItemAttachedPrivate* get(UCListItemAttached *that)
    {
        return that->d_func();
    }

    UCActionPanel *panel;
    UCListItem *listItem;
    QList<UCAction*> visibleActions;
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

class UCActionPanel : public QObject
{
    Q_OBJECT
public:
    ~UCActionPanel();
    static bool grabPanel(UCActionPanel **panel, UCListItem *item, bool leading);
    static void ungrabPanel(UCActionPanel *panel);
    static bool isConnected(UCActionPanel *panel);

    UCListItemActions *actions();
    QQuickItem *panel() const;
    UCListItem::PanelStatus panelStatus()
    {
        return status;
    }
    bool isLeading() const
    {
        return leading;
    }

Q_SIGNALS:
    void statusChanged();

private:
    UCActionPanel(UCListItem *item, bool leading);
    bool createPanel(QQmlComponent *panelDelegate);
    UCListItemAttached *attachedObject();

    UCListItem *listItem;
    QQuickItem *panelItem;
    UCListItem::PanelStatus status;
    bool leading:1;
    bool connected:1;
};

QColor getPaletteColor(const char *profile, const char *color);

QML_DECLARE_TYPE(UCListItemDivider)

#endif // UCVIEWITEM_P_H
