TEMPLATE = subdirs

# Libraries

src_gestures_lib.subdir = Ubuntu/UbuntuGestures
src_gestures_lib.target = sub-gestures-lib

src_metrics_lib.subdir = Ubuntu/UbuntuMetrics
src_metrics_lib.target = sub-metrics-lib

src_toolkit_lib.subdir = Ubuntu/UbuntuToolkit
src_toolkit_lib.target = sub-toolkit-lib
src_toolkit_lib.depends = sub-gestures-lib sub-metrics-lib

# Plugins

src_metrics_lttng_plugin.subdir = Ubuntu/UbuntuMetrics/lttng
src_metrics_lttng_plugin.target = sub-metrics-lttng-plugin

# QML modules

src_metrics_module.subdir = Ubuntu/Metrics
src_metrics_module.target = sub-metrics-module
src_metrics_module.depends = sub-metrics-lib

src_components_module.subdir = Ubuntu/Components
src_components_module.target = sub-components-module
src_components_module.depends = sub-toolkit-lib

src_layouts_module.subdir = Ubuntu/Layouts
src_layouts_module.target = sub-layouts-module

src_performance_metrics_module.subdir = Ubuntu/PerformanceMetrics
src_performance_metrics_module.target = sub-performance-metrics-module

src_test_module.subdir = Ubuntu/Test
src_test_module.target = sub-test-module
src_test_module.depends = sub-toolkit-lib

SUBDIRS += \
    src_gestures_lib \
    src_metrics_lib \
    src_toolkit_lib \
    src_metrics_lttng_plugin \
    src_metrics_module \
    src_components_module \
    src_layouts_module \
    src_performance_metrics_module \
    src_test_module
