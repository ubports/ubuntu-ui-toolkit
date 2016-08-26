%modules = ( # path to module name map
    "UbuntuGestures" => "$basedir/src/Ubuntu/UbuntuGestures",
    "UbuntuMetrics" => "$basedir/src/Ubuntu/UbuntuMetrics",
    "UbuntuToolkit"  => "$basedir/src/Ubuntu/UbuntuToolkit",
);
%moduleheaders = ( # restrict the module headers to those found in relative path
);

%classnames = (
 #add classnames that are not automatically detected
 #e.g. "someheader.h" => "SomeType"
 "ubuntugesturesmodule.h" => "UbuntuGesturesModule",
 "ubuntutoolkitmodule.h" => "UbuntuToolkitModule"
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

#export all classes that are in a Ubuntu* namespace
$publicclassregexp = "^Ubuntu[A-Za-z0-9]*::[^:]*\$"
