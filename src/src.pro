TEMPLATE = subdirs

src_gestures.subdir = Ubuntu/UbuntuGestures
src_gestures.target = sub-gestures

src_metrics.subdir = Ubuntu/UbuntuMetrics
src_metrics.target = sub-metrics

src_metrics_lttng.subdir = Ubuntu/UbuntuMetrics/lttng
src_metrics_lttng.target = sub-metrics-lttng

src_toolkitlib.subdir = Ubuntu/UbuntuToolkit
src_toolkitlib.target = sub-libtoolkit
src_toolkitlib.depends = sub-gestures sub-metrics

src_components.subdir = Ubuntu/Components
src_components.target = sub-components
src_components.depends = sub-libtoolkit 

src_layouts.subdir = Ubuntu/Layouts
src_layouts.target = sub-layouts

src_performance_metrics.subdir = Ubuntu/PerformanceMetrics
src_performance_metrics.target = sub-performance-metrics

src_test.subdir = Ubuntu/Test
src_test.target = sub-test
src_test.depends = sub-components 

SUBDIRS += \
    src_gestures \
    src_metrics \
    src_metrics_lttng \
    src_toolkitlib \
    src_components \
    src_layouts \
    src_performance_metrics \
    src_test
