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
 * Author: Florian Boucault <florian.boucault@canonical.com>
 */

#define protected public
#include "ucalarm.h"
#include "ucalarm_p.h"
#include "alarmmanager_p.h"
#undef protected

#include <QtCore/QString>
#include <QtCore/QTextCodec>
#include <QtCore/QDebug>
#include <QtTest/QTest>
#include <QtTest/QSignalSpy>

class tst_UCAlarms : public QObject
{
    Q_OBJECT

public:
    tst_UCAlarms() {}

private:

    bool containsAlarm(UCAlarm *alarm)
    {
        UCAlarmPrivate *pAlarm = UCAlarmPrivate::get(alarm);
        QList<AlarmData> alarms = AlarmManager::instance().alarms();
        Q_FOREACH(AlarmData i, alarms) {
            if (i == pAlarm->rawData) {
                return true;
            }
        }
        return false;
    }

    bool compareAlarms(UCAlarm *alarm1, UCAlarm *alarm2)
    {
        UCAlarmPrivate *pAlarm1 = UCAlarmPrivate::get(alarm1);
        UCAlarmPrivate *pAlarm2 = UCAlarmPrivate::get(alarm2);
        return pAlarm1->rawData.compare(pAlarm2->rawData);
    }

private Q_SLOTS:

    void cleanupTestCase() {
        // remove all test alarms
        bool loop = true;
        while (loop) {
            loop = false;
            QList<AlarmData> alarms = AlarmManager::instance().alarms();
            Q_FOREACH(AlarmData alarm, alarms) {
                if (alarm.message.startsWith("test_")) {
                    AlarmManager::instance().cancel(alarm);
                    QTest::waitForEvents();
                    loop = true;
                    break;
                }
            }
        }
    }

    void initTestCase() {
        QTest::waitForEvents();
    }

    void test_singleShotAlarmXFail() {
        UCAlarm alarm;
        alarm.save();
        QVERIFY(alarm.error() == UCAlarm::EarlyDate);
    }

    void test_singleShotAlarmPass() {
        UCAlarm alarm(QDateTime::currentDateTime().addSecs(10), "test_singleShotAlarmPass");
        alarm.save();
        QVERIFY(alarm.error() == UCAlarm::NoError);
        QTest::waitForEvents();
        QVERIFY(containsAlarm(&alarm));
    }

    void test_repeating_autoDetect()
    {
        UCAlarm alarm(QDateTime::currentDateTime(), UCAlarm::AutoDetect, "test_repeating_autoDetect");

        alarm.save();
        QVERIFY(alarm.error() == UCAlarm::NoError);
        QTest::waitForEvents();
        QVERIFY(containsAlarm(&alarm));
    }

    void test_repeating_daily()
    {
        UCAlarm alarm(QDateTime::currentDateTime(), UCAlarm::Daily, "test_repeating_daily");

        alarm.save();
        QVERIFY(alarm.error() == UCAlarm::NoError);
        QTest::waitForEvents();
        QVERIFY(containsAlarm(&alarm));
    }

    void test_repeating_givenDay()
    {
        UCAlarm alarm(QDateTime::currentDateTime(), UCAlarm::Wednesday, "test_repeating_givenDay");

        alarm.save();
        QVERIFY(alarm.error() == UCAlarm::NoError);
        QTest::waitForEvents();
        QVERIFY(containsAlarm(&alarm));
    }

    void test_repeating_moreDays()
    {
        UCAlarm alarm(QDateTime::currentDateTime(), UCAlarm::Monday | UCAlarm::Wednesday, "test_repeating_moreDays");

        alarm.save();
        QVERIFY(alarm.error() == UCAlarm::NoError);
        QTest::waitForEvents();
        QVERIFY(containsAlarm(&alarm));
    }

    void test_setAlarmObjectFail_WrongRecurence1()
    {
        UCAlarm alarm(QDateTime::currentDateTime().addDays(1), UCAlarm::Monday | UCAlarm::Tuesday);
        alarm.setType(UCAlarm::OneTime);
        alarm.save();
        QVERIFY(alarm.error() == UCAlarm::OneTimeOnMoreDays);
    }

    void test_setAlarmObjectFail_WrongRecurence2()
    {
        UCAlarm alarm(QDateTime::currentDateTime().addDays(1), UCAlarm::Daily);
        alarm.setType(UCAlarm::OneTime);
        alarm.save();
        QVERIFY(alarm.error() == UCAlarm::OneTimeOnMoreDays);
    }

    void test_cancelFail()
    {
        UCAlarm alarm(QDateTime::currentDateTime().addDays(1), "test_cancelFail");

        alarm.cancel();
        QVERIFY(alarm.error() == UCAlarm::InvalidEvent);
    }

    void test_cancelPass()
    {
        UCAlarm alarm(QDateTime::currentDateTime().addDays(1), "test_cancelPass");

        alarm.save();
        QVERIFY(alarm.error() == UCAlarm::NoError);
        QTest::waitForEvents();
        QVERIFY(containsAlarm(&alarm));

        alarm.cancel();
        QVERIFY(alarm.error() == UCAlarm::NoError);
        QTest::waitForEvents();
        QVERIFY(!containsAlarm(&alarm));
    }

    void test_twoAlarmsOnSameTime()
    {
        QDateTime dt = QDateTime::currentDateTime().addSecs(60);
        UCAlarm alarm1(dt, "test_twoAlarmsOnSameTime");
        UCAlarm alarm2(dt, "test_twoAlarmsOnSameTime");

        alarm1.save();
        alarm2.save();
        QVERIFY(alarm1.error() == UCAlarm::NoError);
        QVERIFY(alarm2.error() == UCAlarm::NoError);
    }

    void test_twoAlarmsOnSameTime2()
    {
        QDateTime dt = QDateTime::currentDateTime().addSecs(60);
        UCAlarm alarm1(dt, "test_twoAlarmsOnSameTime2");
        UCAlarm alarm2(dt, UCAlarm::Daily, "test_twoAlarmsOnSameTime2");

        alarm1.save();
        alarm2.save();
        QVERIFY(alarm1.error() == UCAlarm::NoError);
        QVERIFY(alarm2.error() == UCAlarm::NoError);
    }

    void test_updateAlarm_SameType()
    {
        QDateTime dt = QDateTime::currentDateTime().addSecs(120);
        UCAlarm alarm(dt, "test_updateAlarm_SameType");
        UCAlarm copy(dt, "test_updateAlarm_SameType");

        alarm.save();
        QVERIFY(alarm.error() == UCAlarm::NoError);
        QTest::waitForEvents();
        QVERIFY(containsAlarm(&alarm));

        alarm.setDate(QDateTime::currentDateTime().addDays(10));
        QVERIFY(!compareAlarms(&alarm, &copy));

        alarm.save();
        QVERIFY(alarm.error() == UCAlarm::NoError);
        QTest::waitForEvents();
        QVERIFY(containsAlarm(&alarm));
        QVERIFY(!containsAlarm(&copy));
    }

    void test_updateAlarm_DifferentType()
    {
        QDateTime dt = QDateTime::currentDateTime().addSecs(120);
        UCAlarm alarm(dt, "test_updateAlarm_DifferentType");
        UCAlarm copy(dt, "test_updateAlarm_DifferentType");

        alarm.save();
        QVERIFY(alarm.error() == UCAlarm::NoError);
        QTest::waitForEvents();
        QVERIFY(containsAlarm(&alarm));

        alarm.setType(UCAlarm::Repeating);
        QVERIFY(!compareAlarms(&alarm, &copy));
        alarm.save();
        QVERIFY(alarm.error() == UCAlarm::NoError);
        QTest::waitForEvents();
        QVERIFY(containsAlarm(&alarm));
        QVERIFY(!containsAlarm(&copy));
    }

};

QTEST_MAIN(tst_UCAlarms)

#include "tst_alarms.moc"
