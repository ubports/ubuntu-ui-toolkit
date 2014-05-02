/*
 * Copyright 2013 Canonical Ltd.
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

#include <QtTest/QtTest>
#include <QtTest/QSignalSpy>
#include <QtCore/QCoreApplication>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickView>
#include <QtQuick/QQuickItem>
#include <QVector3D>
#include <QVector4D>
#include <QQuaternion>
#include <QMatrix4x4>
#include <QtQuick/QQuickItem>
#include <QtQml/QQmlProperty>

#include "uctestcase.h"

class tst_OrientationTest : public QObject
{
    Q_OBJECT
public:
    tst_OrientationTest() {}

private Q_SLOTS:

    void test_defaults()
    {
        QScopedPointer<UbuntuTestCase> testCase(new UbuntuTestCase("Defaults.qml"));
        QQuickItem *helper = testCase->findItem<QQuickItem*>("helper");
        QCOMPARE(helper->property("automaticOrientation").toBool(), true);
        // No warnings expected
        QCOMPARE(testCase->warnings(), 0);
    }

    void test_manualAngle()
    {
        QScopedPointer<UbuntuTestCase> testCase(new UbuntuTestCase("ManualAngle.qml"));
        QQuickItem *helper = testCase->findItem<QQuickItem*>("helper");
        // No warning about "window" being undefined must appear
        QSKIP("TypeError: Cannot set property 'contentOrientation' of null");
        QCOMPARE(testCase->warnings(), 0);
        QCOMPARE(helper->property("orientationAngle").toInt(), 90);
        // Verify expected values
        QQuickItem *checkpoint = testCase->findItem<QQuickItem*>("checkpoint");
        // window.contentOrientation 
        QCOMPARE(checkpoint->property("contentOrientation").toInt(), helper->property("orientationAngle").toInt());
    }
};

QTEST_MAIN(tst_OrientationTest)

#include "tst_orientation.moc"



