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
#ifndef INVERSEMOUSEAREATYPE_H
#define INVERSEMOUSEAREATYPE_H

#include <QtQuick/QQuickItem>

class QQuickMouseEvent;
class QMouseEvent;
class InverseMouseAreaType : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(bool pressed READ pressed NOTIFY pressedChanged)
    Q_PROPERTY(Qt::MouseButtons acceptedButtons READ acceptedButtons WRITE setAcceptedButtons NOTIFY acceptedButtonsChanged)
    Q_PROPERTY(Qt::MouseButtons pressedButtons READ pressedButtons NOTIFY pressedButtonsChanged)
    Q_PROPERTY(bool propagateComposedEvents READ propagateComposedEvents WRITE setPropagateComposedEvents NOTIFY propagateComposedEventsChanged)
    Q_PROPERTY(QQuickItem *sensingArea READ sensingArea WRITE setSensingArea NOTIFY sensingAreaChanged)
public:
    explicit InverseMouseAreaType(QQuickItem *parent = 0);

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
    QQuickItem *sensingArea() const;
    void setSensingArea(QQuickItem *sensing);

private:
    void reset();
    QQuickItem *sensing();
    QPointF mapToSensingArea(const QPointF &point);
    bool pointInSensingArea(const QPointF &point);
    bool pointInOSK(const QPointF &point);
    bool mousePress(QMouseEvent *event);
    bool mouseRelease(QMouseEvent *event);
    bool mouseMove(QMouseEvent *event);
    // grab touch events too
    bool touchPressed(QTouchEvent *event);
    bool touchMoved(QTouchEvent *event);
    bool touchReleased(QTouchEvent *event);

Q_SIGNALS:
    void pressedChanged();
    void acceptedButtonsChanged();
    void pressedButtonsChanged();
    void propagateComposedEventsChanged();
    void sensingAreaChanged();

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
    QQuickItem *m_sensingArea;
};

#endif // INVERSEMOUSEAREATYPE_H
