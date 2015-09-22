#define TRACEPOINT_CREATE_PROBES
/*
 * The header containing our TRACEPOINT_EVENTs.
 */
#define TRACEPOINT_DEFINE
#include "app-launch-tracepoints.h"

//compile with gcc -o app-launch-tracepoints app-launch-tracepoints.cpp -llttng-ust -ldl -I.

int main (int argc, char* argv[])
{
    tracepoint(app,invokeApplauncher);
    return 0;
}
