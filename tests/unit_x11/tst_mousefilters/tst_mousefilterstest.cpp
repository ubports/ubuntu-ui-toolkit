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
#include <private/qquickevents_p_p.h>
#include <private/qquickmousearea_p.h>

// keep in sync with QQuickMouseArea PressAndHoldDelay
const int DefaultPressAndHoldDelay = 800;

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
    QString m_modulePath;
    QString eventHandler;
    Qt::MouseButton pressedButton;
    Qt::MouseButtons pressedButtons;

    QQuickView * loadTest(const QString &file, QSignalSpy **spy = 0)
    {
        QQuickView *view = new QQuickView;
        if (spy) {
            *spy = new QSignalSpy(view->engine(), SIGNAL(warnings(QList<QQmlError>)));
            (*spy)->setParent(view);
        }
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

    void mousePressAndHold(QWindow *view, Qt::MouseButton button, Qt::KeyboardModifiers modifiers, const QPoint &point, int delay = DefaultPressAndHoldDelay + 200)
    {
        QTest::mousePress(view, button, modifiers, point);
        QTest::qWait(delay);
    }

    bool inputPanelPresent()
    {
        return !QuickUtils::instance().inputMethodProvider().isEmpty();
    }

    QPoint guPoint(qreal guX, qreal guY)
    {
        return QPoint(UCUnits::instance().gu(guX), UCUnits::instance().gu(guY));
    }

    void preventDblClick()
    {
        QTest::qWait(400);
    }

protected Q_SLOTS:
    void onMouseEvent(QQuickMouseEvent *event)
    {
        eventHandler = "EVENT1";
        pressedButton = (Qt::MouseButton)event->button();
        pressedButtons = (Qt::MouseButtons)event->buttons();
    }
    void onMouseEvent2(QQuickMouseEvent *event)
    {
        eventHandler = "EVENT2";
        pressedButton = (Qt::MouseButton)event->button();
        pressedButtons = (Qt::MouseButtons)event->buttons();
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

    void testCase_pressedInsideTextInput()
    {
        QScopedPointer<QQuickView> view(loadTest("FilterTextInput.qml"));
        QVERIFY(view);
        UCMouse *filter = attachedFilter<UCMouse>(view->rootObject(), "FilterOwner");
        QVERIFY(filter);
        QSignalSpy pressed(filter, SIGNAL(pressed(QQuickMouseEvent*)));
        QSignalSpy released(filter, SIGNAL(released(QQuickMouseEvent*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(QQuickMouseEvent*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(QQuickMouseEvent*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(QQuickMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(QQuickMouseEvent*)));
        QSignalSpy entered(filter, SIGNAL(entered(QQuickMouseEvent*)));
        QSignalSpy exited(filter, SIGNAL(exited(QQuickMouseEvent*)));

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
        QSignalSpy pressed(filter, SIGNAL(pressed(QQuickMouseEvent*)));
        QSignalSpy released(filter, SIGNAL(released(QQuickMouseEvent*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(QQuickMouseEvent*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(QQuickMouseEvent*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(QQuickMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(QQuickMouseEvent*)));
        QSignalSpy entered(filter, SIGNAL(entered(QQuickMouseEvent*)));
        QSignalSpy exited(filter, SIGNAL(exited(QQuickMouseEvent*)));

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
        QSignalSpy pressed(filter, SIGNAL(pressed(QQuickMouseEvent*)));
        QSignalSpy released(filter, SIGNAL(released(QQuickMouseEvent*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(QQuickMouseEvent*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(QQuickMouseEvent*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(QQuickMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(QQuickMouseEvent*)));
        QSignalSpy entered(filter, SIGNAL(entered(QQuickMouseEvent*)));
        QSignalSpy exited(filter, SIGNAL(exited(QQuickMouseEvent*)));

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
        QSignalSpy *spy;
        QScopedPointer<QQuickView> view(loadTest("FilterInverseTextInputAfter.qml", &spy));
        QVERIFY(view);
        QCOMPARE(spy->count(), 1);
        UCInverseMouse *filter = attachedFilter<UCInverseMouse>(view->rootObject(), "FilterOwner");
        QVERIFY(filter);
        QSignalSpy pressed(filter, SIGNAL(pressed(QQuickMouseEvent*)));
        QSignalSpy released(filter, SIGNAL(released(QQuickMouseEvent*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(QQuickMouseEvent*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(QQuickMouseEvent*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(QQuickMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(QQuickMouseEvent*)));
        QSignalSpy entered(filter, SIGNAL(entered(QQuickMouseEvent*)));
        QSignalSpy exited(filter, SIGNAL(exited(QQuickMouseEvent*)));

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
        QSignalSpy pressed(filter, SIGNAL(pressed(QQuickMouseEvent*)));
        QSignalSpy released(filter, SIGNAL(released(QQuickMouseEvent*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(QQuickMouseEvent*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(QQuickMouseEvent*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(QQuickMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(QQuickMouseEvent*)));
        QSignalSpy entered(filter, SIGNAL(entered(QQuickMouseEvent*)));
        QSignalSpy exited(filter, SIGNAL(exited(QQuickMouseEvent*)));

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
        QSignalSpy pressed(filter, SIGNAL(pressed(QQuickMouseEvent*)));
        QSignalSpy released(filter, SIGNAL(released(QQuickMouseEvent*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(QQuickMouseEvent*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(QQuickMouseEvent*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(QQuickMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(QQuickMouseEvent*)));
        QSignalSpy entered(filter, SIGNAL(entered(QQuickMouseEvent*)));
        QSignalSpy exited(filter, SIGNAL(exited(QQuickMouseEvent*)));

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
        QSignalSpy pressed(filter, SIGNAL(pressed(QQuickMouseEvent*)));
        QSignalSpy released(filter, SIGNAL(released(QQuickMouseEvent*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(QQuickMouseEvent*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(QQuickMouseEvent*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(QQuickMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(QQuickMouseEvent*)));
        QSignalSpy entered(filter, SIGNAL(entered(QQuickMouseEvent*)));
        QSignalSpy exited(filter, SIGNAL(exited(QQuickMouseEvent*)));

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
        QSignalSpy pressed(filter, SIGNAL(pressed(QQuickMouseEvent*)));
        QSignalSpy released(filter, SIGNAL(released(QQuickMouseEvent*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(QQuickMouseEvent*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(QQuickMouseEvent*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(QQuickMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(QQuickMouseEvent*)));
        QSignalSpy entered(filter, SIGNAL(entered(QQuickMouseEvent*)));
        QSignalSpy exited(filter, SIGNAL(exited(QQuickMouseEvent*)));

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

        QSignalSpy pressed(filter, SIGNAL(pressed(QQuickMouseEvent*)));
        QSignalSpy released(filter, SIGNAL(released(QQuickMouseEvent*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(QQuickMouseEvent*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(QQuickMouseEvent*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(QQuickMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(QQuickMouseEvent*)));
        QSignalSpy entered(filter, SIGNAL(entered(QQuickMouseEvent*)));
        QSignalSpy exited(filter, SIGNAL(exited(QQuickMouseEvent*)));

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
        QSignalSpy pressed(filter, SIGNAL(pressed(QQuickMouseEvent*)));
        QSignalSpy released(filter, SIGNAL(released(QQuickMouseEvent*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(QQuickMouseEvent*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(QQuickMouseEvent*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(QQuickMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(QQuickMouseEvent*)));
        QSignalSpy entered(filter, SIGNAL(entered(QQuickMouseEvent*)));
        QSignalSpy exited(filter, SIGNAL(exited(QQuickMouseEvent*)));

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
        QSignalSpy pressed(filter, SIGNAL(pressed(QQuickMouseEvent*)));
        QSignalSpy released(filter, SIGNAL(released(QQuickMouseEvent*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(QQuickMouseEvent*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(QQuickMouseEvent*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(QQuickMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(QQuickMouseEvent*)));
        QSignalSpy entered(filter, SIGNAL(entered(QQuickMouseEvent*)));
        QSignalSpy exited(filter, SIGNAL(exited(QQuickMouseEvent*)));

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
        QSignalSpy pressed(filter, SIGNAL(pressed(QQuickMouseEvent*)));
        QSignalSpy released(filter, SIGNAL(released(QQuickMouseEvent*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(QQuickMouseEvent*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(QQuickMouseEvent*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(QQuickMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(QQuickMouseEvent*)));
        QSignalSpy entered(filter, SIGNAL(entered(QQuickMouseEvent*)));
        QSignalSpy exited(filter, SIGNAL(exited(QQuickMouseEvent*)));

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
        QSignalSpy clicked(filter, SIGNAL(clicked(QQuickMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(QQuickMouseEvent*)));

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
        QSignalSpy clicked(filter, SIGNAL(clicked(QQuickMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(QQuickMouseEvent*)));

        preventDblClick();
        mousePressAndHold(view.data(), Qt::LeftButton, 0, guPoint(2, 2));
        QTest::qWait(DefaultPressAndHoldDelay + 200);
        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, guPoint(2, 2));
        QTest::waitForEvents();
        QCOMPARE(view->rootObject()->hasFocus(), true);
        QCOMPARE(pressAndHold.count(), 1);
        QCOMPARE(clicked.count(), 1);
    }

    void testCase_doubleClicked()
    {
        QScopedPointer<QQuickView> view(loadTest("DoubleClicked.qml"));
        QVERIFY(view);
        UCMouse *filter = attachedFilter<UCMouse>(view->rootObject(), "FilterOwner");
        QVERIFY(filter);
        QQuickItem *input = view->rootObject();
        QVERIFY(input);
        QSignalSpy clicked(filter, SIGNAL(clicked(QQuickMouseEvent*)));
        QSignalSpy dblClicked(filter, SIGNAL(doubleClicked(QQuickMouseEvent*)));

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
        QSignalSpy *warningSpy;
        QScopedPointer<QQuickView> view(loadTest("MouseFilterAttachedToNonItem.qml", &warningSpy));
        QVERIFY(view);
        QCOMPARE(warningSpy->count(), 1);
    }

    void testCase_inverseMouseFilterAttachedToNonItem()
    {
        QSignalSpy *warningSpy;
        QScopedPointer<QQuickView> view(loadTest("InverseMouseFilterAttachedToNonItem.qml", &warningSpy));
        QVERIFY(view);
        QCOMPARE(warningSpy->count(), 1);
    }

    void testCase_forwardedEventsToItem()
    {
        QScopedPointer<QQuickView> view(loadTest("ForwardedMouseEvents.qml"));
        QVERIFY(view);
        UCMouse *filter = attachedFilter<UCMouse>(view->rootObject(), "target");
        QVERIFY(filter);
        QQuickItem *input = view->rootObject()->findChild<QQuickItem*>("FilterOwner");
        QVERIFY(input);
        QSignalSpy pressed(filter, SIGNAL(pressed(QQuickMouseEvent*)));
        QSignalSpy released(filter, SIGNAL(released(QQuickMouseEvent*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(QQuickMouseEvent*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(QQuickMouseEvent*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(QQuickMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(QQuickMouseEvent*)));
        QSignalSpy entered(filter, SIGNAL(entered(QQuickMouseEvent*)));
        QSignalSpy exited(filter, SIGNAL(exited(QQuickMouseEvent*)));

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
        QSignalSpy pressed(filter, SIGNAL(pressed(QQuickMouseEvent*)));
        QSignalSpy released(filter, SIGNAL(released(QQuickMouseEvent*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(QQuickMouseEvent*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(QQuickMouseEvent*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(QQuickMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(QQuickMouseEvent*)));
        QSignalSpy entered(filter, SIGNAL(entered(QQuickMouseEvent*)));
        QSignalSpy exited(filter, SIGNAL(exited(QQuickMouseEvent*)));

        preventDblClick();
        QTest::mouseClick(view.data(), Qt::LeftButton, 0, guPoint(2, 2));
        QTest::waitForEvents();
        QCOMPARE(input->hasFocus(), false);
        QCOMPARE(pressed.count(), 0);
        QCOMPARE(released.count(), 0);
        QCOMPARE(clicked.count(), 0);
        QCOMPARE(positionChanged.count(), 0);
        QCOMPARE(doubleClicked.count(), 0);
        QCOMPARE(pressAndHold.count(), 0);
        QCOMPARE(entered.count(), 0);
        QCOMPARE(exited.count(), 0);
    }

    void testCase_forwardedEventsToItemStoppedInForwards()
    {
        QScopedPointer<QQuickView> view(loadTest("ForwardedEventsStoppedInForwards.qml"));
        QVERIFY(view);
        UCMouse *filter = attachedFilter<UCMouse>(view->rootObject(), "target");
        QVERIFY(filter);
        QQuickItem *input = view->rootObject()->findChild<QQuickItem*>("FilterOwner");
        QVERIFY(input);
        QSignalSpy pressed(filter, SIGNAL(pressed(QQuickMouseEvent*)));
        QSignalSpy released(filter, SIGNAL(released(QQuickMouseEvent*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(QQuickMouseEvent*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(QQuickMouseEvent*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(QQuickMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(QQuickMouseEvent*)));
        QSignalSpy entered(filter, SIGNAL(entered(QQuickMouseEvent*)));
        QSignalSpy exited(filter, SIGNAL(exited(QQuickMouseEvent*)));

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
        QSignalSpy pressed(filter, SIGNAL(pressed(QQuickMouseEvent*)));
        QSignalSpy released(filter, SIGNAL(released(QQuickMouseEvent*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(QQuickMouseEvent*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(QQuickMouseEvent*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(QQuickMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(QQuickMouseEvent*)));
        QSignalSpy entered(filter, SIGNAL(entered(QQuickMouseEvent*)));
        QSignalSpy exited(filter, SIGNAL(exited(QQuickMouseEvent*)));

        preventDblClick();
        QTest::mouseClick(view.data(), Qt::LeftButton, 0, guPoint(2, 2));
        QTest::waitForEvents();
        QCOMPARE(input->hasFocus(), true);
        QCOMPARE(pressed.count(), 0);
        QCOMPARE(released.count(), 0);
        QCOMPARE(clicked.count(), 0);
        QCOMPARE(positionChanged.count(), 0);
        QCOMPARE(doubleClicked.count(), 0);
        QCOMPARE(pressAndHold.count(), 0);
        QCOMPARE(entered.count(), 0);
        QCOMPARE(exited.count(), 0);
    }

    void testCase_forwardedEventsToMouseArea()
    {
        QScopedPointer<QQuickView> view(loadTest("ForwardToMouseArea.qml"));
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
        QQuickItem *target = view->rootObject()->findChild<QQuickItem*>("target");
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
        QSignalSpy pressed(filter, SIGNAL(pressed(QQuickMouseEvent*)));
        QSignalSpy released(filter, SIGNAL(released(QQuickMouseEvent*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(QQuickMouseEvent*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(QQuickMouseEvent*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(QQuickMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(QQuickMouseEvent*)));
        QSignalSpy entered(filter, SIGNAL(entered(QQuickMouseEvent*)));
        QSignalSpy exited(filter, SIGNAL(exited(QQuickMouseEvent*)));

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
        QSignalSpy pressed(filter, SIGNAL(pressed(QQuickMouseEvent*)));
        QSignalSpy released(filter, SIGNAL(released(QQuickMouseEvent*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(QQuickMouseEvent*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(QQuickMouseEvent*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(QQuickMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(QQuickMouseEvent*)));
        QSignalSpy entered(filter, SIGNAL(entered(QQuickMouseEvent*)));
        QSignalSpy exited(filter, SIGNAL(exited(QQuickMouseEvent*)));

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
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(QQuickMouseEvent*)));
        QSignalSpy entered(filter, SIGNAL(entered(QQuickMouseEvent*)));
        QSignalSpy exited(filter, SIGNAL(exited(QQuickMouseEvent*)));
        QObject::connect(filter, SIGNAL(entered(QQuickMouseEvent*)), this, SLOT(onMouseEvent(QQuickMouseEvent*)));
        QObject::connect(filter, SIGNAL(exited(QQuickMouseEvent*)), this, SLOT(onMouseEvent(QQuickMouseEvent*)));
        QObject::connect(filter, SIGNAL(positionChanged(QQuickMouseEvent*)), this, SLOT(onMouseEvent2(QQuickMouseEvent*)));

        preventDblClick();
        // Note: press inside the filtered area, so entered() exited() positionChanged() will
        // all receive the pressed button
        QTest::mousePress(view.data(), Qt::LeftButton, 0, guPoint(5, 5));
        QTest::waitForEvents();
        QCOMPARE(entered.count(), 1);
        // when entered(), buttons are the same as when pressed
        QCOMPARE(eventHandler, QString("EVENT1"));
        QCOMPARE(pressedButton, Qt::LeftButton);

        QTest::mouseMove(view.data(), guPoint(15, 5));
        QTest::mouseMove(view.data(), guPoint(25, 5));
        QTest::waitForEvents();
        QCOMPARE(eventHandler, QString("EVENT2"));
        QCOMPARE(pressedButton, Qt::LeftButton);

        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, guPoint(35, 5));
        QTest::waitForEvents();
        QCOMPARE(exited.count(), 1);
        // when entered(), button is the same when pressed, however buttons
        // does no longer contains the button pressed
        QCOMPARE(eventHandler, QString("EVENT1"));
        QCOMPARE(pressedButton, Qt::LeftButton);

        // we have 2 moves, but we get 3 position changes, as the release point differs from the last moved point
        QCOMPARE(positionChanged.count(), 3);
    }

    void testCase_mouseMove()
    {
        QScopedPointer<QQuickView> view(loadTest("HoverEvent.qml"));
        QVERIFY(view);
        UCMouse *filter = attachedFilter<UCMouse>(view->rootObject(), "FilterOwner");
        QVERIFY(filter);
        QSignalSpy moved(filter, SIGNAL(positionChanged(QQuickMouseEvent*)));
        QSignalSpy entered(filter, SIGNAL(entered(QQuickMouseEvent*)));
        QSignalSpy exited(filter, SIGNAL(exited(QQuickMouseEvent*)));
        QObject::connect(filter, SIGNAL(entered(QQuickMouseEvent*)), this, SLOT(onMouseEvent(QQuickMouseEvent*)));
        QObject::connect(filter, SIGNAL(exited(QQuickMouseEvent*)), this, SLOT(onMouseEvent(QQuickMouseEvent*)));
        QObject::connect(filter, SIGNAL(positionChanged(QQuickMouseEvent*)), this, SLOT(onMouseEvent2(QQuickMouseEvent*)));

        preventDblClick();
        // move mouse while entered() triggers
        int i, x = view->rootObject()->width() / 2;
        QTest::mousePress(view.data(), Qt::LeftButton, 0, QPoint(x, 0));
        for (i = 0; (i < view->rootObject()->height()) && (entered.count() < 1); i++) {
            QTest::mouseMove(view.data(), QPoint(x, i));
        }
        // Note: as button was pressed outside the filtered area, no button will be reported
        // by the mouse move.
        QCOMPARE(eventHandler, QString("EVENT1"));
        QCOMPARE(pressedButton, Qt::NoButton);
        QCOMPARE(pressedButtons, Qt::NoButton);
        // continue to move mouse while exited() triggers
        while (++i < view->rootObject()->height()) {
            QTest::mouseMove(view.data(), QPoint(x, i));
            if (exited.count() >= 1) {
                break;
            }
            QCOMPARE(eventHandler, QString("EVENT2"));
            QCOMPARE(pressedButton, Qt::NoButton);
            QCOMPARE(pressedButtons, Qt::NoButton);
        }
        // test if we really exited
        QCOMPARE(eventHandler, QString("EVENT1"));
        QCOMPARE(pressedButton, Qt::NoButton);
        QCOMPARE(pressedButtons, Qt::NoButton);

        // cleanup
        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, QPoint(x, i));
    }

    void testCase_hoverEvents() {
        QScopedPointer<QQuickView> view(loadTest("HoverEvent.qml"));
        QVERIFY(view);
        UCMouse *filter = attachedFilter<UCMouse>(view->rootObject(), "FilterOwner");
        QVERIFY(filter);

        QSignalSpy entered(filter, SIGNAL(entered(QQuickMouseEvent*)));
        QSignalSpy moved(filter, SIGNAL(positionChanged(QQuickMouseEvent*)));
        QSignalSpy exited(filter, SIGNAL(exited(QQuickMouseEvent*)));

        QObject::connect(filter, SIGNAL(entered(QQuickMouseEvent*)), this, SLOT(onMouseEvent(QQuickMouseEvent*)));
        QObject::connect(filter, SIGNAL(exited(QQuickMouseEvent*)), this, SLOT(onMouseEvent(QQuickMouseEvent*)));
        QObject::connect(filter, SIGNAL(positionChanged(QQuickMouseEvent*)), this, SLOT(onMouseEvent2(QQuickMouseEvent*)));

        // move mouse while entered() triggers
        int i, x = view->rootObject()->width() / 2;
        for (i = 0; (i < view->rootObject()->height()) && (entered.count() < 1); i++) {
            QTest::mouseMove(view.data(), QPoint(x, i));
        }
        QCOMPARE(eventHandler, QString("EVENT1"));
        QCOMPARE(pressedButton, Qt::NoButton);
        QCOMPARE(pressedButtons, Qt::NoButton);
        // continue to move mouse while exited() triggers
        while (++i < view->rootObject()->height()) {
            QTest::mouseMove(view.data(), QPoint(x, i));
            if (exited.count() >= 1) {
                break;
            }
            QCOMPARE(eventHandler, QString("EVENT2"));
            QCOMPARE(pressedButton, Qt::NoButton);
            QCOMPARE(pressedButtons, Qt::NoButton);
        }
        // test if we really exited
        QCOMPARE(eventHandler, QString("EVENT1"));
        QCOMPARE(pressedButton, Qt::NoButton);
        QCOMPARE(pressedButtons, Qt::NoButton);
    }
};

QTEST_MAIN(tst_mouseFilterTest)

#include "tst_mousefilterstest.moc"
