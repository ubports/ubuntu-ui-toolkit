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
 *
 */

#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickView>
#include <QtQuick/QQuickItem>
#include <QtCore/QEvent>

#include "inversemouseareatype.h"
#include <private/qquickevents_p_p.h>
#include "uctestcase.h"

class tst_InverseMouseAreaTest : public QObject
{
    Q_OBJECT
    
public:
    tst_InverseMouseAreaTest() {}

private:
    UbuntuTestCase *testCase;
    QQuickView *quickView;
    QQmlEngine *quickEngine;
    QObjectCleanupHandler eventCleanup;

    InverseMouseAreaType *testArea(const QString &document)
    {
        testCase->setFile(document);

        InverseMouseAreaType* area = quickView->rootObject()->findChild<InverseMouseAreaType*>();
        Q_ASSERT(area);
        return area;
    }

protected Q_SLOTS:
    void capturePressed(QQuickMouseEvent *event)
    {
        eventCleanup.add((QObject*)event);
    }

private Q_SLOTS:

    void initTestCase()
    {
        quickView = testCase = new UbuntuTestCase(0);
    }

    void cleanupTestCase()
    {
        delete quickView;
    }

    void testCase_Defaults()
    {
        InverseMouseAreaType *area = testArea("Defaults.qml");
        QVERIFY(area);

        QCOMPARE(area->pressed(), false);
        QCOMPARE(area->acceptedButtons(), Qt::LeftButton);
        QCOMPARE(area->pressedButtons(), Qt::NoButton);
        QCOMPARE(area->propagateComposedEvents(), false);
    }
    void testCase_DoNotPropagateEvents()
    {
        eventCleanup.clear();
        InverseMouseAreaType *area = testArea("InverseMouseAreaDoNotPropagateEvents.qml");
        // connect pressed signal to capture mouse object
        QObject::connect(area, SIGNAL(pressed(QQuickMouseEvent*)), this, SLOT(capturePressed(QQuickMouseEvent*)));
        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(10, 10));
        QTest::waitForEvents();
        QVERIFY(eventCleanup.isEmpty());
    }


    void testCase_PropagateEvents()
    {
        eventCleanup.clear();
        InverseMouseAreaType *area = testArea("InverseMouseAreaPropagateEvents.qml");
        // connect pressed signal to capture mouse object
        QObject::connect(area, SIGNAL(pressed(QQuickMouseEvent*)), this, SLOT(capturePressed(QQuickMouseEvent*)));
        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(10, 10));
        QTest::waitForEvents();
        QVERIFY(eventCleanup.isEmpty());
    }

    void testCase_sensingAreaError()
    {
        InverseMouseAreaType *area = testArea("SensingAreaError.qml");
        QVERIFY(area);

        QTest::mouseClick(quickView, Qt::LeftButton, 0, QPoint(20, 20));
        QTest::waitForEvents();
        QCOMPARE(quickView->rootObject()->property("log").toString(), QString("IMA"));
    }

    void testCase_InverseMouseAreInWindow()
    {
        InverseMouseAreaType *area = testArea("InverseMouseAreaInWindow.qml");
        QVERIFY(area);

        QList<QQuickWindow *> l = quickView->rootObject()->findChildren<QQuickWindow*>("isawindow");
        QVERIFY(l.count());

        QTest::mouseClick(l[0], Qt::LeftButton, 0, QPoint(20, 10));
        QTest::waitForEvents();
        QCOMPARE(quickView->rootObject()->property("log").toString(), QString("IMA"));
    }

    void testCase_OverlappedMouseArea()
    {
        InverseMouseAreaType *area = testArea("OverlappedMouseArea.qml");
        QVERIFY(area);

        QTest::mouseClick(quickView, Qt::LeftButton, 0, QPoint(20, 10));
        QTest::waitForEvents();
        QCOMPARE(quickView->rootObject()->property("log").toString(), QString("MA"));
    }

    void testCase_InverseMouseAreaOnTop()
    {
        InverseMouseAreaType *area = testArea("InverseMouseAreaOnTop.qml");
        QQuickItem *ma1 = testCase->findItem("MA1");
        QQuickItem *ma2 = testCase->findItem("MA2");

        QSignalSpy imaSpy(area, SIGNAL(pressed(QQuickMouseEvent*)));
        QSignalSpy ma1Spy(ma1, SIGNAL(pressed(QQuickMouseEvent*)));
        QSignalSpy ma2Spy(ma2, SIGNAL(pressed(QQuickMouseEvent*)));

        // click in the top rectangle
        QTest::mouseClick(quickView, Qt::LeftButton, 0, QPoint(10, 10));
        QTest::waitForEvents();
        QCOMPARE(ma1Spy.count(), 0);
        QCOMPARE(ma2Spy.count(), 0);
        QCOMPARE(imaSpy.count(), 1);
        imaSpy.clear();

        // click in the second rectangle
        QTest::mouseClick(quickView, Qt::LeftButton, 0, QPoint(10, 65));
        QTest::waitForEvents();
        QCOMPARE(ma1Spy.count(), 0);
        QCOMPARE(ma2Spy.count(), 0);
        QCOMPARE(imaSpy.count(), 1);
        imaSpy.clear();

        // click in teh button
        QTest::mouseClick(quickView, Qt::LeftButton, 0, QPoint(25, 85));
        QTest::waitForEvents();
        QCOMPARE(ma1Spy.count(), 0);
        QCOMPARE(ma2Spy.count(), 1);
        QCOMPARE(imaSpy.count(), 0);

        QSignalSpy imaDSpy(area, SIGNAL(doubleClicked(QQuickMouseEvent*)));
        QTest::mouseDClick(quickView, Qt::LeftButton, 0, QPoint(10, 65));
        QCOMPARE(imaDSpy.count(), 1);
        imaDSpy.clear();

        QTest::mouseDClick(quickView, Qt::LeftButton, 0, QPoint(10, 10));
        QCOMPARE(imaDSpy.count(), 1);
        imaDSpy.clear();
    }

    void testCase_InverseMouseAreaOnTopNoAccept()
    {
        InverseMouseAreaType *area = testArea("InverseMouseAreaOnTopNoAccept.qml");

        QQuickItem *ma1 = testCase->findItem("MA1");
        QQuickItem *ma2 = testCase->findItem("MA2");

        QSignalSpy imaSpy(area, SIGNAL(pressed(QQuickMouseEvent*)));
        QSignalSpy ma1Spy(ma1, SIGNAL(pressed(QQuickMouseEvent*)));
        QSignalSpy ma2Spy(ma2, SIGNAL(pressed(QQuickMouseEvent*)));

        QTest::mouseClick(quickView, Qt::LeftButton, 0, QPoint(10, 10));
        QTest::waitForEvents();
        QCOMPARE(ma1Spy.count(), 1);
        QCOMPARE(ma2Spy.count(), 0);
        QCOMPARE(imaSpy.count(), 1);
        ma1Spy.clear(); imaSpy.clear();

        QTest::mouseClick(quickView, Qt::LeftButton, 0, QPoint(10, 65));
        QTest::waitForEvents();
        QCOMPARE(ma1Spy.count(), 0);
        QCOMPARE(ma2Spy.count(), 1);
        QCOMPARE(imaSpy.count(), 1);
        ma2Spy.clear(); imaSpy.clear();

        QTest::mouseClick(quickView, Qt::LeftButton, 0, QPoint(25, 80));
        QTest::waitForEvents();
        QCOMPARE(ma1Spy.count(), 0);
        QCOMPARE(ma2Spy.count(), 1);
        QCOMPARE(imaSpy.count(), 0);

        // double click should not reach inverse mouse area as onPressed did not accept the events
        QSignalSpy imaDSpy(area, SIGNAL(doubleClicked(QQuickMouseEvent*)));
        QTest::mouseDClick(quickView, Qt::LeftButton, 0, QPoint(10, 65));
        QCOMPARE(imaDSpy.count(), 0);
        imaDSpy.clear();

        QTest::mouseDClick(quickView, Qt::LeftButton, 0, QPoint(10, 10));
        QCOMPARE(imaDSpy.count(), 0);
        imaDSpy.clear();
    }

    void testCase_InverseMouseAreaOnTopTopmost()
    {
        InverseMouseAreaType *area = testArea("InverseMouseAreaOnTop.qml");
        area->setProperty("topmostItem", true);

        QQuickItem *ma2 = testCase->findItem("MA2");

        QSignalSpy imaSpy(area, SIGNAL(pressed(QQuickMouseEvent*)));
        QSignalSpy ma2Spy(ma2, SIGNAL(pressed(QQuickMouseEvent*)));

        QTest::mouseClick(quickView, Qt::LeftButton, 0, QPoint(10, 10));
        QTest::waitForEvents();
        QCOMPARE(ma2Spy.count(), 0);
        QCOMPARE(imaSpy.count(), 1);
        imaSpy.clear();

        QTest::mouseClick(quickView, Qt::LeftButton, 0, QPoint(10, 65));
        QTest::waitForEvents();
        QCOMPARE(ma2Spy.count(), 0);
        QCOMPARE(imaSpy.count(), 1);
        imaSpy.clear();

        QTest::mouseClick(quickView, Qt::LeftButton, 0, QPoint(25, 80));
        QTest::waitForEvents();
        QCOMPARE(ma2Spy.count(), 1);
        QCOMPARE(imaSpy.count(), 0);

        QSignalSpy imaDSpy(area, SIGNAL(doubleClicked(QQuickMouseEvent*)));
        QTest::mouseDClick(quickView, Qt::LeftButton, 0, QPoint(10, 65));
        QCOMPARE(imaDSpy.count(), 1);
        imaDSpy.clear();

        QTest::mouseDClick(quickView, Qt::LeftButton, 0, QPoint(10, 10));
        QCOMPARE(imaDSpy.count(), 1);
        imaDSpy.clear();
    }

    void testCase_InverseMouseAreaSignals()
    {
        InverseMouseAreaType *area = testArea("InverseMouseAreaSignals.qml");

        QSignalSpy pressSpy(area, SIGNAL(pressed(QQuickMouseEvent*)));
        QSignalSpy releaseSpy(area, SIGNAL(released(QQuickMouseEvent*)));
        QSignalSpy clickSpy(area, SIGNAL(clicked(QQuickMouseEvent*)));
        QSignalSpy enteredSpy(area, SIGNAL(entered()));
        QSignalSpy exitedSpy(area, SIGNAL(exited()));

        QTest::mouseClick(quickView, Qt::LeftButton, 0, QPoint(5, 5));
        QCOMPARE(pressSpy.count(), 1);
        QCOMPARE(releaseSpy.count(), 1);
        QCOMPARE(clickSpy.count(), 1);
        QCOMPARE(enteredSpy.count(), 1);
        QCOMPARE(exitedSpy.count(), 1);

        QSignalSpy doubleClickSpy(area, SIGNAL(doubleClicked(QQuickMouseEvent*)));
        QTest::mouseDClick(quickView, Qt::LeftButton, 0, QPoint(5, 5));
        QCOMPARE(doubleClickSpy.count(), 1);
    }

    void testCase_InverseMouseAreaNormalEventStack()
    {
        InverseMouseAreaType *ima = testArea("InverseMouseAreaNormalEventStack.qml");
        QQuickItem *ma = testCase->findItem("MA");

        QSignalSpy imaSpy(ima, SIGNAL(pressed(QQuickMouseEvent*)));
        QSignalSpy maSpy(ma, SIGNAL(pressed(QQuickMouseEvent*)));

        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(15, 15));
        QCOMPARE(imaSpy.count(), 0);
        QCOMPARE(maSpy.count(), 0);

        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(115, 15));
        QCOMPARE(imaSpy.count(), 0);
        QCOMPARE(maSpy.count(), 1);
        maSpy.clear();

        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(115, 115));
        QCOMPARE(imaSpy.count(), 1);
        QCOMPARE(maSpy.count(), 0);
    }

    void testCase_InverseMouseAreaTopmost()
    {
        InverseMouseAreaType *ima = testArea("InverseMouseAreaTopmostItem.qml");
        QQuickItem *ma = testCase->findItem("MA");

        QSignalSpy imaSpy(ima, SIGNAL(pressed(QQuickMouseEvent*)));
        QSignalSpy maSpy(ma, SIGNAL(pressed(QQuickMouseEvent*)));
        QSignalSpy imaDblClick(ima, SIGNAL(doubleClicked(QQuickMouseEvent*)));

        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(15, 15));
        QCOMPARE(imaSpy.count(), 0);
        QCOMPARE(maSpy.count(), 0);

        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(115, 15));
        QCOMPARE(imaSpy.count(), 1);
        QCOMPARE(maSpy.count(), 0);

        imaSpy.clear();
        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(115, 115));
        QCOMPARE(imaSpy.count(), 1);
        QCOMPARE(maSpy.count(), 0);

        QTest::mouseDClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(115, 15));
        QCOMPARE(imaDblClick.count(), 1);
    }

    void testCase_InverseMouseAreaSensingArea()
    {
        InverseMouseAreaType *ima = testArea("InverseMouseAreaSensingArea.qml");
        QQuickItem *ma = testCase->findItem("MA");

        QSignalSpy maSpy(ma, SIGNAL(clicked(QQuickMouseEvent*)));
        QSignalSpy imaSpy(ima, SIGNAL(clicked(QQuickMouseEvent*)));

        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(75, 75));
        QCOMPARE(maSpy.count(), 0);
        QCOMPARE(imaSpy.count(), 1);
        imaSpy.clear();

        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(25, 25));
        QCOMPARE(maSpy.count(), 1);
        QCOMPARE(imaSpy.count(), 0);
        maSpy.clear();

        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(175, 175));
        QCOMPARE(maSpy.count(), 1);
        QCOMPARE(imaSpy.count(), 0);
    }

    void testCase_InverseMouseAreaSensingAreaChange()
    {
        InverseMouseAreaType *ima = testArea("InverseMouseAreaSensingArea.qml");
        QQuickItem *ma = testCase->findItem("MA");

        QSignalSpy maSpy(ma, SIGNAL(clicked(QQuickMouseEvent*)));
        QSignalSpy imaSpy(ima, SIGNAL(clicked(QQuickMouseEvent*)));

        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(75, 75));
        QCOMPARE(maSpy.count(), 0);
        QCOMPARE(imaSpy.count(), 1);
        imaSpy.clear();

        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(25, 25));
        QCOMPARE(maSpy.count(), 1);
        QCOMPARE(imaSpy.count(), 0);
        maSpy.clear();

        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(175, 175));
        QCOMPARE(maSpy.count(), 1);
        QCOMPARE(imaSpy.count(), 0);
        maSpy.clear();

        ima->setProperty("sensingArea", QVariant());

        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(75, 75));
        QCOMPARE(maSpy.count(), 0);
        QCOMPARE(imaSpy.count(), 1);
        imaSpy.clear();

        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(25, 25));
        QCOMPARE(maSpy.count(), 0);
        QCOMPARE(imaSpy.count(), 1);
        imaSpy.clear();

        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(175, 175));
        QCOMPARE(maSpy.count(), 1);
        QCOMPARE(imaSpy.count(), 0);
    }

};

QTEST_MAIN(tst_InverseMouseAreaTest)

#include "tst_inversemouseareatest.moc"
