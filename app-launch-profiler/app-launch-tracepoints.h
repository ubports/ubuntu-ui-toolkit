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
