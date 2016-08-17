/*
 * Copyright 2015 Canonical Ltd.
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
 * Author: Benjamin Zeller <benjamin.zeller@canonical.com>
 *         Zoltán Balogh <zoltan.balogh@canonical.com 
 */

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER app

#undef TRACEPOINT_INCLUDE
#define TRACEPOINT_INCLUDE "./app-launch-tracepoints.h"

#if !defined(TRACEPOINTS_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define TRACEPOINTS_H

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(app, invokeApplauncher, TP_ARGS(0), TP_FIELDS())


#endif /* TRACEPOINTS_H */

#include <lttng/tracepoint-event.h>
