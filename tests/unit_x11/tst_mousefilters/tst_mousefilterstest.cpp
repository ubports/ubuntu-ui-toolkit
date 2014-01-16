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

    QPoint guPoint(qreal guX, qreal guY)
    {
        return QPoint(UCUnits::instance().gu(guX), UCUnits::instance().gu(guY));
    }

    void preventDblClick()
    {
        QTest::qWait(400);
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

        if (!inputPanelPresent()) {
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
        QSignalSpy pressed(filter, SIGNAL(pressed(UCExtendedMouseEvent*)));
        QSignalSpy released(filter, SIGNAL(released(UCExtendedMouseEvent*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(UCExtendedMouseEvent*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(UCExtendedMouseEvent*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(UCExtendedMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(UCExtendedMouseEvent*)));
        QSignalSpy entered(filter, SIGNAL(entered(UCExtendedMouseEvent*)));
        QSignalSpy exited(filter, SIGNAL(exited(UCExtendedMouseEvent*)));

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
        QSignalSpy pressed(filter, SIGNAL(pressed(UCExtendedMouseEvent*)));
        QSignalSpy released(filter, SIGNAL(released(UCExtendedMouseEvent*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(UCExtendedMouseEvent*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(UCExtendedMouseEvent*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(UCExtendedMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(UCExtendedMouseEvent*)));
        QSignalSpy entered(filter, SIGNAL(entered(UCExtendedMouseEvent*)));
        QSignalSpy exited(filter, SIGNAL(exited(UCExtendedMouseEvent*)));

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
        QSignalSpy pressed(filter, SIGNAL(pressed(UCExtendedMouseEvent*)));
        QSignalSpy released(filter, SIGNAL(released(UCExtendedMouseEvent*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(UCExtendedMouseEvent*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(UCExtendedMouseEvent*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(UCExtendedMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(UCExtendedMouseEvent*)));
        QSignalSpy entered(filter, SIGNAL(entered(UCExtendedMouseEvent*)));
        QSignalSpy exited(filter, SIGNAL(exited(UCExtendedMouseEvent*)));

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

        // check if we have OSK visible
        if (!inputPanelPresent()) {
            QEXPECT_FAIL(0, "No OSK installed", Continue);
        }
        QVERIFY(QGuiApplication::inputMethod()->isVisible());

        // connect onPressed to catch OSK press
        oskClick = false;
        QObject::connect(filter, SIGNAL(pressed(UCExtendedMouseEvent*)), this, SLOT(filterPressOverOSK(UCExtendedMouseEvent*)));

        // introduce small delay to suppress double clicks
        preventDblClick();
        QTest::mouseClick(view.data(), Qt::LeftButton, 0, guPoint(10, 10));
        QTest::waitForEvents();
        preventDblClick();
        QTest::mouseClick(view.data(), Qt::LeftButton, 0, guPoint(10, 69));
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
        preventDblClick();
        QTest::mouseClick(view.data(), Qt::LeftButton, 0, guPoint(10, 10));
        QTest::waitForEvents();
        preventDblClick();
        QTest::mouseClick(view.data(), Qt::LeftButton, 0, guPoint(10, 69));
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
        preventDblClick();
        QTest::mousePress(view.data(), Qt::LeftButton, 0, guPoint(2, 2));
        QTest::mouseMove(view.data(), guPoint(2.7, 2.7));
        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, guPoint(2.7, 2.7));
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
        QSignalSpy clicked(filter, SIGNAL(clicked(UCExtendedMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(UCExtendedMouseEvent*)));

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
        QSignalSpy clicked(filter, SIGNAL(clicked(UCExtendedMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(UCExtendedMouseEvent*)));

        preventDblClick();
        mousePressAndHold(view.data(), Qt::LeftButton, 0, guPoint(2, 2));
        QTest::qWait(DefaultPressAndHoldDelay + 200);
        QTest::mouseRelease(view.data(), Qt::LeftButton, 0, guPoint(2, 2));
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

    void testCase_forwardedEventsToItem()
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
        QSignalSpy pressed(filter, SIGNAL(pressed(UCExtendedMouseEvent*)));
        QSignalSpy released(filter, SIGNAL(released(UCExtendedMouseEvent*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(UCExtendedMouseEvent*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(UCExtendedMouseEvent*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(UCExtendedMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(UCExtendedMouseEvent*)));
        QSignalSpy entered(filter, SIGNAL(entered(UCExtendedMouseEvent*)));
        QSignalSpy exited(filter, SIGNAL(exited(UCExtendedMouseEvent*)));

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

    void testCase_forwardedEventsToTargetBlocked()
    {
        QScopedPointer<QQuickView> view(loadTest("ForwardedEventsToTargetBlocked.qml"));
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
        QQuickItem *flickable = view->rootObject();//->findChild<QQuickItem*>("FilterOwner");
        QVERIFY(flickable);
        QSignalSpy pressed(filter, SIGNAL(pressed(UCExtendedMouseEvent*)));
        QSignalSpy released(filter, SIGNAL(released(UCExtendedMouseEvent*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(UCExtendedMouseEvent*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(UCExtendedMouseEvent*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(UCExtendedMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(UCExtendedMouseEvent*)));
        QSignalSpy entered(filter, SIGNAL(entered(UCExtendedMouseEvent*)));
        QSignalSpy exited(filter, SIGNAL(exited(UCExtendedMouseEvent*)));

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
        QCOMPARE(clicked.count(), 0);
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
        QSignalSpy pressed(filter, SIGNAL(pressed(UCExtendedMouseEvent*)));
        QSignalSpy released(filter, SIGNAL(released(UCExtendedMouseEvent*)));
        QSignalSpy clicked(filter, SIGNAL(clicked(UCExtendedMouseEvent*)));
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(UCExtendedMouseEvent*)));
        QSignalSpy doubleClicked(filter, SIGNAL(doubleClicked(UCExtendedMouseEvent*)));
        QSignalSpy pressAndHold(filter, SIGNAL(pressAndHold(UCExtendedMouseEvent*)));
        QSignalSpy entered(filter, SIGNAL(entered(UCExtendedMouseEvent*)));
        QSignalSpy exited(filter, SIGNAL(exited(UCExtendedMouseEvent*)));

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
        QCOMPARE(clicked.count(), 0);
        QCOMPARE(positionChanged.count(), 30);
        QCOMPARE(doubleClicked.count(), 0);
        QCOMPARE(pressAndHold.count(), 0);
        QCOMPARE(entered.count(), 1);
        QCOMPARE(exited.count(), 1);
        QCOMPARE(flickStart.count(), 1);
    }

    void testCase_hover()
    {
        QScopedPointer<QQuickView> view(loadTest("Hover.qml"));
        QVERIFY(view);
        UCMouse *filter = attachedFilter<UCMouse>(view->rootObject(), "FilterOwner");
        QVERIFY(filter);
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(UCExtendedMouseEvent*)));
        QSignalSpy entered(filter, SIGNAL(entered(UCExtendedMouseEvent*)));
        QSignalSpy exited(filter, SIGNAL(exited(UCExtendedMouseEvent*)));

        preventDblClick();
        QTest::mouseMove(view.data(), guPoint(5, 5));
        QTest::mouseMove(view.data(), guPoint(15, 5));
        QTest::mouseMove(view.data(), guPoint(25, 5));
        QTest::mouseMove(view.data(), guPoint(35, 5));
        QTest::mouseMove(view.data(), guPoint(40, 5));
        QTest::waitForEvents();

        QCOMPARE(positionChanged.count(), 3); // only 3 moves inside
        QCOMPARE(entered.count(), 1);
        QCOMPARE(exited.count(), 1);
    }

    void testCase_inverseHover()
    {
        QScopedPointer<QQuickView> view(loadTest("InverseHover.qml"));
        QVERIFY(view);
        UCInverseMouse *filter = attachedFilter<UCInverseMouse>(view->rootObject(), "FilterOwner");
        QVERIFY(filter);
        QSignalSpy positionChanged(filter, SIGNAL(positionChanged(UCExtendedMouseEvent*)));
        QSignalSpy entered(filter, SIGNAL(entered(UCExtendedMouseEvent*)));
        QSignalSpy exited(filter, SIGNAL(exited(UCExtendedMouseEvent*)));

        preventDblClick();
        for (int i = 0; i < 40; i++) {
            QTest::mouseMove(view.data(), guPoint(i, 5));
        }
        QTest::waitForEvents();

        QCOMPARE(positionChanged.count(), 9); // GU 0, 1, 2, 3, 4, 36, 37, 38, 39
        QCOMPARE(entered.count(), 2); // twice, at GU 0 and 36
        QCOMPARE(exited.count(), 1); // @ GU 5
    }
};

QTEST_MAIN(tst_mouseFilterTest)

#include "tst_mousefilterstest.moc"
