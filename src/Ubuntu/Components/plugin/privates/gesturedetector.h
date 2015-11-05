/*
 * Copyright 2015 Canonical Ltd.
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
 * Authors: Zsombor Egri <zsombor.egri@canonical.com>
 */

#ifndef GESTUREDETECTOR_H
#define GESTUREDETECTOR_H

#include <QtCore/QObject>
#include <QtCore/QPointF>
#include <QtCore/QList>

/*
 * A simple gesture detection filter class that can be used in components to detect
 * various gestures. Yet swipe from bottom up is the only gesture handled.
 * It does not grab or consume the event from the environment, acts as a filter.
 */
class QTouchEvent;
class QQuickItem;
class GestureDetector : public QObject
{
    Q_OBJECT
public:
    enum Status {
        Ready,
        Started,
        Detected,
        Completed
    };
    explicit GestureDetector(QObject *parent = 0);
    ~GestureDetector();

    bool isDetecting();

    void setItemFilter(QObject *item);
    void removeItemFilter(QObject *item);

    bool handleTouchEvent(QObject *target, QTouchEvent *event);

Q_SIGNALS:
    void statusChanged(Status status);

    void bottomUpSwipeDetected();

public Q_SLOTS:

protected:
    bool eventFilter(QObject *target, QEvent *event);

    void setStatus(Status status);

private:
    QList<QObject*> m_filteredItems;
    QPointF m_startPoint;
    QQuickItem *m_owner;
    Status m_status;
    bool m_bottomUpSwipeDetected:1;
};

#endif // GESTUREDETECTOR_H
