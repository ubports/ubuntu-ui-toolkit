/*
 * Copyright 2014 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TIMEUTILS_P_H
#define TIMEUTILS_P_H

#include "livetimer.h"

#include <QDateTime>
#include <QLocale>
#include <QObject>
#include <QTimer>

namespace UbuntuToolkit {

/* Check the system locale setting to see if the format is 24-hour
  time or 12-hour time */
inline bool isLocale12h(void)
{
   QString strTimeFormat = QLocale::system().timeFormat();
   QStringList includes; includes << "AP"; includes << "ap";
   QStringList excludes; excludes << "H"; excludes << "HH";

   Q_FOREACH(const QString& exclude, excludes) {
       if (strTimeFormat.contains(exclude)) {
           return false;
       }
   }

   Q_FOREACH(const QString& include, includes) {
       if (strTimeFormat.contains(include)) {
           return true;
       }
   }

   return false;
}

typedef enum
{
   DATE_PROXIMITY_NOW,
   DATE_PROXIMITY_HOUR,
   DATE_PROXIMITY_TODAY,
   DATE_PROXIMITY_YESTERDAY,
   DATE_PROXIMITY_TOMORROW,
   DATE_PROXIMITY_LAST_WEEK,
   DATE_PROXIMITY_NEXT_WEEK,
   DATE_PROXIMITY_FAR_BACK,
   DATE_PROXIMITY_FAR_FORWARD
} date_proximity_t;

inline date_proximity_t getDateProximity(const QDateTime& now, const QDateTime& time)
{
   int now_day = now.date().day();
   int now_month = now.date().month();
   int now_year = now.date().year();

   int time_day = time.date().day();
   int time_month = time.date().month();
   int time_year = time.date().year();

   qint64 diff = time.toMSecsSinceEpoch() - now.toMSecsSinceEpoch();
   if (qAbs(diff) < 30000) return DATE_PROXIMITY_NOW;
   else if (qAbs(diff) < 3600000) return DATE_PROXIMITY_HOUR;

   // does it happen today?
   if ((now_year == time_year) && (now_month == time_month) && (now_day == time_day)) {
       return DATE_PROXIMITY_TODAY;
   }

   // did it happen yesterday?
   QDateTime yesterday(now.addDays(-1));
   int yesterday_day = yesterday.date().day();
   int yesterday_month = yesterday.date().month();
   int yesterday_year = yesterday.date().year();
   if ((yesterday_year == time_year) && (yesterday_month == time_month) && (yesterday_day == time_day)) {
       return DATE_PROXIMITY_YESTERDAY;
   }

   // does it happen tomorrow?
   QDateTime tomorrow(now.addDays(1));
   int tomorrow_day = tomorrow.date().day();
   int tomorrow_month = tomorrow.date().month();
   int tomorrow_year = tomorrow.date().year();
   if ((tomorrow_year == time_year) && (tomorrow_month == time_month) && (tomorrow_day == time_day)) {
       return DATE_PROXIMITY_TOMORROW;
   }

   if (time < now) {
       QDateTime lastWeek(now.addDays(-6));
       QDateTime lastWeekBound(lastWeek.date(), QTime(0, 0, 0, 0));

       // does it happen last week?
       if (time >= lastWeekBound) {
           return DATE_PROXIMITY_LAST_WEEK;
       }
       return DATE_PROXIMITY_FAR_BACK;
   } else {
       QDateTime nextWeek(now.addDays(6));
       QDateTime nextWeekBound(nextWeek.date(), QTime(23, 59, 59, 999));

       // does it happen this week?
       if (time <= nextWeekBound) {
           return DATE_PROXIMITY_NEXT_WEEK;
       }
       return DATE_PROXIMITY_FAR_FORWARD;
   }
}

inline LiveTimer::Frequency frequencyForProximity(date_proximity_t proximity) {
    switch(proximity) {
        case DATE_PROXIMITY_NOW:
            return LiveTimer::Second;
        case DATE_PROXIMITY_HOUR:
            return LiveTimer::Minute;
        case DATE_PROXIMITY_TODAY:
        case DATE_PROXIMITY_TOMORROW:
        case DATE_PROXIMITY_YESTERDAY:
        case DATE_PROXIMITY_LAST_WEEK:
        case DATE_PROXIMITY_NEXT_WEEK:
        case DATE_PROXIMITY_FAR_FORWARD:
            return LiveTimer::Hour;
        case DATE_PROXIMITY_FAR_BACK:
        default:
            break;
    }
    return LiveTimer::Disabled;
}

UT_NAMESPACE_END

#endif // TIMEUTILS_P_H
