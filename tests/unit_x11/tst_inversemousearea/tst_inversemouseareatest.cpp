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

#include "inversemouseareatype.h"
#include "ucunits.h"

class tst_InverseMouseAreaTest : public QObject
{
    Q_OBJECT
    
public:
    tst_InverseMouseAreaTest() {}

private:
    QQuickView *quickView;
    QQmlEngine *quickEngine;
    QObjectCleanupHandler eventCleanup;

    InverseMouseAreaType *testArea(const QString &document)
    {
        // delete previous root
        QObject *rootObject = quickView->rootObject();
        if (rootObject)
            delete rootObject;
        QTest::waitForEvents();

        quickView->setSource(QUrl::fromLocalFile(document));
        QCoreApplication::processEvents();

        rootObject = quickView->rootObject();
        if (!rootObject)
            return 0;

        QList<QQuickItem*> items = rootObject->findChildren<QQuickItem*>();
        Q_FOREACH(QQuickItem *item, items) {
            InverseMouseAreaType *area = qobject_cast<InverseMouseAreaType*>(item);
            if (area)
                return area;
        }
        return 0;
    }

protected Q_SLOTS:
    void capturePressed(QQuickMouseEvent *event)
    {
        eventCleanup.add((QObject*)event);
    }

private Q_SLOTS:

    void initTestCase()
    {
        QString modules("../../../modules");
        QVERIFY(QDir(modules).exists());

        quickView = new QQuickView(0);
        quickEngine = quickView->engine();

        quickView->setGeometry(0,0, 240, 320);
        //add modules folder so we have access to the plugin from QML
        QStringList imports = quickEngine->importPathList();
        imports << QDir(modules).absolutePath();
        quickEngine->setImportPathList(imports);
    }

    void cleanupTestCase()
    {
        delete quickView;
    }

    void testCase_DoNotPropagateEvents()
    {
        eventCleanup.clear();
        InverseMouseAreaType *area = testArea("InverseMouseAreaDoNotPropagateEvents.qml");
        QVERIFY(area);
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
        QVERIFY(area);
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
        quickView->show();

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
        quickView->show();

        QTest::mouseClick(quickView, Qt::LeftButton, 0, QPoint(20, 10));
        QTest::waitForEvents();
        QCOMPARE(quickView->rootObject()->property("log").toString(), QString("MA"));
    }

    void testCase_InverseMouseAreaOnTop()
    {
        InverseMouseAreaType *area = testArea("InverseMouseAreaOnTop.qml");
        QVERIFY(area);
        quickView->show();
        QTest::qWaitForWindowExposed(quickView);

        QQuickItem *ma2 = quickView->rootObject()->findChild<QQuickItem*>("MA2");
        QVERIFY(ma2);

        QSignalSpy imaSpy(area, SIGNAL(pressed(QQuickMouseEvent*)));
        QTest::mouseClick(quickView, Qt::LeftButton, 0, QPoint(10, 10));
        QTest::waitForEvents();
        QCOMPARE(imaSpy.count(), 1);

        QTest::mouseClick(quickView, Qt::LeftButton, 0, QPoint(10, 65));
        QTest::waitForEvents();
        QCOMPARE(imaSpy.count(), 2);

        QSignalSpy ma2Spy(ma2, SIGNAL(pressed(QQuickMouseEvent*)));
        QTest::mouseClick(quickView, Qt::LeftButton, 0, QPoint(25, 80));
        QTest::waitForEvents();
        QCOMPARE(ma2Spy.count(), 1);
    }

    void testCase_InverseMouseAreaSignals()
    {
        InverseMouseAreaType *area = testArea("InverseMouseAreaSignals.qml");
        QVERIFY(area);
        quickView->show();
        QTest::qWaitForWindowExposed(quickView);

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
        QVERIFY(ima);
        quickView->show();
        QTest::qWaitForWindowExposed(quickView);

        QQuickItem *ma = quickView->rootObject()->findChild<QQuickItem*>("MA");
        QVERIFY(ma);

        QSignalSpy imaSpy(ima, SIGNAL(pressed(QQuickMouseEvent*)));
        QSignalSpy maSpy(ma, SIGNAL(pressed(QQuickMouseEvent*)));

        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(15, 15));
        QCOMPARE(imaSpy.count(), 0);
        QCOMPARE(maSpy.count(), 0);

        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(115, 15));
        QCOMPARE(imaSpy.count(), 0);
        QCOMPARE(maSpy.count(), 1);

        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(115, 115));
        QCOMPARE(imaSpy.count(), 1);
        QCOMPARE(maSpy.count(), 1);
    }

    void testCase_InverseMouseAreaTopmost()
    {
        InverseMouseAreaType *ima = testArea("InverseMouseAreaTopmostItem.qml");
        QVERIFY(ima);
        quickView->show();
        QTest::qWaitForWindowExposed(quickView);

        QQuickItem *ma = quickView->rootObject()->findChild<QQuickItem*>("MA");
        QVERIFY(ma);

        QSignalSpy imaSpy(ima, SIGNAL(pressed(QQuickMouseEvent*)));
        QSignalSpy maSpy(ma, SIGNAL(pressed(QQuickMouseEvent*)));

        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(15, 15));
        QCOMPARE(imaSpy.count(), 0);
        QCOMPARE(maSpy.count(), 0);

        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(115, 15));
        QCOMPARE(imaSpy.count(), 1);
        QCOMPARE(maSpy.count(), 0);

        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(115, 115));
        QCOMPARE(imaSpy.count(), 2);
        QCOMPARE(maSpy.count(), 0);
    }

    void testCase_InverseMouseAreaSensingArea()
    {
        InverseMouseAreaType *ima = testArea("InverseMouseAreaSensingArea.qml");
        QVERIFY(ima);
        quickView->show();
        QTest::qWaitForWindowExposed(quickView);

        QQuickItem *ma = quickView->rootObject()->findChild<QQuickItem*>("MA");
        QVERIFY(ma);

        QSignalSpy maSpy(ma, SIGNAL(clicked(QQuickMouseEvent*)));
        QSignalSpy imaSpy(ima, SIGNAL(clicked(QQuickMouseEvent*)));

        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(75, 75));
        QCOMPARE(maSpy.count(), 0);
        QCOMPARE(imaSpy.count(), 1);

        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(25, 25));
        QCOMPARE(maSpy.count(), 1);
        QCOMPARE(imaSpy.count(), 1);

        QTest::mouseClick(quickView, Qt::LeftButton, Qt::NoModifier, QPoint(175, 175));
        QCOMPARE(maSpy.count(), 2);
        QCOMPARE(imaSpy.count(), 1);
    }

};

QTEST_MAIN(tst_InverseMouseAreaTest)

#include "tst_inversemouseareatest.moc"
