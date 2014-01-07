#include "ucmousefilters.h"
#include <QtQml/QQmlInfo>
#include <QtQuick/QQuickItem>
#include <QtGui/QGuiApplication>
#include <private/qqmlglobal_p.h>
#include <QtQuick/private/qquickmousearea_p.h>
#include "inversemouseareatype.h"

#include "quickutils.h"
#include "ucunits.h"
#include "i18n.h"
#include <QDebug>

// keep in sync with QQuickMouseArea PressAndHoldDelay
const int DefaultPressAndHoldDelay = 800;

/*!
   \qmltype ExtendedMouseEvent
   \instantiates UCExtendedMouseEvent
   \inqmlmodule Ubuntu.Components 0.1
   \ingroup ubuntu
   \brief Mouse event extending \l{http://qt-project.org/doc/qt-5.1/qtquick/qml-qtquick2-mouseevent.html}{QML MouseEvent}.
 */

/*!
    \qmlproperty bool ExtendedMouseEvent::overInputArea
    The proeprty specifies whether the mouse event happened over the input panel
    or not. The property has meaning only devices where input method is available.
  */
bool UCExtendedMouseEvent::overInputArea() const
{
    return m_overOsk;
}

/*!
   \qmltype Mouse
   \instantiates UCMouse
   \inqmlmodule Ubuntu.Components 0.1
   \ingroup ubuntu
   \brief Attached object filtering mouse events occured inside the owner.
 */

UCMouse::UCMouse(QObject *parent)
    : QObject(parent)
    , m_owner(static_cast<QQuickItem*>(parent))
    , m_pressedButtons(Qt::NoButton)
    , m_pressAndHoldDelay(DefaultPressAndHoldDelay)
    , m_enabled(false)
    , m_moved(false)
    , m_longPress(false)
    , m_hovered(false)
    , m_doubleClicked(false)
    , m_pointInOSK(false)
{
}

UCMouse *UCMouse::qmlAttachedProperties(QObject *owner)
{
    if (!qobject_cast<QQuickItem*>(owner)) {
        qmlInfo(owner) << "Mouse filter can only be attached to Items";
        return 0;
    }
    UCMouse *filter = new UCMouse(owner);
    filter->setEnabled(true);
    return filter;
}

bool UCMouse::eventFilter(QObject *target, QEvent *event)
{
    switch (event->type()) {
    case QEvent::MouseButtonPress:
    {
        QMouseEvent *mouse = static_cast<QMouseEvent*>(event);
        return mousePressed(mouse);
    } break;
    case QEvent::MouseButtonRelease:
    {
        QMouseEvent *mouse = static_cast<QMouseEvent*>(event);
        return mouseReleased(mouse);
    } break;
    case QEvent::MouseButtonDblClick:
    {
        QMouseEvent *mouse = static_cast<QMouseEvent*>(event);
        return mouseDblClick(mouse);
    } break;
    case QEvent::MouseMove:
    {
        QMouseEvent *mouse = static_cast<QMouseEvent*>(event);
        return mouseMoved(mouse);
    } break;
    case QEvent::HoverEnter:
    {
        QHoverEvent *hover = static_cast<QHoverEvent*>(event);
        return hoverEntered(hover);
    } break;
    case QEvent::HoverMove:
    {
        QHoverEvent *hover = static_cast<QHoverEvent*>(event);
        return hoverMoved(hover);
    } break;
    case QEvent::HoverLeave:
    {
        QHoverEvent *hover = static_cast<QHoverEvent*>(event);
        return hoverExited(hover);
    } break;
    default:
        // just to satisfy switch-case warnings
        break;
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
        m_pressAndHoldTimer.start(m_pressAndHoldDelay, this);

        return mev.isAccepted();
    }

    event->ignore();
    return false;
}

bool UCMouse::mouseMoved(QMouseEvent *event)
{
    if (m_pressedButtons) {
        saveEvent(event);

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
                && !m_longPress && !m_doubleClicked;
        UCExtendedMouseEvent mev(m_lastPos, m_lastButton, m_lastButtons, m_lastModifiers,
                         m_pointInOSK, isClicked, m_longPress);
        Q_EMIT released(&mev);
        // remove button from both inside and outside press
        m_pressedButtons &= ~m_lastButton;
        if (isClicked) {
            // emit clicked
            Q_EMIT clicked(&mev);
        }
        event->setAccepted(mev.isAccepted());

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

    // for test purposes only; simulate OSK area on environments where OSK is not present
    if (QString(getenv("MOUSEFILTER_OSK_TEST_AREA")).toLower() == "true") {
        QQuickItem *mainItem = QuickUtils::instance().rootItem(m_owner);
        if (mainItem) {
            QRect mainRect(0, 0, mainItem->width(), mainItem->height());
            oskRect = QRectF(0, mainRect.height() - UCUnits::instance().gu(29),
                             mainRect.width(), UCUnits::instance().gu(29));
        }
    }
    return oskRect.contains(scenePoint);
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
    if (enabled != m_enabled) {
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
    return m_owner->acceptedMouseButtons();
}

/*!
    \qmlproperty bool Mouse::hoverEnabled
    \readonly
    The property reports whether the owner accepts hover events or not. When
    events are accepted \l entered(), \l positionChanged() and \l exited() signals
    containing the mouse cursor position.
  */
bool UCMouse::hoverEnabled() const
{
    return m_owner->acceptHoverEvents();
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

   Same as Mouse attached object, provides mouse event filtering capabilities
   occurring outside of the owner's area.
 */
UCInverseMouse::UCInverseMouse(QObject *parent)
    : UCMouse(parent)
    , m_excludeOSK(false)
{
}

UCInverseMouse *UCInverseMouse::qmlAttachedProperties(QObject *owner)
{
    if (!qobject_cast<QQuickItem*>(owner)) {
        qmlInfo(owner) << "InverseMouse filter can only be attached to Items";
        return 0;
    }
    UCInverseMouse *filter = new UCInverseMouse(owner);
    filter->setEnabled(true);
    qDebug() << filter->dynamicPropertyNames();
    return filter;
}

/*!
   \qmlproperty InverseMouse::excludeInputArea
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

bool UCInverseMouse::isMouseEvent(QEvent::Type type)
{
    return (type == QEvent::MouseButtonPress) || (type == QEvent::MouseButtonRelease)
            || (type == QEvent::MouseButtonDblClick) || (type == QEvent::MouseMove);
}

bool UCInverseMouse::isHoverEvent(QEvent::Type type)
{
    return (type == QEvent::HoverEnter) || (type == QEvent::HoverMove) || (type == QEvent::HoverLeave);
}

bool UCInverseMouse::eventFilter(QObject *target, QEvent *event)
{
    // exclude MouseArea and InverseMouseArea targets
    if ((target != m_owner) && !qobject_cast<QQuickMouseArea*>(target) && !qobject_cast<InverseMouseAreaType*>(target)) {

        if (isMouseEvent(event->type())) {
            QMouseEvent mouse(mapMouseToOwner(target, static_cast<QMouseEvent*>(event)));
            if (!contains(&mouse)) {
                return QObject::eventFilter(target, event);
            }
            switch (event->type()) {
            case QEvent::MouseButtonPress:
            {
                return mousePressed(&mouse);
            } break;
            case QEvent::MouseButtonRelease:
            {
                return mouseReleased(&mouse);
            } break;
            case QEvent::MouseButtonDblClick:
            {
                return mouseDblClick(&mouse);
            } break;
            case QEvent::MouseMove:
            {
                return mouseMoved(&mouse);
            } break;
            default: break;
            }
        } else if (isHoverEvent(event->type())) {
            QHoverEvent hover(mapHoverToOwner(target, static_cast<QHoverEvent*>(event)));
            switch (event->type()) {
            case QEvent::HoverEnter:
            {
                return hoverEntered(&hover);
            } break;
            case QEvent::HoverMove:
            {
                return hoverMoved(&hover);
            } break;
            case QEvent::HoverLeave:
            {
                return hoverExited(&hover);
            } break;
            default:
                // just to satisfy switch-case warnings
                break;
            }
        }
    }
    return QObject::eventFilter(target, event);
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
    if (m_enabled != enabled) {
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

