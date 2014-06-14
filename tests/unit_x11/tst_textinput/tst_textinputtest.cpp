/*
 * Copyright 2012, 2013, 2014 Canonical Ltd.
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
#include <QtQuick/private/qquickflickable_p.h>

#include "uctestcase.h"
#include "ucunits.h"

class tst_TextInputTest : public QObject
{
    Q_OBJECT
    
public:
    tst_TextInputTest()
    {}

private:

    QPoint guPoint(qreal guX, qreal guY)
    {
        return QPointF(UCUnits::instance().gu(guX), UCUnits::instance().gu(guY)).toPoint();
    }

private Q_SLOTS:

    void initTestCase()
    {
    }

    void cleanupTestCase()
    {
    }

    void test_no_caret_when_no_touchscreen_data()
    {
        QTest::addColumn<QString>("document");

        QTest::newRow("TextField") << "TextFieldTest.qml";
        QTest::newRow("TextArea") << "TextAreaTest.qml";
    }
    void test_no_caret_when_no_touchscreen()
    {
        // skip the test if we are on a touch enabled device
        if (UbuntuTestCase::touchDevicePresent()) {
            QSKIP("This test can be executed on non-touch enabled environment.");
        }
        QFETCH(QString, document);
        QScopedPointer<UbuntuTestCase> test(new UbuntuTestCase(document));
        QVERIFY(test);

        QQuickItem *textField = test->findItem<QQuickItem*>("test_input");
        QVERIFY(textField);

        textField->forceActiveFocus();
        QQuickItem *cursor = test->findItem<QQuickItem*>("textCursor");
        QVERIFY(cursor);
        QQuickItem *caret = cursor->property("caret").value<QQuickItem*>();
        QVERIFY(caret);
        QVERIFY(!caret->isVisible());
    }

    void test_has_caret_when_touchscreen_data()
    {
        UbuntuTestCase::registerTouchDevice();
        QTest::addColumn<QString>("document");

        QTest::newRow("TextField") << "TextFieldTest.qml";
        QTest::newRow("TextArea") << "TextAreaTest.qml";
    }
    void test_has_caret_when_touchscreen()
    {
        QFETCH(QString, document);
        QScopedPointer<UbuntuTestCase> test(new UbuntuTestCase(document));
        QVERIFY(test);

        QQuickItem *textField = test->findItem<QQuickItem*>("test_input");
        QVERIFY(textField);

        textField->forceActiveFocus();
        QQuickItem *cursor = test->findItem<QQuickItem*>("textCursor");
        QVERIFY(cursor);
        QQuickItem *caret = cursor->property("caret").value<QQuickItem*>();
        QVERIFY(caret);
        QVERIFY(caret->isVisible());
    }

    void test_do_not_activate_on_pressed_data()
    {
        QTest::addColumn<QString>("document");
        QTest::addColumn<bool>("touchEvent");

        QTest::newRow("TextField") << "TextFieldTest.qml" << false;
        QTest::newRow("TextArea") << "TextAreaTest.qml" << false;
        QTest::newRow("TextField") << "TextFieldTest.qml" << true;
        QTest::newRow("TextArea") << "TextAreaTest.qml" << true;
    }
    void test_do_not_activate_on_pressed()
    {
        QFETCH(QString, document);
        QFETCH(bool, touchEvent);

        QScopedPointer<UbuntuTestCase> test(new UbuntuTestCase(document));
        QVERIFY(test);

        QQuickItem *textField = test->findItem<QQuickItem*>("test_input");
        QVERIFY(textField);

        QVERIFY(!textField->hasFocus());

        QPoint center = UbuntuTestCase::centerOf(textField).toPoint();
        if (touchEvent) {
            UbuntuTestCase::touchPress(0, test.data(), center);
        } else {
            QTest::mousePress(test.data(), Qt::LeftButton, Qt::NoModifier, center);
        }
        QVERIFY(!textField->hasFocus());
        // consume mouse events!!!
        if (!touchEvent) {
            QTest::mouseRelease(test.data(), Qt::LeftButton, Qt::NoModifier, center);
            // suppress double clicks!
            QTest::qWait(800);
        }
    }

    void test_activate_when_released_data()
    {
        QTest::addColumn<QString>("document");
        QTest::addColumn<bool>("touchEvent");

        QTest::newRow("TextField") << "TextFieldTest.qml" << false;
        QTest::newRow("TextArea") << "TextAreaTest.qml" << false;
        QTest::newRow("TextField") << "TextFieldTest.qml" << true;
        QTest::newRow("TextArea") << "TextAreaTest.qml" << true;
    }

    void test_activate_when_released()
    {
        QFETCH(QString, document);
        QFETCH(bool, touchEvent);

        QScopedPointer<UbuntuTestCase> test(new UbuntuTestCase(document));
        QVERIFY(test);

        QQuickItem *textField = test->findItem<QQuickItem*>("test_input");
        QVERIFY(textField);

        QVERIFY(!textField->hasFocus());

        QPoint center = UbuntuTestCase::centerOf(textField).toPoint();
        if (touchEvent) {
            UbuntuTestCase::touchClick(0, test.data(), center);
        } else {
            QTest::mouseClick(test.data(), Qt::LeftButton, Qt::NoModifier, center);
        }
        QVERIFY(textField->hasFocus());
        // consume mouse events!!!
        if (!touchEvent) {
            // suppress double clicks!
            QTest::qWait(800);
        }
    }

    // touch handling tests
    void test_select_text_doubletap_data()
    {
        QTest::addColumn<QString>("document");

        QTest::newRow("TextField doubleTap") << "TextFieldTest.qml";
        QTest::newRow("TextArea doubleTap") << "TextAreaTest.qml";
    }
    void test_select_text_doubletap()
    {
        QFETCH(QString, document);

        QScopedPointer<UbuntuTestCase> test(new UbuntuTestCase(document));
        QVERIFY(test);

        QQuickItem *input = test->findItem<QQuickItem*>("test_input");
        QVERIFY(input);

        // double tap on the first word
        UbuntuTestCase::touchDoubleClick(0, test.data(), guPoint(1, 1));
        QVERIFY(!input->property("selectedText").value<QString>().isEmpty());
    }

    void test_longtap_when_inactive_has_no_effect_data()
    {
        QTest::addColumn<QString>("document");

        QTest::newRow("TextField longTap") << "TextFieldTest.qml";
        QTest::newRow("TextArea longTap") << "TextAreaTest.qml";
    }
    void test_longtap_when_inactive_has_no_effect()
    {
        QFETCH(QString, document);

        QScopedPointer<UbuntuTestCase> test(new UbuntuTestCase(document));
        QVERIFY(test);

        QQuickItem *input = test->findItem<QQuickItem*>("test_input");
        QVERIFY(input);

        // long tap
        UbuntuTestCase::touchLongPress(0, test.data(), guPoint(1, 1));
        QTest::waitForEvents();
        QVERIFY(!input->hasFocus());
        QVERIFY(input->property("selectedText").value<QString>().isEmpty());

        // cleanup
        UbuntuTestCase::touchRelease(0, test.data(), guPoint(1, 1));
    }

    void test_select_text_longtap_data()
    {
        QTest::addColumn<QString>("document");

        QTest::newRow("TextField longTap") << "TextFieldTest.qml";
        QTest::newRow("TextArea longTap") << "TextAreaTest.qml";
    }
    void test_select_text_longtap()
    {
        QFETCH(QString, document);

        QScopedPointer<UbuntuTestCase> test(new UbuntuTestCase(document));
        QVERIFY(test);

        QQuickItem *input = test->findItem<QQuickItem*>("test_input");
        QVERIFY(input);
        input->forceActiveFocus();
        QTest::qWaitForWindowExposed(test.data());
        QVERIFY(input->hasFocus());

        QQuickItem *handler = test->findItem<QQuickItem*>("input_handler");
        QVERIFY(handler);
        QSignalSpy openPopover(handler, SIGNAL(pressAndHold(int)));

        // long tap
        UbuntuTestCase::touchLongPress(0, test.data(), guPoint(1, 1));
        // this can be flaky!!!
        QVERIFY(openPopover.wait());
        // make sure the longTap is handled
        QTest::waitForEvents();
        QVERIFY(!input->property("selectedText").value<QString>().isEmpty());
        QCOMPARE(openPopover.count(), 1);

        //cleanup
        UbuntuTestCase::touchRelease(0, test.data(), guPoint(1, 1));
    }

    void test_long_tap_on_selected_text_data()
    {
        QTest::addColumn<QString>("document");

        QTest::newRow("TextField longTap") << "TextFieldTest.qml";
        QTest::newRow("TextArea longTap") << "TextAreaTest.qml";
    }
    void test_long_tap_on_selected_text()
    {
        QFETCH(QString, document);

        QScopedPointer<UbuntuTestCase> test(new UbuntuTestCase(document));
        QVERIFY(test);

        QQuickItem *input = test->findItem<QQuickItem*>("test_input");
        QVERIFY(input);
        input->forceActiveFocus();
        QTest::qWaitForWindowExposed(test.data());
        QVERIFY(input->hasFocus());

        QQuickItem *handler = test->findItem<QQuickItem*>("input_handler");
        QVERIFY(handler);
        QSignalSpy openPopover(handler, SIGNAL(pressAndHold(int)));

        // select the first word
        QMetaObject::invokeMethod(input, "selectWord");
        QString selectedText = input->property("selectedText").toString();

        // long tap on it
        UbuntuTestCase::touchLongPress(0, test.data(), guPoint(1, 1));
        // this IS flaky!!!
        QVERIFY(openPopover.wait());
        // make sure the longTap is handled
        QTest::waitForEvents();
        QCOMPARE(selectedText, input->property("selectedText").toString());
        QCOMPARE(openPopover.count(), 1);

        //cleanup
        UbuntuTestCase::touchRelease(0, test.data(), guPoint(1, 1));
        QTest::qWait(200);
    }

    void test_scroll_when_tap_dragged_data()
    {
        QTest::addColumn<QString>("document");
        QTest::addColumn<bool>("withTextSelected");
        QTest::addColumn<QPoint>("from");
        QTest::addColumn<QPoint>("delta");

        QTest::newRow("TextField") << "TextFieldTest.qml"
                                   << false
                                   << guPoint(10, 1)
                                   << guPoint(-10, 1);
        QTest::newRow("TextArea") << "TextAreaTest.qml"
                                  << false
                                  << guPoint(5, 4)
                                  << guPoint(0, -4);
        QTest::newRow("TextField") << "TextFieldTest.qml"
                                   << true
                                   << guPoint(10, 1)
                                   << guPoint(-10, 1);
        QTest::newRow("TextArea") << "TextAreaTest.qml"
                                  << true
                                  << guPoint(5, 4)
                                  << guPoint(0, -4);
    }
    void test_scroll_when_tap_dragged()
    {
        QFETCH(QString, document);
        QFETCH(bool, withTextSelected);
        QFETCH(QPoint, from);
        QFETCH(QPoint, delta);

        QScopedPointer<UbuntuTestCase> test(new UbuntuTestCase(document));
        QVERIFY(test);

        QQuickItem *input = test->findItem<QQuickItem*>("test_input");
        QVERIFY(input);

        QQuickFlickable *flicker = test->findItem<QQuickFlickable*>("input_scroller");
        QVERIFY(flicker);
        QSignalSpy flickerSpy(flicker, SIGNAL(movementStarted()));

        input->forceActiveFocus();
        QTest::qWaitForWindowExposed(test.data());
        QVERIFY(input->hasFocus());

        QString selectedText;
        if (withTextSelected) {
            // select some text
            QMetaObject::invokeMethod(input, "selectWord");
            selectedText = input->property("selectedText").value<QString>();
        }
        QTest::waitForEvents();
        UbuntuTestCase::touchDrag(0, test.data(), from, delta);
        QTest::waitForEvents();
        QVERIFY(flickerSpy.count() > 0);
        if (withTextSelected) {
            QCOMPARE(selectedText, input->property("selectedText").value<QString>());
        }
    }

    void test_drag_cursor_handler_data()
    {
        QTest::addColumn<QString>("document");
        QTest::addColumn<QPoint>("delta");

        QTest::newRow("TextField") << "TextFieldTest.qml"
                                   << guPoint(10, 0);
        QTest::newRow("TextArea") << "TextAreaTest.qml"
                                   << guPoint(10, 4);
    }
    void test_drag_cursor_handler()
    {
        QFETCH(QString, document);
        QFETCH(QPoint, delta);

        QScopedPointer<UbuntuTestCase> test(new UbuntuTestCase(document));
        QVERIFY(test);

        QQuickItem *input = test->findItem<QQuickItem*>("test_input");
        QVERIFY(input);
        input->forceActiveFocus();
        QTest::qWaitForWindowExposed(test.data());
        QVERIFY(input->hasFocus());

        QQuickItem *caret = test->findItem<QQuickItem*>("cursorPosition_draggeditem");
        QVERIFY(caret);

        // drag cursor
        int cursorPosition = input->property("cursorPosition").toInt();
        QPoint from = UbuntuTestCase::centerOf(caret).toPoint();
        UbuntuTestCase::touchDrag(0, test.data(), from, delta);
        QTest::waitForEvents();
        QVERIFY(cursorPosition != input->property("cursorPosition").toInt());
    }

    void test_select_text_by_dragging_cursor_handler_data()
    {
        QTest::addColumn<QString>("document");
        QTest::addColumn<QString>("cursorName");
        QTest::addColumn<int>("initialCursorPosition");
        QTest::addColumn<QPoint>("delta");

        QTest::newRow("TextField selectionEnd") << "TextFieldTest.qml"
                                   << "selectionEnd"
                                   << 0
                                   << guPoint(10, 0);
        QTest::newRow("TextArea selectionEnd") << "TextAreaTest.qml"
                                  << "selectionEnd"
                                  << 0
                                  << guPoint(10, 5);

        QTest::newRow("TextField selectionStart") << "TextFieldTest.qml"
                                   << "selectionStart"
                                   << 50
                                   << guPoint(-10, 0);
        QTest::newRow("TextArea selectionStart") << "TextAreaTest.qml"
                                   << "selectionStart"
                                   << 50
                                   << guPoint(-20, -5);
    }
    void test_select_text_by_dragging_cursor_handler()
    {
        QFETCH(QString, document);
        QFETCH(QString, cursorName);
        QFETCH(int, initialCursorPosition);
        QFETCH(QPoint, delta);

        QScopedPointer<UbuntuTestCase> test(new UbuntuTestCase(document));
        QVERIFY(test);

        QQuickItem *input = test->findItem<QQuickItem*>("test_input");
        QVERIFY(input);
        input->forceActiveFocus();
        QTest::qWaitForWindowExposed(test.data());
        QVERIFY(input->hasFocus());

        // move the cursor to the initial position (selection start cannot bypass selection end)
        input->setProperty("cursorPosition", initialCursorPosition);

        QMetaObject::invokeMethod(input, "selectWord");
        QString selectedText = input->property("selectedText").toString();
        QVERIFY(!selectedText.isEmpty());

        QQuickItem *caret = test->findItem<QQuickItem*>(cursorName + "_draggeditem");
        QVERIFY(caret);

        // drag cursor
        int cursorPosition = input->property(cursorName.toLocal8Bit().constData()).toInt();
        QPoint from = UbuntuTestCase::centerOf(caret).toPoint();
        UbuntuTestCase::touchDrag(0, test.data(), from, delta);
        QTest::waitForEvents();
        QVERIFY(cursorPosition != input->property(cursorName.toLocal8Bit().constData()).toInt());
        QVERIFY(!input->property("selectedText").toString().isEmpty() && selectedText != input->property("selectedText").toString());
    }

    void test_drag_autosizing_textarea_drags_parent_flickable()
    {
        QScopedPointer<UbuntuTestCase> test(new UbuntuTestCase("TextAreaInFlickable.qml"));
        QVERIFY(test);

        QQuickFlickable *flickable = test->findItem<QQuickFlickable*>("flickable");
        QVERIFY(flickable);
        QQuickItem *input = test->findItem<QQuickItem*>("test_input");
        QVERIFY(input);
        QQuickItem *editor = test->findItem<QQuickItem*>("text_input");
        QVERIFY(editor);

        QSignalSpy moveSpy(flickable, SIGNAL(movementEnded()));

        // move when not focused
        QPoint from(editor->x(), editor->y());
        UbuntuTestCase::touchDrag(0, test.data(), from, guPoint(2, 40));
        QVERIFY(moveSpy.wait());

        // focus then repeat dragging
        moveSpy.clear();
        input->forceActiveFocus();
        UbuntuTestCase::touchDrag(0, test.data(), from, guPoint(2, 40));
        QVERIFY(moveSpy.wait());
    }

    // mouse handling
    // contains the problematic test cases transferred from QML test
    void test_select_text_by_double_click_data()
    {
        QTest::addColumn<QString>("document");

        QTest::newRow("TextField doubleTap") << "TextFieldTest.qml";
        QTest::newRow("TextArea doubleTap") << "TextAreaTest.qml";
    }
    void test_select_text_by_double_click()
    {
        QFETCH(QString, document);

        QScopedPointer<UbuntuTestCase> test(new UbuntuTestCase(document));
        QVERIFY(test);

        QQuickItem *input = test->findItem<QQuickItem*>("test_input");
        QVERIFY(input);

        // double tap on the first word, move cursorPosition and get cursorRectangle
        // to find out where to doubleTap
        QTest::mouseDClick(test.data(), Qt::LeftButton, Qt::NoModifier, guPoint(1, 1));
        QVERIFY(!input->property("selectedText").value<QString>().isEmpty());
    }
};

QTEST_MAIN(tst_TextInputTest)

#include "tst_textinputtest.moc"
