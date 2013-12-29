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

#include "ucmousefilters.h"
#include "inversemouseareatype.h"
#include <private/qquickevents_p_p.h>
#include <private/qquickmousearea_p.h>

// keep in sync with QQuickMouseArea PressAndHoldDelay
const int DefaultPressAndHoldDelay = 800;

class tst_mouseFilterTest : public QObject
{
    Q_OBJECT
    
public:
    tst_mouseFilterTest() {}

private:
    QString m_modulePath;
    bool insideClick;
    bool oskClick;

    QQuickView * loadTest(const QString &file)
    {
        QQuickView *view = new QQuickView;
        view->engine()->addImportPath(m_modulePath);

        view->setSource(QUrl::fromLocalFile(file));
        if (!view->rootObject()) {
            delete view;
            view = 0;
        } else {
            view->show();
            QTest::qWaitForWindowExposed(view);
        }
        return view;
    }

    UCMouse *attachedFilter(QQuickItem *item)
    {
        return qobject_cast<UCMouse*>(qmlAttachedPropertiesObject<UCMouse>(item, false));
    }

    void pressAndHold(QWindow *view, Qt::MouseButton button, Qt::KeyboardModifiers modifiers, const QPoint &point, int delay = 900)
    {
        QTest::mousePress(view, button, modifiers, point);
        QTest::qWait(delay);
    }

protected Q_SLOTS:
    void filterPressOverOSK(UCMouseEvent *event)
    {
        oskClick = event->overInputPanel();
    }
    void filterClick(UCMouseEvent *event)
    {
        insideClick = event->inside();
    }

private Q_SLOTS:

    void initTestCase()
    {
        QString modules("../../../modules");
        QVERIFY(QDir(modules).exists());

        m_modulePath = QDir(modules).absolutePath();
    }

    void cleanupTestCase()
    {
    }

    void testCase_pressedInside()
    {
        QScopedPointer<QQuickView> view(loadTest("FilterTextInput.qml"));
        QVERIFY(view);
        UCMouse *filter = attachedFilter(view->rootObject());
        QVERIFY(filter);
        QSignalSpy pressed(filter, SIGNAL(pressed(UCMouseEvent*)));
        QSignalSpy released(filter, SIGNAL(released(UCMouseEvent*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(UCMouseEvent*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(UCMouseEvent*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(UCMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(UCMouseEvent*)));
        QSignalSpy entered(filter, SIGNAL(entered()));
        QSignalSpy exited(filter, SIGNAL(exited()));

        QTest::mouseClick(view.data(), Qt::LeftButton, 0, QPoint(10, 10));
        QTest::waitForEvents();
        QCOMPARE(view->rootObject()->hasFocus(), true);
        QCOMPARE(pressed.count(), 1);
        QCOMPARE(released.count(), 1);
        QCOMPARE(clicked.count(), 1);
        QCOMPARE(positionChanged.count(), 0);
        QCOMPARE(doubleClicked.count(), 0);
        QCOMPARE(pressAndHold.count(), 0);
        QCOMPARE(entered.count(), 1);
        QCOMPARE(exited.count(), 1);
    }

    void testCase_pressAndMovedInside()
    {
        QScopedPointer<QQuickView> view(loadTest("FilterTextInput.qml"));
        QVERIFY(view);
        UCMouse *filter = attachedFilter(view->rootObject());
        QVERIFY(filter);
        QSignalSpy pressed(filter, SIGNAL(pressed(UCMouseEvent*)));
        QSignalSpy released(filter, SIGNAL(released(UCMouseEvent*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(UCMouseEvent*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(UCMouseEvent*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(UCMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(UCMouseEvent*)));
        QSignalSpy entered(filter, SIGNAL(entered()));
        QSignalSpy exited(filter, SIGNAL(exited()));

        // make sure we do not get double click because of the previous tests
        QTest::qWait(300);
        QTest::mousePress(view.data(), Qt::LeftButton, 0, QPoint(10, 10));
        QTest::mouseMove(view.data(), QPoint(15, 15));
        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, QPoint(15, 15));
        QTest::waitForEvents();
        QCOMPARE(view->rootObject()->hasFocus(), true);
        QCOMPARE(pressed.count(), 1);
        QCOMPARE(released.count(), 1);
        QCOMPARE(clicked.count(), 1);
        QCOMPARE(positionChanged.count(), 1);
        QCOMPARE(doubleClicked.count(), 0);
        QCOMPARE(pressAndHold.count(), 0);
        QCOMPARE(entered.count(), 1);
        QCOMPARE(exited.count(), 1);
    }

    void testCase_pressAndHoldDisableClick() {
        QScopedPointer<QQuickView> view(loadTest("PressAndHoldAcceptedEatsClick.qml"));
        QVERIFY(view);
        UCMouse *filter = attachedFilter(view->rootObject());
        QVERIFY(filter);
        QSignalSpy clicked(filter, SIGNAL(clicked(UCMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(UCMouseEvent*)));

        QTest::mousePress(view.data(), Qt::LeftButton, 0, QPoint(10, 10));
        QTest::qWait(DefaultPressAndHoldDelay + 200);
        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, QPoint(10, 10));
        QTest::waitForEvents();
        QCOMPARE(view->rootObject()->hasFocus(), true);
        QCOMPARE(pressAndHold.count(), 1);
        QCOMPARE(clicked.count(), 0);
    }

    void testCase_pressAndHoldAndClick() {
        QScopedPointer<QQuickView> view(loadTest("PressAndHoldAndClicked.qml"));
        QVERIFY(view);
        UCMouse *filter = attachedFilter(view->rootObject());
        QVERIFY(filter);
        QSignalSpy clicked(filter, SIGNAL(clicked(UCMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(UCMouseEvent*)));

        QTest::mousePress(view.data(), Qt::LeftButton, 0, QPoint(10, 10));
        QTest::qWait(DefaultPressAndHoldDelay + 200);
        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, QPoint(10, 10));
        QTest::waitForEvents();
        QCOMPARE(view->rootObject()->hasFocus(), true);
        QCOMPARE(pressAndHold.count(), 1);
        QCOMPARE(clicked.count(), 1);
    }

//    void testCase_PressOverOSK()
//    {
//        QScopedPointer<QQuickView> view(loadTest("PressOverOSK.qml"));
//        QVERIFY(view);

//        QQuickItem *input = view->rootObject()->findChild<QQuickItem*>("input");
//        QVERIFY(input);
//        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
//        QVERIFY(filter);
//        QSignalSpy filterSpy(filter, SIGNAL(pressed(UCMouseEvent*)));
//        QObject::connect(filter, SIGNAL(pressed(UCMouseEvent*)), this, SLOT(filterPressOverOSK(UCMouseEvent*)));

//        // set focus for text input by pressing over it
//        QTest::mouseClick(view.data(), Qt::LeftButton, 0, QPoint (30, 5));
//        QCOMPARE(input->property("focus").toBool(), true);
//        QCOMPARE(filterSpy.count(), 0);

//        // press over the OSK area
//        oskClick = false;
//        QTest::mouseClick(view.data(), Qt::LeftButton, 0, QPoint(30, view->rootObject()->height() - 10));
//        QCOMPARE(filterSpy.count(), 1);
//        if (!qEnvironmentVariableIsSet("QT_IM_MODULE")) {
//            QEXPECT_FAIL(0, "This can be tested only with OSK presence", Continue);
//        }
//        QCOMPARE(oskClick, true);
//    }

//    void testCase_FilterPressedInside()
//    {
//        QScopedPointer<QQuickView> view(loadTest("Filter.qml"));
//        QVERIFY(view);

//        QQuickMouseArea *ma = qobject_cast<QQuickMouseArea*>(view->rootObject()->findChild<QQuickItem*>("MA"));
//        QVERIFY(ma);
//        InverseMouseAreaType *ima = qobject_cast<InverseMouseAreaType*>(view->rootObject()->findChild<QQuickItem*>("IMA"));
//        QVERIFY(ima);
//        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
//        QVERIFY(filter);
//        QSignalSpy maSpy(ma, SIGNAL(pressed(QQuickMouseEvent*)));
//        QSignalSpy imaSpy(ima, SIGNAL(pressed(QQuickMouseEvent*)));
//        QSignalSpy filterSpy(filter, SIGNAL(pressed(UCMouseEvent*)));

//        QTest::mouseClick(view.data(), Qt::LeftButton, 0, QPoint(30, 30));
//        QCOMPARE(maSpy.count(), 1);
//        QCOMPARE(imaSpy.count(), 0);
//        QCOMPARE(filterSpy.count(), 1);
//    }

//    void testCase_FilterPressedOutside()
//    {
//        QScopedPointer<QQuickView> view(loadTest("Filter.qml"));
//        QVERIFY(view);

//        QQuickMouseArea *ma = qobject_cast<QQuickMouseArea*>(view->rootObject()->findChild<QQuickItem*>("MA"));
//        QVERIFY(ma);
//        InverseMouseAreaType *ima = qobject_cast<InverseMouseAreaType*>(view->rootObject()->findChild<QQuickItem*>("IMA"));
//        QVERIFY(ima);
//        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
//        QVERIFY(filter);
//        QSignalSpy maSpy(ma, SIGNAL(pressed(QQuickMouseEvent*)));
//        QSignalSpy imaSpy(ima, SIGNAL(pressed(QQuickMouseEvent*)));
//        QSignalSpy filterSpy(filter, SIGNAL(pressed(UCMouseEvent*)));

//        QTest::mouseClick(view.data(), Qt::LeftButton, 0, QPoint(20, 20));
//        QCOMPARE(maSpy.count(), 0);
//        QCOMPARE(imaSpy.count(), 1);
//        QCOMPARE(filterSpy.count(), 1);
//    }

//    void testCase_FilterNoLongpressAfterPressedInside()
//    {
//        QScopedPointer<QQuickView> view(loadTest("Filter.qml"));
//        QVERIFY(view);

//        QQuickMouseArea *ma = qobject_cast<QQuickMouseArea*>(view->rootObject()->findChild<QQuickItem*>("MA"));
//        QVERIFY(ma);
//        InverseMouseAreaType *ima = qobject_cast<InverseMouseAreaType*>(view->rootObject()->findChild<QQuickItem*>("IMA"));
//        QVERIFY(ima);
//        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
//        QVERIFY(filter);
//        QSignalSpy maSpy(ma, SIGNAL(pressed(QQuickMouseEvent*)));
//        QSignalSpy imaSpy(ima, SIGNAL(pressed(QQuickMouseEvent*)));
//        QSignalSpy filterSpy(filter, SIGNAL(pressed(UCMouseEvent*)));
//        QSignalSpy filterLongPress(filter, SIGNAL(pressAndHold(UCMouseEvent*)));

//        QTest::mousePress(view.data(), Qt::LeftButton, 0, QPoint(30, 30));
//        QTest::mouseMove(view.data(), QPoint(40, 30));
//        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, QPoint(40, 30));
//        QTest::qWait(filter->pressAndHoldDelay() + 200);
//        QCOMPARE(maSpy.count(), 1);
//        QCOMPARE(imaSpy.count(), 0);
//        QCOMPARE(filterSpy.count(), 1);
//        QCOMPARE(filterLongPress.count(), 0);
//    }

//    void testCase_FilterReleasedInside()
//    {
//        QScopedPointer<QQuickView> view(loadTest("Filter.qml"));
//        QVERIFY(view);

//        QQuickMouseArea *ma = qobject_cast<QQuickMouseArea*>(view->rootObject()->findChild<QQuickItem*>("MA"));
//        QVERIFY(ma);
//        InverseMouseAreaType *ima = qobject_cast<InverseMouseAreaType*>(view->rootObject()->findChild<QQuickItem*>("IMA"));
//        QVERIFY(ima);
//        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
//        QVERIFY(filter);
//        QSignalSpy maSpy(ma, SIGNAL(released(QQuickMouseEvent*)));
//        QSignalSpy imaSpy(ima, SIGNAL(released(QQuickMouseEvent*)));
//        QSignalSpy filterSpy(filter, SIGNAL(released(UCMouseEvent*)));

//        QTest::mouseClick(view.data(), Qt::LeftButton, 0, QPoint(30, 30));
//        QCOMPARE(maSpy.count(), 1);
//        QCOMPARE(imaSpy.count(), 0);
//        QCOMPARE(filterSpy.count(), 1);
//    }

//    void testCase_FilterReleasedOutside()
//    {
//        QScopedPointer<QQuickView> view(loadTest("Filter.qml"));
//        QVERIFY(view);

//        QQuickMouseArea *ma = qobject_cast<QQuickMouseArea*>(view->rootObject()->findChild<QQuickItem*>("MA"));
//        QVERIFY(ma);
//        InverseMouseAreaType *ima = qobject_cast<InverseMouseAreaType*>(view->rootObject()->findChild<QQuickItem*>("IMA"));
//        QVERIFY(ima);
//        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
//        QVERIFY(filter);
//        QSignalSpy maSpy(ma, SIGNAL(released(QQuickMouseEvent*)));
//        QSignalSpy imaSpy(ima, SIGNAL(released(QQuickMouseEvent*)));
//        QSignalSpy filterSpy(filter, SIGNAL(released(UCMouseEvent*)));

//        maSpy.clear(); filterSpy.clear();
//        QTest::mouseClick(view.data(), Qt::LeftButton, 0, QPoint(20, 20));
//        QCOMPARE(maSpy.count(), 0);
//        QCOMPARE(imaSpy.count(), 1);
//        QCOMPARE(filterSpy.count(), 1);
//    }

//    void testCase_FilterClickedInside()
//    {
//        QScopedPointer<QQuickView> view(loadTest("Filter.qml"));
//        QVERIFY(view);

//        QQuickMouseArea *ma = qobject_cast<QQuickMouseArea*>(view->rootObject()->findChild<QQuickItem*>("MA"));
//        QVERIFY(ma);
//        InverseMouseAreaType *ima = qobject_cast<InverseMouseAreaType*>(view->rootObject()->findChild<QQuickItem*>("IMA"));
//        QVERIFY(ima);
//        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
//        QVERIFY(filter);
//        QSignalSpy maSpy(ma, SIGNAL(clicked(QQuickMouseEvent*)));
//        QSignalSpy imaSpy(ima, SIGNAL(clicked(QQuickMouseEvent*)));
//        QSignalSpy filterSpy(filter, SIGNAL(clicked(UCMouseEvent*)));
//        QObject::connect(filter, SIGNAL(clicked(UCMouseEvent*)), this, SLOT(filterClick(UCMouseEvent*)));

//        insideClick = false;
//        QTest::mouseClick(view.data(), Qt::LeftButton, 0, QPoint(30, 30));
//        QCOMPARE(maSpy.count(), 1);
//        QCOMPARE(imaSpy.count(), 0);
//        QCOMPARE(filterSpy.count(), 1);
//        QCOMPARE(insideClick, true);
//    }

//    void testCase_FilterClickedOutside()
//    {
//        QScopedPointer<QQuickView> view(loadTest("Filter.qml"));
//        QVERIFY(view);

//        QQuickMouseArea *ma = qobject_cast<QQuickMouseArea*>(view->rootObject()->findChild<QQuickItem*>("MA"));
//        QVERIFY(ma);
//        InverseMouseAreaType *ima = qobject_cast<InverseMouseAreaType*>(view->rootObject()->findChild<QQuickItem*>("IMA"));
//        QVERIFY(ima);
//        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
//        QVERIFY(filter);
//        QSignalSpy maSpy(ma, SIGNAL(clicked(QQuickMouseEvent*)));
//        QSignalSpy imaSpy(ima, SIGNAL(clicked(QQuickMouseEvent*)));
//        QSignalSpy filterSpy(filter, SIGNAL(clicked(UCMouseEvent*)));
//        QObject::connect(filter, SIGNAL(clicked(UCMouseEvent*)), this, SLOT(filterClick(UCMouseEvent*)));

//        insideClick = false;
//        QTest::mouseClick(view.data(), Qt::LeftButton, 0, QPoint(20, 20));
//        QCOMPARE(maSpy.count(), 0);
//        QCOMPARE(imaSpy.count(), 1);
//        QCOMPARE(filterSpy.count(), 1);
//        QCOMPARE(insideClick, false);
//    }

//    void testCase_FilterPressAndHoldInside()
//    {
//        QScopedPointer<QQuickView> view(loadTest("Filter.qml"));
//        QVERIFY(view);

//        QQuickMouseArea *ma = qobject_cast<QQuickMouseArea*>(view->rootObject()->findChild<QQuickItem*>("MA"));
//        QVERIFY(ma);
//        InverseMouseAreaType *ima = qobject_cast<InverseMouseAreaType*>(view->rootObject()->findChild<QQuickItem*>("IMA"));
//        QVERIFY(ima);
//        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
//        QVERIFY(filter);
//        QSignalSpy maSpy(ma, SIGNAL(pressAndHold(QQuickMouseEvent*)));
//        QSignalSpy imaSpy(ima, SIGNAL(pressAndHold(QQuickMouseEvent*)));
//        QSignalSpy filterSpy(filter, SIGNAL(pressAndHold(UCMouseEvent*)));

//        // inside
//        pressAndHold(view.data(), Qt::LeftButton, 0, QPoint(30, 30));
//        // discard press event
//        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, QPoint(30, 30));
//        QCOMPARE(maSpy.count(), 1);
//        QCOMPARE(imaSpy.count(), 0);
//        QCOMPARE(filterSpy.count(), 1);
//    }

//    void testCase_FilterPressAndHoldOutside()
//    {
//        QScopedPointer<QQuickView> view(loadTest("Filter.qml"));
//        QVERIFY(view);

//        QQuickMouseArea *ma = qobject_cast<QQuickMouseArea*>(view->rootObject()->findChild<QQuickItem*>("MA"));
//        QVERIFY(ma);
//        InverseMouseAreaType *ima = qobject_cast<InverseMouseAreaType*>(view->rootObject()->findChild<QQuickItem*>("IMA"));
//        QVERIFY(ima);
//        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
//        QVERIFY(filter);
//        QSignalSpy maSpy(ma, SIGNAL(pressAndHold(QQuickMouseEvent*)));
//        QSignalSpy imaSpy(ima, SIGNAL(pressAndHold(QQuickMouseEvent*)));
//        QSignalSpy filterSpy(filter, SIGNAL(pressAndHold(UCMouseEvent*)));

//        // outside
//        pressAndHold(view.data(), Qt::LeftButton, 0, QPoint(20, 20));
//        // discard press event
//        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, QPoint(20, 20));
//        QCOMPARE(maSpy.count(), 0);
//        QCOMPARE(imaSpy.count(), 1);
//        QCOMPARE(filterSpy.count(), 1);
//    }

//    void testCase_FilterPressAndHoldInsideNewDelay()
//    {
//        QScopedPointer<QQuickView> view(loadTest("FilterBiggerDelay.qml"));
//        QVERIFY(view);

//        QQuickMouseArea *ma = qobject_cast<QQuickMouseArea*>(view->rootObject()->findChild<QQuickItem*>("MA"));
//        QVERIFY(ma);
//        InverseMouseAreaType *ima = qobject_cast<InverseMouseAreaType*>(view->rootObject()->findChild<QQuickItem*>("IMA"));
//        QVERIFY(ima);
//        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
//        QVERIFY(filter);
//        QSignalSpy maSpy(ma, SIGNAL(pressAndHold(QQuickMouseEvent*)));
//        QSignalSpy imaSpy(ima, SIGNAL(pressAndHold(QQuickMouseEvent*)));
//        QSignalSpy filterSpy(filter, SIGNAL(pressAndHold(UCMouseEvent*)));

//        // inside
//        pressAndHold(view.data(), Qt::LeftButton, 0, QPoint(30, 30));
//        // discard press event
//        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, QPoint(30, 30));
//        QCOMPARE(maSpy.count(), 1);
//        QCOMPARE(imaSpy.count(), 0);
//        QCOMPARE(filterSpy.count(), 0);

//        // one more time, with soem extra threshold
//        maSpy.clear();
//        pressAndHold(view.data(), Qt::LeftButton, 0, QPoint(30, 30), 1100);
//        // discard press even
//        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, QPoint(30, 30));
//        QCOMPARE(maSpy.count(), 1);
//        QCOMPARE(imaSpy.count(), 0);
//        QCOMPARE(filterSpy.count(), 1);
//    }

//    void testCase_FilterPressAndHoldOutsideNewDelay()
//    {
//        QScopedPointer<QQuickView> view(loadTest("FilterBiggerDelay.qml"));
//        QVERIFY(view);

//        QQuickMouseArea *ma = qobject_cast<QQuickMouseArea*>(view->rootObject()->findChild<QQuickItem*>("MA"));
//        QVERIFY(ma);
//        InverseMouseAreaType *ima = qobject_cast<InverseMouseAreaType*>(view->rootObject()->findChild<QQuickItem*>("IMA"));
//        QVERIFY(ima);
//        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
//        QVERIFY(filter);
//        QSignalSpy maSpy(ma, SIGNAL(pressAndHold(QQuickMouseEvent*)));
//        QSignalSpy imaSpy(ima, SIGNAL(pressAndHold(QQuickMouseEvent*)));
//        QSignalSpy filterSpy(filter, SIGNAL(pressAndHold(UCMouseEvent*)));

//        // outside
//        pressAndHold(view.data(), Qt::LeftButton, 0, QPoint(20, 20));
//        // discard press event
//        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, QPoint(20, 20));
//        QCOMPARE(maSpy.count(), 0);
//        QCOMPARE(imaSpy.count(), 1);
//        QCOMPARE(filterSpy.count(), 0);

//        // one more time, with soem extra threshold
//        imaSpy.clear();
//        pressAndHold(view.data(), Qt::LeftButton, 0, QPoint(20, 20), 1100);
//        // discard press event
//        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, QPoint(20, 20));
//        QCOMPARE(maSpy.count(), 0);
//        QCOMPARE(imaSpy.count(), 1);
//        QCOMPARE(filterSpy.count(), 1);
//    }

//    void testCase_FilterDoubleClickInside()
//    {
//        QScopedPointer<QQuickView> view(loadTest("Filter.qml"));
//        QVERIFY(view);

//        QQuickMouseArea *ma = qobject_cast<QQuickMouseArea*>(view->rootObject()->findChild<QQuickItem*>("MA"));
//        QVERIFY(ma);
//        InverseMouseAreaType *ima = qobject_cast<InverseMouseAreaType*>(view->rootObject()->findChild<QQuickItem*>("IMA"));
//        QVERIFY(ima);
//        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
//        QVERIFY(filter);
//        QSignalSpy maSpy(ma, SIGNAL(doubleClicked(QQuickMouseEvent*)));
//        QSignalSpy imaSpy(ima, SIGNAL(doubleClicked(QQuickMouseEvent*)));
//        QSignalSpy filterSpy(filter, SIGNAL(doubleClicked(UCMouseEvent*)));
//        QSignalSpy maClickSpy(ma, SIGNAL(clicked(QQuickMouseEvent*)));
//        QSignalSpy imaClickSpy(ima, SIGNAL(clicked(QQuickMouseEvent*)));
//        QSignalSpy filterClickSpy(filter, SIGNAL(clicked(UCMouseEvent*)));

//        // inside
//        QTest::mouseDClick(view.data(), Qt::LeftButton, 0, QPoint(30, 30));
//        QCOMPARE(maSpy.count(), 1);
//        QCOMPARE(imaSpy.count(), 0);
//        QCOMPARE(filterSpy.count(), 1);
//        QCOMPARE(maClickSpy.count(), 1);
//        QCOMPARE(imaClickSpy.count(), 0);
//        QCOMPARE(filterClickSpy.count(), 1);
//    }

//    void testCase_FilterDoubleClickOutside()
//    {
//        QScopedPointer<QQuickView> view(loadTest("Filter.qml"));
//        QVERIFY(view);

//        QQuickMouseArea *ma = qobject_cast<QQuickMouseArea*>(view->rootObject()->findChild<QQuickItem*>("MA"));
//        QVERIFY(ma);
//        InverseMouseAreaType *ima = qobject_cast<InverseMouseAreaType*>(view->rootObject()->findChild<QQuickItem*>("IMA"));
//        QVERIFY(ima);
//        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
//        QVERIFY(filter);
//        QSignalSpy maSpy(ma, SIGNAL(doubleClicked(QQuickMouseEvent*)));
//        QSignalSpy imaSpy(ima, SIGNAL(doubleClicked(QQuickMouseEvent*)));
//        QSignalSpy filterSpy(filter, SIGNAL(doubleClicked(UCMouseEvent*)));
//        QSignalSpy maClickSpy(ma, SIGNAL(clicked(QQuickMouseEvent*)));
//        QSignalSpy imaClickSpy(ima, SIGNAL(clicked(QQuickMouseEvent*)));
//        QSignalSpy filterClickSpy(filter, SIGNAL(clicked(UCMouseEvent*)));

//        // outside
//        QTest::mouseDClick(view.data(), Qt::LeftButton, 0, QPoint(20, 20));
//        QCOMPARE(maSpy.count(), 0);
//        QCOMPARE(imaSpy.count(), 1);
//        QCOMPARE(filterSpy.count(), 1);
//        QCOMPARE(maClickSpy.count(), 0);
//        QCOMPARE(imaClickSpy.count(), 1);
//        QCOMPARE(filterClickSpy.count(), 1);
//    }

//    void testCase_FilterEnteredInside()
//    {
//        QScopedPointer<QQuickView> view(loadTest("Filter.qml"));
//        QVERIFY(view);

//        QQuickMouseArea *ma = qobject_cast<QQuickMouseArea*>(view->rootObject()->findChild<QQuickItem*>("MA"));
//        QVERIFY(ma);
//        InverseMouseAreaType *ima = qobject_cast<InverseMouseAreaType*>(view->rootObject()->findChild<QQuickItem*>("IMA"));
//        QVERIFY(ima);
//        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
//        QVERIFY(filter);
//        QSignalSpy maSpy(ma, SIGNAL(entered()));
//        QSignalSpy imaSpy(ima, SIGNAL(entered()));
//        QSignalSpy filterSpy(filter, SIGNAL(entered()));

//        // inside
//        QTest::mouseClick(view.data(), Qt::LeftButton, 0, QPoint(30, 30));
//        QCOMPARE(maSpy.count(), 1);
//        QCOMPARE(imaSpy.count(), 0);
//        QCOMPARE(filterSpy.count(), 1);
//    }

//    void testCase_FilterEnteredOutside()
//    {
//        QScopedPointer<QQuickView> view(loadTest("Filter.qml"));
//        QVERIFY(view);

//        QQuickMouseArea *ma = qobject_cast<QQuickMouseArea*>(view->rootObject()->findChild<QQuickItem*>("MA"));
//        QVERIFY(ma);
//        InverseMouseAreaType *ima = qobject_cast<InverseMouseAreaType*>(view->rootObject()->findChild<QQuickItem*>("IMA"));
//        QVERIFY(ima);
//        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
//        QVERIFY(filter);
//        QSignalSpy maSpy(ma, SIGNAL(entered()));
//        QSignalSpy imaSpy(ima, SIGNAL(entered()));
//        QSignalSpy filterSpy(filter, SIGNAL(entered()));

//        // outside
//        QTest::mouseClick(view.data(), Qt::LeftButton, 0, QPoint(20, 20));
//        QCOMPARE(maSpy.count(), 0);
//        QCOMPARE(imaSpy.count(), 1);
//        QCOMPARE(filterSpy.count(), 0);
//    }

//    void testCase_FilterExitedInside()
//    {
//        QScopedPointer<QQuickView> view(loadTest("Filter.qml"));
//        QVERIFY(view);

//        QQuickMouseArea *ma = qobject_cast<QQuickMouseArea*>(view->rootObject()->findChild<QQuickItem*>("MA"));
//        QVERIFY(ma);
//        InverseMouseAreaType *ima = qobject_cast<InverseMouseAreaType*>(view->rootObject()->findChild<QQuickItem*>("IMA"));
//        QVERIFY(ima);
//        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
//        QVERIFY(filter);
//        QSignalSpy maSpy(ma, SIGNAL(exited()));
//        QSignalSpy imaSpy(ima, SIGNAL(exited()));
//        QSignalSpy filterSpy(filter, SIGNAL(exited()));

//        // inside
//        QTest::mouseClick(view.data(), Qt::LeftButton, 0, QPoint(50, 50));
//        QCOMPARE(maSpy.count(), 1);
//        QCOMPARE(imaSpy.count(), 0);
//        QCOMPARE(filterSpy.count(), 1);
//    }

//    void testCase_FilterExitedOutside()
//    {
//        QScopedPointer<QQuickView> view(loadTest("Filter.qml"));
//        QVERIFY(view);

//        QQuickMouseArea *ma = qobject_cast<QQuickMouseArea*>(view->rootObject()->findChild<QQuickItem*>("MA"));
//        QVERIFY(ma);
//        InverseMouseAreaType *ima = qobject_cast<InverseMouseAreaType*>(view->rootObject()->findChild<QQuickItem*>("IMA"));
//        QVERIFY(ima);
//        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
//        QVERIFY(filter);
//        QSignalSpy maSpy(ma, SIGNAL(exited()));
//        QSignalSpy imaSpy(ima, SIGNAL(exited()));
//        QSignalSpy filterSpy(filter, SIGNAL(exited()));

//        // outside
//        QTest::mouseClick(view.data(), Qt::LeftButton, 0, QPoint(20, 20));
//        QCOMPARE(maSpy.count(), 0);
//        QCOMPARE(imaSpy.count(), 1);
//        QCOMPARE(filterSpy.count(), 0);
//    }

//    void testCase_FilterMouseMovedInside()
//    {
//        QScopedPointer<QQuickView> view(loadTest("Filter.qml"));
//        QVERIFY(view);

//        QQuickMouseArea *ma = qobject_cast<QQuickMouseArea*>(view->rootObject()->findChild<QQuickItem*>("MA"));
//        QVERIFY(ma);
//        InverseMouseAreaType *ima = qobject_cast<InverseMouseAreaType*>(view->rootObject()->findChild<QQuickItem*>("IMA"));
//        QVERIFY(ima);
//        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
//        QVERIFY(filter);
//        QSignalSpy maSpy(ma, SIGNAL(positionChanged(QQuickMouseEvent*)));
//        QSignalSpy imaSpy(ima, SIGNAL(positionChanged(QQuickMouseEvent*)));
//        QSignalSpy filterSpy(filter, SIGNAL(positionChanged(UCMouseEvent*)));

//        // inside
//        QTest::mousePress(view.data(), Qt::LeftButton, 0, QPoint(30, 30));
//        QTest::mouseMove(view.data(), QPoint(40, 40));
//        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, QPoint(40, 40));
//        QCOMPARE(maSpy.count(), 1);
//        QCOMPARE(imaSpy.count(), 0);
//        QCOMPARE(filterSpy.count(), 1);
//    }

//    void testCase_FilterMouseMovedOutside()
//    {
//        QScopedPointer<QQuickView> view(loadTest("Filter.qml"));
//        QVERIFY(view);

//        QQuickMouseArea *ma = qobject_cast<QQuickMouseArea*>(view->rootObject()->findChild<QQuickItem*>("MA"));
//        QVERIFY(ma);
//        InverseMouseAreaType *ima = qobject_cast<InverseMouseAreaType*>(view->rootObject()->findChild<QQuickItem*>("IMA"));
//        QVERIFY(ima);
//        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
//        QVERIFY(filter);
//        QSignalSpy maSpy(ma, SIGNAL(positionChanged(QQuickMouseEvent*)));
//        QSignalSpy imaSpy(ima, SIGNAL(positionChanged(QQuickMouseEvent*)));
//        QSignalSpy filterSpy(filter, SIGNAL(positionChanged(UCMouseEvent*)));

//        // outside
//        QTest::mousePress(view.data(), Qt::LeftButton, 0, QPoint(20, 20));
//        QTest::mouseMove(view.data(), QPoint(40, 20));
//        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, QPoint(40, 20));
//        QCOMPARE(maSpy.count(), 0);
//        QCOMPARE(imaSpy.count(), 1);
//        QCOMPARE(filterSpy.count(), 1);
//    }

//    void testCase_FilterMouseMovedInsideOut()
//    {
//        QScopedPointer<QQuickView> view(loadTest("Filter.qml"));
//        QVERIFY(view);

//        QQuickMouseArea *ma = qobject_cast<QQuickMouseArea*>(view->rootObject()->findChild<QQuickItem*>("MA"));
//        QVERIFY(ma);
//        InverseMouseAreaType *ima = qobject_cast<InverseMouseAreaType*>(view->rootObject()->findChild<QQuickItem*>("IMA"));
//        QVERIFY(ima);
//        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
//        QVERIFY(filter);
//        QSignalSpy maSpy(ma, SIGNAL(positionChanged(QQuickMouseEvent*)));
//        QSignalSpy imaSpy(ima, SIGNAL(positionChanged(QQuickMouseEvent*)));
//        QSignalSpy filterSpy(filter, SIGNAL(positionChanged(UCMouseEvent*)));

//        // inside -> out
//        QTest::mousePress(view.data(), Qt::LeftButton, 0, QPoint(30, 30));
//        QTest::mouseMove(view.data(), QPoint(20, 20));
//        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, QPoint(20, 20));
//        QCOMPARE(maSpy.count(), 1);
//        QCOMPARE(imaSpy.count(), 0);
//        QCOMPARE(filterSpy.count(), 1);
//    }

//    void testCase_FilterMouseMovedOutsideIn()
//    {
//        QScopedPointer<QQuickView> view(loadTest("Filter.qml"));
//        QVERIFY(view);

//        QQuickMouseArea *ma = qobject_cast<QQuickMouseArea*>(view->rootObject()->findChild<QQuickItem*>("MA"));
//        QVERIFY(ma);
//        InverseMouseAreaType *ima = qobject_cast<InverseMouseAreaType*>(view->rootObject()->findChild<QQuickItem*>("IMA"));
//        QVERIFY(ima);
//        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
//        QVERIFY(filter);
//        QSignalSpy maSpy(ma, SIGNAL(positionChanged(QQuickMouseEvent*)));
//        QSignalSpy imaSpy(ima, SIGNAL(positionChanged(QQuickMouseEvent*)));
//        QSignalSpy filterSpy(filter, SIGNAL(positionChanged(UCMouseEvent*)));

//        // inside -> out
//        QTest::mousePress(view.data(), Qt::LeftButton, 0, QPoint(20, 20));
//        QTest::mouseMove(view.data(), QPoint(40, 40));
//        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, QPoint(40, 40));
//        QCOMPARE(maSpy.count(), 0);
//        QCOMPARE(imaSpy.count(), 1);
//        QCOMPARE(filterSpy.count(), 1);
//    }

//    void testCase_CaptureMouseMovedInside()
//    {
//        QScopedPointer<QQuickView> view(loadTest("CaptureMouseMove.qml"));
//        QVERIFY(view);

//        QQuickMouseArea *ma = qobject_cast<QQuickMouseArea*>(view->rootObject()->findChild<QQuickItem*>("MA"));
//        QVERIFY(ma);
//        InverseMouseAreaType *ima = qobject_cast<InverseMouseAreaType*>(view->rootObject()->findChild<QQuickItem*>("IMA"));
//        QVERIFY(ima);
//        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
//        QVERIFY(filter);
//        QSignalSpy maSpy(ma, SIGNAL(positionChanged(QQuickMouseEvent*)));
//        QSignalSpy imaSpy(ima, SIGNAL(positionChanged(QQuickMouseEvent*)));
//        QSignalSpy filterSpy(filter, SIGNAL(positionChanged(UCMouseEvent*)));

//        // inside
//        QTest::mousePress(view.data(), Qt::LeftButton, 0, QPoint(30, 30));
//        QTest::mouseMove(view.data(), QPoint(40, 40));
//        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, QPoint(40, 40));
//        QCOMPARE(maSpy.count(), 0);
//        QCOMPARE(imaSpy.count(), 0);
//        QCOMPARE(filterSpy.count(), 1);
//    }

//    void testCase_CaptureMouseMovedOutside()
//    {
//        QScopedPointer<QQuickView> view(loadTest("CaptureMouseMove.qml"));
//        QVERIFY(view);

//        QQuickMouseArea *ma = qobject_cast<QQuickMouseArea*>(view->rootObject()->findChild<QQuickItem*>("MA"));
//        QVERIFY(ma);
//        InverseMouseAreaType *ima = qobject_cast<InverseMouseAreaType*>(view->rootObject()->findChild<QQuickItem*>("IMA"));
//        QVERIFY(ima);
//        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
//        QVERIFY(filter);
//        QSignalSpy maSpy(ma, SIGNAL(positionChanged(QQuickMouseEvent*)));
//        QSignalSpy imaSpy(ima, SIGNAL(positionChanged(QQuickMouseEvent*)));
//        QSignalSpy filterSpy(filter, SIGNAL(positionChanged(UCMouseEvent*)));

//        // outside
//        QTest::mousePress(view.data(), Qt::LeftButton, 0, QPoint(20, 20));
//        QTest::mouseMove(view.data(), QPoint(40, 20));
//        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, QPoint(40, 20));
//        QCOMPARE(maSpy.count(), 0);
//        QCOMPARE(imaSpy.count(), 0);
//        QCOMPARE(filterSpy.count(), 1);
//    }

//    void testCase_CaptureMouseMovedInsideOut()
//    {
//        QScopedPointer<QQuickView> view(loadTest("CaptureMouseMove.qml"));
//        QVERIFY(view);

//        QQuickMouseArea *ma = qobject_cast<QQuickMouseArea*>(view->rootObject()->findChild<QQuickItem*>("MA"));
//        QVERIFY(ma);
//        InverseMouseAreaType *ima = qobject_cast<InverseMouseAreaType*>(view->rootObject()->findChild<QQuickItem*>("IMA"));
//        QVERIFY(ima);
//        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
//        QVERIFY(filter);
//        QSignalSpy maSpy(ma, SIGNAL(positionChanged(QQuickMouseEvent*)));
//        QSignalSpy imaSpy(ima, SIGNAL(positionChanged(QQuickMouseEvent*)));
//        QSignalSpy filterSpy(filter, SIGNAL(positionChanged(UCMouseEvent*)));

//        // inside -> out
//        QTest::mousePress(view.data(), Qt::LeftButton, 0, QPoint(30, 30));
//        QTest::mouseMove(view.data(), QPoint(20, 20));
//        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, QPoint(20, 20));
//        QCOMPARE(maSpy.count(), 0);
//        QCOMPARE(imaSpy.count(), 0);
//        QCOMPARE(filterSpy.count(), 1);
//    }

//    void testCase_CaptureMouseMovedOutsideIn()
//    {
//        QScopedPointer<QQuickView> view(loadTest("CaptureMouseMove.qml"));
//        QVERIFY(view);

//        QQuickMouseArea *ma = qobject_cast<QQuickMouseArea*>(view->rootObject()->findChild<QQuickItem*>("MA"));
//        QVERIFY(ma);
//        InverseMouseAreaType *ima = qobject_cast<InverseMouseAreaType*>(view->rootObject()->findChild<QQuickItem*>("IMA"));
//        QVERIFY(ima);
//        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
//        QVERIFY(filter);
//        QSignalSpy maSpy(ma, SIGNAL(positionChanged(QQuickMouseEvent*)));
//        QSignalSpy imaSpy(ima, SIGNAL(positionChanged(QQuickMouseEvent*)));
//        QSignalSpy filterSpy(filter, SIGNAL(positionChanged(UCMouseEvent*)));

//        // inside -> out
//        QTest::mousePress(view.data(), Qt::LeftButton, 0, QPoint(20, 20));
//        QTest::mouseMove(view.data(), QPoint(40, 40));
//        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, QPoint(40, 40));
//        QCOMPARE(maSpy.count(), 0);
//        QCOMPARE(imaSpy.count(), 0);
//        QCOMPARE(filterSpy.count(), 1);
//    }

//    void testCase_MouseFilterInWindowInsidePress()
//    {
//        QScopedPointer<QQuickView> view(loadTest("FilterInWindow.qml"));
//        QVERIFY(view);
//        QList<QQuickWindow*> windowList = view->rootObject()->findChildren<QQuickWindow*>("window");
//        QVERIFY(windowList.count());
//        QQuickWindow *window = windowList[0];

//        QQuickMouseArea *ma = qobject_cast<QQuickMouseArea*>(view->rootObject()->findChild<QQuickItem*>("MA"));
//        QVERIFY(ma);
//        InverseMouseAreaType *ima = qobject_cast<InverseMouseAreaType*>(view->rootObject()->findChild<QQuickItem*>("IMA"));
//        QVERIFY(ima);
//        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
//        QVERIFY(filter);
//        QSignalSpy maSpy(ma, SIGNAL(pressed(QQuickMouseEvent*)));
//        QSignalSpy imaSpy(ima, SIGNAL(pressed(QQuickMouseEvent*)));
//        QSignalSpy filterSpy(filter, SIGNAL(pressed(UCMouseEvent*)));

//        // insideClick
//        QTest::mouseClick(window, Qt::LeftButton, 0, QPoint(30, 30));
//        QCOMPARE(maSpy.count(), 1);
//        QCOMPARE(imaSpy.count(), 0);
//        QCOMPARE(filterSpy.count(), 1);
//    }

//    void testCase_MouseFilterInWindowOutsidePress()
//    {
//        QScopedPointer<QQuickView> view(loadTest("FilterInWindow.qml"));
//        QVERIFY(view);
//        QList<QQuickWindow*> windowList = view->rootObject()->findChildren<QQuickWindow*>("window");
//        QVERIFY(windowList.count());
//        QQuickWindow *window = windowList[0];

//        QQuickMouseArea *ma = qobject_cast<QQuickMouseArea*>(view->rootObject()->findChild<QQuickItem*>("MA"));
//        QVERIFY(ma);
//        InverseMouseAreaType *ima = qobject_cast<InverseMouseAreaType*>(view->rootObject()->findChild<QQuickItem*>("IMA"));
//        QVERIFY(ima);
//        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
//        QVERIFY(filter);
//        QSignalSpy maSpy(ma, SIGNAL(pressed(QQuickMouseEvent*)));
//        QSignalSpy imaSpy(ima, SIGNAL(pressed(QQuickMouseEvent*)));
//        QSignalSpy filterSpy(filter, SIGNAL(pressed(UCMouseEvent*)));

//        QTest::mouseClick(window, Qt::LeftButton, 0, QPoint(20, 20));
//        QCOMPARE(maSpy.count(), 0);
//        QCOMPARE(imaSpy.count(), 1);
//        QCOMPARE(filterSpy.count(), 1);
//    }

//    void testCase_CapturePressedInside()
//    {
//        QScopedPointer<QQuickView> view(loadTest("Capture.qml"));
//        QVERIFY(view);

//        QQuickMouseArea *ma = qobject_cast<QQuickMouseArea*>(view->rootObject()->findChild<QQuickItem*>("MA"));
//        QVERIFY(ma);
//        InverseMouseAreaType *ima = qobject_cast<InverseMouseAreaType*>(view->rootObject()->findChild<QQuickItem*>("IMA"));
//        QVERIFY(ima);
//        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
//        QVERIFY(filter);
//        QSignalSpy maSpy(ma, SIGNAL(pressed(QQuickMouseEvent*)));
//        QSignalSpy imaSpy(ima, SIGNAL(pressed(QQuickMouseEvent*)));
//        QSignalSpy filterSpy(filter, SIGNAL(pressed(UCMouseEvent*)));

//        QTest::mouseClick(view.data(), Qt::LeftButton, 0, QPoint(30, 30));
//        QCOMPARE(maSpy.count(), 0);
//        QCOMPARE(imaSpy.count(), 0);
//        QCOMPARE(filterSpy.count(), 1);
//    }

//    void testCase_CapturePressedOutside()
//    {
//        QScopedPointer<QQuickView> view(loadTest("Capture.qml"));
//        QVERIFY(view);

//        QQuickMouseArea *ma = qobject_cast<QQuickMouseArea*>(view->rootObject()->findChild<QQuickItem*>("MA"));
//        QVERIFY(ma);
//        InverseMouseAreaType *ima = qobject_cast<InverseMouseAreaType*>(view->rootObject()->findChild<QQuickItem*>("IMA"));
//        QVERIFY(ima);
//        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
//        QVERIFY(filter);
//        QSignalSpy maSpy(ma, SIGNAL(pressed(QQuickMouseEvent*)));
//        QSignalSpy imaSpy(ima, SIGNAL(pressed(QQuickMouseEvent*)));
//        QSignalSpy filterSpy(filter, SIGNAL(pressed(UCMouseEvent*)));

//        QTest::mouseClick(view.data(), Qt::LeftButton, 0, QPoint(20, 20));
//        QCOMPARE(maSpy.count(), 0);
//        QCOMPARE(imaSpy.count(), 0);
//        QCOMPARE(filterSpy.count(), 1);
//    }

//    void testCase_CaptureClickedInside()
//    {
//        QScopedPointer<QQuickView> view(loadTest("CaptureClicked.qml"));
//        QVERIFY(view);

//        QQuickMouseArea *ma = qobject_cast<QQuickMouseArea*>(view->rootObject()->findChild<QQuickItem*>("MA"));
//        QVERIFY(ma);
//        InverseMouseAreaType *ima = qobject_cast<InverseMouseAreaType*>(view->rootObject()->findChild<QQuickItem*>("IMA"));
//        QVERIFY(ima);
//        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
//        QVERIFY(filter);
//        QSignalSpy maSpy(ma, SIGNAL(clicked(QQuickMouseEvent*)));
//        QSignalSpy imaSpy(ima, SIGNAL(clicked(QQuickMouseEvent*)));
//        QSignalSpy filterSpy(filter, SIGNAL(clicked(UCMouseEvent*)));

//        QTest::mouseClick(view.data(), Qt::LeftButton, 0, QPoint(30, 30));
//        QCOMPARE(maSpy.count(), 0);
//        QCOMPARE(imaSpy.count(), 0);
//        QCOMPARE(filterSpy.count(), 1);
//    }

//    void testCase_CaptureClickedOutside()
//    {
//        QScopedPointer<QQuickView> view(loadTest("CaptureClicked.qml"));
//        QVERIFY(view);

//        QQuickMouseArea *ma = qobject_cast<QQuickMouseArea*>(view->rootObject()->findChild<QQuickItem*>("MA"));
//        QVERIFY(ma);
//        InverseMouseAreaType *ima = qobject_cast<InverseMouseAreaType*>(view->rootObject()->findChild<QQuickItem*>("IMA"));
//        QVERIFY(ima);
//        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
//        QVERIFY(filter);
//        QSignalSpy maSpy(ma, SIGNAL(clicked(QQuickMouseEvent*)));
//        QSignalSpy imaSpy(ima, SIGNAL(clicked(QQuickMouseEvent*)));
//        QSignalSpy filterSpy(filter, SIGNAL(clicked(UCMouseEvent*)));

//        QTest::mouseClick(view.data(), Qt::LeftButton, 0, QPoint(20, 20));
//        QCOMPARE(maSpy.count(), 0);
//        QCOMPARE(imaSpy.count(), 0);
//        QCOMPARE(filterSpy.count(), 1);
//    }

//    void testCase_CaptureDoubleClickInside()
//    {
//        QScopedPointer<QQuickView> view(loadTest("CaptureDoubleClick.qml"));
//        QVERIFY(view);

//        QQuickMouseArea *ma = qobject_cast<QQuickMouseArea*>(view->rootObject()->findChild<QQuickItem*>("MA"));
//        QVERIFY(ma);
//        InverseMouseAreaType *ima = qobject_cast<InverseMouseAreaType*>(view->rootObject()->findChild<QQuickItem*>("IMA"));
//        QVERIFY(ima);
//        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
//        QVERIFY(filter);
//        QSignalSpy maSpy(ma, SIGNAL(doubleClicked(QQuickMouseEvent*)));
//        QSignalSpy imaSpy(ima, SIGNAL(doubleClicked(QQuickMouseEvent*)));
//        QSignalSpy filterSpy(filter, SIGNAL(doubleClicked(UCMouseEvent*)));
//        QSignalSpy maClickSpy(ma, SIGNAL(clicked(QQuickMouseEvent*)));
//        QSignalSpy imaClickSpy(ima, SIGNAL(clicked(QQuickMouseEvent*)));
//        QSignalSpy filterClickSpy(filter, SIGNAL(clicked(UCMouseEvent*)));

//        // inside
//        QTest::mouseDClick(view.data(), Qt::LeftButton, 0, QPoint(30, 30));
//        QCOMPARE(maSpy.count(), 0);
//        QCOMPARE(imaSpy.count(), 0);
//        QCOMPARE(filterSpy.count(), 1);
//        // only the double click is suppressed!
//        QCOMPARE(maClickSpy.count(), 2);
//        QCOMPARE(imaClickSpy.count(), 0);
//        QCOMPARE(filterClickSpy.count(), 1);
//    }

//    void testCase_CaptureDoubleClickOutside()
//    {
//        QScopedPointer<QQuickView> view(loadTest("CaptureDoubleClick.qml"));
//        QVERIFY(view);

//        QQuickMouseArea *ma = qobject_cast<QQuickMouseArea*>(view->rootObject()->findChild<QQuickItem*>("MA"));
//        QVERIFY(ma);
//        InverseMouseAreaType *ima = qobject_cast<InverseMouseAreaType*>(view->rootObject()->findChild<QQuickItem*>("IMA"));
//        QVERIFY(ima);
//        UCMouseFilter *filter = qobject_cast<UCMouseFilter*>(view->rootObject()->findChild<QQuickItem*>("filter"));
//        QVERIFY(filter);
//        QSignalSpy maSpy(ma, SIGNAL(doubleClicked(QQuickMouseEvent*)));
//        QSignalSpy imaSpy(ima, SIGNAL(doubleClicked(QQuickMouseEvent*)));
//        QSignalSpy filterSpy(filter, SIGNAL(doubleClicked(UCMouseEvent*)));
//        QSignalSpy maClickSpy(ma, SIGNAL(clicked(QQuickMouseEvent*)));
//        QSignalSpy imaClickSpy(ima, SIGNAL(clicked(QQuickMouseEvent*)));
//        QSignalSpy filterClickSpy(filter, SIGNAL(clicked(UCMouseEvent*)));

//        // outside
//        QTest::mouseDClick(view.data(), Qt::LeftButton, 0, QPoint(20, 20));
//        QCOMPARE(maSpy.count(), 0);
//        QCOMPARE(imaSpy.count(), 0);
//        QCOMPARE(filterSpy.count(), 1);
//        QCOMPARE(maClickSpy.count(), 0);
//        // only the double click is suppressed!
//        QCOMPARE(imaClickSpy.count(), 2);
//        QCOMPARE(filterClickSpy.count(), 1);
//    }
};

QTEST_MAIN(tst_mouseFilterTest)

#include "tst_mousefilterstest.moc"
