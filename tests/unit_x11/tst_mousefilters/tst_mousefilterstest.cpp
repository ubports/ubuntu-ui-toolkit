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
    bool insideClick;
    bool oskClick;

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

protected Q_SLOTS:
    void filterPressOverOSK(UCExtendedMouseEvent *event)
    {
        oskClick = event->pointInInputArea();
    }

private Q_SLOTS:

    void initTestCase()
    {
        QString modules("../../../modules");
        QVERIFY(QDir(modules).exists());

        m_modulePath = QDir(modules).absolutePath();

        if (QuickUtils::instance().inputMethodProvider().isEmpty()) {
            setenv("MOUSEFILTER_OSK_TEST_AREA", "true", 1);
            qDebug() << "No input provider set, simulate OSK panel";
        }

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
        QSignalSpy pressed(filter, SIGNAL(pressed(UCExtendedMouseEvent*)));
        QSignalSpy released(filter, SIGNAL(released(UCExtendedMouseEvent*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(UCExtendedMouseEvent*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(UCExtendedMouseEvent*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(UCExtendedMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(UCExtendedMouseEvent*)));
        QSignalSpy entered(filter, SIGNAL(entered(UCExtendedMouseEvent*)));
        QSignalSpy exited(filter, SIGNAL(exited(UCExtendedMouseEvent*)));

        QTest::mouseClick(view.data(), Qt::LeftButton, 0, QPoint(UCUnits::instance().gu(2), UCUnits::instance().gu(2)));
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
        QSignalSpy pressed(filter, SIGNAL(pressed(UCExtendedMouseEvent*)));
        QSignalSpy released(filter, SIGNAL(released(UCExtendedMouseEvent*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(UCExtendedMouseEvent*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(UCExtendedMouseEvent*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(UCExtendedMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(UCExtendedMouseEvent*)));
        QSignalSpy entered(filter, SIGNAL(entered(UCExtendedMouseEvent*)));
        QSignalSpy exited(filter, SIGNAL(exited(UCExtendedMouseEvent*)));

        QTest::mouseClick(view.data(), Qt::LeftButton, 0, QPoint(UCUnits::instance().gu(10), UCUnits::instance().gu(10)));
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
        QScopedPointer<QQuickView> view(loadTest("FilterTextInput.qml"));
        QVERIFY(view);
        UCMouse *filter = attachedFilter<UCMouse>(view->rootObject(), "FilterOwner");
        QVERIFY(filter);
        QSignalSpy pressed(filter, SIGNAL(pressed(UCExtendedMouseEvent*)));
        QSignalSpy released(filter, SIGNAL(released(UCExtendedMouseEvent*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(UCExtendedMouseEvent*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(UCExtendedMouseEvent*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(UCExtendedMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(UCExtendedMouseEvent*)));
        QSignalSpy entered(filter, SIGNAL(entered(UCExtendedMouseEvent*)));
        QSignalSpy exited(filter, SIGNAL(exited(UCExtendedMouseEvent*)));

        QTest::mousePress(view.data(), Qt::LeftButton, 0, QPoint(UCUnits::instance().gu(2), UCUnits::instance().gu(2)));
        QTest::mouseMove(view.data(), QPoint(UCUnits::instance().gu(2.2), UCUnits::instance().gu(2.2)));
        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, QPoint(UCUnits::instance().gu(2.2), UCUnits::instance().gu(2.2)));
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
        QScopedPointer<QQuickView> view(loadTest("FilterInverseTextInput.qml"));
        QVERIFY(view);
        UCInverseMouse *filter = attachedFilter<UCInverseMouse>(view->rootObject(), "FilterOwner");
        QVERIFY(filter);
        QSignalSpy pressed(filter, SIGNAL(pressed(UCExtendedMouseEvent*)));
        QSignalSpy released(filter, SIGNAL(released(UCExtendedMouseEvent*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(UCExtendedMouseEvent*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(UCExtendedMouseEvent*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(UCExtendedMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(UCExtendedMouseEvent*)));
        QSignalSpy entered(filter, SIGNAL(entered(UCExtendedMouseEvent*)));
        QSignalSpy exited(filter, SIGNAL(exited(UCExtendedMouseEvent*)));

        QTest::mousePress(view.data(), Qt::LeftButton, 0, QPoint(UCUnits::instance().gu(10), UCUnits::instance().gu(10)));
        QTest::mouseMove(view.data(), QPoint(UCUnits::instance().gu(10.2), UCUnits::instance().gu(10.2)));
        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, QPoint(UCUnits::instance().gu(10.2), UCUnits::instance().gu(10.2)));
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
        QScopedPointer<QQuickView> view(loadTest("FilterTextInput.qml"));
        QVERIFY(view);
        UCMouse *filter = attachedFilter<UCMouse>(view->rootObject(), "FilterOwner");
        QVERIFY(filter);
        QSignalSpy pressed(filter, SIGNAL(pressed(UCExtendedMouseEvent*)));
        QSignalSpy released(filter, SIGNAL(released(UCExtendedMouseEvent*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(UCExtendedMouseEvent*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(UCExtendedMouseEvent*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(UCExtendedMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(UCExtendedMouseEvent*)));
        QSignalSpy entered(filter, SIGNAL(entered(UCExtendedMouseEvent*)));
        QSignalSpy exited(filter, SIGNAL(exited(UCExtendedMouseEvent*)));

        QTest::mousePress(view.data(), Qt::LeftButton, 0, QPoint(UCUnits::instance().gu(2), UCUnits::instance().gu(2)));
        QTest::mouseMove(view.data(), QPoint(UCUnits::instance().gu(2.7), UCUnits::instance().gu(2.7)));
        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, QPoint(UCUnits::instance().gu(2.7), UCUnits::instance().gu(2.7)));
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
        QScopedPointer<QQuickView> view(loadTest("FilterInverseTextInput.qml"));
        QVERIFY(view);
        UCInverseMouse *filter = attachedFilter<UCInverseMouse>(view->rootObject(), "FilterOwner");
        QVERIFY(filter);
        QSignalSpy pressed(filter, SIGNAL(pressed(UCExtendedMouseEvent*)));
        QSignalSpy released(filter, SIGNAL(released(UCExtendedMouseEvent*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(UCExtendedMouseEvent*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(UCExtendedMouseEvent*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(UCExtendedMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(UCExtendedMouseEvent*)));
        QSignalSpy entered(filter, SIGNAL(entered(UCExtendedMouseEvent*)));
        QSignalSpy exited(filter, SIGNAL(exited(UCExtendedMouseEvent*)));

        QTest::mousePress(view.data(), Qt::LeftButton, 0, QPoint(UCUnits::instance().gu(10), UCUnits::instance().gu(10)));
        QTest::mouseMove(view.data(), QPoint(UCUnits::instance().gu(10.6), UCUnits::instance().gu(10.6)));
        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, QPoint(UCUnits::instance().gu(10.6), UCUnits::instance().gu(10.6)));
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

    void testCase_pressedOverOSK()
    {
        QScopedPointer<QQuickView> view(loadTest("FilterInverseTextInput.qml"));
        QVERIFY(view);
        UCInverseMouse *filter = attachedFilter<UCInverseMouse>(view->rootObject(), "FilterOwner");
        QVERIFY(filter);

        QSignalSpy pressed(filter, SIGNAL(pressed(UCExtendedMouseEvent*)));
        QSignalSpy released(filter, SIGNAL(released(UCExtendedMouseEvent*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(UCExtendedMouseEvent*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(UCExtendedMouseEvent*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(UCExtendedMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(UCExtendedMouseEvent*)));
        QSignalSpy entered(filter, SIGNAL(entered(UCExtendedMouseEvent*)));
        QSignalSpy exited(filter, SIGNAL(exited(UCExtendedMouseEvent*)));

        // focus TextInput
        QQuickItem *input = view->rootObject()->findChild<QQuickItem*>("FilterOwner");
        QVERIFY(input);
        input->forceActiveFocus();
        QCOMPARE(input->hasFocus(), true);

        // connect onPressed to catch OSK press
        oskClick = false;
        QObject::connect(filter, SIGNAL(pressed(UCExtendedMouseEvent*)), this, SLOT(filterPressOverOSK(UCExtendedMouseEvent*)));

        // introduce small delay to suppress double clicks
        QTest::qWait(300);
        QTest::mouseClick(view.data(), Qt::LeftButton, 0, QPoint(UCUnits::instance().gu(10), UCUnits::instance().gu(10)));
        QTest::waitForEvents();
        QTest::qWait(300);
        QTest::mouseClick(view.data(), Qt::LeftButton, 0, QPoint(UCUnits::instance().gu(10), UCUnits::instance().gu(69)));
        QTest::waitForEvents();
        QCOMPARE(oskClick, true);
        QCOMPARE(pressed.count(), 2);
        QCOMPARE(released.count(), 2);
        QCOMPARE(clicked.count(), 2);
        QCOMPARE(positionChanged.count(), 0);
        QCOMPARE(doubleClicked.count(), 0);
        QCOMPARE(pressAndHold.count(), 0);
        QCOMPARE(entered.count(), 2);
        QCOMPARE(exited.count(), 2);
    }

    void testCase_pressedOverOSKExcluded()
    {
        QScopedPointer<QQuickView> view(loadTest("FilterInverseTextInputOSKExcluded.qml"));
        QVERIFY(view);
        UCInverseMouse *filter = attachedFilter<UCInverseMouse>(view->rootObject(), "FilterOwner");
        QVERIFY(filter);

        QSignalSpy pressed(filter, SIGNAL(pressed(UCExtendedMouseEvent*)));
        QSignalSpy released(filter, SIGNAL(released(UCExtendedMouseEvent*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(UCExtendedMouseEvent*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(UCExtendedMouseEvent*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(UCExtendedMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(UCExtendedMouseEvent*)));
        QSignalSpy entered(filter, SIGNAL(entered(UCExtendedMouseEvent*)));
        QSignalSpy exited(filter, SIGNAL(exited(UCExtendedMouseEvent*)));

        // focus TextInput
        QQuickItem *input = view->rootObject()->findChild<QQuickItem*>("FilterOwner");
        QVERIFY(input);
        input->forceActiveFocus();
        QCOMPARE(input->hasFocus(), true);

        // connect onPressed to catch OSK press
        oskClick = false;
        QObject::connect(filter, SIGNAL(pressed(UCExtendedMouseEvent*)), this, SLOT(filterPressOverOSK(UCExtendedMouseEvent*)));

        // introduce small delay to suppress double clicks
        QTest::qWait(300);
        QTest::mouseClick(view.data(), Qt::LeftButton, 0, QPoint(UCUnits::instance().gu(10), UCUnits::instance().gu(10)));
        QTest::waitForEvents();
        QTest::qWait(300);
        QTest::mouseClick(view.data(), Qt::LeftButton, 0, QPoint(UCUnits::instance().gu(10), UCUnits::instance().gu(69)));
        QTest::waitForEvents();
        QCOMPARE(oskClick, false);
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
        QSignalSpy pressed(filter, SIGNAL(pressed(UCExtendedMouseEvent*)));
        QSignalSpy released(filter, SIGNAL(released(UCExtendedMouseEvent*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(UCExtendedMouseEvent*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(UCExtendedMouseEvent*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(UCExtendedMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(UCExtendedMouseEvent*)));
        QSignalSpy entered(filter, SIGNAL(entered(UCExtendedMouseEvent*)));
        QSignalSpy exited(filter, SIGNAL(exited(UCExtendedMouseEvent*)));

        // make sure we do not get double click because of the previous tests
        QTest::qWait(300);
        QTest::mousePress(view.data(), Qt::LeftButton, 0, QPoint(10, 10));
        QTest::mouseMove(view.data(), QPoint(15, 15));
        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, QPoint(15, 15));
        QTest::waitForEvents();
        QCOMPARE(view->rootObject()->hasFocus(), true);
        QCOMPARE(pressed.count(), 1);
        QCOMPARE(released.count(), 1);
        QCOMPARE(clicked.count(), 0); // the move is out of the threshold
        QCOMPARE(positionChanged.count(), 1);
        QCOMPARE(doubleClicked.count(), 0);
        QCOMPARE(pressAndHold.count(), 0);
        QCOMPARE(entered.count(), 1);
        QCOMPARE(exited.count(), 1);
    }

    void testCase_pressAndMovedOutsideTextInputInTolerance()
    {
        QScopedPointer<QQuickView> view(loadTest("FilterInverseTextInput.qml"));
        QVERIFY(view);
        UCInverseMouse *filter = attachedFilter<UCInverseMouse>(view->rootObject(), "FilterOwner");
        QVERIFY(filter);
        QSignalSpy pressed(filter, SIGNAL(pressed(UCExtendedMouseEvent*)));
        QSignalSpy released(filter, SIGNAL(released(UCExtendedMouseEvent*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(UCExtendedMouseEvent*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(UCExtendedMouseEvent*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(UCExtendedMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(UCExtendedMouseEvent*)));
        QSignalSpy entered(filter, SIGNAL(entered(UCExtendedMouseEvent*)));
        QSignalSpy exited(filter, SIGNAL(exited(UCExtendedMouseEvent*)));

        // make sure we do not get double click because of the previous tests
        QTest::qWait(300);
        QTest::mousePress(view.data(), Qt::LeftButton, 0, QPoint(UCUnits::instance().gu(10), UCUnits::instance().gu(10)));
        QTest::mouseMove(view.data(), QPoint(UCUnits::instance().gu(10.5), UCUnits::instance().gu(10.5)));
        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, QPoint(UCUnits::instance().gu(10.5), UCUnits::instance().gu(10.5)));
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
        QScopedPointer<QQuickView> view(loadTest("FilterInverseTextInput.qml"));
        QVERIFY(view);
        UCInverseMouse *filter = attachedFilter<UCInverseMouse>(view->rootObject(), "FilterOwner");
        QVERIFY(filter);
        QSignalSpy pressed(filter, SIGNAL(pressed(UCExtendedMouseEvent*)));
        QSignalSpy released(filter, SIGNAL(released(UCExtendedMouseEvent*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(UCExtendedMouseEvent*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(UCExtendedMouseEvent*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(UCExtendedMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(UCExtendedMouseEvent*)));
        QSignalSpy entered(filter, SIGNAL(entered(UCExtendedMouseEvent*)));
        QSignalSpy exited(filter, SIGNAL(exited(UCExtendedMouseEvent*)));

        // make sure we do not get double click because of the previous tests
        QTest::qWait(300);
        QTest::mousePress(view.data(), Qt::LeftButton, 0, QPoint(UCUnits::instance().gu(10), UCUnits::instance().gu(10)));
        QTest::mouseMove(view.data(), QPoint(UCUnits::instance().gu(15), UCUnits::instance().gu(15)));
        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, QPoint(UCUnits::instance().gu(15), UCUnits::instance().gu(15)));
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
        QSignalSpy clicked(filter, SIGNAL(clicked(UCExtendedMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(UCExtendedMouseEvent*)));

        mousePressAndHold(view.data(), Qt::LeftButton, 0, QPoint(10, 10));
        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, QPoint(10, 10));
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
        QSignalSpy clicked(filter, SIGNAL(clicked(UCExtendedMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(UCExtendedMouseEvent*)));

        QTest::mousePress(view.data(), Qt::LeftButton, 0, QPoint(10, 10));
        QTest::qWait(DefaultPressAndHoldDelay + 200);
        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, QPoint(10, 10));
        QTest::waitForEvents();
        QCOMPARE(view->rootObject()->hasFocus(), true);
        QCOMPARE(pressAndHold.count(), 1);
        QCOMPARE(clicked.count(), 1);
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

    void testCase_forwardedEvents()
    {
        QScopedPointer<QQuickView> view(loadTest("ForwardedMouseEvents.qml"));
        QVERIFY(view);
        UCMouse *filter = attachedFilter<UCMouse>(view->rootObject(), "target");
        QVERIFY(filter);
        QQuickItem *input = view->rootObject()->findChild<QQuickItem*>("FilterOwner");
        QVERIFY(input);
        QSignalSpy pressed(filter, SIGNAL(pressed(UCExtendedMouseEvent*)));
        QSignalSpy released(filter, SIGNAL(released(UCExtendedMouseEvent*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(UCExtendedMouseEvent*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(UCExtendedMouseEvent*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(UCExtendedMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(UCExtendedMouseEvent*)));
        QSignalSpy entered(filter, SIGNAL(entered(UCExtendedMouseEvent*)));
        QSignalSpy exited(filter, SIGNAL(exited(UCExtendedMouseEvent*)));

        QTest::mouseClick(view.data(), Qt::LeftButton, 0, QPoint(UCUnits::instance().gu(2), UCUnits::instance().gu(2)));
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
};

QTEST_MAIN(tst_mouseFilterTest)

#include "tst_mousefilterstest.moc"
