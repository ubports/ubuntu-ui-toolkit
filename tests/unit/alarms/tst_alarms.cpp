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

#include "ucalarm.h"
#include "private/ucalarm_p.h"
#include "private/alarmmanager_p.h"
#include "ucalarmmodel.h"
#include "private/alarmsadapter_p.h"

#include "uctestcase.h"
#include <QtCore/QString>
#include <QtCore/QTextCodec>
#include <QtCore/QDebug>
#include <QtTest/QTest>
#include <QtTest/QSignalSpy>
#include <QtCore/QTimeZone>
#include <QtQml/QQmlEngine>
#include <ubuntutoolkitmodule.h>

UT_USE_NAMESPACE

class tst_UCAlarms : public QObject
{
    Q_OBJECT

public:
    tst_UCAlarms() {}

Q_SIGNALS:
    void alarmUpdated();

private:

    QSignalSpy *insertSpy;
    QSignalSpy *updateSpy;
    QSignalSpy *cancelSpy;
    QQmlEngine *engine;

    void syncFetch()
    {
        // initiate fetch
        QSignalSpy spy(&AlarmManager::instance(), SIGNAL(alarmsRefreshed()));
        AlarmManager::instance().fetchAlarms();
        spy.wait(1000);
    }

    // the function should be used when a new alarm event is added
    // which causes an insert operation.
    void waitForInsert()
    {
        if (!insertSpy->count()) {
            insertSpy->wait(1000);
        }
        insertSpy->clear();
    }

    // the function should be used when an existing alarm event is updated
    // which causes an alarmUpdated() signal to be triggered
    void waitForUpdate()
    {
        if (!updateSpy->count()) {
            updateSpy->wait(1000);
        }
        updateSpy->clear();
    }

    // the function should be used when an alarm event is removed
    // which causes an alarmRemoveStarted()/alarmRemoveFinished() signal pair
    // to be triggered
    void waitForRemove()
    {
        if (!cancelSpy->count()) {
            cancelSpy->wait(1000);
        }
        cancelSpy->clear();
    }

    bool containsAlarm(UCAlarm *alarm, bool trace = false)
    {
        for (int i = 0; i < AlarmManager::instance().alarmCount(); i++) {
            const UCAlarm *element = AlarmManager::instance().alarmAt(i);
            if (trace && (alarm->message() == element->message())) {
                qDebug() << "----------------------";
                qDebug() << "Alarm data:" << alarm->message();
                qDebug() << alarm->date() << element->date();
                qDebug() << alarm->type() << element->type();
                qDebug() << alarm->daysOfWeek() << element->daysOfWeek();
            }
            if (*element == *alarm) {
                return true;
            }
        }
        return false;
    }

private Q_SLOTS:

    void initTestCase()
    {
        engine = new QQmlEngine;
        UbuntuToolkitModule::initializeContextProperties(engine);

        AlarmManager::instance();

        // connect alarmUpdated() and alarmMoveFinished to the test signal so we get either of those on alarm updates

        connect(&AlarmManager::instance(), SIGNAL(alarmUpdated(int)), this, SIGNAL(alarmUpdated()));
        connect(&AlarmManager::instance(), SIGNAL(alarmMoveFinished()), this, SIGNAL(alarmUpdated()));

        insertSpy = new QSignalSpy(&AlarmManager::instance(), SIGNAL(alarmInsertFinished()));
        updateSpy = new QSignalSpy(this, SIGNAL(alarmUpdated()));
        cancelSpy = new QSignalSpy(&AlarmManager::instance(), SIGNAL(alarmRemoveFinished()));
    }

    void cleanupTestCase() {
        // remove all test alarms
        // model may have an outstanding fetch, so wait for its completion!
        QSignalSpy spy(&AlarmManager::instance(), SIGNAL(alarmsRefreshed()));
        UCAlarmModel model;
        model.componentComplete();
        spy.wait();
        int i = 0;
        while (i < model.count()) {
            UCAlarm *alarm = model.get(i);
            if (alarm && alarm->message().startsWith("test_")) {
                alarm->cancel();
                waitForRemove();
                i = 0;
            } else {
                i++;
            }
        }

        delete cancelSpy;
        delete updateSpy;
        delete insertSpy;
        delete engine;
    }

    void cleanup()
    {
        // clear each spy count
        insertSpy->clear();
        updateSpy->clear();
        cancelSpy->clear();
    }

    void test_singleShotAlarmXFail() {
        UCAlarm alarm;
        alarm.save();
        QVERIFY(alarm.error() == UCAlarm::EarlyDate);
    }

    void test_singleShotAlarmPass() {
        UCAlarm alarm(QDateTime::currentDateTime().addSecs(4), "test_singleShotAlarmPass");
        alarm.save();
        waitForInsert();
        QCOMPARE(alarm.error(), (int)UCAlarm::NoError);
        QVERIFY(containsAlarm(&alarm));
    }

    void test_repeating_autoDetect()
    {
        UCAlarm alarm(QDateTime::currentDateTime().addSecs(20), UCAlarm::AutoDetect, "test_repeating_autoDetect");

        alarm.save();
        waitForInsert();
        QCOMPARE(alarm.error(), (int)UCAlarm::NoError);
        QVERIFY(containsAlarm(&alarm));
    }

    void test_repeating_daily()
    {
        UCAlarm alarm(QDateTime::currentDateTime().addSecs(10), UCAlarm::Daily, "test_repeating_daily");

        alarm.save();
        waitForInsert();
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
        waitForInsert();
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
        waitForInsert();
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
        waitForInsert();
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
        waitForInsert();
        QCOMPARE(alarm.error(), (int)UCAlarm::NoError);
        QVERIFY(containsAlarm(&alarm));

        alarm.cancel();
        waitForRemove();
        QCOMPARE(alarm.error(), (int)UCAlarm::NoError);
        QVERIFY(!containsAlarm(&alarm));
    }

    void test_twoAlarmsOnSameTime()
    {
        QDateTime dt = QDateTime::currentDateTime().addSecs(60);
        UCAlarm alarm1(dt, "test_twoAlarmsOnSameTime");
        UCAlarm alarm2(dt, "test_twoAlarmsOnSameTime");

        alarm1.save();
        waitForInsert();
        alarm2.save();
        waitForInsert();
        QCOMPARE(alarm1.error(), (int)UCAlarm::NoError);
        QCOMPARE(alarm2.error(), (int)UCAlarm::NoError);
        QVERIFY(containsAlarm(&alarm1));
        QVERIFY(containsAlarm(&alarm2));
    }

    void test_twoAlarmsOnSameTime2()
    {
        QDateTime dt = QDateTime::currentDateTime().addSecs(60);
        UCAlarm alarm1(dt, "test_twoAlarmsOnSameTime2");
        UCAlarm alarm2(dt, UCAlarm::Daily, "test_twoAlarmsOnSameTime2");

        alarm1.save();
        waitForInsert();
        alarm2.save();
        waitForInsert();
        QCOMPARE(alarm1.error(), (int)UCAlarm::NoError);
        QCOMPARE(alarm2.error(), (int)UCAlarm::NoError);
        QVERIFY(containsAlarm(&alarm2));
        QVERIFY(containsAlarm(&alarm1));
    }

    void test_updateAlarm_SameType()
    {
        QDateTime dt = QDateTime::currentDateTime().addSecs(120);
        UCAlarm alarm(dt, "test_updateAlarm_SameType");
        UCAlarm copy(dt, "test_updateAlarm_SameType");

        alarm.save();
        waitForInsert();
        QCOMPARE(alarm.error(), (int)UCAlarm::NoError);
        QVERIFY(containsAlarm(&alarm));

        alarm.setDate(QDateTime::currentDateTime().addDays(5));
        QVERIFY(!(alarm == copy));

        alarm.save();
        waitForUpdate();
        QCOMPARE(alarm.error(), (int)UCAlarm::NoError);
        QVERIFY(containsAlarm(&alarm));
        QVERIFY(!containsAlarm(&copy));
    }

    void test_updateAlarm_DifferentType()
    {
        QDateTime dt = QDateTime::currentDateTime().addSecs(120);
        UCAlarm alarm(dt, "test_updateAlarm_DifferentType");

        alarm.save();
        waitForInsert();
        QCOMPARE(alarm.error(), (int)UCAlarm::NoError);
        QVERIFY(containsAlarm(&alarm));

        UCAlarm copy(dt, "test_updateAlarm_DifferentType");
        alarm.setType(UCAlarm::Repeating);
        QVERIFY(!(alarm == copy));
        alarm.save();
        // do not fetch
        waitForUpdate();
        QCOMPARE(alarm.error(), (int)UCAlarm::NoError);
        QVERIFY(containsAlarm(&alarm));
        QVERIFY(!containsAlarm(&copy));
    }

    void test_updateAlarm_Repeating()
    {
        UCAlarm alarm(QDateTime::currentDateTime().addMSecs(5000), UCAlarm::AutoDetect, "test_updateAlarm_Repeating");

        alarm.save();
        waitForInsert();
        QCOMPARE(alarm.error(), (int)UCAlarm::NoError);
        QVERIFY(containsAlarm(&alarm));

        alarm.setDate(alarm.date().addDays(1));
        alarm.save();
        waitForUpdate();
        QCOMPARE(alarm.error(), (int)UCAlarm::NoError);
        QVERIFY(containsAlarm(&alarm));

        alarm.setDaysOfWeek(UCAlarm::Daily);
        alarm.save();
        waitForUpdate();
        QCOMPARE(alarm.error(), (int)UCAlarm::NoError);
        QVERIFY(containsAlarm(&alarm));
    }

    void test_fetchAlarmPlus7Days()
    {
        QDateTime dt = QDateTime::currentDateTime().addDays(10);
        UCAlarm alarm(dt, "test_fetchAlarmPlus7Days");

        alarm.save();
        waitForInsert();
        QCOMPARE(alarm.error(), (int)UCAlarm::NoError);
        QVERIFY(containsAlarm(&alarm));

        UCAlarm nextMonth(dt.addMonths(1), "test_fetchAlarmPlus1Month");
        nextMonth.save();
        waitForInsert();
        QCOMPARE(nextMonth.error(), (int)UCAlarm::NoError);
        QVERIFY(containsAlarm(&nextMonth));

        UCAlarm nextYear(dt.addYears(1), "test_fetchAlarmPlus1Year");
        nextYear.save();
        waitForInsert();
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
        waitForInsert();
        QTest::qWait(3000);
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
        waitForInsert();
        QTest::qWait(2000);
        syncFetch();
        QCOMPARE(alarm.error(), (int)UCAlarm::NoError);
        QVERIFY(containsAlarm(&nextAlarm));
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
        waitForInsert();
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

    void test_sound_saving() {
        UCAlarm alarm(QDateTime::currentDateTime().addSecs(60), "test_onetime_sound");
        alarm.setSound(QUrl("file:///usr/share/sounds/ubuntu/ringtones/Celestial.ogg"));
        alarm.save();
        waitForInsert();

        const UCAlarm *saved = AlarmManager::instance().findAlarm(alarm.cookie());
        QVERIFY(saved);
        QCOMPARE(alarm, *saved);
        QCOMPARE(alarm.sound().toString(), saved->sound().toString());
    }

    // guard bug #1360101
    void test_create_update_and_disable_alarm() {
        UCAlarm alarm(QDateTime::currentDateTime(), UCAlarm::AutoDetect, "test_create_update_and_disable_alarm");
        alarm.setSound(QUrl("file:///usr/share/sounds/ubuntu/ringtones/Celestial.ogg"));
        alarm.save();
        waitForInsert();
        QVERIFY(containsAlarm(&alarm));

        // update alarm to occur 1h earlier
        QDateTime date = alarm.date();
        date.addSecs(-60);
        alarm.save();
        waitForUpdate();
        QVERIFY(containsAlarm(&alarm));

        // disable alarm
        alarm.setEnabled(false);
        alarm.save();
        waitForUpdate();
        QVERIFY(containsAlarm(&alarm));
        QVERIFY(AlarmManager::instance().verifyChange(&alarm, AlarmManager::Enabled, false));
    }

    void test_change_alarm_sound()
    {
        UCAlarm alarm(QDateTime::currentDateTime(), UCAlarm::AutoDetect, "test_change_alarm_fields_sound");
        alarm.setSound(QUrl("file:///usr/share/sounds/ubuntu/ringtones/Bliss.ogg"));
        alarm.save();
        waitForInsert();
        QVERIFY(containsAlarm(&alarm));

        // do the change
        alarm.setSound(QUrl("file:///usr/share/sounds/ubuntu/ringtones/Sparkle.ogg"));
        alarm.save();
        waitForUpdate();
        QVERIFY(containsAlarm(&alarm));

        //verify whether we have the desired change
        QVERIFY(AlarmManager::instance().verifyChange(&alarm, AlarmManager::Sound, QVariant::fromValue(alarm.sound())));
    }

    void test_check_alarm_tags_data()
    {
        QTest::addColumn<QString>("message");
        QTest::addColumn<bool>("enabled");

        QTest::newRow("enabled") << "enabled" << true;
        QTest::newRow("disabled") << "disabled" << false;
    }
    void test_check_alarm_tags()
    {
        QFETCH(QString, message);
        QFETCH(bool, enabled);

        UCAlarm alarm(QDateTime::currentDateTime().addDays(1), "test_check_alarm_tags_" + message);
        alarm.setSound(QUrl("file:///usr/share/sounds/ubuntu/ringtones/Marimbach.ogg"));
        alarm.setEnabled(enabled);
        alarm.save();
        waitForInsert();
        QVERIFY(containsAlarm(&alarm));

        // check the tags
        QVERIFY(AlarmManager::instance().verifyChange(&alarm, AlarmManager::Enabled, enabled));
    }
};

QTEST_MAIN(tst_UCAlarms)

#include "tst_alarms.moc"
