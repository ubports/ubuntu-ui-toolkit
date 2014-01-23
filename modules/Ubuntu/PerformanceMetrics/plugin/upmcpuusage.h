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

#ifndef UPMCPUUSAGE_H
#define UPMCPUUSAGE_H

#include <sys/times.h>
#include <QtCore/QObject>
#include <QtCore/QTimer>
#include "upmgraphmodel.h"

class UPMCpuUsage : public QObject
{
    Q_OBJECT

    Q_PROPERTY(UPMGraphModel* graphModel READ graphModel NOTIFY graphModelChanged)
    Q_PROPERTY(int period READ period WRITE setPeriod NOTIFY periodChanged)
    Q_PROPERTY(int samplingInterval READ samplingInterval WRITE setSamplingInterval NOTIFY samplingIntervalChanged)

public:
    explicit UPMCpuUsage(QObject *parent = 0);

    // getters
    UPMGraphModel* graphModel() const;
    int period() const;
    int samplingInterval() const;

    // setters
    void setPeriod(int period);
    void setSamplingInterval(int samplingInterval);

Q_SIGNALS:
    void graphModelChanged();
    void periodChanged();
    void samplingIntervalChanged();

private Q_SLOTS:
    void appendCpuTime();

private:
    UPMGraphModel* m_graphModel;
    int m_period;
    int m_samplingInterval;
    QTimer m_timer;
    float m_cores;
    struct tms m_previousTimes;
    clock_t m_previousClock;
};

#endif // UPMCPUUSAGE_H
