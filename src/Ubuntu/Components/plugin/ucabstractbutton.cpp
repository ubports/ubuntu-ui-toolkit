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
#include "ucunits.h"
#include <QtQuick/private/qquickitem_p.h>
#include <QtQuick/private/qquickmousearea_p.h>
#include <QtQml/private/qqmlglobal_p.h>

#define MIN_SENSING_WIDTH_GU    4
#define MIN_SENSING_HEIGHT_GU   4

/*!
    \qmltype AbstractButton
    \instantiates UCAbstractButton
    \inherits ActionItem
    \inqmlmodule Ubuntu.Components 1.1
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
    addition exposes the \l sensingMargins property which extends the component's
    sensing area in al the directions, so other use cases when the sensing area
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
    : UCActionItem(parent)
    , m_mouseArea(new QQuickMouseArea)
    , m_acceptEvents(true)
    , m_pressAndHoldConnected(false)
{
    QQml_setParent_noEvent(m_mouseArea, this);
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
    m_mouseArea->setParentItem(this);
    QQuickAnchors *anchors = QQuickItemPrivate::get(m_mouseArea)->anchors();
    anchors->setFill(this);
    m_mouseArea->setHoverEnabled(true);
}

void UCAbstractButton::componentComplete()
{
    UCActionItem::componentComplete();
    // connect to the right slot, using macros so we get the proper slot
    connect(m_mouseArea, SIGNAL(clicked(QQuickMouseEvent*)), this, SLOT(trigger()));

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

void UCAbstractButton::adjustSensingArea(const QSizeF &size)
{
    // use the sensingMargins in the minimum calculation
    qreal hDelta = UCUnits::instance().gu(MIN_SENSING_WIDTH_GU)
            - (size.width() + m_sensingMargins.m_left + m_sensingMargins.m_right);
    qreal vDelta = UCUnits::instance().gu(MIN_SENSING_HEIGHT_GU)
            - (size.height() + m_sensingMargins.m_top + m_sensingMargins.m_bottom);

    // adjust the sensing area
    QQuickAnchors *mouseAreaAnchors = QQuickItemPrivate::get(m_mouseArea)->anchors();
    if (hDelta > 0) {
        // the horizontal size is still smaller than the minimum
        mouseAreaAnchors->setLeftMargin(hDelta / 2);
        mouseAreaAnchors->setRightMargin(hDelta / 2);
    } else {
        mouseAreaAnchors->setLeftMargin(m_sensingMargins.m_left);
        mouseAreaAnchors->setRightMargin(m_sensingMargins.m_right);
    }
    if (vDelta) {
        // the vertical size is still smaller than the minimum
        mouseAreaAnchors->setTopMargin(vDelta / 2);
        mouseAreaAnchors->setBottomMargin(vDelta / 2);
    } else {
        mouseAreaAnchors->setTopMargin(m_sensingMargins.m_left);
        mouseAreaAnchors->setBottomMargin(m_sensingMargins.m_right);
    }
}

void UCAbstractButton::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    UCActionItem::geometryChanged(newGeometry, oldGeometry);

    // adjust internal mouse area's size
    adjustSensingArea(newGeometry.size());
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

/*!
 * \qmlpropertygroup ::AbstractButton::sensingMargins
 * \qmlproperty real AbstractButton::sensingMargins.left
 * \qmlproperty real AbstractButton::sensingMargins.right
 * \qmlproperty real AbstractButton::sensingMargins.top
 * \qmlproperty real AbstractButton::sensingMargins.bottom
 * The property group specifies the margins extending the visual area where the
 * touch and mouse events are sensed. Positive values mean the area will be extended
 * on the specified direction outside of the visual area, negative values mean
 * the sensing will fall under the component's visual border.
 * All values default to 0.
 *
 * \note If the visual area and th esensing margins are not reaching the 4x4 grid
 * units limit, the component will fall back to these minimum limits.
 * For example, extending a 2x2 grid unit visual component into 4x10 grid units
 * would look as follows:
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
 *         // we only have to add 2 grid units as the wisth + left margin
 *         // already gives us 3 grid units out of 5
 *         rightMargin: units.gu(2)
 *     }
 * }
 * \endqml
 */
