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
 *
 */

#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickView>
#include <QtQuick/QQuickItem>
#include <QtCore/QEvent>

#include "ucmouse.h"
#include "ucinversemouse.h"

#include "ucunits.h"
#include "quickutils.h"
#include "inversemouseareatype.h"
#include "uctestcase.h"
#include "uctestextras.h"
#include <private/qquickevents_p_p.h>
#include <private/qquickmousearea_p.h>

template<typename T>
T *attachedFilter(QQuickItem *item, const QString &objectName)
{
    QQuickItem *testItem = item->objectName() == objectName ? item : item->findChild<QQuickItem*>(objectName);
    if (testItem) {
        return qobject_cast<T*>(qmlAttachedPropertiesObject<T>(testItem, false));
    }
    return 0;
}

class tst_mouseFilterTest : public QObject
{
    Q_OBJECT
    
public:
    tst_mouseFilterTest() {}

private:
    struct FilterParams {
        QString handler;
        Qt::MouseButton pressedButton;
        Qt::MouseButtons pressedButtons;
        QQuickItem *sender;
    };

    QString m_modulePath;
    FilterParams mouseEventParams;
    FilterParams mouseEvent2Params;
    FilterParams mouseEvent3Params;

    QQuickView * loadTest(const QString &file)
    {
        UbuntuTestCase* testCase = new UbuntuTestCase(file);
        return qobject_cast<QQuickView*>(testCase);
    }

    void mousePressAndHold(QWindow *view, Qt::MouseButton button, Qt::KeyboardModifiers modifiers, const QPoint &point, int delay = DefaultPressAndHoldDelay + 200)
    {
        QTest::mousePress(view, button, modifiers, point);
        QTest::qWait(delay);
    }

    bool inputPanelPresent()
    {
        return !QuickUtils::instance()->inputMethodProvider().isEmpty();
    }

    QPoint guPoint(qreal guX, qreal guY)
    {
        return QPoint(UCUnits::instance()->gu(guX), UCUnits::instance()->gu(guY));
    }

    void preventDblClick()
    {
        QTest::qWait(400);
    }

protected Q_SLOTS:
    void onMouseEvent(QQuickMouseEvent *event, QQuickItem *sender)
    {
        mouseEventParams.handler = "EVENT1";
        mouseEventParams.sender = sender;
        mouseEventParams.pressedButton = (Qt::MouseButton)event->button();
        mouseEventParams.pressedButtons = (Qt::MouseButtons)event->buttons();
    }
    void onMouseEvent2(QQuickMouseEvent *event, QQuickItem *sender)
    {
        mouseEvent2Params.handler = "EVENT2";
        mouseEvent2Params.sender = sender;
        mouseEvent2Params.pressedButton = (Qt::MouseButton)event->button();
        mouseEvent2Params.pressedButtons = (Qt::MouseButtons)event->buttons();
    }
    void onMouseEvent3(QQuickMouseEvent *event, QQuickItem *sender)
    {
        mouseEvent3Params.handler = "EVENT4";
        mouseEvent3Params.sender = sender;
        mouseEvent3Params.pressedButton = (Qt::MouseButton)event->button();
        mouseEvent3Params.pressedButtons = (Qt::MouseButtons)event->buttons();
    }

private Q_SLOTS:

    void initTestCase()
    {
        UCTestExtras::registerTouchDevice();

        QString modules(UBUNTU_QML_IMPORT_PATH);
        QVERIFY(QDir(modules).exists());

        m_modulePath = QDir(modules).absolutePath();
    }

    void cleanupTestCase()
    {
    }

    void testCase_pressedInsideTextInput()
    {
        QScopedPointer<QQuickView> view(loadTest("FilterTextInput.qml"));
        QVERIFY(view);
        UCMouse *filter = attachedFilter<UCMouse>(view->rootObject(), "FilterOwner");
        QVERIFY(filter);
        QSignalSpy pressed(filter, SIGNAL(pressed(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy released(filter, SIGNAL(released(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy entered(filter, SIGNAL(entered(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy exited(filter, SIGNAL(exited(QQuickMouseEvent*, QQuickItem*)));

        preventDblClick();
        QTest::mouseClick(view.data(), Qt::LeftButton, 0, guPoint(2, 2));
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

    void testCase_pressedInsideTextInputHandledAfter()
    {
        QScopedPointer<QQuickView> view(loadTest("FilterTextInputAfter.qml"));
        QVERIFY(view);
        UCMouse *filter = attachedFilter<UCMouse>(view->rootObject(), "FilterOwner");
        QVERIFY(filter);
        QSignalSpy pressed(filter, SIGNAL(pressed(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy released(filter, SIGNAL(released(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy entered(filter, SIGNAL(entered(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy exited(filter, SIGNAL(exited(QQuickMouseEvent*, QQuickItem*)));

        preventDblClick();
        QTest::mouseClick(view.data(), Qt::LeftButton, 0, guPoint(2, 2));
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

    void testCase_pressedOutsideTextInput()
    {
        QScopedPointer<QQuickView> view(loadTest("FilterInverseTextInput.qml"));
        QVERIFY(view);
        UCInverseMouse *filter = attachedFilter<UCInverseMouse>(view->rootObject(), "FilterOwner");
        QVERIFY(filter);
        QSignalSpy pressed(filter, SIGNAL(pressed(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy released(filter, SIGNAL(released(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy entered(filter, SIGNAL(entered(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy exited(filter, SIGNAL(exited(QQuickMouseEvent*, QQuickItem*)));

        preventDblClick();
        QTest::mouseClick(view.data(), Qt::LeftButton, 0, guPoint(10, 10));
        QTest::waitForEvents();
        QCOMPARE(view->rootObject()->hasFocus(), false);
        QCOMPARE(pressed.count(), 1);
        QCOMPARE(released.count(), 1);
        QCOMPARE(clicked.count(), 1);
        QCOMPARE(positionChanged.count(), 0);
        QCOMPARE(doubleClicked.count(), 0);
        QCOMPARE(pressAndHold.count(), 0);
        QCOMPARE(entered.count(), 1);
        QCOMPARE(exited.count(), 1);
    }

    void testCase_pressedOutsideTextInputAfter()
    {
        UbuntuTestCase::ignoreWarning("FilterInverseTextInputAfter.qml", 23, 5, "QML TextInput: Ignoring AfterItem priority for InverseMouse filters.");
        QScopedPointer<UbuntuTestCase> view(new UbuntuTestCase("FilterInverseTextInputAfter.qml"));
        QCOMPARE(view->warnings(), 1);
        UCInverseMouse *filter = attachedFilter<UCInverseMouse>(view->rootObject(), "FilterOwner");
        QVERIFY(filter);
        QSignalSpy pressed(filter, SIGNAL(pressed(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy released(filter, SIGNAL(released(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy entered(filter, SIGNAL(entered(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy exited(filter, SIGNAL(exited(QQuickMouseEvent*, QQuickItem*)));

        preventDblClick();
        QTest::mouseClick(view.data(), Qt::LeftButton, 0, guPoint(10, 10));
        QTest::waitForEvents();
        QCOMPARE(view->rootObject()->hasFocus(), false);
        QCOMPARE(pressed.count(), 1);
        QCOMPARE(released.count(), 1);
        QCOMPARE(clicked.count(), 1);
        QCOMPARE(positionChanged.count(), 0);
        QCOMPARE(doubleClicked.count(), 0);
        QCOMPARE(pressAndHold.count(), 0);
        QCOMPARE(entered.count(), 1);
        QCOMPARE(exited.count(), 1);
    }

    void testCase_pressedInsideTextInputTolerance()
    {
        QScopedPointer<QQuickView> view(loadTest("FilterTextInputTolerance.qml"));
        QVERIFY(view);
        UCMouse *filter = attachedFilter<UCMouse>(view->rootObject(), "FilterOwner");
        QVERIFY(filter);
        QSignalSpy pressed(filter, SIGNAL(pressed(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy released(filter, SIGNAL(released(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy entered(filter, SIGNAL(entered(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy exited(filter, SIGNAL(exited(QQuickMouseEvent*, QQuickItem*)));

        preventDblClick();
        QTest::mousePress(view.data(), Qt::LeftButton, 0, guPoint(2, 2));
        QTest::mouseMove(view.data(), guPoint(2.2, 2.2));
        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, guPoint(2.2, 2.2));
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

    void testCase_pressedOutsideTextInputTolerance()
    {
        QScopedPointer<QQuickView> view(loadTest("FilterInverseTextInputTolerance.qml"));
        QVERIFY(view);
        UCInverseMouse *filter = attachedFilter<UCInverseMouse>(view->rootObject(), "FilterOwner");
        QVERIFY(filter);
        QSignalSpy pressed(filter, SIGNAL(pressed(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy released(filter, SIGNAL(released(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy entered(filter, SIGNAL(entered(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy exited(filter, SIGNAL(exited(QQuickMouseEvent*, QQuickItem*)));

        preventDblClick();
        QTest::mousePress(view.data(), Qt::LeftButton, 0, guPoint(10, 10));
        QTest::mouseMove(view.data(), guPoint(10.2, 10.2));
        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, guPoint(10.2, 10.2));
        QTest::waitForEvents();
        QCOMPARE(view->rootObject()->hasFocus(), false);
        QCOMPARE(pressed.count(), 1);
        QCOMPARE(released.count(), 1);
        QCOMPARE(clicked.count(), 1);
        QCOMPARE(positionChanged.count(), 1);
        QCOMPARE(doubleClicked.count(), 0);
        QCOMPARE(pressAndHold.count(), 0);
        QCOMPARE(entered.count(), 1);
        QCOMPARE(exited.count(), 1);
    }

    void testCase_pressedInsideTextInputOutOfTolerance()
    {
        QScopedPointer<QQuickView> view(loadTest("FilterTextInputTolerance.qml"));
        QVERIFY(view);
        UCMouse *filter = attachedFilter<UCMouse>(view->rootObject(), "FilterOwner");
        QVERIFY(filter);
        QSignalSpy pressed(filter, SIGNAL(pressed(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy released(filter, SIGNAL(released(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy entered(filter, SIGNAL(entered(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy exited(filter, SIGNAL(exited(QQuickMouseEvent*, QQuickItem*)));

        preventDblClick();
        QTest::mousePress(view.data(), Qt::LeftButton, 0, guPoint(2, 2));
        QTest::mouseMove(view.data(), guPoint(2.7, 2.7));
        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, guPoint(2.7, 2.7));
        QTest::waitForEvents();
        QCOMPARE(view->rootObject()->hasFocus(), true);
        QCOMPARE(pressed.count(), 1);
        QCOMPARE(released.count(), 1);
        QCOMPARE(clicked.count(), 0);
        QCOMPARE(positionChanged.count(), 1);
        QCOMPARE(doubleClicked.count(), 0);
        QCOMPARE(pressAndHold.count(), 0);
        QCOMPARE(entered.count(), 1);
        QCOMPARE(exited.count(), 1);
    }

    void testCase_pressedOutsideTextInputOutOfTolerance()
    {
        QScopedPointer<QQuickView> view(loadTest("FilterInverseTextInputTolerance.qml"));
        QVERIFY(view);
        UCInverseMouse *filter = attachedFilter<UCInverseMouse>(view->rootObject(), "FilterOwner");
        QVERIFY(filter);
        QSignalSpy pressed(filter, SIGNAL(pressed(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy released(filter, SIGNAL(released(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy entered(filter, SIGNAL(entered(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy exited(filter, SIGNAL(exited(QQuickMouseEvent*, QQuickItem*)));

        preventDblClick();
        QTest::mousePress(view.data(), Qt::LeftButton, 0, guPoint(10, 10));
        QTest::mouseMove(view.data(), guPoint(10.6, 10.6));
        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, guPoint(10.6, 10.6));
        QTest::waitForEvents();
        QCOMPARE(view->rootObject()->hasFocus(), false);
        QCOMPARE(pressed.count(), 1);
        QCOMPARE(released.count(), 1);
        QCOMPARE(clicked.count(), 0);
        QCOMPARE(positionChanged.count(), 1);
        QCOMPARE(doubleClicked.count(), 0);
        QCOMPARE(pressAndHold.count(), 0);
        QCOMPARE(entered.count(), 1);
        QCOMPARE(exited.count(), 1);
    }

    void testCase_pressedOverOSKExcluded()
    {
        QScopedPointer<QQuickView> view(loadTest("FilterInverseTextInputOSKExcluded.qml"));
        QVERIFY(view);
        UCInverseMouse *filter = attachedFilter<UCInverseMouse>(view->rootObject(), "FilterOwner");
        QVERIFY(filter);

        QSignalSpy pressed(filter, SIGNAL(pressed(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy released(filter, SIGNAL(released(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy entered(filter, SIGNAL(entered(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy exited(filter, SIGNAL(exited(QQuickMouseEvent*, QQuickItem*)));

        // focus TextInput
        QQuickItem *input = view->rootObject()->findChild<QQuickItem*>("FilterOwner");
        QVERIFY(input);
        input->forceActiveFocus();
        QCOMPARE(input->hasFocus(), true);

        // introduce small delay to suppress double clicks
        preventDblClick();
        QTest::mouseClick(view.data(), Qt::LeftButton, 0, guPoint(10, 10));
        QTest::waitForEvents();
        preventDblClick();
        QTest::mouseClick(view.data(), Qt::LeftButton, 0, guPoint(10, 69));
        QTest::waitForEvents();
        if (!inputPanelPresent()) {
            QEXPECT_FAIL(0, "No OSK installed", Abort);
        }
        QCOMPARE(pressed.count(), 1);
        QCOMPARE(released.count(), 1);
        QCOMPARE(clicked.count(), 1);
        QCOMPARE(positionChanged.count(), 0);
        QCOMPARE(doubleClicked.count(), 0);
        QCOMPARE(pressAndHold.count(), 0);
        QCOMPARE(entered.count(), 1);
        QCOMPARE(exited.count(), 1);
    }

    void testCase_pressAndMovedInsideTextInput()
    {
        QScopedPointer<QQuickView> view(loadTest("FilterTextInput.qml"));
        QVERIFY(view);
        UCMouse *filter = attachedFilter<UCMouse>(view->rootObject(), "FilterOwner");
        QVERIFY(filter);
        QSignalSpy pressed(filter, SIGNAL(pressed(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy released(filter, SIGNAL(released(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy entered(filter, SIGNAL(entered(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy exited(filter, SIGNAL(exited(QQuickMouseEvent*, QQuickItem*)));

        // make sure we do not get double click because of the previous tests
        preventDblClick();
        QTest::mousePress(view.data(), Qt::LeftButton, 0, guPoint(2, 2));
        QTest::mouseMove(view.data(), guPoint(2.7, 2.7));
        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, guPoint(2.7, 2.7));
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

    void testCase_pressAndMovedOutsideTextInputInTolerance()
    {
        QScopedPointer<QQuickView> view(loadTest("FilterInverseTextInputTolerance.qml"));
        QVERIFY(view);
        UCInverseMouse *filter = attachedFilter<UCInverseMouse>(view->rootObject(), "FilterOwner");
        QVERIFY(filter);
        QSignalSpy pressed(filter, SIGNAL(pressed(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy released(filter, SIGNAL(released(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy entered(filter, SIGNAL(entered(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy exited(filter, SIGNAL(exited(QQuickMouseEvent*, QQuickItem*)));

        // make sure we do not get double click because of the previous tests
        preventDblClick();
        QTest::mousePress(view.data(), Qt::LeftButton, 0, guPoint(10, 10));
        QTest::mouseMove(view.data(), guPoint(10.5, 10.5));
        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, guPoint(10.5, 10.5));
        QTest::waitForEvents();
        QCOMPARE(view->rootObject()->hasFocus(), false);
        QCOMPARE(pressed.count(), 1);
        QCOMPARE(released.count(), 1);
        QCOMPARE(clicked.count(), 1);
        QCOMPARE(positionChanged.count(), 1);
        QCOMPARE(doubleClicked.count(), 0);
        QCOMPARE(pressAndHold.count(), 0);
        QCOMPARE(entered.count(), 1);
        QCOMPARE(exited.count(), 1);
    }

    void testCase_pressAndMovedOutsideTextInputOutTolerance()
    {
        QScopedPointer<QQuickView> view(loadTest("FilterInverseTextInputTolerance.qml"));
        QVERIFY(view);
        UCInverseMouse *filter = attachedFilter<UCInverseMouse>(view->rootObject(), "FilterOwner");
        QVERIFY(filter);
        QSignalSpy pressed(filter, SIGNAL(pressed(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy released(filter, SIGNAL(released(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy entered(filter, SIGNAL(entered(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy exited(filter, SIGNAL(exited(QQuickMouseEvent*, QQuickItem*)));

        // make sure we do not get double click because of the previous tests
        preventDblClick();
        QTest::mousePress(view.data(), Qt::LeftButton, 0, guPoint(10, 10));
        QTest::mouseMove(view.data(), guPoint(15, 15));
        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, guPoint(15, 15));
        QTest::waitForEvents();
        QCOMPARE(view->rootObject()->hasFocus(), false);
        QCOMPARE(pressed.count(), 1);
        QCOMPARE(released.count(), 1);
        QCOMPARE(clicked.count(), 0);
        QCOMPARE(positionChanged.count(), 1);
        QCOMPARE(doubleClicked.count(), 0);
        QCOMPARE(pressAndHold.count(), 0);
        QCOMPARE(entered.count(), 1);
        QCOMPARE(exited.count(), 1);
    }

    void testCase_pressAndHoldDisableClick() {
        QScopedPointer<QQuickView> view(loadTest("PressAndHoldAcceptedEatsClick.qml"));
        QVERIFY(view);
        UCMouse *filter = attachedFilter<UCMouse>(view->rootObject(), "FilterOwner");
        QVERIFY(filter);
        QSignalSpy clicked(filter, SIGNAL(clicked(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(QQuickMouseEvent*, QQuickItem*)));

        preventDblClick();
        mousePressAndHold(view.data(), Qt::LeftButton, 0, guPoint(2, 2));
        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, guPoint(2, 2));
        QTest::waitForEvents();
        QCOMPARE(view->rootObject()->hasFocus(), true);
        QCOMPARE(pressAndHold.count(), 1);
        QCOMPARE(clicked.count(), 0);
    }

    void testCase_pressAndHoldAndClick() {
        QScopedPointer<QQuickView> view(loadTest("PressAndHoldAndClicked.qml"));
        QVERIFY(view);
        UCMouse *filter = attachedFilter<UCMouse>(view->rootObject(), "FilterOwner");
        QVERIFY(filter);
        QSignalSpy clicked(filter, SIGNAL(clicked(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(QQuickMouseEvent*, QQuickItem*)));

        preventDblClick();
        mousePressAndHold(view.data(), Qt::LeftButton, 0, guPoint(2, 2));
        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, guPoint(2, 2));
        QTest::waitForEvents();
        QCOMPARE(view->rootObject()->hasFocus(), true);
        QCOMPARE(pressAndHold.count(), 1);
        QCOMPARE(clicked.count(), 1);
    }

    void testCase_doubleClicked()
    {
        QSKIP("FIXME: lp#1542215 Flaky test result");
        QScopedPointer<QQuickView> view(loadTest("DoubleClicked.qml"));
        QVERIFY(view);
        UCMouse *filter = attachedFilter<UCMouse>(view->rootObject(), "FilterOwner");
        QVERIFY(filter);
        QQuickItem *input = view->rootObject();
        QVERIFY(input);
        QSignalSpy clicked(filter, SIGNAL(clicked(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy dblClicked(filter, SIGNAL(doubleClicked(QQuickMouseEvent*, QQuickItem*)));

        preventDblClick();
        QTest::mouseDClick(view.data(), Qt::LeftButton, 0, guPoint(2, 2));
        QTest::waitForEvents();
        QVariant selectedText = input->property("selectedText");
        QCOMPARE(input->hasFocus(), true);
        QVERIFY(selectedText.isValid());
        QVERIFY(!selectedText.toString().isEmpty());
        // ther emust be one click, the second one should be suppressed
        // by the doubleClicked() signal being connected at least to signalSpy!
        QCOMPARE(clicked.count(), 1);
        QCOMPARE(dblClicked.count(), 1);
    }

    void testCase_mouseFilterAttachedToNonItem()
    {
        UbuntuTestCase::ignoreWarning("MouseFilterAttachedToNonItem.qml", 21, 5, "QML QtObject: Warning: Mouse filter can only be attached to Items.");
        QScopedPointer<UbuntuTestCase> testCase(new UbuntuTestCase("MouseFilterAttachedToNonItem.qml"));
        QCOMPARE(testCase->warnings(), 1);
    }

    void testCase_inverseMouseFilterAttachedToNonItem()
    {
        UbuntuTestCase::ignoreWarning("InverseMouseFilterAttachedToNonItem.qml", 21, 5, "QML QtObject: Warning: InverseMouse filter can only be attached to Items.");
        QScopedPointer<UbuntuTestCase> testCase(new UbuntuTestCase("InverseMouseFilterAttachedToNonItem.qml"));
        QCOMPARE(testCase->warnings(), 1);
    }

    void testCase_forwardedEventsToItem()
    {
        QScopedPointer<QQuickView> view(loadTest("ForwardedMouseEvents.qml"));
        QVERIFY(view);
        UCMouse *filter = attachedFilter<UCMouse>(view->rootObject(), "target");
        QVERIFY(filter);
        QQuickItem *input = view->rootObject()->findChild<QQuickItem*>("FilterOwner");
        QVERIFY(input);
        QSignalSpy pressed(filter, SIGNAL(pressed(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy released(filter, SIGNAL(released(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy entered(filter, SIGNAL(entered(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy exited(filter, SIGNAL(exited(QQuickMouseEvent*, QQuickItem*)));

        preventDblClick();
        QTest::mouseClick(view.data(), Qt::LeftButton, 0, guPoint(2, 2));
        QTest::waitForEvents();
        QCOMPARE(input->hasFocus(), true);
        QCOMPARE(pressed.count(), 1);
        QCOMPARE(released.count(), 1);
        QCOMPARE(clicked.count(), 1);
        QCOMPARE(positionChanged.count(), 0);
        QCOMPARE(doubleClicked.count(), 0);
        QCOMPARE(pressAndHold.count(), 0);
        QCOMPARE(entered.count(), 1);
        QCOMPARE(exited.count(), 1);
    }

    void testCase_forwardedEventsToItemStopped()
    {
        QScopedPointer<QQuickView> view(loadTest("ForwardedMouseEventsStopped.qml"));
        QVERIFY(view);
        UCMouse *filter = attachedFilter<UCMouse>(view->rootObject(), "target");
        QVERIFY(filter);
        QQuickItem *input = view->rootObject()->findChild<QQuickItem*>("FilterOwner");
        QVERIFY(input);
        QSignalSpy pressed(filter, SIGNAL(pressed(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy released(filter, SIGNAL(released(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy entered(filter, SIGNAL(entered(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy exited(filter, SIGNAL(exited(QQuickMouseEvent*, QQuickItem*)));

        preventDblClick();
        QTest::mouseClick(view.data(), Qt::LeftButton, 0, guPoint(2, 2));
        QTest::waitForEvents();
        QCOMPARE(input->hasFocus(), false);
        // only the pressed was blocked, the rest of the events land in the proxy
        QCOMPARE(pressed.count(), 0);
        QCOMPARE(released.count(), 1);
        QCOMPARE(clicked.count(), 1);
        QCOMPARE(positionChanged.count(), 0);
        QCOMPARE(doubleClicked.count(), 0);
        QCOMPARE(pressAndHold.count(), 0);
        QCOMPARE(entered.count(), 1);
        QCOMPARE(exited.count(), 1);
    }

    void testCase_forwardedEventsToItemStoppedInForwards()
    {
        QScopedPointer<QQuickView> view(loadTest("ForwardedEventsStoppedInForwards.qml"));
        QVERIFY(view);
        UCMouse *filter = attachedFilter<UCMouse>(view->rootObject(), "target");
        QVERIFY(filter);
        QQuickItem *input = view->rootObject()->findChild<QQuickItem*>("FilterOwner");
        QVERIFY(input);
        QSignalSpy pressed(filter, SIGNAL(pressed(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy released(filter, SIGNAL(released(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy entered(filter, SIGNAL(entered(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy exited(filter, SIGNAL(exited(QQuickMouseEvent*, QQuickItem*)));

        preventDblClick();
        QTest::mouseClick(view.data(), Qt::LeftButton, 0, guPoint(2, 2));
        QTest::waitForEvents();
        QCOMPARE(input->hasFocus(), false);
        QCOMPARE(pressed.count(), 1);
        QCOMPARE(released.count(), 1);
        QCOMPARE(clicked.count(), 1);
        QCOMPARE(positionChanged.count(), 0);
        QCOMPARE(doubleClicked.count(), 0);
        QCOMPARE(pressAndHold.count(), 0);
        QCOMPARE(entered.count(), 1);
        QCOMPARE(exited.count(), 1);
    }

    void testCase_forwardedEventsToTargetBlocked()
    {
        QScopedPointer<QQuickView> view(loadTest("ForwardedEventsToTargetBlocked.qml"));
        QVERIFY(view);
        UCMouse *filter = attachedFilter<UCMouse>(view->rootObject(), "target");
        QVERIFY(filter);
        QQuickItem *input = view->rootObject()->findChild<QQuickItem*>("FilterOwner");
        QVERIFY(input);
        QSignalSpy pressed(filter, SIGNAL(pressed(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy released(filter, SIGNAL(released(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy entered(filter, SIGNAL(entered(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy exited(filter, SIGNAL(exited(QQuickMouseEvent*, QQuickItem*)));

        preventDblClick();
        QTest::mouseClick(view.data(), Qt::LeftButton, 0, guPoint(2, 2));
        QTest::waitForEvents();
        QCOMPARE(input->hasFocus(), true);
        QCOMPARE(pressed.count(), 0);
        QCOMPARE(released.count(), 1);
        QCOMPARE(clicked.count(), 1);
        QCOMPARE(positionChanged.count(), 0);
        QCOMPARE(doubleClicked.count(), 0);
        QCOMPARE(pressAndHold.count(), 0);
        QCOMPARE(entered.count(), 1);
        QCOMPARE(exited.count(), 1);
    }

    void testCase_forwardedEventsToMouseArea()
    {
        QScopedPointer<QQuickView> view(loadTest("ForwardToMouseArea.qml"));
        QVERIFY(view);
        QQuickMouseArea *target = view->rootObject()->findChild<QQuickMouseArea*>("target");
        QVERIFY(target);
        QQuickItem *input = view->rootObject()->findChild<QQuickItem*>("FilterOwner");
        QVERIFY(input);
        QSignalSpy pressed(target, SIGNAL(pressed(QQuickMouseEvent*)));
        QSignalSpy released(target, SIGNAL(released(QQuickMouseEvent*)));
        QSignalSpy clicked(target, SIGNAL(clicked(QQuickMouseEvent*)));
        QSignalSpy positionChanged(target, SIGNAL(positionChanged(QQuickMouseEvent*)));
        QSignalSpy doubleClicked(target, SIGNAL(doubleClicked(QQuickMouseEvent*)));
        QSignalSpy pressAndHold(target, SIGNAL(pressAndHold(QQuickMouseEvent*)));
        QSignalSpy entered(target, SIGNAL(entered()));
        QSignalSpy exited(target, SIGNAL(exited()));

        preventDblClick();
        QTest::mouseClick(view.data(), Qt::LeftButton, 0, guPoint(2, 2));
        QTest::waitForEvents();
        QCOMPARE(input->hasFocus(), false);
        QCOMPARE(pressed.count(), 1);
        QCOMPARE(released.count(), 1);
        QCOMPARE(clicked.count(), 1);
        QCOMPARE(positionChanged.count(), 0);
        QCOMPARE(doubleClicked.count(), 0);
        QCOMPARE(pressAndHold.count(), 0);
        QCOMPARE(entered.count(), 1);
        QCOMPARE(exited.count(), 1);
    }

    void testCase_forwardedEventsToTransparentMouseArea()
    {
        QScopedPointer<QQuickView> view(loadTest("ForwardToTransparentMouseArea.qml"));
        QVERIFY(view);
        QQuickMouseArea *target = view->rootObject()->findChild<QQuickMouseArea*>("target");
        QVERIFY(target);
        QQuickItem *input = view->rootObject()->findChild<QQuickItem*>("FilterOwner");
        QVERIFY(input);
        QSignalSpy pressed(target, SIGNAL(pressed(QQuickMouseEvent*)));
        QSignalSpy released(target, SIGNAL(released(QQuickMouseEvent*)));
        QSignalSpy clicked(target, SIGNAL(clicked(QQuickMouseEvent*)));
        QSignalSpy positionChanged(target, SIGNAL(positionChanged(QQuickMouseEvent*)));
        QSignalSpy doubleClicked(target, SIGNAL(doubleClicked(QQuickMouseEvent*)));
        QSignalSpy pressAndHold(target, SIGNAL(pressAndHold(QQuickMouseEvent*)));
        QSignalSpy entered(target, SIGNAL(entered()));
        QSignalSpy exited(target, SIGNAL(exited()));

        preventDblClick();
        QTest::mouseClick(input->window(), Qt::LeftButton, 0, guPoint(10, 7));
        QTest::waitForEvents();
        QCOMPARE(input->hasFocus(), true);
        QCOMPARE(pressed.count(), 1);
        QCOMPARE(released.count(), 1);
        QCOMPARE(clicked.count(), 1);
        QCOMPARE(positionChanged.count(), 0);
        QCOMPARE(doubleClicked.count(), 0);
        QCOMPARE(pressAndHold.count(), 0);
        QCOMPARE(entered.count(), 1);
        QCOMPARE(exited.count(), 1);
    }

    void testCase_forwardedDblClickToTransparentMouseArea()
    {
        QScopedPointer<QQuickView> view(loadTest("ForwardDblClickToTransparentMouseArea.qml"));
        QVERIFY(view);
        QQuickMouseArea *target = view->rootObject()->findChild<QQuickMouseArea*>("target");
        QVERIFY(target);
        QQuickItem *input = view->rootObject()->findChild<QQuickItem*>("FilterOwner");
        QVERIFY(input);
        QSignalSpy doubleClicked(target, SIGNAL(doubleClicked(QQuickMouseEvent*)));

        preventDblClick();
        QTest::mouseDClick(input->window(), Qt::LeftButton, 0, guPoint(10, 7));
        QTest::waitForEvents();
        QCOMPARE(input->hasFocus(), true);
        QVariant selectedText = input->property("selectedText");
        QVERIFY(selectedText.isValid());
        QVERIFY(selectedText.toString().isEmpty());
        QCOMPARE(doubleClicked.count(), 1);
    }

    void testCase_forwardedEventsToInverseMouseArea()
    {
        QScopedPointer<QQuickView> view(loadTest("ForwardToInverseMouseArea.qml"));
        QVERIFY(view);
        QQuickItem *target = view->rootObject()->findChild<QQuickItem*>("target");
        QVERIFY(target);
        QQuickItem *input = view->rootObject()->findChild<QQuickItem*>("FilterOwner");
        QVERIFY(input);
        QSignalSpy pressed(target, SIGNAL(pressed(QQuickMouseEvent*)));
        QSignalSpy released(target, SIGNAL(released(QQuickMouseEvent*)));
        QSignalSpy clicked(target, SIGNAL(clicked(QQuickMouseEvent*)));
        QSignalSpy positionChanged(target, SIGNAL(positionChanged(QQuickMouseEvent*)));
        QSignalSpy doubleClicked(target, SIGNAL(doubleClicked(QQuickMouseEvent*)));
        QSignalSpy pressAndHold(target, SIGNAL(pressAndHold(QQuickMouseEvent*)));
        QSignalSpy entered(target, SIGNAL(entered()));
        QSignalSpy exited(target, SIGNAL(exited()));

        preventDblClick();
        QTest::mouseClick(view.data(), Qt::LeftButton, 0, guPoint(2, 2));
        QTest::waitForEvents();
        QCOMPARE(input->hasFocus(), true);
        QCOMPARE(pressed.count(), 1);
        QCOMPARE(released.count(), 0);
        QCOMPARE(clicked.count(), 0);
        QCOMPARE(positionChanged.count(), 0);
        QCOMPARE(doubleClicked.count(), 0);
        QCOMPARE(pressAndHold.count(), 0);
        QCOMPARE(entered.count(), 1);
        QCOMPARE(exited.count(), 0);
    }

    void testCase_filterFlickable()
    {
        QScopedPointer<QQuickView> view(loadTest("FilterFlickable.qml"));
        QVERIFY(view);
        UCMouse *filter = attachedFilter<UCMouse>(view->rootObject(), "FilterOwner");
        QVERIFY(filter);
        QQuickItem *flickable = view->rootObject();
        QVERIFY(flickable);
        QSignalSpy pressed(filter, SIGNAL(pressed(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy released(filter, SIGNAL(released(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy entered(filter, SIGNAL(entered(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy exited(filter, SIGNAL(exited(QQuickMouseEvent*, QQuickItem*)));

        QSignalSpy flickStart(flickable, SIGNAL(movementStarted()));

        preventDblClick();
        QTest::mousePress(flickable->window(), Qt::LeftButton, 0, guPoint(5, 5));
        for (int i = 0; i < 30; i++) {
            QTest::mouseMove(flickable->window(), guPoint(5 + i, 5 + i));
        }
        QTest::waitForEvents();
        QTest::mouseRelease(flickable->window(), Qt::LeftButton, 0, guPoint(35, 35));
        QTest::waitForEvents();

        QCOMPARE(pressed.count(), 1);
        QCOMPARE(released.count(), 1);
        QCOMPARE(clicked.count(), 1);
        QCOMPARE(positionChanged.count(), 30);
        QCOMPARE(doubleClicked.count(), 0);
        QCOMPARE(pressAndHold.count(), 0);
        QCOMPARE(entered.count(), 1);
        QCOMPARE(exited.count(), 1);
        QCOMPARE(flickStart.count(), 1);
    }

    void testCase_filterFlickableAfter()
    {
        QScopedPointer<QQuickView> view(loadTest("FilterFlickableAfter.qml"));
        QVERIFY(view);
        UCMouse *filter = attachedFilter<UCMouse>(view->rootObject(), "FilterOwner");
        QVERIFY(filter);
        QQuickItem *flickable = view->rootObject()->findChild<QQuickItem*>("FilterOwner");
        QVERIFY(flickable);
        QSignalSpy pressed(filter, SIGNAL(pressed(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy released(filter, SIGNAL(released(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy entered(filter, SIGNAL(entered(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy exited(filter, SIGNAL(exited(QQuickMouseEvent*, QQuickItem*)));

        QSignalSpy flickStart(flickable, SIGNAL(movementStarted()));

        preventDblClick();
        QTest::mousePress(flickable->window(), Qt::LeftButton, 0, guPoint(5, 5));
        for (int i = 0; i < 30; i++) {
            QTest::mouseMove(flickable->window(), guPoint(5 + i, 5 + i));
        }
        QTest::mouseRelease(flickable->window(), Qt::LeftButton, 0, guPoint(35, 35));
        QTest::waitForEvents();

        QCOMPARE(pressed.count(), 1);
        QCOMPARE(released.count(), 1);
        QCOMPARE(clicked.count(), 1);
        QCOMPARE(positionChanged.count(), 30);
        QCOMPARE(doubleClicked.count(), 0);
        QCOMPARE(pressAndHold.count(), 0);
        QCOMPARE(entered.count(), 1);
        QCOMPARE(exited.count(), 1);
        QCOMPARE(flickStart.count(), 1);
    }

    void testCase_hover()
    {
        QScopedPointer<QQuickView> view(loadTest("FilterFlickableAfter.qml"));
        QVERIFY(view);
        UCMouse *filter = attachedFilter<UCMouse>(view->rootObject(), "FilterOwner");
        QVERIFY(filter);
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy entered(filter, SIGNAL(entered(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy exited(filter, SIGNAL(exited(QQuickMouseEvent*, QQuickItem*)));
        QObject::connect(filter, SIGNAL(entered(QQuickMouseEvent*, QQuickItem*)), this, SLOT(onMouseEvent(QQuickMouseEvent*, QQuickItem*)));
        QObject::connect(filter, SIGNAL(exited(QQuickMouseEvent*, QQuickItem*)), this, SLOT(onMouseEvent(QQuickMouseEvent*, QQuickItem*)));
        QObject::connect(filter, SIGNAL(positionChanged(QQuickMouseEvent*, QQuickItem*)), this, SLOT(onMouseEvent2(QQuickMouseEvent*, QQuickItem*)));

        preventDblClick();
        // Note: press inside the filtered area, so entered() exited() positionChanged() will
        // all receive the pressed button
        QTest::mousePress(view.data(), Qt::LeftButton, 0, guPoint(5, 5));
        QTest::waitForEvents();
        QCOMPARE(entered.count(), 1);
        // when entered(), buttons are the same as when pressed
        QCOMPARE(mouseEventParams.handler, QString("EVENT1"));
        QCOMPARE(mouseEventParams.pressedButton, Qt::LeftButton);

        QTest::mouseMove(view.data(), guPoint(15, 5));
        QTest::mouseMove(view.data(), guPoint(25, 5));
        QTest::waitForEvents();
        QCOMPARE(mouseEvent2Params.handler, QString("EVENT2"));
        QCOMPARE(mouseEvent2Params.pressedButton, Qt::LeftButton);

        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, guPoint(35, 5));
        QTest::waitForEvents();
        QCOMPARE(exited.count(), 1);
        // when entered(), button is the same when pressed, however buttons
        // does no longer contains the button pressed
        QCOMPARE(mouseEventParams.handler, QString("EVENT1"));
        QCOMPARE(mouseEventParams.pressedButton, Qt::LeftButton);

        // we have 2 moves, but we get 3 position changes, as the release point differs from the last moved point
        QCOMPARE(positionChanged.count(), 3);
    }

    void testCase_mouseMove()
    {
        QScopedPointer<QQuickView> view(loadTest("HoverEvent.qml"));
        QVERIFY(view);
        UCMouse *filter = attachedFilter<UCMouse>(view->rootObject(), "FilterOwner");
        QVERIFY(filter);
        QSignalSpy moved(filter, SIGNAL(positionChanged(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy entered(filter, SIGNAL(entered(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy exited(filter, SIGNAL(exited(QQuickMouseEvent*, QQuickItem*)));
        QObject::connect(filter, SIGNAL(entered(QQuickMouseEvent*, QQuickItem*)), this, SLOT(onMouseEvent(QQuickMouseEvent*, QQuickItem*)));
        QObject::connect(filter, SIGNAL(exited(QQuickMouseEvent*, QQuickItem*)), this, SLOT(onMouseEvent(QQuickMouseEvent*, QQuickItem*)));
        QObject::connect(filter, SIGNAL(positionChanged(QQuickMouseEvent*, QQuickItem*)), this, SLOT(onMouseEvent2(QQuickMouseEvent*, QQuickItem*)));

        preventDblClick();
        // move mouse while entered() triggers
        int i, x = view->rootObject()->width() / 2;
        QTest::mousePress(view.data(), Qt::LeftButton, 0, QPoint(x, 0));
        for (i = 0; (i < view->rootObject()->height()) && (entered.count() < 1); i++) {
            QTest::mouseMove(view.data(), QPoint(x, i));
        }
        // Note: as button was pressed outside the filtered area, no button will be reported
        // by the mouse move.
        QCOMPARE(mouseEventParams.handler, QString("EVENT1"));
        QCOMPARE(mouseEventParams.pressedButton, Qt::NoButton);
        QCOMPARE(mouseEventParams.pressedButtons, Qt::NoButton);
        // continue to move mouse while exited() triggers
        while (++i < view->rootObject()->height()) {
            QTest::mouseMove(view.data(), QPoint(x, i));
            if (exited.count() >= 1) {
                break;
            }
            QCOMPARE(mouseEvent2Params.handler, QString("EVENT2"));
            QCOMPARE(mouseEvent2Params.pressedButton, Qt::NoButton);
            QCOMPARE(mouseEvent2Params.pressedButtons, Qt::NoButton);
        }
        // test if we really exited
        QCOMPARE(mouseEventParams.handler, QString("EVENT1"));
        QCOMPARE(mouseEventParams.pressedButton, Qt::NoButton);
        QCOMPARE(mouseEventParams.pressedButtons, Qt::NoButton);

        // cleanup
        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, QPoint(x, i));
    }

    void testCase_hoverEvents() {
        QScopedPointer<QQuickView> view(loadTest("HoverEvent.qml"));
        QVERIFY(view);
        UCMouse *filter = attachedFilter<UCMouse>(view->rootObject(), "FilterOwner");
        QVERIFY(filter);

        QSignalSpy entered(filter, SIGNAL(entered(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy moved(filter, SIGNAL(positionChanged(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy exited(filter, SIGNAL(exited(QQuickMouseEvent*, QQuickItem*)));

        QObject::connect(filter, SIGNAL(entered(QQuickMouseEvent*, QQuickItem*)), this, SLOT(onMouseEvent(QQuickMouseEvent*, QQuickItem*)));
        QObject::connect(filter, SIGNAL(exited(QQuickMouseEvent*, QQuickItem*)), this, SLOT(onMouseEvent(QQuickMouseEvent*, QQuickItem*)));
        QObject::connect(filter, SIGNAL(positionChanged(QQuickMouseEvent*, QQuickItem*)), this, SLOT(onMouseEvent2(QQuickMouseEvent*, QQuickItem*)));

        // move mouse while entered() triggers
        int i, x = view->rootObject()->width() / 2;
        for (i = 0; (i < view->rootObject()->height()) && (entered.count() < 1); i++) {
            QTest::mouseMove(view.data(), QPoint(x, i));
        }
        QCOMPARE(mouseEventParams.handler, QString("EVENT1"));
        QCOMPARE(mouseEventParams.pressedButton, Qt::NoButton);
        QCOMPARE(mouseEventParams.pressedButtons, Qt::NoButton);
        // continue to move mouse while exited() triggers
        while (++i < view->rootObject()->height()) {
            QTest::mouseMove(view.data(), QPoint(x, i));
            if (exited.count() >= 1) {
                break;
            }
            QCOMPARE(mouseEvent2Params.handler, QString("EVENT2"));
            QCOMPARE(mouseEvent2Params.pressedButton, Qt::NoButton);
            QCOMPARE(mouseEvent2Params.pressedButtons, Qt::NoButton);
        }
        // test if we really exited
        QCOMPARE(mouseEventParams.handler, QString("EVENT1"));
        QCOMPARE(mouseEventParams.pressedButton, Qt::NoButton);
        QCOMPARE(mouseEventParams.pressedButtons, Qt::NoButton);
    }

    void testCase_forwardComposedEventsToProxy()
    {
        QScopedPointer<UbuntuTestCase> test(new UbuntuTestCase("ForwardComposedEvents.qml"));

        UCMouse *host = attachedFilter<UCMouse>(test->rootObject(), "host");
        QVERIFY(host);
        UCMouse *proxy = attachedFilter<UCMouse>(test->rootObject(), "proxy");
        QVERIFY(proxy);

        QSignalSpy hostHoldSpy(host, SIGNAL(pressAndHold(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy proxyHoldSpy(proxy, SIGNAL(pressAndHold(QQuickMouseEvent*, QQuickItem*)));

        // produce long press
        // FIXME move this under UbuntuTestCase
        mousePressAndHold(test.data(), Qt::LeftButton, 0, guPoint(20, 30));
        QTest::waitForEvents();
        QCOMPARE(hostHoldSpy.count(), 1);
        QCOMPARE(proxyHoldSpy.count(), 1);

        // click
        QTest::mouseRelease(test.data(), Qt::LeftButton, 0, guPoint(20, 30));
        QTest::waitForEvents();
    }

    void testCase_forwardComposedEventsToProxy_whenMoved()
    {
        QScopedPointer<UbuntuTestCase> test(new UbuntuTestCase("ForwardComposedEvents.qml"));

        UCMouse *host = attachedFilter<UCMouse>(test->rootObject(), "host");
        QVERIFY(host);
        UCMouse *proxy = attachedFilter<UCMouse>(test->rootObject(), "proxy");
        QVERIFY(proxy);

        QSignalSpy hostHoldSpy(host, SIGNAL(pressAndHold(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy proxyHoldSpy(proxy, SIGNAL(pressAndHold(QQuickMouseEvent*, QQuickItem*)));

        // produce long press, but move mouse while in delay
        // FIXME move this under UbuntuTestCase
        QTest::mousePress(test.data(), Qt::LeftButton, 0, guPoint(20, 30));
        QTest::qWait(DefaultPressAndHoldDelay / 2);
        for (int i = 1; i <= 4; i++) {
            QTest::mouseMove(test.data(), guPoint(20 + i, 30 + i));
        }
        QTest::qWait(DefaultPressAndHoldDelay / 2 + 200);
        QTest::waitForEvents();
        QCOMPARE(hostHoldSpy.count(), 0);
        QCOMPARE(proxyHoldSpy.count(), 0);

        // click
        QTest::mouseRelease(test.data(), Qt::LeftButton, 0, guPoint(20, 30));
        QTest::waitForEvents();
    }

    void testCase_forwardToChainedItems()
    {
        QScopedPointer<UbuntuTestCase> test(new UbuntuTestCase("ForwardEventChained.qml"));

        UCMouse *host = attachedFilter<UCMouse>(test->rootObject(), "host");
        QVERIFY(host);
        UCMouse *proxy1 = attachedFilter<UCMouse>(test->rootObject(), "proxy1");
        QVERIFY(proxy1);
        UCMouse *proxy2 = attachedFilter<UCMouse>(test->rootObject(), "proxy2");
        QVERIFY(proxy2);

        QSignalSpy hostPressed(host, SIGNAL(pressed(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy proxy1Pressed(proxy1, SIGNAL(pressed(QQuickMouseEvent*, QQuickItem*)));
        QSignalSpy proxy2Pressed(proxy2, SIGNAL(pressed(QQuickMouseEvent*, QQuickItem*)));

        QObject::connect(host, SIGNAL(pressed(QQuickMouseEvent*,QQuickItem*)), this, SLOT(onMouseEvent(QQuickMouseEvent*,QQuickItem*)));
        QObject::connect(proxy1, SIGNAL(pressed(QQuickMouseEvent*,QQuickItem*)), this, SLOT(onMouseEvent2(QQuickMouseEvent*,QQuickItem*)));
        QObject::connect(proxy2, SIGNAL(pressed(QQuickMouseEvent*,QQuickItem*)), this, SLOT(onMouseEvent3(QQuickMouseEvent*,QQuickItem*)));

        QTest::mousePress(test.data(), Qt::LeftButton, 0, guPoint(20, 30));
        QTest::waitForEvents();
        QCOMPARE(hostPressed.count(), 1);
        QCOMPARE(proxy1Pressed.count(), 1);
        QCOMPARE(proxy2Pressed.count(), 1);

        QCOMPARE(mouseEventParams.sender, qobject_cast<QQuickItem*>(host->parent()));
        QCOMPARE(mouseEvent2Params.sender, qobject_cast<QQuickItem*>(host->parent()));
        QCOMPARE(mouseEvent3Params.sender, qobject_cast<QQuickItem*>(proxy1->parent()));

        // click
        QTest::mouseRelease(test.data(), Qt::LeftButton, 0, guPoint(20, 30));
        QTest::waitForEvents();
    }

    void testCase_ignoreSynthesizedEvents() {
        QScopedPointer<UbuntuTestCase> test(new UbuntuTestCase("FilterSynthesizedEvents.qml"));
        QQuickMouseArea *rootMouseArea = qobject_cast<QQuickMouseArea *>(test->rootObject());
        QQuickMouseArea* overlayArea = test->findItem<QQuickMouseArea *>("overlayArea");
        UCMouse *overlayFilter = attachedFilter<UCMouse>(test->rootObject(), "overlayArea");

        QCOMPARE(rootMouseArea != Q_NULLPTR, true);
        QCOMPARE(overlayArea != Q_NULLPTR, true);
        QCOMPARE(overlayFilter != Q_NULLPTR, true);

        QSignalSpy areaPressed(rootMouseArea, SIGNAL(pressed(QQuickMouseEvent *)));
        QSignalSpy overlayAreaPressed(overlayArea, SIGNAL(pressed(QQuickMouseEvent *)));
        QSignalSpy overlayFilterPressed(overlayFilter, SIGNAL(pressed(QQuickMouseEvent*, QQuickItem*)));
        QCOMPARE(overlayFilter->property("ignoreSynthesizedEvents").toBool(), true);

        //we're assuming the priority is set to BeforeItem, the functionality is not priority-dependent anyway,
        //just the outcome is.
        QCOMPARE(overlayFilter->priority(), UCMouse::BeforeItem);
        QCOMPARE(overlayArea->isEnabled(), true);

        //send a touch event, which will be converted to a synthesized mouse event, since
        //no item in this QML is handling touch events
        UCTestExtras::touchPress(0, overlayArea, guPoint(15, 15));
        QTest::waitForEvents();

        QCOMPARE(areaPressed.count(), 0);
        QCOMPARE(overlayAreaPressed.count(), 1);
        QCOMPARE(overlayFilterPressed.count(), 0);
        UCTestExtras::touchRelease(0, overlayArea, guPoint(15, 15));

        QTest::waitForEvents();

        overlayFilter->setProperty("ignoreSynthesizedEvents", false);
        QCOMPARE(overlayFilter->property("ignoreSynthesizedEvents").toBool(), false);
        UCTestExtras::touchPress(1, overlayArea, guPoint(15, 15));
        QTest::waitForEvents();

        QCOMPARE(areaPressed.count(), 0);
        //the filter doesn't accept the pressed event by default
        QCOMPARE(overlayAreaPressed.count(), 2);
        QCOMPARE(overlayFilterPressed.count(), 1);
        UCTestExtras::touchRelease(1, overlayArea, guPoint(15, 15));
        QTest::waitForEvents();

        overlayArea->setEnabled(false);
        QCOMPARE(overlayArea->isEnabled(), false);

        UCTestExtras::touchPress(2, overlayArea, guPoint(15, 15));
        QTest::waitForEvents();

        //the filter gets the event but its owner is not enabled, so we expect it
        //to propagate to the area underneath
        QCOMPARE(areaPressed.count(), 1);
        QCOMPARE(overlayAreaPressed.count(), 2);
        QCOMPARE(overlayFilterPressed.count(), 2);
        UCTestExtras::touchRelease(2, overlayArea, guPoint(15, 15));
        QTest::waitForEvents();
    }
};

QTEST_MAIN(tst_mouseFilterTest)

#include "tst_mousefilterstest.moc"
