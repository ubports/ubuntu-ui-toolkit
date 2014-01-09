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
public:
    explicit UCMouse(QObject *parent = 0);

    static UCMouse *qmlAttachedProperties(QObject *owner);

    bool isEnabled() const;
    virtual void setEnabled(bool enabled);
    Qt::MouseButtons acceptedButtons() const;
    bool hoverEnabled() const;

Q_SIGNALS:
    void enabledChanged();
    void acceptedButtonsChanged();
    void hoverEnabledChanged();

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

protected:
    QQuickItem *m_owner;
    QBasicTimer m_pressAndHoldTimer;
    QPointF m_lastPos;
    QPointF m_lastScenePos;
    Qt::MouseButton m_lastButton;
    Qt::MouseButtons m_lastButtons;
    Qt::KeyboardModifiers m_lastModifiers;
    Qt::MouseButtons m_pressedButtons;

    bool m_ownerHandlesMouse: 1;
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
    bool isMouseEvent(QEvent::Type type);
    bool isHoverEvent(QEvent::Type type);
    virtual bool eventFilter(QObject *, QEvent *);
    bool contains(QMouseEvent *mouse);

    bool m_excludeOSK:1;
};
QML_DECLARE_TYPE(UCInverseMouse)
QML_DECLARE_TYPEINFO(UCInverseMouse, QML_HAS_ATTACHED_PROPERTIES)

#endif // UCMOUSE_H
