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
 */

#include "uctestcase.h"
#include "ucserviceproperties.h"
#include "ucserviceproperties_p.h"
#include <QtCore/QString>
#include <QtCore/QDebug>
#include <QtTest/QTest>
#include <QtTest/QSignalSpy>

class tst_ServiceProperties : public QObject
{
    Q_OBJECT

public:
    tst_ServiceProperties() {}

private:

    QString error;

    // FIXME use UbuntuTestCase::ignoreWaring in Vivid
    void ignoreWarning(const QString& fileName, uint line, uint column, const QString& message, uint occurences=1)
    {
        for (uint i = 0; i < occurences; i++) {
            QString url(QUrl::fromLocalFile(QFileInfo(fileName).absoluteFilePath()).toEncoded());
            QString warning(QString("%1:%2:%3: %4").arg(url).arg(line).arg(column).arg(message));
            QTest::ignoreMessage(QtWarningMsg, warning.toUtf8());
        }
    }


private Q_SLOTS:

    void initTestCase()
    {
        // check if the connection is possible, otherwise we must skip all tests
        QScopedPointer<UbuntuTestCase> test(new UbuntuTestCase("IncomingCallVibrateWatcher.qml"));
        UCServiceProperties *watcher = static_cast<UCServiceProperties*>(test->rootObject()->property("service").value<QObject*>());
        QVERIFY(watcher);
        if (watcher->status() == UCServiceProperties::Synchronizing ||
            watcher->status() == UCServiceProperties::Inactive) {
            QSignalSpy wait(watcher, SIGNAL(statusChanged()));
            wait.wait();
        }
        if (watcher->status() == UCServiceProperties::ConnectionError) {
            error = "Skip test: " + watcher->error();
        }
    }

    void test_change_property()
    {
        if (!error.isEmpty()) {
            QSKIP(qPrintable(error));
        }
        QScopedPointer<UbuntuTestCase> test(new UbuntuTestCase("IncomingCallVibrateWatcher.qml"));
        UCServiceProperties *watcher = static_cast<UCServiceProperties*>(test->rootObject()->property("service").value<QObject*>());
        QVERIFY(watcher);

        bool backup = watcher->property("incomingCallVibrate").toBool();
        UCServicePropertiesPrivate *pWatcher = UCServicePropertiesPrivate::get(watcher);
        QSignalSpy spy(watcher, SIGNAL(incomingCallVibrateChanged()));
        pWatcher->testProperty("IncomingCallVibrate", !backup);
        spy.wait(400);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(watcher->property("incomingCallVibrate").toBool(), !backup);

        // restore value
        spy.clear();
        pWatcher->testProperty("IncomingCallVibrate", backup);
        spy.wait(400);
    }

    void test_invalid_property()
    {
        if (!error.isEmpty()) {
            QSKIP(qPrintable(error));
        }
        ignoreWarning("InvalidPropertyWatcher.qml", 22, 5, "QML ServiceProperties: No such property 'ThisIsAnInvalidPropertyToWatch'");
        QScopedPointer<UbuntuTestCase> test(new UbuntuTestCase("InvalidPropertyWatcher.qml"));
        UCServiceProperties *watcher = static_cast<UCServiceProperties*>(test->rootObject()->property("service").value<QObject*>());
        QVERIFY(watcher);
        // error should not be set
        QCOMPARE(watcher->property("error").toString(), QString());
    }

    void test_one_valid_one_invalid_property()
    {
        if (!error.isEmpty()) {
            QSKIP(qPrintable(error));
        }
        ignoreWarning("InvalidPropertyWatcher2.qml", 22, 5, "QML ServiceProperties: No such property 'ThisIsAnInvalidPropertyToWatch'");
        QScopedPointer<UbuntuTestCase> test(new UbuntuTestCase("InvalidPropertyWatcher2.qml"));
        UCServiceProperties *watcher = static_cast<UCServiceProperties*>(test->rootObject()->property("service").value<QObject*>());
        QVERIFY(watcher);
        // error should not be set
        QCOMPARE(watcher->property("error").toString(), QString());
    }

    void test_change_connection_props_data()
    {
        QTest::addColumn<QString>("property");
        QTest::addColumn<QString>("value");

        QTest::newRow("Changing servcie") << "service" << "anything.else";
        QTest::newRow("Changing interface") << "serviceInterface" << "anything.else";
        QTest::newRow("Changing adaptor") << "adaptorInterface" << "anything.else";
    }
    void test_change_connection_props()
    {
        QFETCH(QString, property);
        QFETCH(QString, value);

        if (!error.isEmpty()) {
            QSKIP(qPrintable(error));
        }
        ignoreWarning("IncomingCallVibrateWatcher.qml", 22, 5, "QML ServiceProperties: Changing connection parameters forbidden.");
        QScopedPointer<UbuntuTestCase> test(new UbuntuTestCase("IncomingCallVibrateWatcher.qml"));
        UCServiceProperties *watcher = static_cast<UCServiceProperties*>(test->rootObject()->property("service").value<QObject*>());
        QVERIFY(watcher);

        // try to change the property
        watcher->setProperty(property.toLocal8Bit().constData(), value);
        // no error should be reported
        QCOMPARE(watcher->property("error").toString(), QString());
    }

};

QTEST_MAIN(tst_ServiceProperties)

#include "tst_serviceproperties.moc"
