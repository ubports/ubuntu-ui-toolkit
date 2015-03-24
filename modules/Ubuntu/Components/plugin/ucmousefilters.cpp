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
#include <QtGui/QGuiApplication>
#include <private/qqmlglobal_p.h>
#include <QtQuick/private/qquickmousearea_p.h>
#include "inversemouseareatype.h"
#include "i18n.h"

#include "quickutils.h"
#include "ucunits.h"

// keep in sync with QQuickMouseArea PressAndHoldDelay
const int DefaultPressAndHoldDelay = 800;

QEvent::Type ForwardedEvent::m_eventBase = QEvent::None;
void ForwardedEvent::registerForwardedEvent()
{
    if (m_eventBase > 0) {
        return;
    }
    m_eventBase = (QEvent::Type)QEvent::registerEventType();
}

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
   \qmltype Mouse
   \instantiates UCMouse
   \inqmlmodule Ubuntu.Components 1.1
   \ingroup ubuntu
   \brief Attached property filtering mouse events occured inside the owner.

   Sometimes we need to provide additional functionality on mouse events beside
   a QML element's default behavior. Placing a MouseArea over a component however
   will grab the mouse events from the component underneath, no matter if we set
   \a preventStealing to false or not. Setting mouse.accepted to false in \a onPressed
   would result in having the event forwarded to the MouseArea's parent, however
   MouseArea will no longer receive other mouse events.

   \qml
   import QtQuick 2.4

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
   import QtQuick 2.4
   import Ubuntu.Components 1.2

   TextInput {
       width: 100
       height: 20
       // do not accept event so it gets propagated to the parent item
       Mouse.onPressed: console.log("mouse button pressed: " + mouse.button)
       Mouse.onReleased: console.log("mouse button released: " + mouse.button)
   }
   \endqml

   The event details are reported in the \a mouse parameter, of MouseEvent
   type, which extends QtQuick's MouseEvent with additional properties.

   The filter will accept the same mouse buttons the owner accepts, and will accept
   hover events if the owner does. However it is not possible to alter these settings
   through the filter. If button handling other than the default ones specified for
   the primitive is required, MouseAreas can be declared to handle those events.

   Example of handling right button clicks over a TextInput:
   \qml
   import QtQuick 2.4
   import Ubuntu.Components 1.2

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
   import QtQuick 2.4
   import Ubuntu.Components 1.2

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
   one, and we want to provide additional filtering possibility to the user. These
   type of items are called proxy handlers.

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
   the TextInput. The topmost item itself does not handle mouse events, therefore
   it will be a sinple proxy handler item. However, proxies can themself handle
   mouse events. Therefore each mouse event signal has the \a host parameter specifying
   the sender of the mouse event reported.

   \note The forwarded events are handled in the proxy handlers only if the mouse
   position points inside their area. If the forwarded mouse position falls outside
   the target area, the event will not be reported, however will be forwarded further
   to the items in the list. In the following example the mouse press in red rectangle
   will be printed as well as the proxied mouse press from the main item.
   \qml
   import QtQuick 2.4
   import Ubuntu.Components 1.2

   Item {
       id: main
       width: units.gu(40)
       height: units.gu(71)

       Mouse.onPressed: console.log("got the mouse press forwarded by " + host.objectName)

       Column {
           anchors.fill: parent
           spacing: units.gu(1)

           Rectangle {
               id: blueRect
               objectName: "BlueRect"
               width: parent.width
               height: units.gu(20)
               color: "blue"
               Mouse.forwardTo: [main]
               Mouse.onPressed: console.log("This should not be printed")
           }
           Rectangle {
               objectName: "RedRect"
               width: parent.width
               height: units.gu(20)
               color: "red"
               MouseArea {
                   anchors.fill: parent
                   Mouse.forwardTo: [blueRect]
                   Mouse.onPressed: console.log("Pressed in " + host.objectName)
               }
           }
       }
   }
   \endqml

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
   import QtQuick 2.4
   import Ubuntu.Components 1.2

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


   Mouse filter provides ability to control the order of the event dispatching.
   The filter can receive the events prior the owner or after the owner. This
   can be controlled through the \l priority property. In the following example
   we make sure the TextInput always receives the events before the filter:
   \qml
   import QtQuick 2.4
   import Ubuntu.Components 1.2

   TextInput {
       id: input
       width: units.gu(40)
       height: units.gu(5)
       activeFocusOnPress: true

       Mouse.prority: Mouse.AfterItem
       Mouse.onPressed: if (input.activeFocus) console.log("Text input already handled it")
   }
   \endqml

   Another feature of the mouse filters is the ability to restrict when the composed
   events like \l onClicked and \l onPressAndHold should be triggered. By default
   these events are triggered no matter what is the distance between the mouse pressed
   position and the current position after a certain timeout (for \l onPressAndHold)
   or upon mouse release (for \l onClicked). In this way the \l onClicked will be
   emitted even if the user presses the mouse at the left-top edge of the component,
   then moves it to the right-bottom corner and releases it. This may not be the
   preferred behavior on certain components (like TextInput). Therefore MouseFilter
   provides a property which can alter this behavior, the \l clickAndHoldThreshold.
   This property specifies the radius of the area the up-mentioned composed events
   are emitted during a mouse move.

   \qml
   import QtQuick 2.4
   import Ubuntu.Components 1.2

   TextInput {
       width: units.gu(40)
       height: units.gu(5)
       activeFocusOnPress: true
       selectByMouse: true

       // emit composed events only if the mouse moves within 2 GU radius area
       Mouse.clickAndHoldThreshold: units.gu(2)
       Mouse.onClicked: console.log("click happened within threshold value")
       Mouse.onPressAndHold: console.log("pressAndHold happened within threshold value")
   }
   \endqml

   Similar functionality for the case when the mouse event occurs outside of the
   owner is brought by the \l InverseMouse attached property.
 */

UCMouse::UCMouse(QObject *parent)
    : QObject(parent)
    , m_owner(qobject_cast<QQuickItem*>(parent))
    , m_lastButton(Qt::NoButton)
    , m_lastButtons(Qt::NoButton)
    , m_lastModifiers(Qt::NoModifier)
    , m_pressedButtons(Qt::NoButton)
    , m_priority(BeforeItem)
    , m_moveThreshold(0.0)
    , m_enabled(false)
    , m_moved(false)
    , m_longPress(false)
    , m_hovered(false)
    , m_doubleClicked(false)
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
    } else if (type == ForwardedEvent::baseType()) {
        // this is a forwarded event, handle it as such
        return forwardedEvents(static_cast<ForwardedEvent*>(event));
    }

    return QObject::eventFilter(target, event);
}
void UCMouse::timerEvent(QTimerEvent *event)
{
    if (isEnabled() && (event->timerId() == m_pressAndHoldTimer.timerId())) {
        m_pressAndHoldTimer.stop();
        if (m_pressedButtons && m_hovered) {
            m_longPress = true;
            QQuickMouseEvent mev(m_lastPos.x(), m_lastPos.y(), m_lastButton, m_lastButtons, m_lastModifiers,
                                     false, m_longPress);
            mev.setAccepted(false);
            Q_EMIT pressAndHold(&mev, m_owner);
            mev.setAccepted(forwardEvent(ForwardedEvent::MouseLongPress, 0, &mev));
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
    return result || event->isAccepted();
}

// emit signals and forward the events further
bool UCMouse::forwardedEvents(ForwardedEvent *event)
{
    // the quick event is always specified!
    switch (event->subType()) {
    case ForwardedEvent::MousePress: {
        Q_EMIT pressed(event->quickEvent(), event->sender());
    } break;
    case ForwardedEvent::MouseRelease: {
        Q_EMIT released(event->quickEvent(), event->sender());
    } break;
    case ForwardedEvent::MouseMove: {
        Q_EMIT positionChanged(event->quickEvent(), event->sender());
    } break;
    case ForwardedEvent::MouseDblClick: {
        Q_EMIT doubleClicked(event->quickEvent(), event->sender());
    } break;
    case ForwardedEvent::HoverEnter: {
        Q_EMIT entered(event->quickEvent(), event->sender());
    } break;
    case ForwardedEvent::HoverExit: {
        Q_EMIT exited(event->quickEvent(), event->sender());
    } break;
        // composed events
    case ForwardedEvent::MouseClick: {
        Q_EMIT clicked(event->quickEvent(), event->sender());
    } break;
    case ForwardedEvent::MouseLongPress: {
        Q_EMIT pressAndHold(event->quickEvent(), event->sender());
    } break;
    default: break;
    }

    // forward event, but use the current owner as sender
    event->setAccepted(forwardEvent(event->subType(), event->originalEvent(), event->quickEvent()));
    return event->isAccepted();
}

bool UCMouse::hasAttachedFilter(QQuickItem *item)
{
    return (qmlAttachedPropertiesObject<UCMouse>(item, false) != 0);
}

void UCMouse::setHovered(bool hovered, QEvent *hoverEvent)
{
    if (m_hovered != hovered) {
        m_hovered = hovered;
        QQuickMouseEvent mev(m_lastPos.x(), m_lastPos.y(), m_lastButton, m_lastButtons, m_lastModifiers,
                                 false, false);
        mev.setAccepted(false);
        ForwardedEvent::EventType type = (m_hovered) ? ForwardedEvent::HoverEnter : ForwardedEvent::HoverExit;
        if (m_hovered) {
            Q_EMIT entered(&mev, m_owner);
        } else {
            m_pressAndHoldTimer.stop();
            Q_EMIT exited(&mev, m_owner);
        }
        forwardEvent(type, hoverEvent, &mev);
    }
}

bool UCMouse::mousePressed(QMouseEvent *event)
{
    m_moved = false;
    if (event->button() & m_owner->acceptedMouseButtons()) {
        saveEvent(event);
        m_pressedButtons |= m_lastButton;
        m_longPress = m_doubleClicked = false;

        setHovered(true, 0);

        QQuickMouseEvent mev(m_lastPos.x(), m_lastPos.y(), m_lastButton, m_lastButtons, m_lastModifiers,
                                 false, m_longPress);
        mev.setAccepted(false);
        Q_EMIT pressed(&mev, m_owner);
        event->setAccepted(forwardEvent(ForwardedEvent::MousePress, event, &mev));

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

        setHovered(true, 0);
        m_moved = true;
        m_doubleClicked = false;
        QQuickMouseEvent mev(m_lastPos.x(), m_lastPos.y(), m_lastButton, m_lastButtons, m_lastModifiers,
                                 false, m_longPress);
        mev.setAccepted(false);
        Q_EMIT positionChanged(&mev, m_owner);
        event->setAccepted(forwardEvent(ForwardedEvent::MouseMove, event, &mev));
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
        QQuickMouseEvent mev(m_lastPos.x(), m_lastPos.y(), m_lastButton, m_lastButtons, m_lastModifiers,
                                 isClicked, m_longPress);
        mev.setAccepted(false);
        Q_EMIT released(&mev, m_owner);
        event->setAccepted(forwardEvent(ForwardedEvent::MouseRelease, event, &mev));

        // remove button from press
        m_pressedButtons &= ~m_lastButton;
        if (isClicked) {
            // emit clicked
            mev.setAccepted(false);
            Q_EMIT clicked(&mev, m_owner);
            forwardEvent(ForwardedEvent::MouseClick, 0, &mev);
        }

        if (!m_pressedButtons && !m_owner->acceptHoverEvents()) {
            setHovered(false, 0);
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

        QQuickMouseEvent mev(m_lastPos.x(), m_lastPos.y(), m_lastButton, m_lastButtons, m_lastModifiers,
                                 true, m_longPress);
        mev.setAccepted(false);
        // if double click connected, suppress release() and click() signals
        if (isDoubleClickConnected()) {
            Q_EMIT doubleClicked(&mev, m_owner);
            m_doubleClicked = true;
        }
        // forward event even if it wasn't handled for the owner
        event->setAccepted(forwardEvent(ForwardedEvent::MouseDblClick, event, &mev));
        return event->isAccepted();
    }

    event->ignore();
    return false;
}

bool UCMouse::hoverEntered(QHoverEvent *event)
{
    m_lastPos = event->posF();
    m_lastModifiers = event->modifiers();
    m_lastButton = Qt::NoButton;
    m_lastButtons = Qt::NoButton;
    setHovered(true, event);
    return false;
}

bool UCMouse::hoverMoved(QHoverEvent *event)
{
    m_lastPos = event->posF();
    m_lastModifiers = event->modifiers();
    QQuickMouseEvent mev(m_lastPos.x(), m_lastPos.y(), Qt::NoButton, Qt::NoButton, m_lastModifiers,
                             false, m_longPress);
    mev.setAccepted(false);
    Q_EMIT positionChanged(&mev, m_owner);
    event->setAccepted(forwardEvent(ForwardedEvent::MouseMove, event, &mev));
    return false;
}

bool UCMouse::hoverExited(QHoverEvent *event)
{
    m_lastPos = event->posF();
    m_lastModifiers = event->modifiers();
    m_lastButton = Qt::NoButton;
    m_lastButtons = Qt::NoButton;
    setHovered(false, event);
    return false;
}

void UCMouse::saveEvent(QMouseEvent *event)
{
    m_lastPos = event->localPos();
    m_lastScenePos = event->windowPos();
    if (event->type() != QEvent::MouseMove) {
        m_lastButton = event->button();
        m_lastButtons = event->buttons();
    }
    m_lastModifiers = event->modifiers();
    if ((event->type() == QEvent::MouseButtonPress) && (m_moveThreshold > 0.0)) {
        m_toleranceArea.setX(m_lastPos.x() - m_moveThreshold);
        m_toleranceArea.setY(m_lastPos.y() - m_moveThreshold);
        m_toleranceArea.setWidth(2 * m_moveThreshold);
        m_toleranceArea.setHeight(2 * m_moveThreshold);
    }
}
bool UCMouse::isDoubleClickConnected()
{
    IS_SIGNAL_CONNECTED(this, UCMouse, doubleClicked, (QQuickMouseEvent*,QQuickItem*));
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


/*
 * Forwards the events to the listed items. The event coordinates are mapped to the destination's coordinates
 * and sent to the destination in case the destination has no filter attached. Otherwise the quick event
 * coordinates will be mapped and sent as ForwardedEvents.
 */
bool UCMouse::forwardEvent(ForwardedEvent::EventType type, QEvent *event, QQuickMouseEvent *quickEvent)
{
    // first set the accepted flag to the original event
    if (event && quickEvent) {
        event->setAccepted(quickEvent->isAccepted());
    }
    bool accepted = event ? event->isAccepted() : (quickEvent ? quickEvent->isAccepted() : false);

    Q_FOREACH(QQuickItem *item, m_forwardList) {

        if (accepted) {
            // the event got accepted, therefore should not be forwarded
            return accepted;
        }
        // skip InverseMouseArea otherwise those will get the event twice
        if (qobject_cast<InverseMouseAreaType*>(item)) {
            continue;
        }

        // map the normal event coordinates to item
        QEvent *mappedEvent = 0;
        if (event) {
            if (isMouseEvent(event->type())) {
                QMouseEvent *mouse = static_cast<QMouseEvent*>(event);
                QPointF itemPos = item->mapFromScene(m_owner->mapToScene(mouse->pos()));
                mappedEvent = new QMouseEvent(event->type(), itemPos, mouse->button(), mouse->buttons(), mouse->modifiers());
            } else if (isHoverEvent(event->type())){
                QHoverEvent *hover = static_cast<QHoverEvent*>(event);
                QPointF itemPos = item->mapFromScene(m_owner->mapToScene(hover->pos()));
                QPointF itemOldPos = item->mapFromScene(m_owner->mapToScene(hover->oldPos()));
                mappedEvent = new QHoverEvent(event->type(), itemPos, itemOldPos, hover->modifiers());
            }
        }

        // if the item has no filter attached, deliver the mapped event to it as it is
        UCMouse *filter = qobject_cast<UCMouse*>(qmlAttachedPropertiesObject<UCMouse>(item, false));
        if (!filter && mappedEvent) {
            QGuiApplication::sendEvent(item, mappedEvent);
            accepted = mappedEvent->isAccepted();
        } else if (quickEvent) {
            // map the quick event coordinates as well
            QPoint itemPos(item->mapFromScene(m_owner->mapToScene(QPointF(quickEvent->x(), quickEvent->y()))).toPoint());
            QQuickMouseEvent mev(itemPos.x(), itemPos.y(), (Qt::MouseButton)quickEvent->button(), (Qt::MouseButtons)quickEvent->buttons(),
                                 (Qt::KeyboardModifiers)quickEvent->modifiers(), quickEvent->isClick(), quickEvent->wasHeld());
            mev.setAccepted(false);
            ForwardedEvent forwardedEvent(type, m_owner, mappedEvent, &mev);
            QGuiApplication::sendEvent(item, &forwardedEvent);
            accepted = mev.isAccepted();
        }

        // cleanup and transfer accepted flag
        delete mappedEvent;
        if (event) {
            event->setAccepted(accepted);
        }
        if (quickEvent) {
            quickEvent->setAccepted(accepted);
        }
    }

    return accepted;
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
            m_pressAndHoldTimer.stop();
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
   \qmlproperty int Mouse::clickAndHoldThreshold
   The property holds the radius of the tolerance area the mouse can move in both
   x and y axis when the mouse is pressed, during which the composed events such
   as \l onClicked and \l onPressAndHold will still be emitted. If the mouse is
   moved out of this area while the button is pressed, no composed events will be
   emitted.

   When this value is 0, the signals will be emitted as in MouseArea, meaning the
   composed events will come until the mouse is moved inside the owner's area.

   The default value is 0.

   \note The value has no effect for the forwarded events. The threshold is only
   valid when the host handles mouse events.
 */
int UCMouse::clickAndHoldThreshold() const
{
    return m_moveThreshold;
}
void UCMouse::setClickAndHoldThreshold(int threshold)
{
    if (m_moveThreshold != threshold) {
        m_moveThreshold = threshold;
        Q_EMIT clickAndHoldThresholdChanged();
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
   \qmlsignal Mouse::onPressed(MouseEvent event, Item host)
   The signal reports the mouse press. The \a host specifies the item that triggered
   the event.
 */

/*!
   \qmlsignal Mouse::onReleased(MouseEvent event)
   The signal reports the mouse release. The \a host specifies the item that triggered
   the event.
 */

/*!
   \qmlsignal Mouse::onClicked(MouseEvent event)
   The signal reports the mouse click. The signal is not emitted if the onPressAndHold
   got triggered or if onDoubleClicked is handled (a slot is connected to it). The \a
   host specifies the item that triggered the event.
 */

/*!
   \qmlsignal Mouse::onPressAndHold(MouseEvent event)
   The signal reports the mouse press and hold. The \a host specifies the item that triggered
   the event.
 */

/*!
   \qmlsignal Mouse::onDoubleClicked(MouseEvent event)
   The signal reports mouse double click. The \a host specifies the item that triggered
   the event.
 */

/*!
   \qmlsignal Mouse::onPositionChanged(MouseEvent event)
   The signal reports the mouse pointer position change. If the hover events are
   enabled for the owner, the signal will come continuously. Otherwise the position
   chanes are reported when one of the accepted mouse buttons are being kept pressed.
   The \a host specifies the item that triggered the event.
 */

/*!
   \qmlsignal Mouse::onEntered(MouseEvent event)
   The signal reports that the mouse has entered into the area. The signal is
   emitted when the hover events are enabled and the mouse enters the area or
   when one of the accepted mouse button is pressed. The \a host specifies the
   item that triggered the event.
 */

/*!
   \qmlsignal Mouse::onExited(MouseEvent event)
   The signal reports that the mouse has left the area. The signal is emitted when
   the hover events are enabled for the owner or if not, when one of the accepted
   button is released. The \a host specifies the item that triggered the event.
 */

/******************************************************************************
 **/
/*!
   \qmltype InverseMouse
   \instantiates UCInverseMouse
   \inqmlmodule Ubuntu.Components 1.1
   \ingroup ubuntu
   \brief Attached object filtering mouse events occured outside the owner.

   Similar to Mouse filter attached property, provides mouse event filtering
   capabilities but for events occurring outside of the owner's area, excluding
   the input method area.

   Being derived from Mouse filter element, provides the same behavior as the
   Mouse filter, except that all the signals are emitted when the mouse event
   occurs outside the owner's area. Note that accepting composed mouse events
   does not have any effect on event propagation.

   Altering \a priority property has no effect on inverse mouse filter as mouse
   events captured by the filter are not forwarded to the owner, hence forwarding
   those events first to the owner will not have any effect.
 */
UCInverseMouse::UCInverseMouse(QObject *parent)
    : UCMouse(parent)
{
}

UCInverseMouse *UCInverseMouse::qmlAttachedProperties(QObject *owner)
{
    return createAttachedFilter<UCInverseMouse>(owner, "InverseMouse");
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

// check whether the mouse point is in OSK area
bool UCInverseMouse::pointInOSK(const QPointF &point)
{
    QPointF scenePoint = m_owner->mapToScene(point);
    QRectF oskRect = QGuiApplication::inputMethod()->keyboardRectangle();
    return oskRect.contains(scenePoint);
}

// returns true if the point is in the inverse area
bool UCInverseMouse::contains(QMouseEvent *mouse)
{
    QPointF localPos = mouse->localPos();
    return !m_owner->contains(localPos) && !pointInOSK(localPos);
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

