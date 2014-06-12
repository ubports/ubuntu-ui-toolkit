/*
 * Copyright 2013-2014 Canonical Ltd.
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
 * Author: Christian Dywan <christian.dywan@canonical.com>
 */

#ifndef UBUNTU_TEST_UBUNTUTESTCASE_H
#define UBUNTU_TEST_UBUNTUTESTCASE_H

#include <QtTest/QtTest>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickView>
#include <QtTest/QSignalSpy>

#define CHECK_TOUCH_DEVICE()    if (!checkTouchDevice(__FUNCTION__)) return

class UbuntuTestCase : public QQuickView
{
    Q_OBJECT

public:
    UbuntuTestCase(const QString& file, QWindow* parent = 0);
    int warnings() const;
    // getter
    template<class T>
    inline T findItem(const QString& objectName) const {
        T item = rootObject()->findChild<T>(objectName);
        if (item)
            return item;
        if (rootObject()->findChild<QObject*>(objectName))
            qFatal("Item '%s' found with unexpected type", qPrintable(objectName));
        qFatal("No item '%s' found", qPrintable(objectName));
    }

    inline static QPointF centerOf(QQuickItem *item, bool windowPos = true)
    {
        QPointF center;
        if (item) {
            center = item->boundingRect().center();
            if (windowPos) {
                center = item->mapToScene(center);
            }
        }
        return center;
    }

    static void registerTouchDevice();

    inline static void touchPress(int touchId, QWindow *window, const QPoint &point)
    {
        CHECK_TOUCH_DEVICE();
        QTest::touchEvent(window, m_touchDevice).press(touchId, point, window);
    }
    inline static void touchRelease(int touchId, QWindow *window, const QPoint &point)
    {
        CHECK_TOUCH_DEVICE();
        QTest::touchEvent(window, m_touchDevice).release(touchId, point, window);
    }
    inline static void touchClick(int touchId, QWindow *window, const QPoint &point)
    {
        CHECK_TOUCH_DEVICE();
        touchPress(touchId, window, point);
        QTest::qWait(10);
        touchRelease(touchId, window, point);
    }
    inline static void touchLongPress(int touchId, QWindow *window, const QPoint &point)
    {
        CHECK_TOUCH_DEVICE();
        touchPress(touchId, window, point);
        QTest::qWait(800);
    }
    inline static void touchDoubleClick(int touchId, QWindow *window, const QPoint &point)
    {
        CHECK_TOUCH_DEVICE();
        touchClick(touchId, window, point);
        QTest::qWait(10);
        touchClick(touchId, window, point);
    }
    inline static void touchMove(int touchId, QWindow *window, const QPoint &point)
    {
        CHECK_TOUCH_DEVICE();
        QTest::touchEvent(window, m_touchDevice).move(touchId, point, window);
    }
    inline static void touchDrag(int touchId, QWindow *window, const QPoint &from, const QPoint &delta, int steps = 5)
    {
        touchPress(touchId, window, from);
        QTest::qWait(10);
        QTest::touchEvent(window, m_touchDevice).move(touchId, from, window);
        qreal stepDx = delta.x() / steps;
        qreal stepDy = delta.y() / steps;
        if (!delta.isNull()) {
            for (int i = 0; i < steps; i++) {
                QTest::qWait(10);
                QTest::touchEvent(window, m_touchDevice).move(touchId, from + QPoint(i * stepDx, i * stepDy), window);
            }
        }
        QTest::qWait(10);
        touchRelease(touchId, window, from + QPoint(stepDx, stepDy));
    }


private:
    QSignalSpy* m_spy;
    static QTouchDevice *m_touchDevice;

    static inline bool checkTouchDevice(const char *func)
    {
        if (!m_touchDevice) {
            qWarning() << QString("No touch device registered. Register one using registerTouchDevice() before using %1").arg(func);
            return false;
        }
        return true;
    }
};

#endif // UBUNTU_TEST_UBUNTUTESTCASE_H
