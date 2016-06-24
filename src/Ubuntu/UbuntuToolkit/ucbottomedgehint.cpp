/*
 * Copyright 2015 Canonical Ltd.
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
 *
 * Authors: Zsombor Egri <zsombor.egri@canonical.com>
 */

#include "ucbottomedgehint.h"
#include "ucbottomedgehint_p.h"
#include "ucstyleditembase_p.h"
#include "quickutils.h"
#include "ucunits.h"
#include "ucaction.h"
#include "private/ucswipearea_p.h"
#include <propertychange.h>
#include <QtQml/private/qqmlproperty_p.h>
#include <QtQuick/private/qquickflickable_p.h>

#define SWIPE_AREA_HEIGHT_GU    3

UG_USE_NAMESPACE
UT_NAMESPACE_BEGIN

UCBottomEdgeHintPrivate::UCBottomEdgeHintPrivate()
    : UCActionItemPrivate()
    , swipeArea(new UCSwipeArea)
    , flickable(Q_NULLPTR)
    , flickableBottomMargin(Q_NULLPTR)
    , deactivateTimeout(800)
    // FIXME: we need QInputDeviceInfo to be complete with the locked!!
    // https://bugs.launchpad.net/ubuntu/+source/ubuntu-ui-toolkit/+bug/1276808
    , status(QuickUtils::instance()->mouseAttached() ? UCBottomEdgeHint::Locked : UCBottomEdgeHint::Inactive)
    , pressed(false)
{
}

void UCBottomEdgeHintPrivate::init()
{
    Q_Q(UCBottomEdgeHint);
    QObject::connect(q, &UCBottomEdgeHint::clicked, [=]() {
        // make sure the overloaded trigger is called!
        invokeTrigger<UCBottomEdgeHint>(q, QVariant());
    });
    /*
     * we cannot use setStyleName as that will trigger style loading
     * and the qmlEngine is not known at this phase of the of the initialization
     * Therefore we simply set the style name default. Style loading will
     * happen during component completion.
     */
    styleDocument = "BottomEdgeHintStyle";

    // connect old stateChanged
    QObject::connect(q, &QQuickItem::stateChanged, q, &UCBottomEdgeHint::stateChanged);

    // FIXME: use QInputDeviceInfo once available
    // https://bugs.launchpad.net/ubuntu/+source/ubuntu-ui-toolkit/+bug/1276808
    QObject::connect(QuickUtils::instance(), &QuickUtils::mouseAttachedChanged, q, &UCBottomEdgeHint::onMouseAttached);

    // accept mouse events
    q->setAcceptedMouseButtons(Qt::LeftButton);
}

/*!
    \qmltype BottomEdgeHint
    \inqmlmodule Ubuntu.Components
    \ingroup ubuntu
    \inherits ActionItem
    \brief The BottomEdgeHint shows the availability of extra features
    available from the bottom edge of the application.

    It displays a label and/or an icon at the bottom of the component it is
    attached to.

    When used with a mouse it acts like a button. The typical action associated
    with clicking on it should be revealing the extra features provided by the
    bottom edge.

    Example:
    \qml
    BottomEdgeHint {
        id: bottomEdgeHint
        text: i18n.tr("Favorites")
        onClicked: revealBottomEdge()
    }
    \endqml

    The component is styled through \b BottomEdgeHintStyle.
*/
UCBottomEdgeHint::UCBottomEdgeHint(QQuickItem *parent)
    : UCActionItem(*(new UCBottomEdgeHintPrivate), parent)
{
    d_func()->init();
}

UCBottomEdgeHint::UCBottomEdgeHint(UCBottomEdgeHintPrivate &&dd, QQuickItem *parent)
    : UCActionItem(dd, parent)
{
    d_func()->init();
}

void UCBottomEdgeHint::classBegin()
{
    UCActionItem::classBegin();
    init();
}

void UCBottomEdgeHint::init()
{
    Q_D(UCBottomEdgeHint);
    QQml_setParent_noEvent(d->swipeArea, this);
    d->swipeArea->setParentItem(this);

    // set context
    QQmlEngine::setContextForObject(d->swipeArea, qmlContext(this));

    // initialize swipe area size
    QQuickAnchors *anchors = QQuickItemPrivate::get(d->swipeArea)->anchors();
    QQuickItemPrivate *thisPrivate = QQuickItemPrivate::get(this);
    anchors->setLeft(thisPrivate->left());
    anchors->setBottom(thisPrivate->bottom());
    anchors->setRight(thisPrivate->right());
    d->swipeArea->setImplicitHeight(UCUnits::instance()->gu(SWIPE_AREA_HEIGHT_GU));

    // direction
    d->swipeArea->setDirection(UCSwipeArea::Upwards);

    // grid unit sync
    connect(UCUnits::instance(), &UCUnits::gridUnitChanged, this, &UCBottomEdgeHint::onGridUnitChanged);

    // connect to gesture detection
    connect(d->swipeArea, &UCSwipeArea::draggingChanged,
            this, &UCBottomEdgeHint::onDraggingChanged, Qt::DirectConnection);
}

void UCBottomEdgeHint::onMouseAttached()
{
    Q_D(UCBottomEdgeHint);
    setStatus(QuickUtils::instance()->mouseAttached() ? Locked : Active);
    if (d->status == Active) {
        d->deactivationTimer.start(d->deactivateTimeout, this);
        if (d->flickableBottomMargin) {
            delete d->flickableBottomMargin;
            d->flickableBottomMargin = Q_NULLPTR;
        }
    } else if (d->flickable) {
        adjustFlickableBottomMargin();
    }
}

void UCBottomEdgeHint::adjustFlickableBottomMargin()
{
    Q_D(UCBottomEdgeHint);
    if (!d->flickableBottomMargin) {
        d->flickableBottomMargin = new PropertyChange(d->flickable, "bottomMargin");
    }
    PropertyChange::setValue(d->flickableBottomMargin, height());
    d->flickable->setContentY(d->flickable->contentY() + height());
}

void UCBottomEdgeHint::onGridUnitChanged()
{
    Q_D(UCBottomEdgeHint);
    d->swipeArea->setImplicitHeight(UCUnits::instance()->gu(SWIPE_AREA_HEIGHT_GU));
}

void UCBottomEdgeHint::itemChange(ItemChange change, const ItemChangeData &data)
{
    UCActionItem::itemChange(change, data);
    if (change == ItemParentHasChanged) {
        QQmlProperty bottomAnchors(this, "anchors.bottom", qmlContext(this));
        if (data.item && !QQmlPropertyPrivate::binding(bottomAnchors)) {
            QQuickAnchors *anchors = QQuickItemPrivate::get(this)->anchors();
            anchors->setBottom(QQuickItemPrivate::get(data.item)->bottom());
        }
    }
}

void UCBottomEdgeHint::timerEvent(QTimerEvent *event)
{
    Q_D(UCBottomEdgeHint);
    UCActionItem::timerEvent(event);
    if (event->timerId() == d->deactivationTimer.timerId()) {
        setStatus(Inactive);
        d->deactivationTimer.stop();
    }
}

// handle clicked event when locked and enter or return is pressed
void UCBottomEdgeHint::keyPressEvent(QKeyEvent *event)
{
    UCActionItem::keyPressEvent(event);
    if (status() == Hidden)
        return;
    switch (event->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Space:
            Q_EMIT clicked();
            break;
        default:
            break;
    }
}

// handle click event
void UCBottomEdgeHint::mousePressEvent(QMouseEvent *event)
{
    Q_D(UCBottomEdgeHint);
    if (contains(event->localPos()) && (d->status >= Active)) {
        d->pressed = true;
        event->accept();
        // also call requestFocus
        requestFocus(Qt::MouseFocusReason);
    } else {
        UCActionItem::mousePressEvent(event);
    }
}
void UCBottomEdgeHint::mouseReleaseEvent(QMouseEvent *event)
{
    Q_D(UCBottomEdgeHint);
    if (d->pressed && (d->status >= Active) && contains(event->localPos())) {
        Q_EMIT clicked();
        event->accept();
    } else {
        UCActionItem::mouseReleaseEvent(event);
    }
}

// watch gesture detection status changes
void UCBottomEdgeHint::onDraggingChanged(bool dragging)
{
    Q_D(UCBottomEdgeHint);
    if (dragging) {
        d->deactivationTimer.stop();
        setStatus(Active);
    } else if (d->status == Active) {
        d->deactivationTimer.start(d->deactivateTimeout, this);
    }
}

/*!
  \qmlsignal void BottomEdgeHint::clicked()
   This handler is called when there is a mouse click on the BottomEdgeHint
   and the BottomEdgeHint is not disabled.
*/

/*!
  \qmlproperty string BottomEdgeHint::text
  The label displayed by the BottomEdgeHint.
 */

/*!
  \qmlproperty url BottomEdgeHint::iconSource
  The icon displayed by the BottomEdgeHint.

  This is the URL of any image file.
  If both iconSource and \l iconName are defined, \l iconName will be ignored.
 */

/*!
  \qmlproperty string BottomEdgeHint::iconName
  The icon associated with the BottomEdgeHint in the icon theme.

  If both \l iconSource and iconName are defined, iconName will be ignored.
 */

/*!
  \qmlproperty Flickable BottomEdgeHint::flickable
  The property holds the flickable, which when flicked hides the hint.
  \e Hidden state is reached when this property is set to a Flickable
  which is flicking or moving. It is recommended to set the property
  when the hint is placed above a flickable content. Defaults to null.
  */
QQuickFlickable *UCBottomEdgeHint::flickable() const
{
    Q_D(const UCBottomEdgeHint);
    return d->flickable;
}
void UCBottomEdgeHint::setFlickable(QQuickFlickable *flickable)
{
    Q_D(UCBottomEdgeHint);
    if (flickable == d->flickable) {
        return;
    }
    if (d->flickable) {
        disconnect(d->flickable, &QQuickFlickable::flickingChanged,
                   this, &UCBottomEdgeHint::handleFlickableActivation);
        disconnect(d->flickable, &QQuickFlickable::movingChanged,
                   this, &UCBottomEdgeHint::handleFlickableActivation);
    }
    d->flickable = flickable;
    if (d->flickable) {
        connect(d->flickable, &QQuickFlickable::flickingChanged,
                this, &UCBottomEdgeHint::handleFlickableActivation, Qt::DirectConnection);
        connect(d->flickable, &QQuickFlickable::movingChanged,
                this, &UCBottomEdgeHint::handleFlickableActivation, Qt::DirectConnection);
    }
    Q_EMIT flickableChanged();
}

// flickable moves hide the hint only if the current status is not Locked
void UCBottomEdgeHint::handleFlickableActivation()
{
    Q_D(UCBottomEdgeHint);
    if (d->status < Locked && !d->swipeArea->dragging() && !d->deactivationTimer.isActive()) {
        bool moving = d->flickable->isFlicking() || d->flickable->isMoving();
        if (moving) {
            setStatus(Hidden);
        } else if (d->status == Hidden) {
            setStatus(Inactive);
        }
    }
}

/*!
  \qmlproperty string BottomEdgeHint::state
  \deprecated
  BottomEdgeHint can take 2 states of visibility: \e Hidden, \e Visible.
  \table
  \header
    \li State
    \li Description
  \row
    \li Hidden
    \li The hint is not shown at all and cannot be activated.
  \row
    \li Visible
    \li The hint is in a state where it is visible but not active. \l clicked
        signal is not emitted.
  \endtable

  Defaults to \e Visible.
 */
QString UCBottomEdgeHint::state() const
{
    return QQuickItem::state();
}
void UCBottomEdgeHint::setState(const QString &state)
{
    QQuickItem::setState(state);
    UC_QML_DEPRECATION_WARNING("Overloaded 'state' property deprecated, will be removed from 1.3 release. Use 'status' instead.");

    Q_D(UCBottomEdgeHint);
    QQuickItem *style = d->styleItem;
    if (!style) {
        return;
    }
    if (state == "Hidden") {
        setStatus(Hidden);
    }
    if (state == "Visible") {
        setStatus(Inactive);
    }
}

/*!
  \qmlproperty Status BottomEdgeHint::status
  The property represents the status of the hint. The property is writable so it
  can be set to any of the following values programatically:
  \table
  \header
    \li Status
    \li Description
  \row
    \li Hidden
    \li The hint is not shown. Equivalent with setting \e visible to \c false,
        however visuals may do animations when altering this property. It can
        only be set if the current status is not \e Locked.
  \row
    \li Inactive
    \li The hint is shown and inactive. Styles can represent this state with
        different visuals. When inactive, \l clicked signal cannot be emitted.
  \row
    \li Active
    \li The hint is shown and active, meaning \l clicked signal is emitted when
        clicked with mouse.
  \row
    \li Locked
    \li Similar to \e Active the hint is shown and active, but no automatic transition
        to any other state is allowed. This is relevant for style implementations.
  \endtable
  \note \e Locked status value is set automatically when the system detects a
  mouse attached. In this case any change into other state value than \e Locked
  is rejected.
  Defaults to
  \list
    \li Inactive if no mouse is attached or
    \li Locked if there is a mouse detected.
  \endlist
  */
UCBottomEdgeHint::Status UCBottomEdgeHint::status()
{
    // FIXME: we won't need this once we get the QInputDeviceInfo reporting mouse attach/detach
    // https://bugs.launchpad.net/ubuntu/+source/ubuntu-ui-toolkit/+bug/1276808
    Q_D(UCBottomEdgeHint);
    if (QuickUtils::instance()->mouseAttached()) {
        d->status = Locked;
    }
    return d->status;
}

void UCBottomEdgeHint::setStatus(Status status)
{
    // FIXME: we need QInputDeviceInfo to complete this!
    // https://bugs.launchpad.net/ubuntu/+source/ubuntu-ui-toolkit/+bug/1276808
    // cannot unlock if mouse is attached or we don't have touch screen available
    Q_D(UCBottomEdgeHint);
    if (status == d->status || (status != Locked && QuickUtils::instance()->mouseAttached())) {
        return;
    }
    // if the previous state was Locked and the new one is Active, start deactivation timer
    if (d->status == Locked && status == Active && !d->deactivationTimer.isActive()) {
        d->deactivationTimer.start(d->deactivateTimeout, this);
    } else if (status != Active && d->deactivationTimer.isActive()) {
        // make sure we stop the deactivation timer if Inactive or Locked
        d->deactivationTimer.stop();
    }
    d->status = status;
    Q_EMIT statusChanged();
}

/*!
 * \qmlproperty int BottomEdgeHint::deactivateTimeout
 * The property specifies the timeout interval in milliseconds the \l status
 * is set to \e Inactive after a gesture based activation. Gesture based activation
 * is only possible when mouse is not attached to the device. Defaults to 800
 * milliseconds.
 */
int UCBottomEdgeHint::deactivateTimeout() const
{
    Q_D(const UCBottomEdgeHint);
    return d->deactivateTimeout;
}
void UCBottomEdgeHint::setDeactivateTimeout(int timeout)
{
    Q_D(UCBottomEdgeHint);
    if (timeout == d->deactivateTimeout || timeout < 0) {
        return;
    }
    d->deactivateTimeout = timeout;
    if (d->deactivationTimer.isActive()) {
        d->deactivationTimer.stop();
        d->deactivationTimer.start(d->deactivateTimeout, this);
    }
    Q_EMIT deactivateTimeoutChanged();
}

/*!
 * \qmlproperty SwipeArea BottomEdgeHint::swipeArea
 * \readonly
 * The property specifies the SwipeArea attached to the component driving its
 * behavior.
 */
UG_PREPEND_NAMESPACE(UCSwipeArea) *UCBottomEdgeHint::swipeArea() const
{
    return d_func()->swipeArea;
}

UT_NAMESPACE_END

#include "moc_ucbottomedgehint.cpp"
