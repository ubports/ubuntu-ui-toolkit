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
#include "uctestextras.h"
#include "ucunits.h"

class tst_TextInputTest : public QObject
{
    Q_OBJECT
    
public:
    bool skipTextCursorTests;
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
        // text cursor related test cases cannot be tested if the active focus is not set
        // when executed with xvfb the active focus item si not set, therefore we must skip all of them
        // detect whether activeFocus is set
        QScopedPointer<UbuntuTestCase> test(new UbuntuTestCase("TextFieldTest.qml"));
        QVERIFY(test);
        QQuickItem *input = test->findItem<QQuickItem*>("test_input");
        input->forceActiveFocus();
        skipTextCursorTests = (test->activeFocusItem() == 0);
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
        if (UCTestExtras::touchDevicePresent()) {
            QSKIP("This test can be executed on non-touch enabled environment.");
        }
        if (skipTextCursorTests) {
            QSKIP("XVFB environment hickup, activeFocusItem is not set, cannot retrieve text cursor for tests..");
        }
        QFETCH(QString, document);
        QScopedPointer<UbuntuTestCase> test(new UbuntuTestCase(document));
        QVERIFY(test);

        QQuickItem *textField = test->findItem<QQuickItem*>("test_input");
        QVERIFY(textField);

        textField->forceActiveFocus();
        QTest::qWaitForWindowActive(textField->window(), 1000);
        QVERIFY(test->activeFocusItem());

        QQuickItem *cursor = test->findItem<QQuickItem*>("textCursor");
        QVERIFY(cursor);
        QQuickItem *caret = cursor->property("caret").value<QQuickItem*>();
        QVERIFY(caret);
        QVERIFY(!caret->isVisible());
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
