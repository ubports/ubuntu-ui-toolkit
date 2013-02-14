/*
 * Copyright 2012 Canonical Ltd.
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

#include "inversemouseareatype.h"
#include <QtQuick/QQuickWindow>
#include <QtCore/QEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QInputMethod>
#include <QtGui/QGuiApplication>
#include <QtQuick/private/qquickevents_p_p.h>
#include "quickutils.h"

#include <QDebug>

/*!
  \internal
  \qmltype InverseMouseArea
  \instantiates InverseMouseAreaType
  \inqmlmodule Ubuntu.Components 0.1
  \ingroup ubuntu
  \brief The InverseMouseArea captures mouse events happening outside of a given
  area.

  A typical use case is hiding of a popup or tooltip when the user presses or taps
  outside of the popup or tooltip area. The following example illustrates the use
  of InverseMouseArea in a Popup.

  Popup.qml
  \qml
  import QtQuick 2.0
  import Ubuntu.Components 0.1

  Rectangle {
      anchors.centerIn: parent
      width: 200; height: 200
      color: "darkgray"
      radius: 10
      InverseMouseArea {
         anchors.fill: parent
         acceptedButtons: Qt.LeftButton
         onPressed: parent.destroy()
      }
  }
  \endqml

  In the Popup above the mouse presses happening outside the area will cause
  closing and destroying the popup.

  \qml
  import QtQuick 2.0
  import Ubuntu.Components 0.1

  Item {
      width: 640
      height: 480
      Button {
          id: button
          text: "Press me"
          onClicked: {
              var component = Qt.createComponent("Popup.qml");
              var obj = component.create(parent);
              obj.visible = true;
          }
      }
  }
  \endqml

 */

/*!
  \internal
 */
InverseMouseAreaType::InverseMouseAreaType(QQuickItem *parent) :
    QQuickItem(parent),
    m_pressed(false),
    m_moved(false),
    m_propagateEvents(false),
    m_acceptedButtons(Qt::LeftButton),
    m_sensingArea(QuickUtils::instance().rootObject()),
    m_event(new QMouseEvent(QEvent::None, QPointF(), Qt::NoButton, Qt::NoButton, Qt::NoModifier))
{
    setAcceptedMouseButtons(m_acceptedButtons);
    setFiltersChildMouseEvents(true);
    setFlag(QQuickItem::ItemHasContents);

    QObject::connect(this, SIGNAL(enabledChanged()), this, SLOT(update()));

    if (!m_sensingArea)
        QObject::connect(&QuickUtils::instance(), SIGNAL(rootObjectChanged()), this, SLOT(update()));

    QGuiApplication::instance()->installEventFilter(this);
}

InverseMouseAreaType::~InverseMouseAreaType()
{
    if (m_event)
        delete m_event;
    m_event = 0;
}


/*!
  \internal
  Slot connected to enabledChanged signal to update the state of the handler.
 */
void InverseMouseAreaType::update()
{
    // update sensing area
    if (!m_sensingArea)
        m_sensingArea = QuickUtils::instance().rootObject();
    if (!isEnabled() || !isVisible())
        reset();
}

/*!
  \qmlproperty bool InverseMouseArea::pressed
  This property holds whether the acceptedButtons are pressed outside the area or not.
 */
bool InverseMouseAreaType::pressed() const
{
    return m_pressed;
}
/*!
  \qmlproperty Qt::MouseButtons InverseMouseArea::acceptedButtons
  This property holds the mouse buttons that the inverse mouse area reacts to.

  To specify that the InverseMouseArea will react to multiple buttons, Qt::MouseButtons
  flag values are combined using the "|" (or) operator:
  \qml
  InverseMouseArea { acceptedButtons: Qt.LeftButton | Qt.RightButton }
  \endqml
  To indicate that all possible mouse buttons are to be accepted, the special
  value 'Qt.AllButtons' may be used:
  \qml
  InverseMouseArea { acceptedButtons: Qt.AllButtons }
  \endqml
  The default value is Qt.LeftButton.
 */
Qt::MouseButtons InverseMouseAreaType::acceptedButtons() const
{
    return m_acceptedButtons;
}
void InverseMouseAreaType::setAcceptedButtons(Qt::MouseButtons buttons)
{
    if (m_acceptedButtons != buttons) {
        m_acceptedButtons = buttons;
        setAcceptedMouseButtons(m_acceptedButtons);
        Q_EMIT acceptedButtonsChanged();
    }
}

/*!
  \qmlproperty MouseButtons InverseMouseArea::pressedButtons
  This property holds the mouse buttons currently pressed.

  It contains a bitwise combination of:
  \list
    \li Qt.LeftButton
    \li Qt.RightButton
    \li Qt.MiddleButton
  \endlist
  Note: this property only handles buttons specified in acceptedButtons.
 */
Qt::MouseButtons InverseMouseAreaType::pressedButtons() const
{
    return m_event->buttons();
}

/*!
  \qmlproperty bool InverseMouseArea::propagateComposedEvents
  This property holds whether composed mouse events will automatically propagate
  to other MouseAreas and InverseMouseAreas that fall into the active InverseMouseArea's
  active area. If the property is true, all enabled InverseMouseAreas will get
  the ouse events that happen inside their sensed area. Mouse events occurred
  inside the mouse area will be triggered to other mouse areas.

  In the following example by pressing outside of the blue box will print "outer
  blue". When clicked inside the blue box, both "outer red", "outer green" and
  "inner blue" will be shown.
  \qml
  Item {
      width: 640; height: 480
      Column {
          spacing: 10
          Rectangle {
              width: 100; height: 30
              color: "red"
              InverseMouseArea {
                  anchors.fill: parent
                  propagateComposedEvents: true
                  onPressed: print("outer red")
              }
          }
          Rectangle {
              width: 100; height: 30
              color: "green"
              InverseMouseArea {
                  anchors.fill: parent
                  propagateComposedEvents: true
                  onPressed: print("outer green")
              }
          }
          Rectangle {
              width: 100; height: 30
              color: "blue"
              MouseArea {
                  anchors.fill: parent
                  onPressed: print("inner blue")
              }
              InverseMouseArea {
                  anchors.fill: parent
                  propagateComposedEvents: false
                  onPressed: print("outer blue")
              }
          }
      }
  }
  \endqml

  By modifying the code by setting propagateComposedEvents to false for the green
  box, the following output will be shown: when pressed outside of blue box
  "outer blue" will be printed; when pressed inside blue box, "outer green"
  wil lbe printed. This is due to the fact that InverseMouseArea is not active
  anymore inside its area, so the events will be captured by the inverse mouse
  area of the green box.

  By default, this property is false.

 */
bool InverseMouseAreaType::propagateComposedEvents() const
{
    return m_propagateEvents;
}
void InverseMouseAreaType::setPropagateComposedEvents(bool v)
{
    if (m_propagateEvents != v) {
        m_propagateEvents = v;
        Q_EMIT propagateComposedEventsChanged();
    }
}

/*!
  \qmlproperty Item InverseMouseArea::sensingArea
  This property holds the sensing area of the inverse mouse area. By default it
  is the root item but it can be set to any other area. The area can be reset
  to the root item by setting null to the property.
 */
QQuickItem *InverseMouseAreaType::sensingArea() const
{
    return m_sensingArea;
}
void InverseMouseAreaType::setSensingArea(QQuickItem *sensing)
{
    if (!sensing)
        sensing = QuickUtils::instance().rootObject();
    if (sensing != m_sensingArea) {
        m_sensingArea = sensing;
        reset();
        Q_EMIT sensingAreaChanged();
    }
}


/*!
  \internal
  Resets the state of the mouse area.
 */
void InverseMouseAreaType::reset()
{
    m_pressed = m_moved = false;
    *m_event = QMouseEvent(QEvent::None, QPointF(), Qt::NoButton, Qt::NoButton, Qt::NoModifier);
}

/*!
 * \internal
 * Saves the mouse event.
 */
void InverseMouseAreaType::saveEvent(const QMouseEvent &event)
{
    QPointF pos = mapToSensingArea(event.windowPos());
    *m_event = QMouseEvent(event.type(), pos, event.button(), event.buttons(), event.modifiers());
}

/*!
  \internal
  Maps the mouse point to the sensing area.
 */
QPointF InverseMouseAreaType::mapToSensingArea(const QPointF &point)
{
    return (m_sensingArea) ? m_sensingArea->mapFromScene(point) : QPointF();
}

/*!
 * \internal
 * Emits a signal asynchronously no matter of how it is being connected.
 */
void InverseMouseAreaType::asyncEmit(SignalType signal, bool isClick, bool wasHeld)
{
    // slight optimization, don't allocate event if the signal is not connected
    QMetaMethod metaSignal = QMetaMethod::fromSignal(signal);
    if (isSignalConnected(metaSignal)) {
        QQuickMouseEvent *ev = new QQuickMouseEvent(m_event->x(), m_event->y(),
                                                    m_event->button(), m_event->buttons(), m_event->modifiers(),
                                                    isClick, wasHeld);
        metaSignal.invoke(this, Qt::QueuedConnection, Q_ARG(QQuickMouseEvent*, ev));
    }
}

/*!
  \internal
  Handles mouse press event. Consumes event depending on the propagateEvent state.
 */
bool InverseMouseAreaType::mousePress(QMouseEvent *event)
{
    m_pressed = contains(event->windowPos());
    if (m_pressed && !(event->button() & m_acceptedButtons))
        m_pressed = false;
    if (m_pressed) {
        saveEvent(*event);
        Q_EMIT pressedChanged();
        Q_EMIT pressedButtonsChanged();
        asyncEmit(&InverseMouseAreaType::pressed);
        if (!m_propagateEvents)
            event->accept();
        return !m_propagateEvents;
    }
    return false;
}

/*!
 * \internal
 * Captures touch events and forwards them as left mouse button presses.
 */
bool InverseMouseAreaType::touchPressed(QTouchEvent *event)
{
    QList<QTouchEvent::TouchPoint> points = event->touchPoints();
    // check if any of the points fall into the area
    Q_FOREACH(const QTouchEvent::TouchPoint &point, points) {
        m_pressed = contains(point.scenePos());
        if (m_pressed) {
            saveEvent(QMouseEvent(QEvent::MouseButtonPress, point.scenePos(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier));
            Q_EMIT pressedChanged();
            Q_EMIT pressedButtonsChanged();
            asyncEmit(&InverseMouseAreaType::pressed);
            if (!m_propagateEvents)
                event->accept();
            return !m_propagateEvents;
        }
    }
    return false;
}

/*!
  \internal
  Handles mouse release and clicked composed event. Consumes event depending on
  the propagateEvent state. Does not consume events from inside the area.
 */
bool InverseMouseAreaType::mouseRelease(QMouseEvent *event)
{
    bool consume = !m_propagateEvents;
    if (m_pressed && contains(event->windowPos())) {
        // released outside (inside the sensing area)
        saveEvent(*event);
        m_pressed = false;
        asyncEmit(&InverseMouseAreaType::released, !m_moved);
        Q_EMIT pressedChanged();
        if (!m_moved)
            asyncEmit(&InverseMouseAreaType::clicked, true);
        m_moved = false;
    } else {
        // the release happened inside the area, which is outside of the active area
        reset();
        consume = false;
    }
    if (consume)
        event->accept();
    return consume;
}

/*!
 * \internal
 * Same as mouseReleased but for touch.
 */
bool InverseMouseAreaType::touchReleased(QTouchEvent *event)
{
    bool consume = !m_propagateEvents;
    QList<QTouchEvent::TouchPoint> points = event->touchPoints();
    // check if any of the points fall into the area
    Q_FOREACH(const QTouchEvent::TouchPoint &point, points) {
        if (m_pressed && contains(point.scenePos())) {
            saveEvent(QMouseEvent(QEvent::MouseButtonRelease, point.scenePos(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier));
            m_pressed = false;
            asyncEmit(&InverseMouseAreaType::released, !m_moved);
            Q_EMIT pressedChanged();
            if (!m_moved)
                asyncEmit(&InverseMouseAreaType::clicked, true);
            m_moved = false;
            break;
        } else {
            // the release happened inside the area, which is outside of the active area
            reset();
            consume = false;
        }
    }
    if (consume)
        event->accept();
    return consume;
}

/*!
  \internal
  Captures mouse move event. Consumes event depending on the propagateEvent state.
  Depending whether the mouse was moved after being pressed, the clicked composed
  event will not be emitted.
 */
bool InverseMouseAreaType::mouseMove(QMouseEvent *event)
{
    QPointF mappedPos = mapToSensingArea(event->windowPos());
    // use localPos as we saved the mapped position as
    if (m_pressed && (mappedPos != m_event->localPos())) {
        m_moved = true;
        if (!m_propagateEvents)
            event->accept();
    }
    return false;
}

/*!
 * \internal
 * Same as mouseMove but for touch events.
 */
bool InverseMouseAreaType::touchMoved(QTouchEvent *event)
{
    if (m_pressed) {
        QList<QTouchEvent::TouchPoint> points = event->touchPoints();
        // check if any of the points fall into the area
        Q_FOREACH(const QTouchEvent::TouchPoint &point, points) {
            QPointF mappedPos = mapToSensingArea(point.scenePos());
            // use localPos as we saved the mapped position as
            if (mappedPos != m_event->localPos()) {
                m_moved = true;
                if (!m_propagateEvents)
                    event->accept();
                break;
            }
        }
    }
    return false;
}

/*
 * Overrides QQuickItem's contains method.
 */
bool InverseMouseAreaType::contains(const QPointF &point) const
{
    QRectF oskRect = QGuiApplication::inputMethod()->keyboardRectangle();
    bool pointInArea = QQuickItem::contains(mapFromScene(point));
    bool pointInOSK = oskRect.contains(QuickUtils::instance().rootObject()->mapFromScene(point));
    bool pointOutArea = (m_sensingArea && m_sensingArea->contains(m_sensingArea->mapFromScene(point)));
    return !pointInArea && !pointInOSK && pointOutArea;
}

/*!
  \internal
  Filters mouse events.
 */
bool InverseMouseAreaType::eventFilter(QObject *obj, QEvent *ev)
{
    Q_UNUSED(obj)

    if (!isEnabled() || !isVisible())
        return false;
    switch (ev->type()) {
    case QEvent::MouseButtonPress: {
        return mousePress(static_cast<QMouseEvent*>(ev));
        break;
    }
    case QEvent::MouseButtonRelease: {
        return mouseRelease(static_cast<QMouseEvent*>(ev));
    }
    case QEvent::MouseMove: {
        return mouseMove(static_cast<QMouseEvent*>(ev));
    }
    case QEvent::TouchBegin: {
        return touchPressed(static_cast<QTouchEvent*>(ev));
    }
    case QEvent::TouchUpdate: {
        return touchMoved(static_cast<QTouchEvent*>(ev));
    }
    case QEvent::TouchEnd: {
        return touchReleased(static_cast<QTouchEvent*>(ev));
    }
    default:
        break;
    }

    return false;
}
