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
#include <QtQuick/private/qquickrectangle_p.h>

class QQuickFlickable;
class QQuickPropertyAnimation;
class UCListItemContent;
class UCListItemDivider;
class UCListItemActions;
class UCListItemSnapAnimator;
class UCListItemStyle;
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
    void _q_updateThemedData();
    void _q_rebound();
    void promptRebound();
    void _q_updateSize();
    void _q_updateIndex();
    int index();
    bool canHighlight(QMouseEvent *event);
    void setHighlighted(bool pressed);
    void setSwiped(bool tugged);
    bool grabPanel(UCListItemActions *optionList, bool isTugged);
    void listenToRebind(bool listen);
    void resize();
    void update();
    void clampAndMoveX(qreal &x, qreal dx);

    bool highlighted:1;
    bool contentMoved:1;
    bool highlightColorChanged:1;
    bool swiped:1;
    bool suppressClick:1;
    bool ready:1;
    bool customStyle:1;
    bool customColor:1;
    bool customOvershoot:1;
    bool flicked:1;
    qreal xAxisMoveThresholdGU;
    qreal overshoot;
    QPointF lastPos;
    QPointF pressedPos;
    QColor color;
    QColor highlightColor;
    QPointer<QQuickItem> countOwner;
    QPointer<QQuickFlickable> flickable;
    QPointer<UCListItemAttached> attachedProperties;
    QQuickItem *contentItem;
    UCListItemDivider *divider;
    UCListItemActions *leadingActions;
    UCListItemActions *trailingActions;
    UCListItemSnapAnimator *animator;

    // FIXME move these to StyledItemBase togehther with subtheming.
    QQmlComponent *styleComponent;
    UCListItemStyle *styleItem;

    // getters/setters
    qreal swipeOvershoot() const;
    void setSwipeOvershoot(qreal overshoot);
    QQmlListProperty<QObject> data();
    QQmlListProperty<QQuickItem> children();
    bool contentMoving() const;
    void setContentMoving(bool moved);
    QQmlComponent *style() const;
    void setStyle(QQmlComponent *delegate);
    bool loadStyle(bool reload);
    void initStyleItem();
    QQuickItem *styleInstance() const;
};

class PropertyChange;
class UCListItemAttachedPrivate
{
    Q_DECLARE_PUBLIC(UCListItemAttached)
public:
    UCListItemAttachedPrivate(UCListItemAttached *qq);
    ~UCListItemAttachedPrivate();

    void clearFlickablesList();
    void buildFlickablesList();
    void clearChangesList();
    void buildChangesList(const QVariant &newValue);

    UCListItemAttached *q_ptr;
    bool globalDisabled;
    QList< QPointer<QQuickFlickable> > flickables;
    QList< PropertyChange* > changes;
    QPointer<UCListItem> boundItem;
    QPointer<UCListItem> disablerItem;
};

class UCListItemDivider : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool visible MEMBER m_visible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(qreal leftMargin MEMBER m_leftMargin WRITE setLeftMargin NOTIFY leftMarginChanged)
    Q_PROPERTY(qreal rightMargin MEMBER m_rightMargin WRITE setRightMargin NOTIFY rightMarginChanged)
    Q_PROPERTY(QColor colorFrom MEMBER m_colorFrom WRITE setColorFrom NOTIFY colorFromChanged)
    Q_PROPERTY(QColor colorTo MEMBER m_colorTo WRITE setColorTo NOTIFY colorToChanged)
public:
    explicit UCListItemDivider(QObject *parent = 0);
    ~UCListItemDivider();
    void init(UCListItem *listItem);

Q_SIGNALS:
    void visibleChanged();
    void leftMarginChanged();
    void rightMarginChanged();
    void colorFromChanged();
    void colorToChanged();

protected:
    QSGNode *paint(QSGNode *node, const QRectF &rect);

private Q_SLOTS:
    void unitsChanged();
    void paletteChanged();

private:
    void updateGradient();
    void setVisible(bool visible);
    void setLeftMargin(qreal leftMargin);
    void setRightMargin(qreal rightMargin);
    void setColorFrom(const QColor &color);
    void setColorTo(const QColor &color);

    bool m_visible:1;
    bool m_leftMarginChanged:1;
    bool m_rightMarginChanged:1;
    bool m_colorFromChanged:1;
    bool m_colorToChanged:1;
    qreal m_thickness;
    qreal m_leftMargin;
    qreal m_rightMargin;
    QColor m_colorFrom;
    QColor m_colorTo;
    QGradientStops m_gradient;
    UCListItemPrivate *m_listItem;
    friend class UCListItem;
    friend class UCListItemPrivate;
};

QColor getPaletteColor(const char *profile, const char *color);

QML_DECLARE_TYPE(UCListItemDivider)

class QQuickPropertyAnimation;
class UCListItemSnapAnimator : public QObject
{
    Q_OBJECT
public:
    UCListItemSnapAnimator(UCListItem *item);
    ~UCListItemSnapAnimator();

    bool snap(qreal to);
    void complete();

public Q_SLOTS:
    void snapOut();
    void snapIn();

    QQuickPropertyAnimation *getDefaultAnimation();

private:
    bool active;
    UCListItem *item;
};

#endif // UCVIEWITEM_P_H
