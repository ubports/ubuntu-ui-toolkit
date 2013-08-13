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
#include "ucalarmmanager.h"
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

    bool containsAlarm(const QList<UCAlarm*> &alarms, UCAlarm *alarm)
    {
        Q_FOREACH(UCAlarm *i, alarms) {
            if (*i == *alarm) {
                return true;
            }
        }
        return false;
    }

private Q_SLOTS:

    void cleanupTestCase() {
        // remove all test alarms
        UCAlarmManager alarms;
        bool loop = true;
        while (alarms.alarms().count() > 0 && loop) {
            loop = false;
            Q_FOREACH(UCAlarm *alarm, alarms.alarms()) {
                if (alarm->message().startsWith("test_")) {
                    alarms.cancel(alarm);
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
        UCAlarmManager alarms;
        QTest::waitForEvents();
        QVERIFY(!alarms.setOneTime(QDateTime::currentDateTime(), "test_"));
    }

    void test_singleShotAlarmPass() {
        UCAlarmManager alarms;
        QSignalSpy spy(&alarms, SIGNAL(alarmsChanged()));
        UCAlarm alarm(QDateTime::currentDateTime().addSecs(10), "test_singleShotAlarmPass");

        QVERIFY(alarms.setOneTime(alarm.date(), alarm.message()));
        QTest::waitForEvents();
        QCOMPARE(spy.count(), 1);
        QVERIFY(containsAlarm(alarms.alarms(), &alarm));
    }

    void test_repeating_autoDetect()
    {
        UCAlarmManager alarms;
        QSignalSpy spy(&alarms, SIGNAL(alarmsChanged()));
        UCAlarm alarm(QDateTime::currentDateTime(), UCAlarm::Repeating, UCAlarm::AutoDetect, "test_repeating_autoDetect");

        QVERIFY(alarms.setRepeating(alarm.date(), UCAlarm::AutoDetect, alarm.message()));
        QTest::waitForEvents();
        QCOMPARE(spy.count(), 1);
        QVERIFY(containsAlarm(alarms.alarms(), &alarm));
    }

    void test_repeating_daily()
    {
        UCAlarmManager alarms;
        QSignalSpy spy(&alarms, SIGNAL(alarmsChanged()));

        QVERIFY(alarms.setRepeating(QDateTime::currentDateTime(), UCAlarm::Daily, "test_repeating_daily"));
        QTest::waitForEvents();
        QCOMPARE(spy.count(), 1);
    }

    void test_repeating_givenDay()
    {
        UCAlarmManager alarms;
        QSignalSpy spy(&alarms, SIGNAL(alarmsChanged()));

        QVERIFY(alarms.setRepeating(QDateTime::currentDateTime(), UCAlarm::Wednesday, "test_repeating_givenDay"));
        QTest::waitForEvents();
        QCOMPARE(spy.count(), 1);
    }

    void test_repeating_moreDays()
    {
        UCAlarmManager alarms;
        QSignalSpy spy(&alarms, SIGNAL(alarmsChanged()));

        QVERIFY(alarms.setRepeating(QDateTime::currentDateTime(), UCAlarm::Monday | UCAlarm::Wednesday, "test_repeating_moreDays"));
        QTest::waitForEvents();
        QCOMPARE(spy.count(), 1);
    }


    void test_setOneTimeAlarmObject1()
    {
        UCAlarmManager alarms;
        UCAlarm alarm;

        alarm.setMessage("test_setSingleShotAlarmObject1");
        alarm.setDate(alarm.date().addSecs(60));

        QVERIFY(alarms.set(&alarm));
        QTest::waitForEvents();
        QVERIFY(containsAlarm(alarms.alarms(), &alarm));
    }

    void test_setOneTimeAlarmObject2()
    {
        UCAlarmManager alarms;
        UCAlarm alarm(QDateTime::currentDateTime().addSecs(60), "test_setSingleShotAlarmObject2");

        QVERIFY(alarms.set(&alarm));
        QTest::waitForEvents();
        QVERIFY(containsAlarm(alarms.alarms(), &alarm));
    }

    void test_setAlarmObjectFail_WrongDate()
    {
        UCAlarmManager alarms;
        UCAlarm alarm;

        QVERIFY(!alarms.set(&alarm));
    }

    void test_setAlarmObjectFail_WrongRecurence1()
    {
        UCAlarmManager alarms;
        UCAlarm alarm(QDateTime::currentDateTime().addDays(1), UCAlarm::OneTime, UCAlarm::Monday | UCAlarm::Tuesday);
        QVERIFY(!alarms.set(&alarm));
    }

    void test_setAlarmObjectFail_WrongRecurence2()
    {
        UCAlarmManager alarms;
        UCAlarm alarm(QDateTime::currentDateTime().addDays(1), UCAlarm::OneTime, UCAlarm::Daily);
        QVERIFY(!alarms.set(&alarm));
    }

    void test_setAlarmObjectDaily() {
        UCAlarmManager alarms;
        UCAlarm alarm(QDateTime::currentDateTime(), UCAlarm::Repeating, UCAlarm::Daily, "test_setAlarmObjectDaily");
        QVERIFY(alarms.set(&alarm));
        QTest::waitForEvents();
        QVERIFY(containsAlarm(alarms.alarms(), &alarm));
    }

    void test_setAlarmObjectWeeklyOnGivenDay() {
        UCAlarmManager alarms;
        UCAlarm alarm(QDateTime::currentDateTime(), UCAlarm::Repeating, UCAlarm::Monday, "test_setAlarmObjectWeeklyOnGivenDay");
        QVERIFY(alarms.set(&alarm));
        QTest::waitForEvents();
        QVERIFY(containsAlarm(alarms.alarms(), &alarm));
    }

    void test_setAlarmObjectWeeklyOnSpecificDays() {
        UCAlarmManager alarms;
        UCAlarm alarm(QDateTime::currentDateTime(), UCAlarm::Repeating, UCAlarm::Monday|UCAlarm::Wednesday, "test_setAlarmObjectWeeklyOnSpecificDays");
        QVERIFY(alarms.set(&alarm));
        QTest::waitForEvents();
        QVERIFY(containsAlarm(alarms.alarms(), &alarm));
    }

    void test_cancelFail()
    {
        UCAlarmManager alarms;
        UCAlarm alarm(QDateTime::currentDateTime().addDays(1), "test_cancelFail");

        QVERIFY(!alarms.cancel(&alarm));
    }

    void test_cancelPass()
    {
        UCAlarmManager alarms;
        UCAlarm alarm(QDateTime::currentDateTime().addDays(1), "test_cancelPass");

        QVERIFY(alarms.set(&alarm));
        QTest::waitForEvents();
        QVERIFY(containsAlarm(alarms.alarms(), &alarm));

        QVERIFY(alarms.cancel(&alarm));
        QTest::waitForEvents();
        QVERIFY(!containsAlarm(alarms.alarms(), &alarm));
    }

    void test_twoAlarmsOnSameTime()
    {
        UCAlarmManager alarms;
        QDateTime dt = QDateTime::currentDateTime().addSecs(60);

        QVERIFY(alarms.setOneTime(dt, "test_twoAlarmsOnSameTime"));
        QVERIFY(alarms.setOneTime(dt, "test_twoAlarmsOnSameTime"));
    }

    void test_twoAlarmsOnSameTime2()
    {
        UCAlarmManager alarms;
        QDateTime dt = QDateTime::currentDateTime().addSecs(60);

        QVERIFY(alarms.setOneTime(dt, "test_twoAlarmsOnSameTime2"));
        QVERIFY(alarms.setRepeating(dt, UCAlarm::Daily, "test_twoAlarmsOnSameTime2"));
    }

    void test_updateAlarm_SameType()
    {
        UCAlarmManager alarms;
        UCAlarm alarm(QDateTime::currentDateTime().addSecs(120), "test_updateAlarm_SameType");

        QVERIFY(alarms.set(&alarm));
        QTest::waitForEvents();
        QVERIFY(containsAlarm(alarms.alarms(), &alarm));

        UCAlarm copy(alarm);

        alarm.setDate(QDateTime::currentDateTime().addDays(10));
        QVERIFY(!(alarm == copy));
        QVERIFY(alarms.set(&alarm));
        QTest::waitForEvents();
        QVERIFY(containsAlarm(alarms.alarms(), &alarm));
        QVERIFY(!containsAlarm(alarms.alarms(), &copy));
    }

    void test_updateAlarm_DifferentType()
    {
        UCAlarmManager alarms;
        UCAlarm alarm(QDateTime::currentDateTime().addSecs(120), "test_updateAlarm_SameType");

        QVERIFY(alarms.set(&alarm));
        QTest::waitForEvents();
        QVERIFY(containsAlarm(alarms.alarms(), &alarm));

        UCAlarm copy(alarm);

        alarm.setType(UCAlarm::Repeating);
        QVERIFY(!(alarm == copy));
        QVERIFY(alarms.set(&alarm));
        QTest::waitForEvents();
        QVERIFY(containsAlarm(alarms.alarms(), &alarm));
        QVERIFY(!containsAlarm(alarms.alarms(), &copy));
    }

};

QTEST_MAIN(tst_UCAlarms)

#include "tst_alarms.moc"
