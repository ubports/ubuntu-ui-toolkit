/*
 * Copyright 2016 Canonical Ltd.
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

#include "ucswipearea_p.h"
#include "ucswipearea_p_p.h"

#include <qpa/qwindowsysteminterface.h>
#include <private/qobject_p.h>
#include <QSysInfo>
#include <UbuntuToolkit/private/mousetouchadaptor_p.h>

UT_USE_NAMESPACE

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

UCTestExtras *UCTestExtras::m_testExtras = 0;

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
    m_testExtras = this;
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
    if (MouseTouchAdaptor::registerTouchDevice() && m_testExtras) {
        Q_EMIT m_testExtras->touchDevicePresentChanged();
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
    QTest::touchEvent(item->window(), MouseTouchAdaptor::touchDevice()).press(touchId, item->mapToScene(point).toPoint(), item->window());
}
/*!
 * \qmlmethod TestExtras::touchRelease(touchId, item, point)
 * The function produces a touch release event on a given \a touchId performed on
 * \a item at a \a point.
 */
void UCTestExtras::touchRelease(int touchId, QQuickItem *item, const QPoint &point)
{
    CHECK_TOUCH_DEVICE(touchId, item);
    QTest::touchEvent(item->window(), MouseTouchAdaptor::touchDevice()).release(touchId, item->mapToScene(point).toPoint(), item->window());
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
    QTest::touchEvent(item->window(), MouseTouchAdaptor::touchDevice()).move(touchId, item->mapToScene(point).toPoint(), item->window());
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
    QTest::qWait(20);
    touchMove(touchId, item, from);
    QPoint movePoint(from);
    qreal stepDx = delta.x() / steps;
    qreal stepDy = delta.y() / steps;
    if (!delta.isNull()) {
        for (int i = 0; i < steps - 1; i++) {
            QTest::qWait(20);
            movePoint += QPoint(stepDx, stepDy);
            touchMove(touchId, item, movePoint);
        }
    }
    QTest::qWait(20);
    touchRelease(touchId, item, from + delta);
}

/*!
 * \qmlmethod void TestExtras::mouseDrag(touchId, item, from, delta, button, stateKey, steps = 5, delay = 20)
 * The function performs a drag with a mouse over an \a item from the starting
 * point \a from with a \a delta. The gesture is realized with a mouse press,
 * \a step moves and a release event, with a \e delay in between each mouse event.
 *
 * By default the function uses 5 steps to produce the drag. This value can be any
 * positive number, driving the gesture appliance to be faster (less than 5 moves) or
 * slower (more than 5 moves). If a negative or 0 value is given, the function will
 * use the default 5 steps to produce the gesture.
 */

void UCTestExtras::mouseDrag(QQuickItem *item, const QPoint &from, const QPoint &delta, Qt::MouseButton button, Qt::KeyboardModifiers stateKey, int steps, int delay)
{
    if (delta.isNull()) {
        qWarning() << "delta point is invalid";
        return;
    }
    if (steps <= 0) {
        steps = 5;
    }
    if (delay < 20) {
        delay = 20;
    }
    QTest::mousePress(item->window(), button, stateKey, from, delay);
    QPoint movePoint(from);
    qreal stepDx = delta.x() / steps;
    qreal stepDy = delta.y() / steps;
    if (!delta.isNull()) {
        for (int i = 0; i < steps; i++) {
            QTest::mouseMove(item->window(), movePoint, delay);
            movePoint += QPoint(stepDx, stepDy);
        }
    }
    QTest::mouseRelease(item->window(), button, stateKey, movePoint, delay);
}

/*
 * void TestExtras::touchDragWithPoints(touchId, item, list<point> ponts, delay = 20)
 * Similar to \l touchDrag function, but here the points must be specified in \e points property.
 * The first point is expected to be the starting position, after which each point is a relative
 * point defining the move. A minimum of 5 points are needed to properly produce a drag.
 * \qml
 * import Qt.Test 1.0
 * import Ubuntu.Test 1.0
 * Item {
 *     id: testItem
 *     UbuntuTestCase {
 *
 *         function test_vertical_drag_upwards() {
 *             var points = [];
 *             points.push(centerOf(testItem));
 *             points.push(Qt.point(0, -1));
 *             points.push(Qt.point(0, -3));
 *             points.push(Qt.point(0, -6));
 *             points.push(Qt.point(0, -5));
 *             points.push(Qt.point(0, -2));
 *             TestExtras.touchDragWithPoints(0, testItem, points);
 *         }
 *     }
 * }
 * \endqml
 * The delay must be a minimum of 20 milliseconds.
 */
void UCTestExtras::touchDragWithPoints(int touchId, QQuickItem *item, QList<QPoint> points, int delay)
{
    if (points.size() < 5) {
        qWarning() << "minimum 5 points are needed.";
        return;
    }
    if (delay < 20) {
        delay = 20;
    }

    QPoint movePoint(points[0]);
    touchPress(touchId, item, movePoint);
    QTest::qWait(delay);
    touchMove(touchId, item, movePoint);
    for (int i = 1; i < points.size(); ++i) {
        QTest::qWait(delay);
        movePoint += points[i];
        touchMove(touchId, item, movePoint);
    }
    QTest::qWait(delay);
    touchRelease(touchId, item, movePoint);
}

/*
 * void TestExtras::mouseDragWithPoints(item, button, list<point> ponts, stateKey = 0, delay = 20)
 * Similar to \l mouseDrag function, but here the points must be specified in \e points property.
 * The first point is expected to be the starting position, after which each point is a relative
 * point defining the move. A minimum of 5 points are needed to properly produce a drag.
 * \qml
 * import Qt.Test 1.0
 * import Ubuntu.Test 1.0
 * Item {
 *     id: testItem
 *     UbuntuTestCase {
 *
 *         function test_vertical_drag_upwards() {
 *             var points = [];
 *             points.push(centerOf(testItem));
 *             points.push(Qt.point(0, -1));
 *             points.push(Qt.point(0, -3));
 *             points.push(Qt.point(0, -6));
 *             points.push(Qt.point(0, -5));
 *             points.push(Qt.point(0, -2));
 *             TestExtras.mouseDragWithPoints(testItem, Qt.LeftButton, points);
 *         }
 *     }
 * }
 * \endqml
 * The delay must be a minimum of 20 milliseconds.
 */
void UCTestExtras::mouseDragWithPoints(QQuickItem *item, QList<QPoint> points, Qt::MouseButton button, Qt::KeyboardModifiers stateKey, int delay)
{
    if (points.size() < 5) {
        qWarning() << "minimum 5 points are needed.";
        return;
    }
    if (delay < 20) {
        delay = 20;
    }
    QTest::mousePress(item->window(), button, stateKey, item->mapToScene(points[0]).toPoint(), delay);
    QPoint movePoint(item->mapToScene(points[0]).toPoint());
    QTest::mouseMove(item->window(), movePoint, delay);
    for (int i = 1; i < points.size(); ++i) {
        movePoint += points[i];
        QTest::mouseMove(item->window(), movePoint, delay);
    }
    QTest::mouseRelease(item->window(), button, stateKey, movePoint, delay);
}


void UCTestExtras::removeTimeConstraintsFromSwipeArea(QQuickItem *item)
{
    UG_PREPEND_NAMESPACE(UCSwipeArea) *swipeArea = dynamic_cast<UG_PREPEND_NAMESPACE(UCSwipeArea)*>(item);
    if (!swipeArea) {
        qWarning() << item << "is not a SwipeArea";
        return;
    }

    UG_PREPEND_NAMESPACE(UCSwipeAreaPrivate) *priv =
            static_cast<UG_PREPEND_NAMESPACE(UCSwipeAreaPrivate)*>(QObjectPrivate::get(swipeArea));
    priv->setMaxTime(60 * 60 * 1000);
    priv->setCompositionTime(0);
}
