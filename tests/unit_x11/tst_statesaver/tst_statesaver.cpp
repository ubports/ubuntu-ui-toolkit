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
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickView>
#include <QtQuick/QQuickItem>

#define protected public
#include "ucstatesaver.h"
#include "statesaverbackend_p.h"
#undef protected

class tst_StateSaverTest : public QObject
{
    Q_OBJECT
public:
    tst_StateSaverTest() {}

private:
    QString m_modulePath;

    QQuickView *createView(const QString &file)
    {
        QQuickView *view = new QQuickView(0);
        view->engine()->addImportPath(m_modulePath);
        view->setSource(QUrl::fromLocalFile(file));
        if (!view->rootObject()) {
            return 0;
        }
        view->show();
        QTest::qWaitForWindowExposed(view);
        return view;
    }

private Q_SLOTS:

    void initTestCase()
    {
        QCoreApplication::setApplicationName("tst_statesaver");
        QCoreApplication::setOrganizationName("");
        QDir modules ("../../../modules");
        QVERIFY(modules.exists());
        m_modulePath = modules.absolutePath();
    }

    void cleanupTestCase()
    {
        StateSaverBackend::instance().reset();
    }

    void test_SaveBool()
    {
        QQuickView *view = createView("SaveBool.qml");
        QVERIFY(view);
        QObject *testItem = view->rootObject();
        QVERIFY(testItem);
        testItem->setProperty("boolValue", true);
        delete view;
        StateSaverBackend::instance().resetRegister();

        view = createView("SaveBool.qml");
        QVERIFY(view);
        testItem = view->rootObject();
        QVERIFY(testItem);
        QCOMPARE(testItem->property("boolValue").toBool(), true);
        delete view;
        StateSaverBackend::instance().resetRegister();
    }


    void test_SaveInt()
    {
        QQuickView *view = createView("SaveInt.qml");
        QVERIFY(view);
        QObject *testItem = view->rootObject();
        QVERIFY(testItem);
        testItem->setProperty("intValue", 100);
        delete view;
        StateSaverBackend::instance().resetRegister();

        view = createView("SaveInt.qml");
        QVERIFY(view);
        testItem = view->rootObject();
        QVERIFY(testItem);
        QCOMPARE(testItem->property("intValue").toInt(), 100);
        delete view;
        StateSaverBackend::instance().resetRegister();
    }

    void test_SaveReal()
    {
        QQuickView *view = createView("SaveReal.qml");
        QVERIFY(view);
        QObject *testItem = view->rootObject();
        QVERIFY(testItem);
        testItem->setProperty("realValue", 123.45);
        delete view;
        StateSaverBackend::instance().resetRegister();

        view = createView("SaveReal.qml");
        QVERIFY(view);
        testItem = view->rootObject();
        QVERIFY(testItem);
        QCOMPARE(testItem->property("realValue").toReal(), 123.45);
        delete view;
        StateSaverBackend::instance().resetRegister();
    }

    void test_SaveString()
    {
        QQuickView *view = createView("SaveString.qml");
        QVERIFY(view);
        QObject *testItem = view->rootObject();
        QVERIFY(testItem);
        testItem->setProperty("stringValue", "test string");
        delete view;
        StateSaverBackend::instance().resetRegister();

        view = createView("SaveString.qml");
        QVERIFY(view);
        testItem = view->rootObject();
        QVERIFY(testItem);
        QCOMPARE(testItem->property("stringValue").toString(), QString("test string"));
        delete view;
        StateSaverBackend::instance().resetRegister();
    }
};

QTEST_MAIN(tst_StateSaverTest)

#include "tst_statesaver.moc"



