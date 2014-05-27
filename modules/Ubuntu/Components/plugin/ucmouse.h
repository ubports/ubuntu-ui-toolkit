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

#ifndef UCMOUSE_H
#define UCMOUSE_H

#include <QtCore/QObject>
#include <QtQml>
#include <QtQuick/QQuickItem>
#include <private/qquickevents_p_p.h>
#include <QtCore/qbasictimer.h>

class ForwardedEvent : public QEvent {
public:
    enum EventType {
        MousePress,
        MouseRelease,
        MouseMove,
        MouseDblClick,
        HoverEnter,
        HoverExit,
        MouseClick,
        MouseLongPress,
    };
    ForwardedEvent(EventType type, QQuickItem *sender, QEvent *originalEvent, QQuickMouseEvent *quickEvent)
        : QEvent((QEvent::Type)m_eventBase)
        , m_subType(type)
        , m_sender(sender)
        , m_originalEvent(originalEvent)
        , m_quickEvent(quickEvent)
    {
        setAccepted(false);
    }

    static void registerForwardedEvent();

    EventType subType() const { return m_subType; }
    QQuickItem *sender() const { return m_sender; }
    QQuickMouseEvent *quickEvent() const { return m_quickEvent; }
    QEvent *originalEvent() const { return m_originalEvent; }
    static QEvent::Type baseType() { return m_eventBase; }
private:
    EventType m_subType;
    QPointer<QQuickItem> m_sender;
    QEvent *m_originalEvent;
    QPointer<QQuickMouseEvent> m_quickEvent;
    static QEvent::Type m_eventBase;
};

class UCMouse : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(Qt::MouseButtons acceptedButtons READ acceptedButtons NOTIFY acceptedButtonsChanged)
    Q_PROPERTY(bool hoverEnabled READ hoverEnabled NOTIFY hoverEnabledChanged)
    Q_PROPERTY(int clickAndHoldThreshold READ clickAndHoldThreshold WRITE setClickAndHoldThreshold NOTIFY clickAndHoldThresholdChanged)
    Q_PROPERTY(QQmlListProperty<QQuickItem> forwardTo READ forwardTo)
    Q_PROPERTY(Priority priority READ priority WRITE setPriority NOTIFY priorityChanged)
    Q_ENUMS(Priority)
public:
    enum Priority {
        BeforeItem,
        AfterItem
    };

    explicit UCMouse(QObject *parent = 0);

    static UCMouse *qmlAttachedProperties(QObject *owner);

    bool isEnabled() const;
    virtual void setEnabled(bool enabled);
    Qt::MouseButtons acceptedButtons() const;
    bool hoverEnabled() const;
    int clickAndHoldThreshold() const;
    void setClickAndHoldThreshold(int threshold);
    QQmlListProperty<QQuickItem> forwardTo();
    Priority priority() const;
    virtual void setPriority(Priority priority);

Q_SIGNALS:
    void enabledChanged();
    void acceptedButtonsChanged();
    void hoverEnabledChanged();
    void clickAndHoldThresholdChanged();
    void priorityChanged();

    void pressed(QQuickMouseEvent *mouse, QQuickItem *host);
    void released(QQuickMouseEvent *mouse, QQuickItem *host);
    void clicked(QQuickMouseEvent *mouse, QQuickItem *host);
    void pressAndHold(QQuickMouseEvent *mouse, QQuickItem *host);
    void doubleClicked(QQuickMouseEvent *mouse, QQuickItem *host);
    void positionChanged(QQuickMouseEvent *mouse, QQuickItem *host);
    void entered(QQuickMouseEvent *event, QQuickItem *host);
    void exited(QQuickMouseEvent *event, QQuickItem *host);

protected:
    virtual bool eventFilter(QObject *, QEvent *);
    virtual void timerEvent(QTimerEvent *event);
    virtual bool mouseEvents(QObject *target, QMouseEvent *event);
    virtual bool hoverEvents(QObject *target, QHoverEvent *event);
    virtual bool forwardedEvents(ForwardedEvent *event);
    virtual bool hasAttachedFilter(QQuickItem *item);

    void setHovered(bool hovered, QEvent *hoverEvent);
    bool mousePressed(QMouseEvent *event);
    bool mouseReleased(QMouseEvent *event);
    bool mouseDblClick(QMouseEvent *event);
    bool mouseMoved(QMouseEvent *event);
    bool hoverEntered(QHoverEvent *event);
    bool hoverMoved(QHoverEvent *event);
    bool hoverExited(QHoverEvent *event);

    virtual void saveEvent(QMouseEvent *event);
    bool isDoubleClickConnected();
    bool isMouseEvent(QEvent::Type type);
    bool isHoverEvent(QEvent::Type type);
    bool forwardEvent(ForwardedEvent::EventType type, QEvent *event, QQuickMouseEvent *quickEvent);

protected:
    QQuickItem *m_owner;
    QList<QQuickItem*> m_forwardList;
    QBasicTimer m_pressAndHoldTimer;
    QRectF m_toleranceArea;
    QPointF m_lastPos;
    QPointF m_lastScenePos;
    Qt::MouseButton m_lastButton;
    Qt::MouseButtons m_lastButtons;
    Qt::KeyboardModifiers m_lastModifiers;
    Qt::MouseButtons m_pressedButtons;
    Priority m_priority;
    int m_moveThreshold;

    bool m_signalWhenContains:1;
    bool m_enabled: 1;
    bool m_moved:1;
    bool m_longPress:1;
    bool m_hovered:1;
    bool m_doubleClicked:1;
};
QML_DECLARE_TYPEINFO(UCMouse, QML_HAS_ATTACHED_PROPERTIES)

extern const int DefaultPressAndHoldDelay;

#endif // UCMOUSE_H
