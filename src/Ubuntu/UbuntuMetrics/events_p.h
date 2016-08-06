// Copyright © 2016 Canonical Ltd.
// Author: Loïc Molinari <loic.molinari@canonical.com>
//
// This file is part of Ubuntu UI Toolkit.
//
// Ubuntu UI Toolkit is free software: you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by the
// Free Software Foundation; version 3.
//
// Ubuntu UI Toolkit is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
// for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Ubuntu UI Toolkit. If not, see <http://www.gnu.org/licenses/>.

#ifndef EVENTS_P_H
#define EVENTS_P_H

#include "events.h"
#include "ubuntumetricsglobal_p.h"
#include <QtCore/QElapsedTimer>
#include <sys/times.h>

class UBUNTU_METRICS_PRIVATE_EXPORT EventUtilsPrivate
{
public:
    EventUtilsPrivate();
    ~EventUtilsPrivate();

    void updateCpuUsage(UMEvent* event);
    void updateProcStatMetrics(UMEvent* event);

    char* m_buffer;
    QElapsedTimer m_cpuTimer;
    struct tms m_cpuTimes;
    clock_t m_cpuTicks;
    quint16 m_cpuOnlineCores;
    quint16 m_pageSize;
};

#endif  // EVENTS_P_H
