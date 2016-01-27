%modules = ( # path to module name map
    "UbuntuGestures" => "$basedir/src/Ubuntu/UbuntuGestures",
);
%moduleheaders = ( # restrict the module headers to those found in relative path
);

%classnames = (
 "debughelpers.h" => "DebugHelpers",
 "pool.h" => "Pool",
 "timesource.h" => "TimeSource",
 "touchregistry.h" => "TouchRegistry",
 "candidateinactivitytimer.h" => "CandidateInactivityTimer",
 "timer.h" => "Timer",
 "touchownershipevent.h" => "TouchOwnershipEvent",
 "unownedtouchevent.h" => "UnownedTouchEvent"
);

# Module dependencies.
# Every module that is required to build this module should have one entry.
# Each of the module version specifiers can take one of the following values:
#   - A specific Git revision.
#   - any git symbolic ref resolvable from the module's repository (e.g. "refs/heads/master" to track master branch)
#
%dependencies = (
    "qtbase" => "",
    "qtdeclarative" => "",
    "qtfeedback" => "",
    "qtpim" => "",
);
