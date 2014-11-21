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
// access private data of the watcher in testing
#define private public
#include "adapters/dbuspropertywatcher_p.h"
#undef private
#include <QtCore/QString>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>
#include <QtCore/QDebug>
#include <QtTest/QTest>
#include <QtTest/QSignalSpy>

class tst_DBusPropertyWatcher : public QObject
{
    Q_OBJECT

public:
    tst_DBusPropertyWatcher() {}

private:

    bool setDBusValue(DBusPropertyWatcher *watcher, const QString &interface, const QString &property, const QVariant &value)
    {
        QDBusInterface iface(watcher->iface.interface(), watcher->objectPath,
                             "org.freedesktop.DBus.Properties", watcher->connection);
        if (iface.isValid()) {
            QDBusMessage msg = iface.call("Set", interface, property, QVariant::fromValue(QDBusVariant(value)));
            return msg.type() == QDBusMessage::ReplyMessage;
        }
        return false;
    }

private Q_SLOTS:

    void initTestCase()
    {
    }

    void cleanupTestCase()
    {
    }

    void test_change_single_property()
    {
        DBusPropertyWatcher watcher(QDBusConnection::systemBus(),
                                    "org.freedesktop.Accounts",
                                    "/org/freedesktop/Accounts",
                                    "org.freedesktop.Accounts",
                                    QStringList("IncomingCallVibrate"));
        QSignalSpy propertyChangeSpy(&watcher, SIGNAL(propertyChanged(QString,QVariant)));
        watcher.syncProperties("com.ubuntu.touch.AccountsService.Sound");
        propertyChangeSpy.wait(400);
        QCOMPARE(propertyChangeSpy.count(), 1);

        QList<QVariant> arguments = propertyChangeSpy.takeFirst();
        QCOMPARE(arguments.at(0).toString(), QString("IncomingCallVibrate"));

        // save original value and set a new one
        bool originalValue = arguments.at(1).toBool();

        propertyChangeSpy.clear();
        bool newValue = !originalValue;
        QVERIFY(setDBusValue(&watcher, "com.ubuntu.touch.AccountsService.Sound", "IncomingCallVibrate", newValue));
        propertyChangeSpy.wait(400);
        QCOMPARE(propertyChangeSpy.count(), 1);
        arguments = propertyChangeSpy.takeFirst();
        QCOMPARE(arguments.at(0).toString(), QString("IncomingCallVibrate"));
        QCOMPARE(arguments.at(1).toBool(), newValue);

        // restore original value
        setDBusValue(&watcher, "com.ubuntu.touch.AccountsService.Sound", "IncomingCallVibrate", originalValue);
        propertyChangeSpy.wait(400);
    }

    void test_change_multiple_property()
    {
        QStringList watchedProperties = (QStringList() << "IncomingCallVibrate" << "DialpadSoundsEnabled");
        DBusPropertyWatcher watcher(QDBusConnection::systemBus(),
                                    "org.freedesktop.Accounts",
                                    "/org/freedesktop/Accounts",
                                    "org.freedesktop.Accounts",
                                    watchedProperties);
        QSignalSpy propertyChangeSpy(&watcher, SIGNAL(propertyChanged(QString,QVariant)));
        watcher.syncProperties("com.ubuntu.touch.AccountsService.Sound");
        propertyChangeSpy.wait(400);
        QCOMPARE(propertyChangeSpy.count(), 2);

        QList<QVariant> arguments = propertyChangeSpy.takeFirst();
        QCOMPARE(arguments.at(0).toString(), QString("IncomingCallVibrate"));
        arguments = propertyChangeSpy.takeFirst();
        QCOMPARE(arguments.at(0).toString(), QString("DialpadSoundsEnabled"));

        // save original value and set a new one
        bool originalValue = arguments.at(1).toBool();

        propertyChangeSpy.clear();
        bool newValue = !originalValue;
        QVERIFY(setDBusValue(&watcher, "com.ubuntu.touch.AccountsService.Sound", "DialpadSoundsEnabled", newValue));
        propertyChangeSpy.wait(400);
        QCOMPARE(propertyChangeSpy.count(), 1);
        arguments = propertyChangeSpy.takeFirst();
        QCOMPARE(arguments.at(0).toString(), QString("DialpadSoundsEnabled"));
        QCOMPARE(arguments.at(1).toBool(), newValue);

        // restore original value
        setDBusValue(&watcher, "com.ubuntu.touch.AccountsService.Sound", "DialpadSoundsEnabled", originalValue);
        propertyChangeSpy.wait(400);
    }

};

QTEST_MAIN(tst_DBusPropertyWatcher)

#include "tst_dbuswatcher.moc"
