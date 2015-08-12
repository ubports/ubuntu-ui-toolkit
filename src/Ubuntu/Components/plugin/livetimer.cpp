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

#include "livetimer.h"
#include "livetimer_p.h"

LiveTimer::LiveTimer(QObject *parent)
    : QObject(parent)
    , m_frequency(Disabled)
    , m_effectiveFrequency(Disabled)
    , m_lastUpdate(0)
{
}

LiveTimer::~LiveTimer()
{
    unregisterTimer();
}

LiveTimer::Frequency LiveTimer::frequency() const
{
    return m_frequency;
}

void LiveTimer::setFrequency(LiveTimer::Frequency frequency)
{
    if (m_frequency != frequency) {
        m_frequency = frequency;
        Q_EMIT frequencyChanged();

        if (m_frequency != Disabled && (m_frequency != Relative || m_relativeTime.isValid())) {
            registerTimer();
        } else {
            unregisterTimer();
        }
    }
}

QDateTime LiveTimer::relativeTime() const
{
    return m_relativeTime;
}

void LiveTimer::setRelativeTime(const QDateTime &relativeTime)
{
    if (m_relativeTime != relativeTime) {
        m_relativeTime = relativeTime;
        Q_EMIT relativeTimeChanged();

        if (m_frequency == Relative) {
            if (m_relativeTime.isValid()) {
                registerTimer();
            } else {
                unregisterTimer();
            }
        }
    }
}

LiveTimer::Frequency LiveTimer::effectiveFrequency() const
{
    return m_effectiveFrequency;
}

void LiveTimer::registerTimer()
{
    SharedLiveTimer::instance().registerTimer(this);

    QObject::connect(&SharedLiveTimer::instance(), &SharedLiveTimer::trigger, this, &LiveTimer::trigger);
}

void LiveTimer::unregisterTimer()
{
    QObject::disconnect(&SharedLiveTimer::instance(), &SharedLiveTimer::trigger, this, 0);
    SharedLiveTimer::instance().unregisterTimer(this);

}

void LiveTimer::setEffectiveFrequency(LiveTimer::Frequency frequency)
{
    m_effectiveFrequency = frequency;
}
