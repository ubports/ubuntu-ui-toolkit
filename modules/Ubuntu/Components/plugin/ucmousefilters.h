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
#include <private/qquickevents_p_p.h>
#include <QtCore/qbasictimer.h>

class UCExtendedMouseEvent : public QQuickMouseEvent
{
    Q_OBJECT
    Q_PROPERTY(bool pointInInputArea READ pointInInputArea)
public:
    UCExtendedMouseEvent(QPointF pos, Qt::MouseButton button, Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers
                  , bool overOsk, bool isClick, bool wasHeld)
        : QQuickMouseEvent(pos.x(), pos.y(), button, buttons, modifiers, isClick, wasHeld)
        , m_overOsk(overOsk)
    {
        // contrary to the original class, do not accept events by default
        setAccepted(false);
    }
    bool pointInInputArea() const;
private:
    bool m_overOsk;
};
QML_DECLARE_TYPE(UCExtendedMouseEvent)

class QQuickItem;
class UCMouse : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(Qt::MouseButtons acceptedButtons READ acceptedButtons NOTIFY acceptedButtonsChanged)
    Q_PROPERTY(bool hoverEnabled READ hoverEnabled NOTIFY hoverEnabledChanged)
    Q_PROPERTY(qreal mouseMoveThreshold READ mouseMoveThreshold WRITE setMouseMoveThreshold NOTIFY mouseMoveThresholdChanged)
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
    qreal mouseMoveThreshold() const;
    void setMouseMoveThreshold(qreal threshold);
    QQmlListProperty<QQuickItem> forwardTo();
    Priority priority() const;
    void setPriority(Priority priority);

Q_SIGNALS:
    void enabledChanged();
    void acceptedButtonsChanged();
    void hoverEnabledChanged();
    void mouseMoveThresholdChanged();
    void priorityChanged();

    void pressed(UCExtendedMouseEvent *mouse);
    void released(UCExtendedMouseEvent *mouse);
    void clicked(UCExtendedMouseEvent *mouse);
    void pressAndHold(UCExtendedMouseEvent *mouse);
    void doubleClicked(UCExtendedMouseEvent *mouse);
    void positionChanged(UCExtendedMouseEvent *mouse);
    void entered(UCExtendedMouseEvent *event);
    void exited(UCExtendedMouseEvent *event);

protected:
    virtual bool eventFilter(QObject *, QEvent *);
    virtual void timerEvent(QTimerEvent *event);

    void setHovered(bool hovered);
    virtual bool mouseEvents(QObject *target, QMouseEvent *event);
    virtual bool hoverEvents(QObject *target, QHoverEvent *event);
    bool mousePressed(QMouseEvent *event);
    bool mouseReleased(QMouseEvent *event);
    bool mouseDblClick(QMouseEvent *event);
    bool mouseMoved(QMouseEvent *event);
    bool hoverEntered(QHoverEvent *event);
    bool hoverMoved(QHoverEvent *event);
    bool hoverExited(QHoverEvent *event);

    virtual void saveEvent(QMouseEvent *event);
    bool isDoubleClickConnected();
    bool pointInOSK(const QPointF &point);
    bool isMouseEvent(QEvent::Type type);
    bool isHoverEvent(QEvent::Type type);
    void forwardEvent(QEvent *event);

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
    qreal m_moveThreshold;

    bool m_enabled: 1;
    bool m_moved:1;
    bool m_longPress:1;
    bool m_hovered:1;
    bool m_doubleClicked:1;
    bool m_pointInOSK:1;
};
QML_DECLARE_TYPEINFO(UCMouse, QML_HAS_ATTACHED_PROPERTIES)

class UCInverseMouse : public UCMouse {
    Q_OBJECT
    Q_PROPERTY(bool excludeInputArea READ excludeInputArea WRITE setExcludeInputArea NOTIFY excludeInputAreaChanged)
public:
    explicit UCInverseMouse(QObject *parent = 0);

    static UCInverseMouse *qmlAttachedProperties(QObject *owner);

    virtual void setEnabled(bool enabled);
    bool excludeInputArea() const;
    void setExcludeInputArea(bool value);

Q_SIGNALS:
    void excludeInputAreaChanged();

protected:
    QMouseEvent mapMouseToOwner(QObject *target, QMouseEvent* event);
    QHoverEvent mapHoverToOwner(QObject *target, QHoverEvent *event);
    virtual bool eventFilter(QObject *, QEvent *);
    virtual bool mouseEvents(QObject *target, QMouseEvent *event);
    virtual bool hoverEvents(QObject *target, QHoverEvent *event);
    bool contains(QMouseEvent *mouse);

    bool m_excludeOSK:1;
};
QML_DECLARE_TYPE(UCInverseMouse)
QML_DECLARE_TYPEINFO(UCInverseMouse, QML_HAS_ATTACHED_PROPERTIES)

#endif // UCMOUSE_H
