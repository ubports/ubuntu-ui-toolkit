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
#include "systemsettings_p.h"
#include "systemsettings_p_p.h"
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>
#include <QtCore/QString>
#include <QtCore/QDebug>
#include <QtTest/QTest>
#include <QtTest/QSignalSpy>

class tst_SystemSettings : public QObject
{
    Q_OBJECT

public:
    tst_SystemSettings() {}

private Q_SLOTS:

    void initTestCase()
    {
        SystemSettings::instance();
    }

    void test_vibraEnabled_on_if_no_systemsetting_defined()
    {
        SystemSettingsPrivate *pSettings = SystemSettingsPrivate::get(&SystemSettings::instance());
        if (!pSettings->hasProperty("OtherVibrate")) {
            // verify that the vibration is turned on
            QVERIFY(SystemSettings::instance().vibraEnabled());
        }
    }
    void test_vibraEnabled_change()
    {
        SystemSettingsPrivate *pSettings = SystemSettingsPrivate::get(&SystemSettings::instance());
        if (!pSettings->hasProperty("OtherVibrate")) {
            QSKIP("OtherVibrate system property not defined, test skipped.");
        }

        QSignalSpy vibraSpy(&SystemSettings::instance(), SIGNAL(vibraEnabledChanged()));
        bool oldValue = SystemSettings::instance().vibraEnabled();
        QVERIFY(pSettings->testProperty("OtherVibrate", !oldValue));
        vibraSpy.wait(400);
        QCOMPARE(vibraSpy.count(), 1);

        //restore original value
        vibraSpy.clear();
        pSettings->testProperty("OtherVibrate", oldValue);
        vibraSpy.wait(400);
    }

};

QTEST_MAIN(tst_SystemSettings)

#include "tst_systemsettings.moc"
