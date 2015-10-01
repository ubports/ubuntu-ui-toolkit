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
#include "uchaptics.h"
#include <QtQuick/private/qquickitem_p.h>
#include <QtQuick/private/qquickmousearea_p.h>
#include <QtQml/private/qqmlglobal_p.h>

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
    : UCActionItem(parent)
    , m_mouseArea(new QQuickMouseArea)
    , m_acceptEvents(true)
    , m_pressAndHoldConnected(false)
{
    setActiveFocusOnPress(true);
}

bool UCAbstractButton::isPressAndHoldConnected()
{
    IS_SIGNAL_CONNECTED(this, UCAbstractButton, pressAndHold, ());
}

void UCAbstractButton::classBegin()
{
    UCActionItem::classBegin();

    // make sure we have the haptics set up!
    HapticsProxy::instance().initialize();

    // set up mouse area
    QQml_setParent_noEvent(m_mouseArea, this);
    m_mouseArea->setParentItem(this);
    QQuickAnchors *anchors = QQuickItemPrivate::get(m_mouseArea)->anchors();
    anchors->setFill(this);
    m_mouseArea->setHoverEnabled(true);
}

void UCAbstractButton::componentComplete()
{
    UCActionItem::componentComplete();
    // connect to the right slot, using macros so we get the proper slot
    connect(this, SIGNAL(clicked()), this, SLOT(trigger()));

    // bind mouse area
    connect(m_mouseArea, &QQuickMouseArea::pressedChanged, this, &UCAbstractButton::pressedChanged);
    connect(m_mouseArea, &QQuickMouseArea::hoveredChanged, this, &UCAbstractButton::hoveredChanged);
    connect(m_mouseArea, SIGNAL(clicked(QQuickMouseEvent*)), this, SLOT(_q_mouseAreaClicked()));
    connect(m_mouseArea, SIGNAL(pressed(QQuickMouseEvent*)), this, SLOT(_q_mouseAreaPressed()));
}

// check the pressAndHold connection on runtime, as Connections
// may not be available on compoennt completion
void UCAbstractButton::_q_mouseAreaPressed()
{
    bool longPressConnected = isPressAndHoldConnected();
    if (longPressConnected && !m_pressAndHoldConnected) {
        // do not use UniqueConnection as that has huge impact on performance
        connect(m_mouseArea, SIGNAL(pressAndHold(QQuickMouseEvent*)),
                this, SLOT(_q_mouseAreaPressAndHold()));
        m_pressAndHoldConnected = true;
    } else if (!longPressConnected && m_pressAndHoldConnected) {
        disconnect(m_mouseArea, SIGNAL(pressAndHold(QQuickMouseEvent*)),
                   this, SLOT(_q_mouseAreaPressAndHold()));
        m_pressAndHoldConnected = false;
    }
}

// handle mouseClicked with Haptics
void UCAbstractButton::_q_mouseAreaClicked()
{
    // required by the deprecated ListItem module
    if (!m_acceptEvents) {
        return;
    }
    // play haptics
    HapticsProxy::instance().play(QVariant());
    Q_EMIT clicked();
}

// handle pressAndHold
void UCAbstractButton::_q_mouseAreaPressAndHold()
{
    // required by the deprecated ListItem module
    if (!m_acceptEvents) {
        return;
    }
    Q_EMIT pressAndHold();
}

// emit clicked when Enter/Return is pressed
void UCAbstractButton::keyPressEvent(QKeyEvent *event)
{
    UCActionItem::keyPressEvent(event);

    switch (event->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        // FIXME: space may also come here, however that depends on the button type
        // (i.e default Dialog btn) so we may need to add that to Button
        {
            Q_EMIT clicked();
            break;
        }
    }
}

/*!
 * \qmlproperty bool AbstractButton::pressed
 * True if the user presses a mouse button in the button's mouse area.
 */
bool UCAbstractButton::pressed() const
{
    return m_mouseArea ? m_mouseArea->pressed() : false;
}

/*!
 * \qmlproperty bool AbstractButton::hovered
 * True if the mouse cursor hovers over the button's mouse area.
 */
bool UCAbstractButton::hovered() const
{
    return m_mouseArea ? m_mouseArea->hovered() : false;
}

QQuickMouseArea *UCAbstractButton::privateMouseArea() const
{
    return m_mouseArea;
}
