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
 */

#ifndef TESTEXTRAS_H
#define TESTEXTRAS_H

#include <QtCore/QObject>

class QQuickItem;
class QTouchDevice;
class UCTestExtras : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool touchPresent READ touchDevicePresent NOTIFY touchDevicePresentChanged)
public:
    explicit UCTestExtras(QObject *parent = 0);

Q_SIGNALS:
    void touchDevicePresentChanged();

public Q_SLOTS:
    static QString openGLflavor();
    static QString cpuArchitecture();
    static bool touchDevicePresent();
    static void registerTouchDevice();
    static void touchPress(int touchId, QQuickItem *item, const QPoint &point);
    static void touchRelease(int touchId, QQuickItem *item, const QPoint &point);
    static void touchClick(int touchId, QQuickItem *item, const QPoint &point);
    static void touchLongPress(int touchId, QQuickItem *item, const QPoint &point);
    static void touchDoubleClick(int touchId, QQuickItem *item, const QPoint &point);
    static void touchMove(int touchId, QQuickItem *item, const QPoint &point);
    static void touchDrag(int touchId, QQuickItem *item, const QPoint &from, const QPoint &delta, int steps = 5);

    static void mouseDrag(QQuickItem *item, const QPoint &from, const QPoint &delta, Qt::MouseButton button, Qt::KeyboardModifiers stateKey = 0, int steps = -1, int delay = -1);

    static void removeTimeConstraintsFromSwipeArea(QQuickItem *item);

public: // yet for cpp use
    static void touchDragWithPoints(int touchId, QQuickItem *item, QList<QPoint> points, int delay = -1);
    static void mouseDragWithPoints(QQuickItem *item, QList<QPoint> points, Qt::MouseButton button, Qt::KeyboardModifiers stateKey = 0, int delay = -1);

private:
    static UCTestExtras *m_testExtras;

    friend class UCMouseTouchAdaptor;
};

#endif // TESTEXTRAS_H
