#include "inversemousearea.h"
#include <QtQuick/QQuickWindow>
#include <QtCore/QEvent>
#include <QtGui/QMouseEvent>
#include <QtQuick/private/qquickevents_p_p.h>
#include "quickutils.h"

#include <QDebug>

InverseMouseArea::InverseMouseArea(QQuickItem *parent) :
    QQuickItem(parent),
    m_pressed(false),
    m_moved(false),
    m_propagateEvents(false),
    m_pressedButtons(Qt::NoButton),
    m_acceptedButtons(Qt::AllButtons),
    m_lastPos(0,0)
{
    setAcceptedMouseButtons(m_acceptedButtons);
    //setFiltersChildMouseEvents(true);
    setFlag(QQuickItem::ItemHasContents);

    QObject::connect(this, SIGNAL(enabledChanged()), this, SLOT(update()));
}

void InverseMouseArea::update()
{
    if (!isEnabled())
        reset();
}

bool InverseMouseArea::pressed() const
{
    return m_pressed;
}
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

Qt::MouseButtons InverseMouseArea::pressedButtons() const
{
    return m_pressedButtons;
}

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


void InverseMouseArea::reset()
{
    m_pressed = m_moved = false;
    m_lastPos = QPointF();
    m_pressedButtons = Qt::NoButton;
}

QPointF InverseMouseArea::mapToRootItem(const QPointF &point)
{
    QQuickItem *root = QuickUtils::instance().rootObject();
    return (root) ? root->mapFromScene(point) : QPointF();
}

bool InverseMouseArea::mousePress(QMouseEvent *event)
{
    QPointF mappedPos = mapToRootItem(event->windowPos());
    m_pressed = !contains(mapFromScene(event->windowPos()));
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

bool InverseMouseArea::mouseRelease(QMouseEvent *event)
{
    bool consume = !m_propagateEvents;
    QPointF mappedPos = mapToRootItem(event->windowPos());
    if (m_pressed && !contains(mapFromScene(event->windowPos()))) {
        m_pressed = false;
        m_pressedButtons = Qt::NoButton;
        QQuickMouseEvent me(mappedPos.x(), mappedPos.y(), event->button(), event->buttons(), event->modifiers(), false, false);
        Q_EMIT released(&me);
        Q_EMIT pressedChanged();
        if (!m_moved)
            Q_EMIT clicked(&me);
        m_moved = false;
    } else {
        // the release happened inside the area, therefore reset state
        reset();
        consume = false;
    }
    if (consume)
        event->accept();
    return consume;
}

bool InverseMouseArea::mouseMove(QMouseEvent *event)
{
    QPointF mappedPos = mapToRootItem(event->windowPos());
    if (m_pressed && (mappedPos != m_lastPos))
        m_moved = true;
    if (!m_propagateEvents)
        event->accept();
    return !m_propagateEvents;
}

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
