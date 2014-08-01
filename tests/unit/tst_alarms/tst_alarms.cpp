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
#include "ucalarmmodel.h"
#include "alarmrequest_p.h"
#include "adapters/alarmsadapter_p.h"
#undef protected

#include "uctestcase.h"
#include <QtCore/QString>
#include <QtCore/QTextCodec>
#include <QtCore/QDebug>
#include <QtTest/QTest>
#include <QtTest/QSignalSpy>
#include <QtCore/QTimeZone>

class tst_UCAlarms : public QObject
{
    Q_OBJECT

public:
    tst_UCAlarms() {}

private:

    void syncFetch()
    {
        // initiate fetch
        AlarmsAdapter *adapter = AlarmsAdapter::get();
        if (!adapter->fetchRequest) {
            adapter->fetchAlarms();
        }
        if (adapter->fetchRequest) {
            adapter->fetchRequest->wait();
        }
        QTest::waitForEvents();
    }

    void waitForRequest(UCAlarm *alarm)
    {
        UCAlarmPrivate *pAlarm = UCAlarmPrivate::get(alarm);
        if (pAlarm->request) {
            pAlarm->request->wait();
        }
        QTest::waitForEvents();
        // also complete any pending fetch!
        syncFetch();
    }

    bool containsAlarm(UCAlarm *alarm, bool trace = false)
    {
        UCAlarmPrivate *pAlarm = UCAlarmPrivate::get(alarm);
        QList<AlarmData> alarms = AlarmManager::instance().alarms();
        Q_FOREACH(AlarmData i, alarms) {
            if (trace && (alarm->message() == i.message)) {
                qDebug() << "----------------------";
                qDebug() << "Alarm data:" << alarm->message();
                qDebug() << alarm->date() << i.date;
                qDebug() << alarm->type() << i.type;
                qDebug() << alarm->daysOfWeek() << i.days;
            }
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

    void initTestCase()
    {
        AlarmManager::instance();
        // make sure the first fetch is completed
        AlarmsAdapter *adapter = AlarmsAdapter::get();
        if (adapter->fetchRequest) {
            adapter->fetchRequest->wait();
        }
    }

    void cleanupTestCase() {
        // remove all test alarms
        UCAlarmModel model;
        int i = 0;
        while (i < model.count()) {
            UCAlarm *alarm = model.get(i);
            if (alarm && alarm->message().startsWith("test_")) {
                alarm->cancel();
                waitForRequest(alarm);
                i = 0;
            } else {
                i++;
            }
        }
    }

    void test_singleShotAlarmXFail() {
        UCAlarm alarm;
        alarm.save();
        QVERIFY(alarm.error() == UCAlarm::EarlyDate);
    }

    void test_singleShotAlarmPass() {
        UCAlarm alarm(QDateTime::currentDateTime().addSecs(4), "test_singleShotAlarmPass");
        alarm.save();
        waitForRequest(&alarm);
        QCOMPARE(alarm.error(), (int)UCAlarm::NoError);
        QVERIFY(containsAlarm(&alarm));
    }

    void test_repeating_autoDetect()
    {
        UCAlarm alarm(QDateTime::currentDateTime().addSecs(20), UCAlarm::AutoDetect, "test_repeating_autoDetect");

        alarm.save();
        waitForRequest(&alarm);
        QCOMPARE(alarm.error(), (int)UCAlarm::NoError);
        QVERIFY(containsAlarm(&alarm));
    }

    void test_repeating_daily()
    {
        UCAlarm alarm(QDateTime::currentDateTime().addSecs(10), UCAlarm::Daily, "test_repeating_daily");

        alarm.save();
        waitForRequest(&alarm);
        QCOMPARE(alarm.error(), (int)UCAlarm::NoError);
        QVERIFY(containsAlarm(&alarm));
    }

    void test_repeating_givenDay_exact_data()
    {
        QTest::addColumn<QString>("message");
        QTest::addColumn<int>("day");

        QTest::newRow("Monday") << "Monday" << (int)UCAlarm::Monday;
        QTest::newRow("Tuesday") << "Tuesday" << (int)UCAlarm::Tuesday;
        QTest::newRow("Wednesday") << "Wednesday" << (int)UCAlarm::Wednesday;
        QTest::newRow("Thursday") << "Thursday" << (int)UCAlarm::Thursday;
        QTest::newRow("Friday") << "Friday" << (int)UCAlarm::Friday;
        QTest::newRow("Saturday") << "Saturday" << (int)UCAlarm::Saturday;
        QTest::newRow("Sunday") << "Sunday" << (int)UCAlarm::Sunday;
    }

    void test_repeating_givenDay_exact()
    {
        QFETCH(QString, message);
        QFETCH(int, day);
        UCAlarm alarm(QDateTime::currentDateTime(), (UCAlarm::DaysOfWeek)day, "test_repeating_givenDay_exact_" + message);

        alarm.save();
        waitForRequest(&alarm);
        QCOMPARE(alarm.error(), (int)UCAlarm::NoError);
        QVERIFY(containsAlarm(&alarm));
    }

    void test_repeating_moreDays_data()
    {
        QTest::addColumn<QString>("message");
        QTest::addColumn<int>("days");

        QTest::newRow("First on Monday") << "Monday" << (int)(UCAlarm::Monday | UCAlarm::Sunday);
        QTest::newRow("First on Tuesday") << "Tuesday" << (int)(UCAlarm::Tuesday | UCAlarm::Monday);
        QTest::newRow("First on Wednesday") << "Wednesday" << (int)(UCAlarm::Wednesday | UCAlarm::Tuesday);
        QTest::newRow("First on Thursday") << "Thursday" << (int)(UCAlarm::Thursday | UCAlarm::Wednesday);
        QTest::newRow("First on Friday") << "Friday" << (int)(UCAlarm::Friday | UCAlarm::Thursday);
        QTest::newRow("First on Saturday") << "Saturday" << (int)(UCAlarm::Saturday | UCAlarm::Friday);
        QTest::newRow("First on Sunday") << "Sunday" << (int)(UCAlarm::Sunday | UCAlarm::Saturday);
    }

    void test_repeating_moreDays()
    {
        QFETCH(QString, message);
        QFETCH(int, days);

        QDateTime currentDate(QDateTime::currentDateTime());

        UCAlarm alarm(currentDate, (UCAlarm::DaysOfWeek)days, "test_repeating_moreDays" + message);
        // the distance is always 6 days between the occurrences
        UCAlarm firstOccurrence(currentDate, alarm.daysOfWeek(), alarm.message());
        // make sure we have the same setup as the original one; checkAlarm() adjusts all alarm data
        UCAlarmPrivate::get(&firstOccurrence)->checkAlarm();

        alarm.save();
        waitForRequest(&alarm);
        QCOMPARE(alarm.error(), (int)UCAlarm::NoError);
        QVERIFY(containsAlarm(&alarm));
        QVERIFY(containsAlarm(&firstOccurrence));
    }

    void test_repeating_weekly_data() {
        QTest::addColumn<QString>("message");
        QTest::addColumn<int>("dow");

        QTest::newRow("Monday") << "Monday" << (int)UCAlarm::Monday;
        QTest::newRow("Tuesday") << "Tuesday" << (int)UCAlarm::Tuesday;
        QTest::newRow("Wednesday") << "Wednesday" << (int)UCAlarm::Wednesday;
        QTest::newRow("Thursday") << "Thursday" << (int)UCAlarm::Thursday;
        QTest::newRow("Friday") << "Friday" << (int)UCAlarm::Friday;
        QTest::newRow("Saturday") << "Saturday" << (int)UCAlarm::Saturday;
        QTest::newRow("Sunday") << "Sunday" << (int)UCAlarm::Sunday;
    }
    void test_repeating_weekly()
    {
        QFETCH(QString, message);
        QFETCH(int, dow);

        UCAlarm alarm(QDateTime::currentDateTime().addSecs(3600), (UCAlarm::DaysOfWeek)dow, "test_repeating_weekly_" + message);
        alarm.save();
        waitForRequest(&alarm);
        QCOMPARE(alarm.error(), (int)UCAlarm::NoError);
        QVERIFY(containsAlarm(&alarm));
    }

    void test_setAlarmObjectFail_WrongRecurence1()
    {
        UCAlarm alarm(QDateTime::currentDateTime().addDays(1), UCAlarm::Monday | UCAlarm::Tuesday);
        alarm.setType(UCAlarm::OneTime);
        alarm.save();
        QCOMPARE(alarm.error(), (int)UCAlarm::OneTimeOnMoreDays);
    }

    void test_setAlarmObjectFail_WrongRecurence2()
    {
        UCAlarm alarm(QDateTime::currentDateTime().addDays(1), UCAlarm::Daily);
        alarm.setType(UCAlarm::OneTime);
        alarm.save();
        QCOMPARE(alarm.error(), (int)UCAlarm::OneTimeOnMoreDays);
    }

    void test_cancelFail()
    {
        UCAlarm alarm(QDateTime::currentDateTime().addDays(1), "test_cancelFail");

        alarm.cancel();
        QCOMPARE(alarm.error(), (int)UCAlarm::InvalidEvent);
    }

    void test_cancelPass()
    {
        UCAlarm alarm(QDateTime::currentDateTime().addDays(1), "test_cancelPass");

        alarm.save();
        waitForRequest(&alarm);
        QCOMPARE(alarm.error(), (int)UCAlarm::NoError);
        QVERIFY(containsAlarm(&alarm));

        alarm.cancel();
        waitForRequest(&alarm);
        QCOMPARE(alarm.error(), (int)UCAlarm::NoError);
        QVERIFY(!containsAlarm(&alarm));
    }

    void test_twoAlarmsOnSameTime()
    {
        QDateTime dt = QDateTime::currentDateTime().addSecs(60);
        UCAlarm alarm1(dt, "test_twoAlarmsOnSameTime");
        UCAlarm alarm2(dt, "test_twoAlarmsOnSameTime");

        alarm1.save();
        waitForRequest(&alarm1);
        alarm2.save();
        waitForRequest(&alarm2);
        QCOMPARE(alarm1.error(), (int)UCAlarm::NoError);
        QCOMPARE(alarm2.error(), (int)UCAlarm::NoError);
    }

    void test_twoAlarmsOnSameTime2()
    {
        QDateTime dt = QDateTime::currentDateTime().addSecs(60);
        UCAlarm alarm1(dt, "test_twoAlarmsOnSameTime2");
        UCAlarm alarm2(dt, UCAlarm::Daily, "test_twoAlarmsOnSameTime2");

        alarm1.save();
        waitForRequest(&alarm1);
        alarm2.save();
        waitForRequest(&alarm2);
        QCOMPARE(alarm1.error(), (int)UCAlarm::NoError);
        QCOMPARE(alarm2.error(), (int)UCAlarm::NoError);
    }

    void test_updateAlarm_SameType()
    {
        QDateTime dt = QDateTime::currentDateTime().addSecs(120);
        UCAlarm alarm(dt, "test_updateAlarm_SameType");
        UCAlarm copy(dt, "test_updateAlarm_SameType");

        alarm.save();
        waitForRequest(&alarm);
        QCOMPARE(alarm.error(), (int)UCAlarm::NoError);
        QVERIFY(containsAlarm(&alarm));

        alarm.setDate(QDateTime::currentDateTime().addDays(5));
        QVERIFY(!compareAlarms(&alarm, &copy));

        alarm.save();
        waitForRequest(&alarm);
        QCOMPARE(alarm.error(), (int)UCAlarm::NoError);
        QVERIFY(containsAlarm(&alarm));
        QVERIFY(!containsAlarm(&copy));
    }

    void test_updateAlarm_DifferentType()
    {
        QDateTime dt = QDateTime::currentDateTime().addSecs(120);
        UCAlarm alarm(dt, "test_updateAlarm_DifferentType");
        UCAlarm copy(dt, "test_updateAlarm_DifferentType");

        alarm.save();
        waitForRequest(&alarm);
        QCOMPARE(alarm.error(), (int)UCAlarm::NoError);
        QVERIFY(containsAlarm(&alarm));

        alarm.setType(UCAlarm::Repeating);
        QVERIFY(!compareAlarms(&alarm, &copy));
        alarm.save();
        waitForRequest(&alarm);
        QCOMPARE(alarm.error(), (int)UCAlarm::NoError);
        QVERIFY(containsAlarm(&alarm));
        QVERIFY(!containsAlarm(&copy));
    }

    void test_updateAlarm_Repeating()
    {
        UCAlarm alarm(QDateTime::currentDateTime().addMSecs(5000), UCAlarm::AutoDetect, "test_updateAlarm_Repeating");

        alarm.save();
        waitForRequest(&alarm);
        QCOMPARE(alarm.error(), (int)UCAlarm::NoError);
        QSKIP("https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1322558");
        QVERIFY(containsAlarm(&alarm));

        alarm.setDate(alarm.date().addDays(1));
        alarm.save();
        waitForRequest(&alarm);
        QCOMPARE(alarm.error(), (int)UCAlarm::NoError);
        QVERIFY(containsAlarm(&alarm));

        alarm.setDaysOfWeek(UCAlarm::Daily);
        alarm.save();
        waitForRequest(&alarm);
        QCOMPARE(alarm.error(), (int)UCAlarm::NoError);
        QVERIFY(containsAlarm(&alarm));
    }

    void test_fetchAlarmPlus7Days()
    {
        QDateTime dt = QDateTime::currentDateTime().addDays(10);
        UCAlarm alarm(dt, "test_fetchAlarmPlus7Days");

        alarm.save();
        waitForRequest(&alarm);
        QCOMPARE(alarm.error(), (int)UCAlarm::NoError);
        QVERIFY(containsAlarm(&alarm));

        UCAlarm nextMonth(dt.addMonths(1), "test_fetchAlarmPlus1Month");
        nextMonth.save();
        waitForRequest(&nextMonth);
        QCOMPARE(nextMonth.error(), (int)UCAlarm::NoError);
        QVERIFY(containsAlarm(&nextMonth));

        UCAlarm nextYear(dt.addYears(1), "test_fetchAlarmPlus1Year");
        nextYear.save();
        waitForRequest(&nextYear);
        QCOMPARE(nextYear.error(), (int)UCAlarm::NoError);
        QVERIFY(containsAlarm(&nextYear));
    }

    void test_correctAlarmOrderDaily()
    {
        QDate cd = QDate::currentDate();
        QTime ct = QTime::currentTime();
        QDateTime dt(cd, QTime(ct.hour(), ct.minute(), ct.second() + 2));
        QDateTime nextDt(cd.addDays(1), QTime(ct.hour(), ct.minute(), ct.second() + 2));
        UCAlarm alarm(dt, UCAlarm::Daily, "test_correctAlarmOrderDaily");
        UCAlarm nextAlarm(nextDt, UCAlarm::Daily, "test_correctAlarmOrderDaily");

        alarm.save();
        waitForRequest(&alarm);
        QTest::qWait(2000);
        syncFetch();
        QCOMPARE(alarm.error(), (int)UCAlarm::NoError);
        QVERIFY(containsAlarm(&nextAlarm));
    }

    void test_correctAlarmOrderWeekly()
    {
        QDate cd = QDate::currentDate();
        QTime ct = QTime::currentTime();
        QDateTime dt(cd, QTime(ct.hour(), ct.minute(), ct.second() + 1));
        QDateTime nextDt(cd.addDays(7), QTime(ct.hour(), ct.minute(), ct.second() + 1));
        UCAlarm alarm(dt, UCAlarm::AutoDetect, "test_correctAlarmOrderWeekly");
        UCAlarm nextAlarm(nextDt, UCAlarm::AutoDetect, "test_correctAlarmOrderWeekly");

        alarm.save();
        waitForRequest(&alarm);
        QTest::qWait(2000);
        syncFetch();
        QCOMPARE(alarm.error(), (int)UCAlarm::NoError);
        QVERIFY(containsAlarm(&nextAlarm));
    }

    void test_transcodeDate_data()
    {
        QTest::addColumn<QDateTime>("date");
        QTest::addColumn<int>("srcSpec");
        QTest::addColumn<int>("dstSpec");
        QTest::addColumn<bool>("xfail");

        QDateTime now = QDateTime::currentDateTime();
        QTest::newRow("Local to Local") << now << (int)Qt::LocalTime << (int)Qt::LocalTime << true;
        QTest::newRow("Local to UTC") << now << (int)Qt::LocalTime << (int)Qt::UTC << false;
        QTest::newRow("UTC to UTC") << now << (int)Qt::UTC << (int)Qt::UTC << true;
        QTest::newRow("UTC to Local") << now << (int)Qt::UTC << (int)Qt::LocalTime << false;
    }

    void test_transcodeDate()
    {
        QFETCH(QDateTime, date);
        QFETCH(int, srcSpec);
        QFETCH(int, dstSpec);
        QFETCH(bool, xfail);

        QDateTime srcDate = AlarmData::normalizeDate(QDateTime(date.date(), date.time(), (Qt::TimeSpec)srcSpec));
        QDateTime dstDate(AlarmData::transcodeDate(srcDate, (Qt::TimeSpec)dstSpec));
        QCOMPARE(srcDate.date(), dstDate.date());
        QCOMPARE(srcDate.time(), dstDate.time());
        if (xfail) {
            QEXPECT_FAIL("", "Similar timespec set, expect fail", Continue);
        }
        QVERIFY(srcDate.timeZone() != dstDate.timeZone());
    }

    void test_oneTime_dow_data() {
        QTest::addColumn<QString>("message");
        QTest::addColumn<int>("dow");

        QTest::newRow("Monday") << "Monday" << (int)UCAlarm::Monday;
        QTest::newRow("Tuesday") << "Tuesday" << (int)UCAlarm::Tuesday;
        QTest::newRow("Wednesday") << "Wednesday" << (int)UCAlarm::Wednesday;
        QTest::newRow("Thursday") << "Thursday" << (int)UCAlarm::Thursday;
        QTest::newRow("Friday") << "Friday" << (int)UCAlarm::Friday;
        QTest::newRow("Saturday") << "Saturday" << (int)UCAlarm::Saturday;
        QTest::newRow("Sunday") << "Sunday" << (int)UCAlarm::Sunday;
    }
    void test_oneTime_dow() {
        QFETCH(QString, message);
        QFETCH(int, dow);

        UCAlarm alarm(QDateTime::currentDateTime().addSecs(3600), "test_oneTime_dow_" + message);
        alarm.setDaysOfWeek((UCAlarm::DaysOfWeek)dow);
        alarm.save();
        waitForRequest(&alarm);
        QCOMPARE(alarm.error(), (int)UCAlarm::NoError);
        QVERIFY(containsAlarm(&alarm));
    }

    void test_oneTime_setDate_does_not_change_dow_data() {
        QTest::addColumn<QString>("message");
        QTest::addColumn<int>("dow");

        QTest::newRow("Monday") << "Monday" << (int)UCAlarm::Monday;
        QTest::newRow("Tuesday") << "Tuesday" << (int)UCAlarm::Tuesday;
        QTest::newRow("Wednesday") << "Wednesday" << (int)UCAlarm::Wednesday;
        QTest::newRow("Thursday") << "Thursday" << (int)UCAlarm::Thursday;
        QTest::newRow("Friday") << "Friday" << (int)UCAlarm::Friday;
        QTest::newRow("Saturday") << "Saturday" << (int)UCAlarm::Saturday;
        QTest::newRow("Sunday") << "Sunday" << (int)UCAlarm::Sunday;
    }
    void test_oneTime_setDate_does_not_change_dow() {
        QFETCH(QString, message);
        QFETCH(int, dow);

        UCAlarm alarm(QDateTime::currentDateTime(), "test_oneTime_setDate_does_not_change_dow_" + message);
        alarm.setDaysOfWeek((UCAlarm::DaysOfWeek)dow);
        alarm.setDate(QDateTime::currentDateTime().addSecs(3600));
        QCOMPARE((int)alarm.daysOfWeek(), dow);
    }
};

QTEST_MAIN(tst_UCAlarms)

#include "tst_alarms.moc"
