#include "inversemousearea.h"
#include <QtQuick/QQuickWindow>
#include <QtCore/QEvent>
#include <QtGui/QMouseEvent>
#include <QtQuick/private/qquickevents_p_p.h>
#include "quickutils.h"

/*!
 * \qmltype InverseMouseArea
 * \inmodule Ubuntu.Components 0.1
 * \ingroup ubuntu
 * \brief The InverseMouseArea captures mouse events happening outside of a given
 * area.
 *
 * A typical use case is the handling the hiding of a popup or tooltip when the
 * user presses or taps outside of the popup or tooltip area.
 *
 * Popup.qml
 * \qml
 * import QtQuick 2.0
 * import Ubuntu.Components 0.1
 *
 * Rectangle {
 *     anchors.centerIn: parent
 *     width: 200; height: 200
 *     color: "darkgray"
 *     radous: 10
 *     InverseMouseArea {
 *        anchors.fill: parent
 *        acceptedButtons: Qt.LeftButton
 *        onPressed: parent.destroy()
 *     }
 * }
 * \endqml
 *
 * In the Popup above the mouse presses happening outside the area will cause
 * closing and destroying the popup.
 *
 * \qml
 * import QtQuick 2.0
 * import Ubuntu.Components 0.1
 *
 * Item {
 *     width: 640
 *     height: 480
 *     Button {
 *         id: button
 *         text: "Press me"
 *         onClicked: {
 *             var component = Qt.createComponent("Popup.qml");
 *             var obj = component.create(parent);
 *             obj.visible = true;
 *         }
 *     }
 * }
 * \endqml
 *
 */

InverseMouseArea::InverseMouseArea(QQuickItem *parent) :
    QQuickItem(parent),
    m_pressed(false),
    m_moved(false),
    m_propagateEvents(false),
    m_pressedButtons(Qt::NoButton),
    m_acceptedButtons(Qt::LeftButton),
    m_lastPos(0,0)
{
    setAcceptedMouseButtons(m_acceptedButtons);
    //setFiltersChildMouseEvents(true);
    setFlag(QQuickItem::ItemHasContents);

    QObject::connect(this, SIGNAL(enabledChanged()), this, SLOT(update()));
}

/*
 * Slot connected to enabledChanged signal to update the state of the handler.
 */
void InverseMouseArea::update()
{
    if (!isEnabled())
        reset();
}

/*!
 * \qmlproperty bool InverseMouseArea::pressed
 * This property holds whether the acceptedButtons are pressed outside the area or not.
 */
bool InverseMouseArea::pressed() const
{
    return m_pressed;
}
/*!
 * \qmlproperty Qt::MouseButtons InverseMouseArea::acceptedButtons
 * This property holds the mouse buttons that the inverse mouse area reacts to.
 *
 * To specify that the InverseMouseArea will react to multiple buttons, Qt::MouseButtons
 * flag values are combined using the "|" (or) operator:
 * \qml
 * InverseMouseArea { acceptedButtons: Qt.LeftButton | Qt.RightButton }
 * \endqml
 * To indicate that all possible mouse buttons are to be accepted, the special
 * value 'Qt.AllButtons' may be used:
 * \qml
 * InverseMouseArea { acceptedButtons: Qt.AllButtons }
 * \endqml
 * The default value is Qt.LeftButton.
 */
Qt::MouseButtons InverseMouseArea::acceptedButtons() const
{
    return m_acceptedButtons;
}
void InverseMouseArea::setAcceptedButtons(Qt::MouseButtons buttons)
{
    if (m_acceptedButtons != buttons) {
        m_acceptedButtons = buttons;
        setAcceptedMouseButtons(m_acceptedButtons);
        Q_EMIT acceptedButtonsChanged();
    }
}

/*!
 * \qmlproperty MouseButtons InverseMouseArea::pressedButtons
 * This property holds the mouse buttons currently pressed.
 *
 * It contains a bitwise combination of:
 * \list
 *   \li Qt.LeftButton
 *   \li Qt.RightButton
 *   \li Qt.MiddleButton
 * \endlist
 * Note: this property only handles buttons specified in acceptedButtons.
 */
Qt::MouseButtons InverseMouseArea::pressedButtons() const
{
    return m_pressedButtons;
}

/*!
 * \qmlproperty bool InverseMouseArea::propagateComposedEvents
 * This property holds whether composed mouse events will automatically propagate
 * to other MouseAreas and InverseMouseAreas that fall into the active InverseMouseArea's
 * active area. If the property is true, all enabled InverseMouseAreas will get
 * the ouse events that happen inside their sensed area. Mouse events occurred
 * inside the mouse area will be triggered to other mouse areas.
 *
 * In the following example by pressing outside of the blue box will print "outer
 * blue". When clicked inside the blue box, both "outer red", "outer green" and
 * "inner blue" will be shown.
 * \qml
 * Item {
 *     width: 640; height: 480
 *     Column {
 *         spacing: 10
 *         Rectangle {
 *             width: 100; height: 30
 *             color: "red"
 *             InverseMouseArea {
 *                 anchors.fill: parent
 *                 propagateComposedEvents: true
 *                 onPressed: print("outer red")
 *             }
 *         }
 *         Rectangle {
 *             width: 100; height: 30
 *             color: "green"
 *             InverseMouseArea {
 *                 anchors.fill: parent
 *                 propagateComposedEvents: true
 *                 onPressed: print("outer green")
 *             }
 *         }
 *         Rectangle {
 *             width: 100; height: 30
 *             color: "blue"
 *             MouseArea {
 *                 anchors.fill: parent
 *                 onPressed: print("inner blue")
 *             }
 *             InverseMouseArea {
 *                 anchors.fill: parent
 *                 propagateComposedEvents: false
 *                 onPressed: print("outer blue")
 *             }
 *         }
 *     }
 * }
 * \endqml
 *
 * By modifying the code by setting propagateComposedEvents to false for the green
 * box, the following output will be shown: when pressed outside of blue box
 * "outer blue" will be printed; when pressed inside blue box, "outer green"
 * wil lbe printed. This is due to the fact that InverseMouseArea is not active
 * anymore inside its area, so the events will be captured by the inverse mouse
 * area of the green box.
 *
 * By default, this property is false.
 *
 */
bool InverseMouseArea::propagateComposedEvents() const
{
    return m_propagateEvents;
}
void InverseMouseArea::setPropagateComposedEvents(bool v)
{
    if (m_propagateEvents != v) {
        m_propagateEvents = v;
        Q_EMIT propagateComposedEventsChanged();
    }
}

/*!
 * \internal
 * Resets the state of the mouse area.
 */
void InverseMouseArea::reset()
{
    m_pressed = m_moved = false;
    m_lastPos = QPointF();
    m_pressedButtons = Qt::NoButton;
}

/*
 * Maps the mouse point to the root item.
 */
QPointF InverseMouseArea::mapToRootItem(const QPointF &point)
{
    QQuickItem *root = QuickUtils::instance().rootObject();
    return (root) ? root->mapFromScene(point) : QPointF();
}

/*
 * Handles mouse press event. Consumes event depending on the propagateEvent state.
 */
bool InverseMouseArea::mousePress(QMouseEvent *event)
{
    QPointF mappedPos = mapToRootItem(event->windowPos());
    m_pressed = !contains(mapFromScene(event->windowPos()));
    if (m_pressed && !(event->button() & m_acceptedButtons))
        m_pressed = false;
    if (m_pressed) {
        m_pressedButtons = event->buttons();
        m_lastPos = mappedPos;
        Q_EMIT pressedChanged();
        Q_EMIT pressedButtonsChanged();
        QQuickMouseEvent me(mappedPos.x(), mappedPos.y(), event->button(), event->buttons(), event->modifiers(), true, false);
        Q_EMIT pressed(&me);
        if (!m_propagateEvents)
            event->accept();
        return !m_propagateEvents;
    }
    return false;
}

/*
 * Handles mouse release and clicked composed event. Consumes event depending on
 * the propagateEvent state. Does not consume events from inside the area.
 */
bool InverseMouseArea::mouseRelease(QMouseEvent *event)
{
    bool consume = !m_propagateEvents;
    QPointF mappedPos = mapToRootItem(event->windowPos());
    if (m_pressed && !contains(mapFromScene(event->windowPos()))) {
        // released outside (inside the sensing area)
        m_pressed = false;
        m_pressedButtons = Qt::NoButton;
        QQuickMouseEvent me(mappedPos.x(), mappedPos.y(), event->button(), event->buttons(), event->modifiers(), false, false);
        Q_EMIT released(&me);
        Q_EMIT pressedChanged();
        if (!m_moved)
            Q_EMIT clicked(&me);
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

/*
 * Captures mouse move event. Consumes event depending on the propagateEvent state.
 * Depending whether the mouse was moved after being pressed, the clicked composed
 * event will not be emitted.
 */
bool InverseMouseArea::mouseMove(QMouseEvent *event)
{
    QPointF mappedPos = mapToRootItem(event->windowPos());
    if (m_pressed && (mappedPos != m_lastPos))
        m_moved = true;
    if (!m_propagateEvents)
        event->accept();
    return !m_propagateEvents;
}

/*
 * Filters mouse events.
 */
bool InverseMouseArea::eventFilter(QObject *obj, QEvent *ev)
{
    Q_UNUSED(obj)

    if (!isEnabled() || !isVisible())
        return false;
    switch (ev->type()) {
    case QEvent::MouseButtonPress: {
        return mousePress(static_cast<QMouseEvent*>(ev));
    }
    case QEvent::MouseButtonRelease: {
        return mouseRelease(static_cast<QMouseEvent*>(ev));
    }
    case QEvent::MouseMove: {
        return mouseMove(static_cast<QMouseEvent*>(ev));
    }
    default:
        break;
    }

    return false;
}

/*
 * Catches item specific changes such as scene and visible change.
 */
void InverseMouseArea::itemChange(ItemChange change, const ItemChangeData &data)
{
    switch (change) {
    case ItemSceneChange: {
        QQuickWindow *oldWindow = window();
        if (oldWindow)
            oldWindow->removeEventFilter(this);

        reset();

        if (data.window)
            data.window->installEventFilter(this);
        break;
    }
    case ItemVisibleHasChanged: {
        if (!isVisible())
            reset();
        break;
    }
    default:
        break;
    }

    QQuickItem::itemChange(change, data);
}
