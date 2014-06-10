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

    static void registerTouchDevice();

    inline static void touchPress(QWindow *window, const QPoint &point)
    {
        if (!m_touchDevice) {
            qWarning() << QString("No touch device registered. Register one using registerTouchDevice() before using %1").arg(__FUNCTION__);
            return;
        }
        QTest::touchEvent(window, m_touchDevice).press(0, point, window);
    }
    inline static void touchRelease(QWindow *window, const QPoint &point)
    {
        if (!m_touchDevice) {
            qWarning() << QString("No touch device registered. Register one using registerTouchDevice() before using %1").arg(__FUNCTION__);
            return;
        }
        QTest::touchEvent(window, m_touchDevice).release(0, point, window);
    }
    inline static void touchClick(QWindow *window, const QPoint &point)
    {
        if (!m_touchDevice) {
            qWarning() << QString("No touch device registered. Register one using registerTouchDevice() before using %1").arg(__FUNCTION__);
            return;
        }
        touchPress(window, point);
        QTest::qWait(10);
        touchRelease(window, point);
    }
    inline static void touchLongPress(QWindow *window, const QPoint &point)
    {
        if (!m_touchDevice) {
            qWarning() << QString("No touch device registered. Register one using registerTouchDevice() before using %1").arg(__FUNCTION__);
            return;
        }
        touchPress(window, point);
        QTest::qWait(800);
    }
    inline static void touchDoubleClick(QWindow *window, const QPoint &point)
    {
        if (!m_touchDevice) {
            qWarning() << QString("No touch device registered. Register one using registerTouchDevice() before using %1").arg(__FUNCTION__);
            return;
        }
        touchPress(window, point);
        QTest::qWait(10);
        touchRelease(window, point);
        QTest::qWait(10);
        touchPress(window, point);
        QTest::qWait(10);
        touchRelease(window, point);
    }
    inline static void touchMove(QWindow *window, const QPoint &point)
    {
        if (!m_touchDevice) {
            qWarning() << QString("No touch device registered. Register one using registerTouchDevice() before using %1").arg(__FUNCTION__);
            return;
        }
        QTest::touchEvent(window, m_touchDevice).move(0, point, window);
    }
    inline static void touchDrag(QWindow *window, const QPoint &from, const QPoint &delta, int steps = 5)
    {
        if (!m_touchDevice) {
            qWarning() << QString("No touch device registered. Register one using registerTouchDevice() before using %1").arg(__FUNCTION__);
            return;
        }
        touchPress(window, from);
        QTest::qWait(10);
        QTest::touchEvent(window, m_touchDevice).move(0, from, window);
        qreal stepDx = delta.x() / steps;
        qreal stepDy = delta.y() / steps;
        if (!delta.isNull()) {
            for (int i = 0; i < steps; i++) {
                QTest::qWait(10);
                QTest::touchEvent(window, m_touchDevice).move(0, from + QPoint(i * stepDx, i * stepDy), window);
            }
        }
        QTest::qWait(10);
        touchRelease(window, from + QPoint(stepDx, stepDy));
    }


private:
    QSignalSpy* m_spy;
    static QTouchDevice *m_touchDevice;
};

#endif // UBUNTU_TEST_UBUNTUTESTCASE_H
