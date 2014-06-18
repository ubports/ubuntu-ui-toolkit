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

#include "uctestextras.h"
#include "uctestcase.h"

#include <qpa/qwindowsysteminterface.h>

#define CHECK_TOUCH_DEVICE()    if (!checkTouchDevice(__FUNCTION__)) return

QTouchDevice *TestExtras::m_touchDevice = 0;

TestExtras::TestExtras(QObject *parent) :
    QObject(parent)
{
}

bool TestExtras::checkTouchDevice(const char *func)
{
    if (!m_touchDevice) {
        qWarning() << QString("No touch device registered. Register one using registerTouchDevice() before using %1").arg(func);
        return false;
    }
    return true;
}

/*!
 * Registers a touch device if there's none registered.
 */
bool TestExtras::touchDevicePresent()
{
    QList<const QTouchDevice*> touchDevices = QTouchDevice::devices();
    Q_FOREACH(const QTouchDevice *device, touchDevices) {
        if (device->type() == QTouchDevice::TouchScreen) {
            return true;
        }
    }
    return false;
}

/*!
 * Registers a touch device if there's none registered.
 */
void TestExtras::registerTouchDevice()
{
    // check if there is any touch device registered in the system
    if (!m_touchDevice) {
        QList<const QTouchDevice*> touchDevices = QTouchDevice::devices();
        Q_FOREACH(const QTouchDevice *device, touchDevices) {
            if (device->type() == QTouchDevice::TouchScreen) {
                m_touchDevice = const_cast<QTouchDevice*>(device);
                break;
            }
        }
    }
    // if none, register one
    if (!m_touchDevice) {
        m_touchDevice = new QTouchDevice;
        m_touchDevice->setType(QTouchDevice::TouchScreen);
        QWindowSystemInterface::registerTouchDevice(m_touchDevice);
    }
}

void TestExtras::touchPress(int touchId, QQuickItem *item, const QPoint &point)
{
    CHECK_TOUCH_DEVICE();
    QTest::touchEvent(item->window(), m_touchDevice).press(touchId, point, item->window());
}
void TestExtras::touchRelease(int touchId, QQuickItem *item, const QPoint &point)
{
    CHECK_TOUCH_DEVICE();
    QTest::touchEvent(item->window(), m_touchDevice).release(touchId, point, item->window());
}
void TestExtras::touchClick(int touchId, QQuickItem *item, const QPoint &point)
{
    CHECK_TOUCH_DEVICE();
    touchPress(touchId, item, point);
    QTest::qWait(100);
    touchRelease(touchId, item, point);
}
void TestExtras::touchLongPress(int touchId, QQuickItem *item, const QPoint &point)
{
    CHECK_TOUCH_DEVICE();
    touchPress(touchId, item, point);
    // 800 miliseconds + 200 to let events processed
    QTest::qWait(1000);
}
void TestExtras::touchDoubleClick(int touchId, QQuickItem *item, const QPoint &point)
{
    CHECK_TOUCH_DEVICE();
    touchClick(touchId, item, point);
    QTest::qWait(100);
    touchClick(touchId, item, point);
}
void TestExtras::touchMove(int touchId, QQuickItem *item, const QPoint &point)
{
    CHECK_TOUCH_DEVICE();
    QTest::touchEvent(item->window(), m_touchDevice).move(touchId, point, item->window());
}
void TestExtras::touchDrag(int touchId, QQuickItem *item, const QPoint &from, const QPoint &delta, int steps)
{
    touchPress(touchId, item, from);
    QTest::qWait(10);
    QTest::touchEvent(item->window(), m_touchDevice).move(touchId, from, item->window());
    qreal stepDx = delta.x() / steps;
    qreal stepDy = delta.y() / steps;
    QPoint movePoint(from);
    if (!delta.isNull()) {
        for (int i = 0; i < steps - 1; i++) {
            QTest::qWait(10);
            movePoint += QPoint(stepDx, stepDy);
            QTest::touchEvent(item->window(), m_touchDevice).move(touchId, movePoint, item->window());
        }
    }
    QTest::qWait(10);
    touchRelease(touchId, item, movePoint);
}
