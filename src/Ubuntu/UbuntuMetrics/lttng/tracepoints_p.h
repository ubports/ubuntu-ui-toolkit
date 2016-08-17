// Copyright © 2016 Canonical Ltd.
// Authors: Loïc Molinari <loic.molinari@canonical.com>
//          Albert Astals Cid <albert.astals@canonical.com>
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

#include "lttng_p.h"

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER UbuntuMetrics
#undef TRACEPOINT_INCLUDE
#define TRACEPOINT_INCLUDE "./tracepoints_p.h"
#if !defined(TRACEPOINTS_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define TRACEPOINTS_P_H
#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(
    UbuntuMetrics, process,
    TP_ARGS(
        UMLTTNGProcessEvent*, processEvent
    ),
    TP_FIELDS(
        ctf_integer(uint16_t, cpu_usage, processEvent->cpuUsage)
        ctf_integer(uint32_t, vsz_memory, processEvent->vszMemory)
        ctf_integer(uint32_t, rss_memory, processEvent->rssMemory)
        ctf_integer(uint16_t, thread_count, processEvent->threadCount)
    )
)

TRACEPOINT_EVENT(
    UbuntuMetrics, frame,
    TP_ARGS(
        UMLTTNGFrameEvent*, frameEvent
    ),
    TP_FIELDS(
        ctf_integer(uint32_t, window, frameEvent->window)
        ctf_integer(uint32_t, number, frameEvent->number)
        ctf_float(float, delta_time, frameEvent->deltaTime)
        ctf_float(float, sync_time, frameEvent->syncTime)
        ctf_float(float, render_time, frameEvent->renderTime)
        ctf_float(float, gpu_time, frameEvent->gpuTime)
        ctf_float(float, swap_time, frameEvent->swapTime)
    )
)

TRACEPOINT_EVENT(
    UbuntuMetrics, window,
    TP_ARGS(
        UMLTTNGWindowEvent*, windowEvent
    ),
    TP_FIELDS(
        ctf_integer(uint32_t, id, windowEvent->id)
        ctf_string(state, windowEvent->state)
        ctf_integer(uint16_t, width, windowEvent->width)
        ctf_integer(uint16_t, height, windowEvent->height)
    )
)

TRACEPOINT_EVENT(
    UbuntuMetrics, generic,
    TP_ARGS(
        UMLTTNGGenericEvent*, genericEvent
    ),
    TP_FIELDS(
        ctf_integer(uint32_t, id, genericEvent->id)
        ctf_string(string, genericEvent->string)
    )
)

#endif  // TRACEPOINTS_P_H
#include <lttng/tracepoint-event.h>
