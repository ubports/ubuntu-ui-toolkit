#ifndef INVERSEMOUSEAREA_H
#define INVERSEMOUSEAREA_H

#include <QtQuick/QQuickItem>

class QQuickMouseEvent;
class QMouseEvent;
class InverseMouseArea : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(bool pressed READ pressed NOTIFY pressedChanged)
    Q_PROPERTY(Qt::MouseButtons acceptedButtons READ acceptedButtons WRITE setAcceptedButtons NOTIFY acceptedButtonsChanged)
    Q_PROPERTY(Qt::MouseButtons pressedButtons READ pressedButtons NOTIFY pressedButtonsChanged)
    Q_PROPERTY(bool propagateComposedEvents READ propagateComposedEvents WRITE setPropagateComposedEvents NOTIFY propagateComposedEventsChanged)
public:
    explicit InverseMouseArea(QQuickItem *parent = 0);

protected:
    bool eventFilter(QObject *obj, QEvent *ev);
    virtual void itemChange(ItemChange change, const ItemChangeData &data);

private: // getter/setter
    bool pressed() const;
    Qt::MouseButtons acceptedButtons() const;
    void setAcceptedButtons(Qt::MouseButtons buttons);
    Qt::MouseButtons pressedButtons() const;
    bool propagateComposedEvents() const;
    void setPropagateComposedEvents(bool v);

private:
    void reset();
    QPointF mapToRootItem(const QPointF &point);
    bool mousePress(QMouseEvent *event);
    bool mouseRelease(QMouseEvent *event);
    bool mouseMove(QMouseEvent *event);

Q_SIGNALS:
    void pressedChanged();
    void acceptedButtonsChanged();
    void pressedButtonsChanged();
    void propagateComposedEventsChanged();

    void pressed(QQuickMouseEvent *mouse);
    void released(QQuickMouseEvent *mouse);
    void clicked(QQuickMouseEvent *mouse);
    
private Q_SLOTS:
    void update();
    
private:
    bool m_pressed;
    bool m_moved;
    bool m_propagateEvents;
    Qt::MouseButtons m_pressedButtons;
    Qt::MouseButtons m_acceptedButtons;
    QPointF m_lastPos;
};

#endif // INVERSEMOUSEAREA_H
