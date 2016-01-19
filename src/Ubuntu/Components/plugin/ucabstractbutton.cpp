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
#include "ucaction.h"
#include <QtQuick/private/qquickitem_p.h>
#include <QtQuick/private/qquickmousearea_p.h>
#include <QtQml/private/qqmlglobal_p.h>

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
    \instantiates UCAbstractButton
    \inqmlmodule Ubuntu.Components 1.1
    \ingroup ubuntu
    \brief The AbstractButton class defines the behavior of the button.

    This class defines the behavior of the button. All components deriving from
    this class support haptic feedback out of the box.

    If an action is specified, the button's clicked signal will trigger the action.
    Subclasses of AbstractButton can use other properties of action (for example
    the text and iconName).
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

    // make sure we have the haptics set up!
    HapticsProxy::instance().initialize();

    // set up mouse area
    Q_D(UCAbstractButton);
    QQml_setParent_noEvent(d->mouseArea, this);
    d->mouseArea->setParentItem(this);
    QQuickAnchors *anchors = QQuickItemPrivate::get(d->mouseArea)->anchors();
    anchors->setFill(this);
    d->mouseArea->setHoverEnabled(true);
}

void UCAbstractButtonPrivate::completeComponentInitialization()
{
    UCActionItemPrivate::completeComponentInitialization();
    Q_Q(UCAbstractButton);

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
    HapticsProxy::instance().play(QVariant());
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

// emit clicked when Enter/Return is pressed
void UCAbstractButton::keyReleaseEvent(QKeyEvent *event)
{
    UCActionItem::keyReleaseEvent(event);

    switch (event->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Space:
            d_func()->onClicked();
            break;
        default:
            break;
    }
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

#include "moc_ucabstractbutton.cpp"
