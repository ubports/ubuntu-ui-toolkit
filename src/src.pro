TEMPLATE = subdirs

src_gestures.subdir  = Ubuntu/UbuntuGestures
src_gestures.target  = sub-gestures

src_toolkitlib.subdir  = Ubuntu/UbuntuToolkit
src_toolkitlib.target  = sub-libtoolkit
src_components.depends = sub-gestures

src_components.subdir = Ubuntu/Components
src_components.target = sub-components
src_components.depends = sub-libtoolkit 

src_layouts.subdir = Ubuntu/Layouts
src_layouts.target = sub-layouts

src_performance_metrics.subdir = Ubuntu/PerformanceMetrics
src_performance_metrics.target = sub-performance-metrics

src_metrics.subdir  = Ubuntu/UbuntuMetrics
src_metrics.target  = sub-metrics

src_test.subdir  = Ubuntu/Test
src_test.target  = sub-test
src_test.depends = sub-components 

# Order is important, dependencies first.
SUBDIRS += \
    src_gestures \
    src_metrics \
    src_toolkitlib \
    src_components \
    src_layouts \
    src_performance_metrics \
    src_test
