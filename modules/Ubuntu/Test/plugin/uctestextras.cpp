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
#include <QSysInfo>

const char *DEVICE_MISSING_MSG = "No touch device registered. Register one using registerTouchDevice() before using %1";

#define CHECK_TOUCH_DEVICE(touchId, item) \
    if (!touchDevicePresent()) { \
        qWarning() << QString(DEVICE_MISSING_MSG).arg(__FUNCTION__); \
        return; \
    } \
    if (touchId < 0) { \
        qWarning() << "Invalid touchId specified."; \
        return; \
    } \
    if (!item) { \
        qWarning() << "Invalid item specified."; \
        return; \
    }

QTouchDevice *UCTestExtras::m_touchDevice = 0;

/*!
 * \qmltype TestExtras
 * \instantiates UCTestExtras
 * \inqmlmodule Ubuntu.Test 1.0
 * \ingroup ubuntu-test
 * \brief Singleton type providing additional test functions.
 *
 * The component provides additional test functions like touch handling, registering
 * touch device on non-touch screen enabled environment.
 */

UCTestExtras::UCTestExtras(QObject *parent) :
    QObject(parent)
{
}

/*!
 * \qmlmethod TestExtras::openGLflavor()
 * Returns "opengl" or "opengles2".
 */
QString UCTestExtras::openGLflavor()
{
#if defined(QT_OPENGL_ES)
    return QString("opengles2");
#else
    return QString("opengl");
#endif
}

/*!
 * \qmlmethod TestExtras::cpuArchitecture()
 * Returns the build architecure, including but not limited to
 * "arm", "arm64", "i386" and "x86_64".
 */
QString UCTestExtras::cpuArchitecture()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
    return QSysInfo::buildCpuArchitecture();
#elif defined(Q_PROCESSOR_X86_64)
    return "x86_64";
#elif defined(Q_PROCESSOR_X86_32)
    return "i386";
#elif defined(Q_PROCESSOR_ARM_64)
    return "arm64";
#elif defined(Q_PROCESSOR_ARM)
    return "arm";
#else
    return "unknown";
#endif
}

/*!
 * \qmlmethod TestExtras::touchDevicePresent()
 * Returns true if the system has a touch device registered.
 */
bool UCTestExtras::touchDevicePresent()
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
 * \qmlmethod TestExtras::registerTouchDevice()
 * Registers a touch device if there's none registered. Calling the function in
 * touch enabled environment has no effect. The function must be called in initTestCase()
 * in order to perform touch related tests.
 */
void UCTestExtras::registerTouchDevice()
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

/*!
 * \qmlmethod TestExtras::touchPress(touchId, item, point)
 * The function triggers a touch press event for a given \a touchId on a specific
 * \a item. The \a point contains the (x,y) coordinates of the event in \a item
 * coordinates.
 */
void UCTestExtras::touchPress(int touchId, QQuickItem *item, const QPoint &point)
{
    CHECK_TOUCH_DEVICE(touchId, item);
    QTest::touchEvent(item->window(), m_touchDevice).press(touchId, item->mapToScene(point).toPoint(), item->window());
}
/*!
 * \qmlmethod TestExtras::touchRelease(touchId, item, point)
 * The function produces a touch release event on a given \a touchId performed on
 * \a item at a \a point.
 */
void UCTestExtras::touchRelease(int touchId, QQuickItem *item, const QPoint &point)
{
    CHECK_TOUCH_DEVICE(touchId, item);
    QTest::touchEvent(item->window(), m_touchDevice).release(touchId, item->mapToScene(point).toPoint(), item->window());
}
/*!
 * \qmlmethod TestExtras::touchClick(touchId, item, point)
 * The function performs a pair of \l touchPress and \l touchRelease calls on the same
 * point resulting in a click like event.
 */
void UCTestExtras::touchClick(int touchId, QQuickItem *item, const QPoint &point)
{
    CHECK_TOUCH_DEVICE(touchId, item);
    touchPress(touchId, item, point);
    QTest::qWait(100);
    touchRelease(touchId, item, point);
}
/*!
 * \qmlmethod TestExtras::touchLongPress(touchId, item, point)
 * The function produces a \l touchPress event with a timeout equivalent to the
 * mouse \c pressAndHold timeout, after which the function returns.
 */
void UCTestExtras::touchLongPress(int touchId, QQuickItem *item, const QPoint &point)
{
    CHECK_TOUCH_DEVICE(touchId, item);
    touchPress(touchId, item, point);
    // 800 miliseconds + 200 to let events processed
    QTest::qWait(1000);
}
/*!
 * \qmlmethod TestExtras::touchDoubleClick(touchId, item, point)
 * The function performs two consecutive \l touchClick events with a slight delay
 * in between each click event.
 */
void UCTestExtras::touchDoubleClick(int touchId, QQuickItem *item, const QPoint &point)
{
    CHECK_TOUCH_DEVICE(touchId, item);
    touchClick(touchId, item, point);
    QTest::qWait(100);
    touchClick(touchId, item, point);
}
/*!
 * \qmlmethod TestExtras::touchMove(touchId, item, point)
 * The function moves the touch point identified by the \a touchId to the destination
 * \a point. The point is in \a item coordinates. The touch point identified by the
 * \a touchId must be pressed before calling this function in order to produce the
 * desired functionality. The event can be captured in a \c MultiPointTouchArea through
 * \c updated() signal.
 */
void UCTestExtras::touchMove(int touchId, QQuickItem *item, const QPoint &point)
{
    CHECK_TOUCH_DEVICE(touchId, item);
    QTest::touchEvent(item->window(), m_touchDevice).move(touchId, item->mapToScene(point).toPoint(), item->window());
}
/*!
 * \qmlmethod TestExtras::touchDrag(touchId, item, from, delta, steps = 5)
 * The function performs a drag gesture on a touch point identified by \a touchId
 * over an \a item from the starting point \a from with a \a delta. The gesture
 * is realized with a touch press, \a step moves and a release event.
 *
 * By default the function uses 5 steps to produce the gesture. This value can be any
 * positive number, driving the gesture appliance to be faster (less than 5 moves) or
 * slower (more than 5 moves). If a negative or 0 value is given, the function will
 * use the default 5 steps to produce the gesture.
 */
void UCTestExtras::touchDrag(int touchId, QQuickItem *item, const QPoint &from, const QPoint &delta, int steps)
{
    CHECK_TOUCH_DEVICE(touchId, item);
    if (delta.isNull()) {
        qWarning() << "delta point is invalid";
        return;
    }
    if (steps <= 0) {
        steps = 5;
    }
    touchPress(touchId, item, from);
    QTest::qWait(10);
    touchMove(touchId, item, from);
    QPoint movePoint(from);
    qreal stepDx = delta.x() / steps;
    qreal stepDy = delta.y() / steps;
    if (!delta.isNull()) {
        for (int i = 0; i < steps - 1; i++) {
            QTest::qWait(10);
            movePoint += QPoint(stepDx, stepDy);
            touchMove(touchId, item, movePoint);
        }
    }
    QTest::qWait(10);
    touchRelease(touchId, item, from + delta);
}
