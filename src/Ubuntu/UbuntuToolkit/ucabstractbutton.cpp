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
 */

#include "ucabstractbutton.h"
#include "ucabstractbutton_p.h"
#include "uchaptics.h"
#include "ucunits.h"
#include "ucaction.h"
#include <QtQuick/private/qquickitem_p.h>
#include <QtQuick/private/qquickmousearea_p.h>
#include <QtQml/private/qqmlglobal_p.h>

#define MIN_SENSING_WIDTH_GU    4
#define MIN_SENSING_HEIGHT_GU   4

//#define SENSING_DEBUG

UT_NAMESPACE_BEGIN

UCAbstractButtonPrivate::UCAbstractButtonPrivate()
    : UCActionItemPrivate()
    , mouseArea(new QQuickMouseArea)
    , acceptEvents(true)
    , pressAndHoldConnected(false)
{
}
void UCAbstractButtonPrivate::init()
{
    Q_Q(UCAbstractButton);
    q->setActiveFocusOnPress(true);
    q->setActiveFocusOnTab(true);
}

/*!
    \qmltype AbstractButton
    \instantiates UbuntuToolkit::UCAbstractButton
    \inherits ActionItem
    \inqmlmodule Ubuntu.Components
    \ingroup ubuntu
    \brief The AbstractButton class defines the behavior of the button.

    This class defines the behavior of the button. All components deriving from
    this class support haptic feedback out of the box.

    If an action is specified, the button's clicked signal will trigger the action.
    Subclasses of AbstractButton can use other properties of action (for example
    the text and iconName).

    \section2 Sensing area
    It has been proven that, on touch devices in order to properly aim an active
    component a minimum of 8x8 millimeters (i.e. 4x4 grid units) area has to be
    provided. However not all the visuals are of that size, as Icons for example
    are defaulted to be 2x2 grid units, but a component containing a single Icon
    still has to be able to capture the press events. Therefore AbstractButton
    makes sure this rule of 4x4 grid units for the sensing area is provided. In
    addition it exposes the \l sensingMargins property which extends the component's
    sensing area in all the directions, so other use cases when the sensing area
    needs to be extended outside of the component's area, or restricted on a
    given area of the component can be implemented. The following example extends
    the sensing area on the left, top and bottom with 1 grid units, and on the
    right with 10 grid units.
    \qml
    AbstractButton {
        width: units.gu(2)
        height: units.gu(2)
        sensingMargins {
            left: units.gu(1)
            top: units.gu(1)
            bottom: units.gu(1)
            right: units.gu(10)
        }
    }
    \endqml
    \note Do not set clipping for the component as that will restrict the sensing
    area to be available on the visual area only.
*/

/*!
 *
 * \qmlsignal AbstractButton::clicked()
 * This handler is called when there is a mouse click on the button and the button
 * is not disabled. If \l {ActionItem::action}{action} is defined, the action will be triggered.
 */

/*!
 *
 * \qmlsignal AbstractButton::pressAndHold()
 * This handler is called when there is a long press.
 */

UCAbstractButton::UCAbstractButton(QQuickItem *parent)
    : UCActionItem(*(new UCAbstractButtonPrivate), parent)
{
    d_func()->init();
}
UCAbstractButton::UCAbstractButton(UCAbstractButtonPrivate &&dd, QQuickItem *parent)
    : UCActionItem(dd, parent)
{
    d_func()->init();
}

bool UCAbstractButtonPrivate::isPressAndHoldConnected()
{
    Q_Q(UCAbstractButton);
    IS_SIGNAL_CONNECTED(q, UCAbstractButton, pressAndHold, ());
}

void UCAbstractButtonPrivate::onClicked()
{
    Q_Q(UCAbstractButton);
    // call the overridden QML trigger function
    invokeTrigger<UCAbstractButton>(q, QVariant());
    // then emit the clicked signal
    Q_EMIT q->clicked();
}

void UCAbstractButton::classBegin()
{
    UCActionItem::classBegin();

    // set up mouse area
    Q_D(UCAbstractButton);
    QQml_setParent_noEvent(d->mouseArea, this);
    d->mouseArea->setParentItem(this);
    QQuickAnchors *anchors = QQuickItemPrivate::get(d->mouseArea)->anchors();
    anchors->setFill(this);
    d->mouseArea->setHoverEnabled(true);

#ifdef SENSING_DEBUG
    // keep this code for visual debugging purposes to track sensing area changes
    QQmlComponent component(qmlEngine(this));
    QString data = "import QtQuick 2.4; Rectangle { anchors.fill: parent; color: '#19b6ee'; opacity: 0.3 }";
    component.setData(data.toLocal8Bit(), QUrl());
    if (component.isError()) {
        qDebug() << "ERROR" << component.errorString();
    } else {
        QQuickItem *item = qobject_cast<QQuickItem*>(component.beginCreate(qmlContext(this)));
        if (item) {
            item->setParent(d->mouseArea);
            item->setParentItem(d->mouseArea);
            component.completeCreate();
        }
    }
#endif
}

void UCAbstractButtonPrivate::completeComponentInitialization()
{
    UCActionItemPrivate::completeComponentInitialization();
    Q_Q(UCAbstractButton);

    // adjust sensing area
    _q_adjustSensingArea();

    // bind mouse area
    QObject::connect(mouseArea, &QQuickMouseArea::pressedChanged, q, &UCAbstractButton::pressedChanged);
    QObject::connect(mouseArea, &QQuickMouseArea::hoveredChanged, q, &UCAbstractButton::hoveredChanged);
    QObject::connect(mouseArea, SIGNAL(clicked(QQuickMouseEvent*)), q, SLOT(_q_mouseAreaClicked()));
    QObject::connect(mouseArea, SIGNAL(pressed(QQuickMouseEvent*)), q, SLOT(_q_mouseAreaPressed()));
}

// check the pressAndHold connection on runtime, as Connections
// may not be available on compoennt completion
void UCAbstractButtonPrivate::_q_mouseAreaPressed()
{
    if (!mouseArea->pressed()) {
        return;
    }
    bool longPressConnected = isPressAndHoldConnected();
    Q_Q(UCAbstractButton);
    if (longPressConnected && !pressAndHoldConnected) {
        // do not use UniqueConnection as that has huge impact on performance
        QObject::connect(mouseArea, SIGNAL(pressAndHold(QQuickMouseEvent*)),
                q, SLOT(_q_mouseAreaPressAndHold()));
        pressAndHoldConnected = true;
    } else if (!longPressConnected && pressAndHoldConnected) {
        QObject::disconnect(mouseArea, SIGNAL(pressAndHold(QQuickMouseEvent*)),
                   q, SLOT(_q_mouseAreaPressAndHold()));
        pressAndHoldConnected = false;
    }
}

// handle mouseClicked with Haptics
void UCAbstractButtonPrivate::_q_mouseAreaClicked()
{
    // required by the deprecated ListItem module
    if (!acceptEvents) {
        return;
    }
    // play haptics
    HapticsProxy::instance()->play(QVariant());
    onClicked();
}

// handle pressAndHold
void UCAbstractButtonPrivate::_q_mouseAreaPressAndHold()
{
    // required by the deprecated ListItem module
    Q_Q(UCAbstractButton);
    if (!acceptEvents) {
        return;
    }
    Q_EMIT q->pressAndHold();
}

// emit clicked when Enter/Return/Space is pressed
void UCAbstractButton::keyReleaseEvent(QKeyEvent *event)
{
    UCActionItem::keyReleaseEvent(event);

    switch (event->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Space:
            event->accept();
            d_func()->onClicked();
            break;
        default:
            break;
    }
}

void UCAbstractButton::touchEvent(QTouchEvent *event)
{
    UCActionItem::touchEvent(event);

    Q_D(UCAbstractButton);
    switch (event->type()) {
    case QEvent::TouchBegin:
    {
        // send an event to MouseArea to handle the event
        QMouseEvent mouse(QEvent::MouseButtonPress, QPointF(0,0), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
        qApp->sendEvent(d->mouseArea, &mouse);
        event->accept();
        break;
    }
    case QEvent::TouchCancel:
    case QEvent::TouchEnd:
    {
        QMouseEvent mouse(QEvent::MouseButtonRelease, QPointF(0,0), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
        qApp->sendEvent(d->mouseArea, &mouse);
        event->accept();
        break;
    }
    default:
        break;
    }
}

void UCAbstractButtonPrivate::_q_adjustSensingArea()
{
    Q_Q(UCAbstractButton);
    if (!componentComplete) {
        // we do not hammer the component until completion
        return;
    }
    // use the sensingMargins in the minimum calculation
    qreal minimumWidth = UCUnits::instance()->gu(MIN_SENSING_WIDTH_GU);
    qreal minimumHeight = UCUnits::instance()->gu(MIN_SENSING_HEIGHT_GU);
    qreal hDelta = minimumWidth
            - (q->width() + (sensingMargins ? (sensingMargins->left() + sensingMargins->right()) : 0.0));
    qreal vDelta = minimumHeight
            - (q->height() + (sensingMargins ? (sensingMargins->top() + sensingMargins->bottom()) : 0.0));

    // adjust the sensing area
    qreal dx1, dy1, dx2, dy2;
    if (hDelta >= 0) {
        // the horizontal size is still smaller than the minimum
        dx1 = -(hDelta / 2 + (sensingMargins ? sensingMargins->left() : 0.0));
        dx2 = (hDelta / 2 + (sensingMargins ? sensingMargins->right() : 0.0));
    } else {
        dx1 = sensingMargins ? -sensingMargins->left() : 0.0;
        dx2 = sensingMargins ? sensingMargins->right() : 0.0;
    }
    if (vDelta >= 0) {
        // the vertical size is still smaller than the minimum
        dy1 = -(vDelta / 2 + (sensingMargins ? sensingMargins->top() : 0.0));
        dy2 = (vDelta / 2 + (sensingMargins ? sensingMargins->bottom() : 0.0));
    } else {
        dy1 = sensingMargins ? -sensingMargins->top() : 0.0;
        dy2 = sensingMargins ? sensingMargins->bottom() : 0.0;
    }
    sensingArea = q->boundingRect();
    sensingArea.adjust(dx1, dy1, dx2, dy2);
}

void UCAbstractButton::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    UCActionItem::geometryChanged(newGeometry, oldGeometry);

    // adjust internal mouse area's size
    d_func()->_q_adjustSensingArea();
}

/*!
 * \qmlproperty bool AbstractButton::pressed
 * True if the user presses a mouse button in the button's mouse area.
 */
bool UCAbstractButton::pressed() const
{
    Q_D(const UCAbstractButton);
    return d->mouseArea ? d->mouseArea->pressed() : false;
}

/*!
 * \qmlproperty bool AbstractButton::hovered
 * True if the mouse cursor hovers over the button's mouse area.
 */
bool UCAbstractButton::hovered() const
{
    Q_D(const UCAbstractButton);
    return d->mouseArea ? d->mouseArea->hovered() : false;
}

bool UCAbstractButton::acceptEvents() const
{
    Q_D(const UCAbstractButton);
    return d->acceptEvents;
}
void UCAbstractButton::setAcceptEvents(bool value)
{
    Q_D(UCAbstractButton);
    d->acceptEvents = value;
}

QQuickMouseArea *UCAbstractButton::privateMouseArea() const
{
    Q_D(const UCAbstractButton);
    return d->mouseArea;
}

/*!
 * \qmlpropertygroup ::AbstractButton::sensingMargins
 * \qmlproperty real AbstractButton::sensingMargins.left
 * \qmlproperty real AbstractButton::sensingMargins.right
 * \qmlproperty real AbstractButton::sensingMargins.top
 * \qmlproperty real AbstractButton::sensingMargins.bottom
 * \qmlproperty real AbstractButton::sensingMargins.all
 * The property group specifies the margins extending the visual area where the
 * touch events are sensed. Positive values mean the area will be extended
 * on the specified direction outside of the visual area, negative values mean
 * the sensing will fall under the component's visual border.
 * All values default to 0. This does not affect mouse sensing area, which only
 * covers the visual area of the component.
 *
 * \note If the visual area and the sensing margins are not reaching the 4x4 grid
 * units limit, the component will fall back to these minimum limits.
 * For example, extending a 2x2 grid unit visual component into 5x4 grid units
 * sensing area would look as follows:
 * \qml
 * AbstractButton {
 *     width: units.gu(2)
 *     height: units.gu(2)
 *     Icon {
 *         name: "settings"
 *     }
 *     sensingArea {
 *         // no need to set the vertical direction as the minimum of
 *         // 4 grid units will be taken automatically
 *         leftMargin: units.gu(1)
 *         // we only have to add 2 grid units as the width + left margin
 *         // already gives us 3 grid units out of 5
 *         rightMargin: units.gu(2)
 *     }
 * }
 * \endqml
 */
UCMargins *UCAbstractButton::sensingMargins()
{
    Q_D(UCAbstractButton);
    if (!d->sensingMargins) {
        d->sensingMargins = new UCMargins(this);

        // as this is the first time we create the sensing margins we only
        // connect now to grid unit changes to keep sensing area size in sync
        connect(UCUnits::instance(), SIGNAL(gridUnitChanged()), this, SLOT(_q_adjustSensingArea()));
        // also connect to the margin changes
        connect(d->sensingMargins, SIGNAL(leftChanged()), this, SLOT(_q_adjustSensingArea()));
        connect(d->sensingMargins, SIGNAL(rightChanged()), this, SLOT(_q_adjustSensingArea()));
        connect(d->sensingMargins, SIGNAL(topChanged()), this, SLOT(_q_adjustSensingArea()));
        connect(d->sensingMargins, SIGNAL(bottomChanged()), this, SLOT(_q_adjustSensingArea()));
        connect(d->sensingMargins, SIGNAL(allChanged()), this, SLOT(_q_adjustSensingArea()));
    }
    return d->sensingMargins;
}

bool UCAbstractButton::contains(const QPointF &point) const
{
    Q_D(const UCAbstractButton);
    return d->sensingArea.contains(point);
}

UT_NAMESPACE_END

#include "moc_ucabstractbutton.cpp"
