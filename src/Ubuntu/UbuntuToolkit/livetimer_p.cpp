/*
 * Copyright 2015 Canonical Ltd.
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

#include "livetimer_p_p.h"
#include "timeutils_p.h"

#include <QDBusConnection>

UT_NAMESPACE_BEGIN

SharedLiveTimer::SharedLiveTimer(QObject* parent)
    : QObject(parent)
    , m_frequency(LiveTimer::Disabled)
{
    m_timer.setSingleShot(true);
    connect(&m_timer, &QTimer::timeout, this, &SharedLiveTimer::timeout);

    QDBusConnection::systemBus().connect("org.freedesktop.timedate1",
                                         "/org/freedesktop/timedate1",
                                         "org.freedesktop.DBus.Properties",
                                         "PropertiesChanged",
                                         this,
                                         SLOT(timedate1PropertiesChanged(QString, QVariantMap, QStringList)));
}

void SharedLiveTimer::registerTimer(LiveTimer *timer)
{
    if (m_liveTimers.contains(timer)) {
        unregisterTimer(timer);
    }
    m_liveTimers.append(timer);
    updateFrequency();
}

void SharedLiveTimer::unregisterTimer(LiveTimer *timer)
{
    if (!m_liveTimers.contains(timer)) return;

    m_liveTimers.removeAll(timer);
    updateFrequency();
}

void SharedLiveTimer::updateFrequency()
{
    LiveTimer::Frequency newFreq = LiveTimer::Disabled;
    if (m_liveTimers.count() == 0) {
        newFreq = LiveTimer::Disabled;
    } else {
        Q_FOREACH(LiveTimer* timer, m_liveTimers) {
            LiveTimer::Frequency freq = timer->frequency();
            if (freq == LiveTimer::Relative) {
                date_proximity_t proximity = getDateProximity(QDateTime::currentDateTime(), timer->relativeTime());
                freq = frequencyForProximity(proximity);
            }
            timer->setEffectiveFrequency(freq);

            if (freq != LiveTimer::Disabled && (newFreq == LiveTimer::Disabled || freq < newFreq)) {
                newFreq =  freq;
            }
        }
    }
    if (newFreq != m_frequency) {
        m_frequency = newFreq;
        reInitTimer();
    }
}

void SharedLiveTimer::reInitTimer()
{
    QDateTime now(QDateTime::currentDateTime());
    m_nextUpdate = now;

    switch(m_frequency) {
        case LiveTimer::Second:
            m_nextUpdate.setTime(QTime(now.time().hour(), now.time().minute(), now.time().second(), 0));
            m_nextUpdate = m_nextUpdate.addSecs(1);
            break;

        case LiveTimer::Minute:
            m_nextUpdate.setTime(QTime(now.time().hour(), now.time().minute(), 0, 0));
            m_nextUpdate = m_nextUpdate.addSecs(60);
            break;

        case LiveTimer::Hour:
            m_nextUpdate.setTime(QTime(now.time().hour(), 0, 0, 0));
            m_nextUpdate = m_nextUpdate.addSecs(60*60);
            break;

        default:
            m_timer.stop();
            return;
    }

    qint64 diff = m_nextUpdate.toMSecsSinceEpoch() - now.toMSecsSinceEpoch();
    m_timer.start(diff);
}

void SharedLiveTimer::timeout()
{
    QDateTime now(QDateTime::currentDateTime());
    qint64 currentMSecsSinceEpoch = now.currentMSecsSinceEpoch();
    qint64 earlyMs = m_nextUpdate.toMSecsSinceEpoch() - currentMSecsSinceEpoch;
    if (earlyMs > 0) { // timer shouldn't have happened yet.
        reInitTimer();
        return;
    }

    bool isHourUpdate = m_lastUpdate.date() != now.date() ||
            m_lastUpdate.time().hour() != now.time().hour();
    bool isMinuteUpdate = isHourUpdate ||
            m_lastUpdate.time().minute() != now.time().minute();
    bool isSecondUpdate = isMinuteUpdate ||
            m_lastUpdate.time().second() != now.time().second();

    bool needsFrequencyUpdate = false;
    QList<LiveTimer*> tmpTimers(m_liveTimers);
    Q_FOREACH(LiveTimer* timer, tmpTimers) {

        LiveTimer::Frequency effectiveFrequency = timer->effectiveFrequency();
        if (effectiveFrequency == LiveTimer::Disabled) continue;

        if (isHourUpdate) {
            Q_EMIT timer->trigger();
        } else if (isMinuteUpdate) {
            if (effectiveFrequency >= LiveTimer::Second && effectiveFrequency <= LiveTimer::Minute) {
                Q_EMIT timer->trigger();
            }
        } else if (isSecondUpdate) {
            if (effectiveFrequency == LiveTimer::Second) {
                Q_EMIT timer->trigger();
            }
        }

        // re-evaluate relative timer frequency
        if (timer->frequency() == LiveTimer::Relative) {
            date_proximity_t newProximity = getDateProximity(now, timer->relativeTime());
            needsFrequencyUpdate |= effectiveFrequency != frequencyForProximity(newProximity);
        }
    }

    if (needsFrequencyUpdate) {
        updateFrequency();
    }
    reInitTimer();
    m_lastUpdate = now;
}

void SharedLiveTimer::timedate1PropertiesChanged(const QString &interface, const QVariantMap &changed, const QStringList &)
{
    if (interface != "org.freedesktop.timedate1") return;
    if (!changed.contains("Timezone")) return;

    QList<LiveTimer*> tmpTimers(m_liveTimers);
    Q_FOREACH(LiveTimer* timer, tmpTimers) {
        Q_EMIT timer->trigger();
    }
    reInitTimer();
}

UT_NAMESPACE_END
