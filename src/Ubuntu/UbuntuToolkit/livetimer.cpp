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

UT_NAMESPACE_BEGIN

/*! \qmltype LiveTimer
    \inqmlmodule Ubuntu.Components
    \ingroup ubuntu
    \since Ubuntu.Components 1.3
    \brief A live timing source providing peridioc updates.

    The LiveTimer is a source for periodic signals triggered on second/minute/hour boundaries. The
    timer can also be set up to provide signals with an increasing frequency the closer a given time
    is to current time.

    Examples:

    \qml
    import Ubuntu.Components 1.3

    LiveTimer {
        frequency: LiveTimer.Second
        onTrigger: console.log("The time is now", new Date().toString());
    }
    \endqml

    \qml
    import Ubuntu.Components 1.3

    LiveTimer {
        frequency: LiveTimer.Relative
        relativeTime: new Date()
    }
    \endqml
*/
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

/*!
 * \qmlsignal LiveTimer::trigger()
 * Signal called when the timer is triggered
 */

/*! \qmlproperty enumeration LiveTimer::frequency
    \since Ubuntu.Components 1.3

    This properties defines the frequency at which the LiveTimer signals notifications.

    \list
    \li \b LiveTimer.Disabled - disable the LiveTimer \l trigger signal
    \li \b LiveTimer.Second - emit the \l trigger signal on every change of second.
    \li \b LiveTimer.Minute - emit the \l trigger signal on every change of minute.
    \li \b LiveTimer.Hour - emit the \l trigger signal on every change of hour.
    \li \b LiveTimer.Relative - emit the \l trigger signal periodically depending on how close current time is to to
    \l relativeTime. If \l relativeTime is within 30 seconds of the current time, trigger every 30 seconds. Within an hour,
    trigger every minute. Otherwise, trigger every hour until the relative time is more than a week past current time, after which
    updates are disabled.

    \note Setting the frequency to LiveTimer.Relative will disable the timer until a \l relativeTime is set.

    \endlist
*/
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

/*! \qmlproperty datetime LiveTimer::relativeTime
    \since Ubuntu.Components 1.3

    This properties defines the value used for proximity evaluation when using Relative mode.

    \note This property has no impact unless the \l frequency is set to LiveTimer.Relative
*/
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

UT_NAMESPACE_END
