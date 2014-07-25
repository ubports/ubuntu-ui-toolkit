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

#include "ucunits.h"
#include "uctheme.h"
#include "ucviewitem.h"
#include "ucviewitem_p.h"
#include "ucviewitembackground.h"
#include <QtQml/QQmlInfo>
#include <QtQuick/private/qquickitem_p.h>
#include <QtQuick/private/qquickflickable_p.h>
#include <QtQuick/private/qquickpositioners_p.h>

#include <QtQml/private/qqmlabstractbinding_p.h>
#define foreach Q_FOREACH //workaround to fix private includes
#include <QtQml/private/qqmlbinding_p.h>     // for QmlBinding
#undef foreach

/*
 * The function creates a property binding between \c target and \c src
 * objects' \c script using the given \c context if the target has no binding.
 * If the context is 0, the source's context will be used.
 */
void ucBindProperty(const QQmlProperty &target, const QString &script, QObject *src, QQmlContext *context)
{
    QQmlAbstractBinding *prevBinding = QQmlPropertyPrivate::binding(target);
    if (!prevBinding) {
        // create new binding
        if (!context) {
            context = qmlContext(src);
        }
        QQmlBinding *newBinding = new QQmlBinding(script, src, context);
        newBinding->setTarget(target);
        prevBinding = QQmlPropertyPrivate::setBinding(target, newBinding);
        if (prevBinding) {
            // destroy any previous binding
            prevBinding->destroy();
        }
    }
}

UCViewItemPrivate::UCViewItemPrivate(UCViewItem *qq)
    : q_ptr(qq)
    , background(new UCViewItemBackground)
    , pressed(false)
{
    background->setObjectName("ViewItemHolder");
    background->setParent(q_ptr);
    background->setParentItem(q_ptr);
    // content will be redirected to the background, therefore we must report
    // children changes as it would come from the main component
    QObject::connect(background, &UCViewItemBackground::childrenChanged,
                     q_ptr, &UCViewItem::childrenChanged);
}

void UCViewItemPrivate::_q_rebound()
{
    setPressed(false);
    // disconnect the flickable
    listenToRebind(false);
}

// set pressed flag and update background
void UCViewItemPrivate::setPressed(bool pressed)
{
    if (this->pressed != pressed) {
        this->pressed = pressed;
        background->update();
        Q_Q(UCViewItem);
        Q_EMIT q->pressedChanged();
    }
}

// connects/disconnects from the Flickable anchestor to get notified when to do rebound
void UCViewItemPrivate::listenToRebind(bool listen)
{
    if (flickable.isNull()) {
        return;
    }
    if (listen) {
        QObject::connect(flickable.data(), SIGNAL(movementStarted()), q_ptr, SLOT(_q_rebound()));
    } else {
        QObject::disconnect(flickable.data(), SIGNAL(movementStarted()), q_ptr, SLOT(_q_rebound()));
    }
}

/*!
 * \qmltype ViewItem
 * \instantiates UCViewItem
 * \inqmlmodule Ubuntu.Components 1.1
 * \ingroup ubuntu
 * \brief Standard component to hold contents placed in a view cell.
 */

UCViewItem::UCViewItem(QQuickItem *parent)
    : QQuickItem(parent)
    , d_ptr(new UCViewItemPrivate(this))
{
    setFlag(QQuickItem::ItemHasContents, true);
    setAcceptedMouseButtons(Qt::LeftButton);
}

UCViewItem::~UCViewItem()
{
}

// initialize height
void UCViewItem::classBegin()
{
    QQuickItem::classBegin();

    QQmlProperty heightProperty(this, "height", qmlContext(this));
    // use the root context as the units is a context property
    ucBindProperty(heightProperty, "units.gu(6)", &UCUnits::instance(), qmlEngine(this)->rootContext());
}

void UCViewItem::componentComplete()
{
    QQuickItem::componentComplete();

    // bind parent's width
    Q_D(UCViewItem);
    QQuickItem *item = d->flickable ? d->flickable : parentItem();
    QQmlProperty widthProperty(this, "width", qmlContext(this));
    ucBindProperty(widthProperty, "width", item);
}

void UCViewItem::itemChange(ItemChange change, const ItemChangeData &data)
{
    QQuickItem::itemChange(change, data);
    if (change == ItemParentHasChanged) {
        Q_D(UCViewItem);
        // make sure we are not connected to the previous Flickable
        if (!d->flickable.isNull()) {
            QObject::disconnect(d->flickable.data(), SIGNAL(movementStarted()), this, SLOT(_q_rebound()));
        }
        // check if we are in a positioner, and if that positioner is in a Flickable
        QQuickBasePositioner *positioner = qobject_cast<QQuickBasePositioner*>(data.item);
        if (positioner && positioner->parentItem()) {
            d->flickable = qobject_cast<QQuickFlickable*>(positioner->parentItem()->parentItem());
        } else if (data.item && data.item->parentItem()){
            // check if we are in a Flickable then
            d->flickable = qobject_cast<QQuickFlickable*>(data.item->parentItem());
        }
    }
}

void UCViewItem::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
    // resize background item
    Q_D(UCViewItem);
    QRectF rect(boundingRect());
    d->background->setSize(rect.size());
}
void UCViewItem::mousePressEvent(QMouseEvent *event)
{
    QQuickItem::mousePressEvent(event);
    Q_D(UCViewItem);
    if (!d->flickable.isNull() && d->flickable->isMoving()) {
        // while moving, we cannot select any items
        return;
    }
    d->setPressed(true);
    // connect the Flickable to know when to rebound
    if (!d->flickable.isNull()) {
        QObject::connect(d->flickable.data(), SIGNAL(movementStarted()), this, SLOT(_q_rebound()));
    }
    // accept the event so we get the rest of the events as well
    event->setAccepted(true);
}

void UCViewItem::mouseReleaseEvent(QMouseEvent *event)
{
    QQuickItem::mouseReleaseEvent(event);
    Q_D(UCViewItem);
    // set released
    if (d->pressed) {
        Q_EMIT clicked();
    }
    d->setPressed(false);
}

UCViewItemBackground* UCViewItem::background() const
{
    Q_D(const UCViewItem);
    return d->background;
}

bool UCViewItem::pressed() const
{
    Q_D(const UCViewItem);
    return d->pressed;
}

QQmlListProperty<QObject> UCViewItem::data()
{
    Q_D(UCViewItem);
    return QQuickItemPrivate::get(d->background)->data();
}

QQmlListProperty<QQuickItem> UCViewItem::children()
{
    Q_D(UCViewItem);
    return QQuickItemPrivate::get(d->background)->children();
}

#include "moc_ucviewitem.cpp"
