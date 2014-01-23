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
 *
 * Author: Florian Boucault <florian.boucault@canonical.com>
 */

#include "upmcpuusage.h"
#include <unistd.h>

UPMCpuUsage::UPMCpuUsage(QObject *parent) :
    QObject(parent),
    m_graphModel(new UPMGraphModel(this)),
    m_period(5000),
    m_samplingInterval(500)
{
    m_cores = 1.0f / sysconf(_SC_NPROCESSORS_ONLN);
    m_previousClock = times(&m_previousTimes);

    QObject::connect(&m_timer, &QTimer::timeout, this, &UPMCpuUsage::appendCpuTime);
    m_timer.setSingleShot(false);
    m_timer.setInterval(m_samplingInterval);
    m_timer.start();
}

UPMGraphModel* UPMCpuUsage::graphModel() const
{
    return m_graphModel;
}

int UPMCpuUsage::period() const
{
    return m_period;
}

void UPMCpuUsage::setPeriod(int period)
{
    if (period != m_period) {
        m_period = period;
        m_graphModel->setSamples(m_period / m_samplingInterval);
        m_timer.setInterval(m_samplingInterval);
        Q_EMIT periodChanged();
    }
}

int UPMCpuUsage::samplingInterval() const
{
    return m_samplingInterval;
}

void UPMCpuUsage::setSamplingInterval(int samplingInterval)
{
    if (samplingInterval != m_samplingInterval) {
        m_samplingInterval = samplingInterval;
        m_graphModel->setSamples(m_period / m_samplingInterval);
        Q_EMIT samplingIntervalChanged();
    }
}

void UPMCpuUsage::appendCpuTime()
{
    struct tms newTimes;
    clock_t newClock = times(&newTimes);

    float elapsed = (newTimes.tms_utime + newTimes.tms_stime - m_previousTimes.tms_utime - m_previousTimes.tms_stime)
                        / (float)(newClock - m_previousClock);

    memcpy(&m_previousTimes, &newTimes, sizeof(tms));
    m_previousClock = newClock;

    int width = ((qreal)m_graphModel->samples() / m_period) * m_samplingInterval;
    m_graphModel->appendValue(width, elapsed * 100);
}
