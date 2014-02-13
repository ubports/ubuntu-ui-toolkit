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
 *
 */

#include "ucmouse.h"
#include "ucinversemouse.h"
#include <QtQml/QQmlInfo>
#include <QtQuick/QQuickItem>
#include <QtGui/QGuiApplication>
#include <private/qqmlglobal_p.h>
#include <QtQuick/private/qquickmousearea_p.h>
#include "inversemouseareatype.h"
#include "i18n.h"

#include "quickutils.h"
#include "ucunits.h"

// keep in sync with QQuickMouseArea PressAndHoldDelay
const int DefaultPressAndHoldDelay = 800;

/*
 * Attached filter instantiator template
 */
template<typename T>
T *createAttachedFilter(QObject *owner, const QString &qmlName)
{
    QQuickItem *item = qobject_cast<QQuickItem*>(owner);
    if (!item) {
        qmlInfo(owner) << UbuntuI18n::instance().
                          tr(QString("Warning: %1 filter can only be attached to Items.").arg(qmlName));
    }

    T *filter = new T(owner);
    filter->setEnabled(true);
    return filter;
}

/*!
   \qmltype ExtendedMouseEvent
   \instantiates UCExtendedMouseEvent
   \inqmlmodule Ubuntu.Components 0.1
   \ingroup ubuntu
   \brief Mouse event extending QtQuick's MouseEvent type.

   ExtendedMouseEvent is used by Mouse and InverseMouse attached signals holding
   information about the mouse event. Extends the MouseEvent with additional
   information on the mouse event, like specifying whether the mouse pointer is
   in the input method area or not. This is useful in InverseMouse filter case
   when the detection whether the mouse press or click occured inside the input
   method area or not.

   An other difference from MouseEvent is that when all MouseEvenths are accepted
   by default, every ExtendedMouseEventis set to not accepted, meaning that every
   extended mouse event will be propagated to its owner. In case event propagation
   is not wanted, the \a accepted property must be set explicitly.
 */

/*!
    \qmlproperty bool ExtendedMouseEvent::pointInInputArea
    The property specifies whether the mouse event happened over the input method
    or not. The value is true when the device has input method specified, the
    input method is active and the mouse is positioned over the input area.
    The property has meaning mostly for the InverseMouse filter as is the Mouse
    filter's owner is covered by the input method, all mouse events are consumed
    by the input method.
  */
bool UCExtendedMouseEvent::pointInInputArea() const
{
    return m_overOsk;
}

/*!
   \qmltype Mouse
   \instantiates UCMouse
   \inqmlmodule Ubuntu.Components 0.1
   \ingroup ubuntu
   \brief Attached property filtering mouse events occured inside the owner.

   Sometimes we need to provide additional functionality on mouse events beside
   a QML element's default behavior. Placing a MouseArea over a component however
   will grab the mouse events from the component underneath, no matter if we set
   \a preventStealing to false or not. Setting mouse.accepted to false in \a onPressed
   would result in having the event forwarded to the MouseArea's parent, however
   MouseArea will no longer receive other mouse events.

   \qml
   import QtQuick 2.0

   TextInput {
       width: 100
       height: 20
       MouseArea {
           anchors.fill: parent
           preventStealing: false
           // do not accept event so it gets propagated to the parent item
           onPressed: mouse.accepted = false;
           onReleased: console.log("this will not be printed")
       }
   }
   \endqml

   Ubuntu UI Toolkit declares filter components similar to Keys, which can be
   attached to any visual primitve. Mouse filter however will have effect only
   when attached to items handling mouse events. Events are handled through signals,
   where the event data is presented through the \a mouse parameter. Events
   should be accepted if the propagation of those to the owner is not wanted.
   This is not valid to \l onClicked, \l onPressAndHold composed events.

   The previous code sample using Mouse filter, which will print the pressed and
   released mouse buttons would look as follows:
   \qml
   import QtQuick 2.0
   import Ubuntu.Components 0.1

   TextInput {
       width: 100
       height: 20
       // do not accept event so it gets propagated to the parent item
       Mouse.onPressed: console.log("mouse button pressed: " + mouse.button)
       Mouse.onReleased: console.log("mouse button released: " + mouse.button)
   }
   \endqml

   The event details are reported in the \a mouse parameter, of ExtendedMouseEvent
   type, which extends QtQuick's MouseEvent with additional properties.

   The filter will accept the same mouse buttons the owner accepts, and will accept
   hover events if the owner does. However it is not possible to alter these settings
   through the filter. If button handling other than the default ones specified for
   the primitive is required, MouseAreas can be declared to handle those events.

   Example of handling right button clicks over a TextInput:
   \qml
   import QtQuick 2.0
   import Ubuntu.Components 0.1

   TextInput {
       width: 100
       height: 20

       MouseArea {
           anchors.fill: parent
           acceptedButtons: Qt.RightButton
           onClicked: console.log("right button clicked")
       }
   }
   \endqml
   In this example left and middle mouse button clicks will reach TextInput as
   MouseArea only grabs right button events.

   Mouse filter can be used in combination with MouseArea, where the filter brings
   additional functionality on top of existing primitive functionality, and MouseArea
   add new functionality to the primitive.

   \qml
   import QtQuick 2.0
   import Ubuntu.Components 0.1

   TextInput {
       width: 100
       height: 20
       // do not accept event so it gets propagated to the parent item
       Mouse.onPressed: {
           if (mouse.button === Qt.LeftButton) {
               // do something
           }
       }
       MouseArea {
           anchors.fill: parent
           acceptedButtons: Qt.RightButton
           Mouse.onPressed: console.log("right button clicked")
       }
   }
   \endqml

   As mentioned, mouse filters can be attached to any visual item. Attaching it
   to items that do not handle any mouse events will not have any effect. However
   child items which handle mouse events can forward the events they handle to
   their parent. In this way mouse events will land in these items too, and mouse
   filter attached to those can also handle the event. This is useful when creating
   custom types where the mouse handling item is nested into a non-mouse handling
   one, and we want to provide additional filtering possibility to the user.

   \qml
   Item {
       id: top
       width: 100
       height: 50

       Mouse.onPressed: console.log("mouse received from input")

       TextItem {
           anchors.fill: parent
           Mouse.forvardTo: [top]
           Mouse.onPressed: console.log("pressed over input")
           Mouse.onPressAndHold: console.log("longpress handled here")
       }
   }
   \endqml

   In this example the mouse press is first handled by the mouse filter attached
   to TextInput, then it is forwarded to the top item and finally to the TextInput.
   Accepting the mouse event will stop propagation to the top item as well as to
   the TextInput.

   An interesting feature that can be achieved using Mouse filter is the event
   "transparency" towards the MouseArea lying behind the items which handle mouse
   events. This means for example that by forwarding mouse events occurred on a
   TextInput to a MouseArea that stays behind it in the item hierarchy, the
   MouseArea will also get all the events occurred on the area covered by the
   TextInput, acting like it would be above the TextInput. However, due to the
   nature of the MouseArea event acceptance policy (all events are accepted by
   default) TextInput will not get these mouse events unless we set the \a accepted
   field of the mouse event to false in MouseArea. This normally leads to the
   MouseArea no longer getting further mouse events. However, Mouse filter will
   continue to forward other mouse events to the MouseArea, so setting \a accepted
   to false in \a onPressed, \a onReleased will not have the default effect. This
   is only valid to press and release events, double-click or mouse position change
   will be blocked by the MouseArea still.
   \qml
   import QtQuick 2.0
   import Ubuntu.Components 0.1

   MouseArea {
       id: topArea
       width: units.gu(50)
       height: units.gu(10)

       onPressed: {
           console.log("forwarded pressed")
           mouse.accepted = false
       }
       onReleased: {
           console.log("released")
           mouse.accepted = false
       }

       TextInput {
           width: units.gu(40)
           height: units.gu(5)
           anchors.centerIn: parent

           Mouse.forwardTo: [topArea]
           Mouse.onPressed: console.log("input pressed")
           Mouse.onReleased: console.log("input released")
       }
   }
   \endqml

   Similar functionality for the case when the mouse event occurs outside of the
   owner is brought by the \l InverseMouse attached property.
 */

UCMouse::UCMouse(QObject *parent)
    : QObject(parent)
    , m_owner(qobject_cast<QQuickItem*>(parent))
    , m_pressedButtons(Qt::NoButton)
    , m_priority(BeforeItem)
    , m_moveThreshold(0.0)
    , m_enabled(false)
    , m_moved(false)
    , m_longPress(false)
    , m_hovered(false)
    , m_doubleClicked(false)
    , m_pointInOSK(false)
{
    // if owner is MouseArea or InverseMouseArea, connect to the acceptedButtons
    // and hoverEnabled change signals
    QQuickMouseArea* mouseOwner = qobject_cast<QQuickMouseArea*>(m_owner);
    if (mouseOwner) {
        connect(mouseOwner, SIGNAL(acceptedButtonsChanged()), this, SIGNAL(acceptedButtonsChanged()));
        connect(mouseOwner, SIGNAL(hoverEnabledChanged()), this, SIGNAL(hoverEnabledChanged()));
    }
}

UCMouse *UCMouse::qmlAttachedProperties(QObject *owner)
{
    return createAttachedFilter<UCMouse>(owner, "Mouse");
}

bool UCMouse::eventFilter(QObject *target, QEvent *event)
{
    QEvent::Type type = event->type();
    bool mouseEvent = isMouseEvent(type);
    bool hoverEvent = isHoverEvent(type);
    if (mouseEvent || hoverEvent) {
        static bool forwardLock = false;

        if (forwardLock) {
            return QObject::eventFilter(target, event);
        }
        if (m_priority == AfterItem) {
            // send the event first to the owner, but lock out ourself to avoid
            // getting into infinite recurence
            forwardLock = true;
            QCoreApplication::sendEvent(m_owner, event);
            forwardLock = false;

        }
        if (mouseEvent) {
            return mouseEvents(target, static_cast<QMouseEvent*>(event));
        } else {
            return hoverEvents(target, static_cast<QHoverEvent*>(event));
        }
    }

    return QObject::eventFilter(target, event);
}
void UCMouse::timerEvent(QTimerEvent *event)
{
    if (isEnabled() && (event->timerId() == m_pressAndHoldTimer.timerId())) {
        m_pressAndHoldTimer.stop();
        if (m_pressedButtons && m_hovered) {
            m_longPress = true;
            UCExtendedMouseEvent mev(m_lastPos, m_lastButton, m_lastButtons, m_lastModifiers,
                                     m_pointInOSK, false, m_longPress);
            Q_EMIT pressAndHold(&mev);
            // if the event wasn't handled, allow click
            if (!mev.isAccepted()) {
                m_longPress = false;
            }
            event->setAccepted(mev.isAccepted());
        }
    } else {
        QObject::timerEvent(event);
    }
}
bool UCMouse::mouseEvents(QObject *target, QMouseEvent *event)
{
    bool result = false;
    Q_UNUSED(target);
    switch (event->type()) {
    case QEvent::MouseButtonPress:
    {
        result = mousePressed(event);
    } break;
    case QEvent::MouseButtonRelease:
    {
        result = mouseReleased(event);
    } break;
    case QEvent::MouseButtonDblClick:
    {
        result = mouseDblClick(event);
    } break;
    case QEvent::MouseMove:
    {
        result = mouseMoved(event);
    } break;
    default:
        break;
    }
    forwardEvent(event);
    return result || event->isAccepted();
}

bool UCMouse::hoverEvents(QObject *target, QHoverEvent *event)
{
    bool result = false;
    Q_UNUSED(target)
    switch (event->type()) {
    case QEvent::HoverEnter:
    {
        result = hoverEntered(event);
    } break;
    case QEvent::HoverMove:
    {
        result = hoverMoved(event);
    } break;
    case QEvent::HoverLeave:
    {
        result = hoverExited(event);
    } break;
    default:
        break;
    }
    forwardEvent(event);
    return result || event->isAccepted();
}

bool UCMouse::hasAttachedFilter(QQuickItem *item)
{
    return (qmlAttachedPropertiesObject<UCMouse>(item, false) != 0);
}

void UCMouse::setHovered(bool hovered)
{
    if (m_hovered != hovered) {
        m_hovered = hovered;
        UCExtendedMouseEvent mev(m_lastPos, m_lastButton, m_lastButtons, m_lastModifiers,
                                 m_pointInOSK, false, false);
        if (m_hovered) {
            Q_EMIT entered(&mev);
        } else {
            Q_EMIT exited(&mev);
        }
    }
}

bool UCMouse::mousePressed(QMouseEvent *event)
{
    m_moved = false;
    if (event->button() & m_owner->acceptedMouseButtons()) {
        saveEvent(event);
        m_pressedButtons |= m_lastButton;
        m_longPress = m_doubleClicked = false;

        setHovered(true);

        UCExtendedMouseEvent mev(m_lastPos, m_lastButton, m_lastButtons, m_lastModifiers,
                                 m_pointInOSK, false, m_longPress);
        Q_EMIT pressed(&mev);
        event->setAccepted(mev.isAccepted());

        // start long press timer
        m_pressAndHoldTimer.start(DefaultPressAndHoldDelay, this);

        return mev.isAccepted();
    }

    event->ignore();
    return false;
}

bool UCMouse::mouseMoved(QMouseEvent *event)
{
    if (m_pressedButtons) {
        saveEvent(event);

        // check if we should stop pressAndHold
        if (!m_toleranceArea.contains(m_lastPos)) {
            m_pressAndHoldTimer.stop();
        }

        setHovered(true);
        m_moved = true;
        m_doubleClicked = false;
        UCExtendedMouseEvent mev(m_lastPos, m_lastButton, m_lastButtons, m_lastModifiers,
                                 m_pointInOSK, false, m_longPress);
        Q_EMIT positionChanged(&mev);
        event->setAccepted(mev.isAccepted());
        return mev.isAccepted();
    }

    event->ignore();
    return false;
}

bool UCMouse::mouseReleased(QMouseEvent *event)
{
    if (m_pressedButtons) {
        saveEvent(event);
        // stop long press timer event
        m_pressAndHoldTimer.stop();
        bool isClicked = (m_pressedButtons & m_lastButton)
                && !m_longPress && !m_doubleClicked &&
                ((m_moveThreshold <= 0.0) || m_toleranceArea.contains(m_lastPos));
        UCExtendedMouseEvent mev(m_lastPos, m_lastButton, m_lastButtons, m_lastModifiers,
                                 m_pointInOSK, isClicked, m_longPress);
        Q_EMIT released(&mev);
        event->setAccepted(mev.isAccepted());

        // remove button from press
        m_pressedButtons &= ~m_lastButton;
        if (isClicked) {
            // emit clicked
            Q_EMIT clicked(&mev);
        }

        if (!m_pressedButtons && !m_owner->acceptHoverEvents()) {
            setHovered(false);
        }
        return mev.isAccepted();
    }

    event->ignore();
    return false;
}

bool UCMouse::mouseDblClick(QMouseEvent *event)
{
    if (m_pressedButtons) {
        saveEvent(event);
        // if double click connected, suppress release() and click() signals
        if (isDoubleClickConnected()) {
            UCExtendedMouseEvent mev(m_lastPos, m_lastButton, m_lastButtons, m_lastModifiers,
                                     m_pointInOSK, true, m_longPress);
            Q_EMIT doubleClicked(&mev);
            event->setAccepted(mev.isAccepted());
            m_doubleClicked = true;
        } else {
            // set accepted to false still
            event->setAccepted(false);
        }
        return event->isAccepted();
    }

    event->ignore();
    return false;
}

bool UCMouse::hoverEntered(QHoverEvent *event)
{
    m_lastPos = event->posF();
    m_lastModifiers = event->modifiers();
    setHovered(true);
    return false;
}

bool UCMouse::hoverMoved(QHoverEvent *event)
{
    m_lastPos = event->posF();
    m_lastModifiers = event->modifiers();
    UCExtendedMouseEvent mev(m_lastPos, m_lastButton, m_lastButtons, m_lastModifiers,
                             m_pointInOSK, false, m_longPress);
    Q_EMIT positionChanged(&mev);
    forwardEvent(event);
    return false;
}

bool UCMouse::hoverExited(QHoverEvent *event)
{
    m_lastPos = event->posF();
    m_lastModifiers = event->modifiers();
    setHovered(false);
    return false;
}

void UCMouse::saveEvent(QMouseEvent *event)
{
    m_lastPos = event->localPos();
    m_lastScenePos = event->windowPos();
    m_lastButton = event->button();
    m_lastButtons = event->buttons();
    m_lastModifiers = event->modifiers();
    m_pointInOSK = pointInOSK(event->localPos());
    if ((event->type() == QEvent::MouseButtonPress) && (m_moveThreshold > 0.0)) {
        m_toleranceArea.setX(m_lastPos.x() - m_moveThreshold);
        m_toleranceArea.setY(m_lastPos.y() - m_moveThreshold);
        m_toleranceArea.setWidth(2 * m_moveThreshold);
        m_toleranceArea.setHeight(2 * m_moveThreshold);
    }
}
bool UCMouse::isDoubleClickConnected()
{
    IS_SIGNAL_CONNECTED(this, UCMouse, doubleClicked, (UCExtendedMouseEvent*));
}

// check whether the mouse point is in OSK area
bool UCMouse::pointInOSK(const QPointF &point)
{
    QPointF scenePoint = m_owner->mapToScene(point);
    QRectF oskRect = QGuiApplication::inputMethod()->keyboardRectangle();
    return oskRect.contains(scenePoint);
}

bool UCMouse::isMouseEvent(QEvent::Type type)
{
    return (type == QEvent::MouseButtonPress) || (type == QEvent::MouseButtonRelease)
            || (type == QEvent::MouseButtonDblClick) || (type == QEvent::MouseMove);
}

bool UCMouse::isHoverEvent(QEvent::Type type)
{
    return (type == QEvent::HoverEnter) || (type == QEvent::HoverMove) || (type == QEvent::HoverLeave);
}

// forwards the events to the listed items; only mouse and hover events qualify
void UCMouse::forwardEvent(QEvent *event)
{
    /*
     * alter acceptedButtons and hoverEnabled for the time the event is delivered
     * exclude MouseArea and InverseMouseArea!!
     */
    Q_FOREACH(QQuickItem *item, m_forwardList) {

        if (event->isAccepted()) {
            // the event got accepted, therefore should not be forwarded
            return;
        }
        // skip InverseMouseArea otherwise those will get the event twice
        if (qobject_cast<InverseMouseAreaType*>(item)) {
            continue;
        }
        /*
         * temporarily enable mouse buttons and hover for items which have Mouse or InverseMouse
         * filter attached, but exclude targets which is MouseArea or InverseMouseArea
         */
        Qt::MouseButtons acceptedButtons = item->acceptedMouseButtons();
        bool hoverEnabled = item->acceptHoverEvents();
        if (!qobject_cast<QQuickMouseArea*>(item) && hasAttachedFilter(item)) {
            // set accepted buttons and hover temporarily
            item->setAcceptedMouseButtons(m_owner->acceptedMouseButtons());
            item->setAcceptHoverEvents(m_owner->acceptHoverEvents());
        }

        // forward events
        bool accepted = false;
        if (isMouseEvent(event->type())) {
            QMouseEvent *mouse = static_cast<QMouseEvent*>(event);
            QPointF itemPos = item->mapFromScene(m_owner->mapToScene(mouse->pos()));
            QMouseEvent me = QMouseEvent(event->type(), itemPos, mouse->button(), mouse->buttons(), mouse->modifiers());
            QGuiApplication::sendEvent(item, &me);
            accepted = me.isAccepted();
        } else {
            QHoverEvent *hover = static_cast<QHoverEvent*>(event);
            QPointF itemPos = item->mapFromScene(m_owner->mapToScene(hover->pos()));
            QPointF itemOldPos = item->mapFromScene(m_owner->mapToScene(hover->oldPos()));
            QHoverEvent he = QHoverEvent(event->type(), itemPos, itemOldPos, hover->modifiers());
            QGuiApplication::sendEvent(item, &he);
            accepted = he.isAccepted();
        }
        event->setAccepted(accepted);

        // restore acceptedButtons and hover
        item->setAcceptedMouseButtons(acceptedButtons);
        item->setAcceptHoverEvents(hoverEnabled);
    }
}


/*!
   \qmlproperty bool Mouse::enabled

 */
bool UCMouse::isEnabled() const
{
    return m_enabled;
}
void UCMouse::setEnabled(bool enabled)
{
    if ((enabled != m_enabled) && m_owner) {
        m_enabled = enabled;
        if (m_enabled) {
            m_owner->installEventFilter(this);
        } else {
            m_owner->removeEventFilter(this);
        }
        Q_EMIT enabledChanged();
    }
}

/*!
   \qmlproperty Qt::MouseButtons Mouse::acceptedButtons
   \readonly
   The property holds the accepted mouse buttons of the owner.
 */
Qt::MouseButtons UCMouse::acceptedButtons() const
{
    return m_owner ? m_owner->acceptedMouseButtons() : Qt::NoButton;
}

/*!
    \qmlproperty bool Mouse::hoverEnabled
    \readonly
    The property reports whether the owner accepts hover events or not. When
    events are accepted \l onEntered, \l onPositionChanged and \l onExited signals
    containing the mouse cursor position.
  */
bool UCMouse::hoverEnabled() const
{
    return m_owner ? m_owner->acceptHoverEvents() : false;
}

/*!
   \qmlproperty int Mouse::composedEventThreshold
   The property holds the tolerance value the mouse can move in both x and y axis
   when the mouse is pressed, during which the composed events such as \l onClicked
   and \l onPressAndHold will still be emitted.

   The tolerance area is set around the mouse position the button was pressed.
   While the button is pressed, moving the mouse out of this area will not result
   in emitting the composed signals.

   When this value is 0, the signals will be emitted as in MouseArea, meaning the
   composed events will come if the mouse move does not leave the area.

   The default value is 0.
 */
int UCMouse::composedEventThreshold() const
{
    return m_moveThreshold;
}
void UCMouse::setComposedEventThreshold(int threshold)
{
    if (m_moveThreshold != threshold) {
        m_moveThreshold = threshold;
        Q_EMIT composedEventThresholdChanged();
    }
}

/*!
  \qmlproperty list<Item> Mouse::forwardTo
  The property provides a way to forward mouse presses, releases, moves and double
  click events to other items. This can be useful when you want other items to
  handle different parts of the same mouse event or to handle other mouse events.

  The items listed will receive the event only if the mouse event falls into their
  area. Once an item that has forwarded mouse events accepts the event, that will
  no longer be delivered to the rest of the items in the list. This rule is also
  applied on the owner when the priority is set to \a BeforeItem.
  */
QQmlListProperty<QQuickItem> UCMouse::forwardTo()
{
    return QQmlListProperty<QQuickItem>(this, m_forwardList);
}

/*!
  \qmlproperty enumeration Mouse::priority
  The property specifies the event dispach relation between the filter, the elements
  the event is forwarded to and the owner.
  Similar to Keys' \a priority property, the event dispach is performed in two
  ways: berfore (\a BeforeItem) or after (\a AfterItem) the owner receives the events.

  When \a BeforeItem is set the event dispach happens based as follows:
  \list i
    \li the event is handled by the mouse filter
    \li if there are items listed in \l forwardTo property, the event will be forwarded
        to those items
    \li the event is handed over the owner.
  \endlist

  \raw HTML
    <br />
  \endraw
  When \a AfterItem is set the event dispach happens based as follows:
  \list i
    \li the event is handed over the owner;
    \li the event is handled by the mouse filter;
    \li if there are items listed in \l forwardTo property, the event will be forwarded
        to those items.
  \endlist

  The default value is \a BeforeItem.
 */
UCMouse::Priority UCMouse::priority() const
{
    return m_priority;
}
void UCMouse::setPriority(Priority priority)
{
    if (priority != m_priority) {
        m_priority = priority;
        Q_EMIT priorityChanged();
    }
}

/*!
   \qmlsignal Mouse::onPressed(ExtendedMouseEvent event)
   The signal reports the mouse press.
 */

/*!
   \qmlsignal Mouse::onReleased(ExtendedMouseEvent event)
   The signal reports the mouse release.
 */

/*!
   \qmlsignal Mouse::onClicked(ExtendedMouseEvent event)
   The signal reports the mouse click. The signal is not emitted if the onPressAndHold
   got triggered or if onDoubleClicked is handled (a slot is connected to it).
 */

/*!
   \qmlsignal Mouse::onPressAndHold(ExtendedMouseEvent event)
   The signal reports the mouse press and hold.
 */

/*!
   \qmlsignal Mouse::onDoubleClicked(ExtendedMouseEvent event)
   The signal reports mouse double click.
 */

/*!
   \qmlsignal Mouse::onPositionChanged(ExtendedMouseEvent event)
   The signal reports the mouse pointer position change. If the hover events are
   enabled for the owner, the signal will come continuously. Otherwise the position
   chanes are reported when one of the accepted mouse buttons are being kept pressed.
 */

/*!
   \qmlsignal Mouse::onEntered(ExtendedMouseEvent event)
   The signal reports that the mouse has entered into the area. The signal is
   emitted when the hover events are enabled and the mouse enters the area or
   when one of the accepted mouse button is pressed.
 */

/*!
   \qmlsignal Mouse::onExited(ExtendedMouseEvent event)
   The signal reports that the mouse has left the area. The signal is emitted when
   the hover events are enabled for the owner or if not, when one of the accepted
   button is released.
 */

/******************************************************************************
 **/
/*!
   \qmltype InverseMouse
   \instantiates UCInverseMouse
   \inqmlmodule Ubuntu.Components 0.1
   \ingroup ubuntu
   \brief Attached object filtering mouse events occured outside the owner.

   Similar to Mouse filter attached property, provides mouse event filtering
   capabilities but for events occurring outside of the owner's area. Beside that
   the filter also provides abilities to detect whether the mouse occurred inside
   the input method area or not.

   Being derived from Mouse filter element, provides the same behavior as the
   Mouse filter, except that all the signals are emitted when the mouse event
   occurs outside the owner's area.

   By default all mouse events occurring outside the owner are reported, including
   events over the active input method area. This can be controlled through the
   \l excludeInputArea property. When that is set, no mouse event occurring over
   the input method area will be reported.

   The following example blocks mouse event propagation to input method when the
   mouse press occurs over input method area.
   \qml
   import QtQuick 2.0
   import Ubuntu.Components 0.1

   TestInput {
       width: 100
       height: 20

       InverseMouse.onPressed: {
           if (mouse.pointInInputArea) {
               mouse.accepted = true;
           }
       }
   }
   \endqml

   Note that accepting composed mouse events does not have any effect on event
   propagation.

   Altering \a priority property has no effect on inverse mouse filter as mouse
   events captured by the filter are not forwarded to the owner, hence forwarding
   those events first to the owner will not have any effect.
 */
UCInverseMouse::UCInverseMouse(QObject *parent)
    : UCMouse(parent)
    , m_excludeOSK(false)
{
}

UCInverseMouse *UCInverseMouse::qmlAttachedProperties(QObject *owner)
{
    return createAttachedFilter<UCInverseMouse>(owner, "InverseMouse");
}

/*!
   \qmlproperty bool InverseMouse::excludeInputArea
   The property specifies whether the mouse events happening over the input panel
   area should be excluded or not. The default value is false, so mouse events
   are reported all over the outside area of the owner.
 */
bool UCInverseMouse::excludeInputArea() const
{
    return m_excludeOSK;
}
void UCInverseMouse::setExcludeInputArea(bool value)
{
    if (m_excludeOSK != value) {
        m_excludeOSK = value;
        Q_EMIT excludeInputAreaChanged();
    }
}

QMouseEvent UCInverseMouse::mapMouseToOwner(QObject *target, QMouseEvent* event)
{
    QQuickItem *item = qobject_cast<QQuickItem*>(target);
    QPointF localPos = (target != m_owner) ? m_owner->mapFromItem(item, event->localPos()) : event->localPos();
    QPointF windowPos = (target != m_owner) ? m_owner->mapFromItem(item, event->windowPos()) : event->windowPos();
    return QMouseEvent(event->type(), localPos, windowPos, event->screenPos(),
                       event->button(), event->buttons(), event->modifiers());
}

QHoverEvent UCInverseMouse::mapHoverToOwner(QObject *target, QHoverEvent *event)
{
    QQuickItem *item = qobject_cast<QQuickItem*>(target);
    QPointF pos = (target != m_owner) ? m_owner->mapFromItem(item, event->posF()) : event->posF();
    QPointF oldPos = (target != m_owner) ? m_owner->mapFromItem(item, event->oldPosF()) : event->oldPosF();
    return QHoverEvent(event->type(), pos, oldPos, event->modifiers());
}

bool UCInverseMouse::eventFilter(QObject *target, QEvent *event)
{
    // exclude MouseArea and InverseMouseArea targets; InverseMosueArea casts to QQuickMouseArea
    if ((target != m_owner) && !qobject_cast<QQuickMouseArea*>(target)) {
        return UCMouse::eventFilter(target, event);
    }
    return QObject::eventFilter(target, event);
}
bool UCInverseMouse::mouseEvents(QObject *target, QMouseEvent *event)
{
    QMouseEvent mouse(mapMouseToOwner(target, event));
    if (!contains(&mouse)) {
        // do not handle mouse event if it's inside the owner
        return false;
    }
    // call base handler
    return UCMouse::mouseEvents(target, &mouse);
}

bool UCInverseMouse::hoverEvents(QObject *target, QHoverEvent *event)
{
    QHoverEvent hover(mapHoverToOwner(target, event));
    return UCMouse::hoverEvents(target, &hover);
}

bool UCInverseMouse::hasAttachedFilter(QQuickItem *item)
{
    return UCMouse::hasAttachedFilter(item) || (qmlAttachedPropertiesObject<UCInverseMouse>(item, false) != 0);
}

// returns true if the point is in the inverse area
bool UCInverseMouse::contains(QMouseEvent *mouse)
{
    bool result = !m_owner->contains(mouse->localPos());
    if (m_excludeOSK) {
        result = result && !pointInOSK(mouse->localPos());
    }
    return result;
}

void UCInverseMouse::setEnabled(bool enabled)
{
    if ((m_enabled != enabled) && m_owner) {
        m_enabled = enabled;
        if (m_enabled) {
            // FIXME: use application's main till we don't get touch events
            // forwarded to the QQuickItem
            QGuiApplication::instance()->installEventFilter(this);
        } else {
            QGuiApplication::instance()->removeEventFilter(this);
        }
        Q_EMIT enabledChanged();
    }
}

void UCInverseMouse::setPriority(Priority priority)
{
    if (priority != m_priority) {
        qmlInfo(m_owner) << UbuntuI18n::instance().tr("Ignoring AfterItem priority for InverseMouse filters.");
    }
}

