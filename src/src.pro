TEMPLATE = subdirs

src_components.subdir = Ubuntu/Components
src_components.target = sub-components
src_components.depends = sub-gestures sub-libtoolkit  # just for the module .pri file

src_layouts.subdir = Ubuntu/Layouts
src_layouts.target = sub-layouts

src_performance_metrics.subdir = Ubuntu/PerformanceMetrics
src_performance_metrics.target = sub-performance-metrics

src_gestures.subdir  = Ubuntu/UbuntuGestures
src_gestures.target  = sub-gestures

src_toolkitlib.subdir  = Ubuntu/UbuntuToolkit
src_toolkitlib.target  = sub-libtoolkit

src_test.subdir  = Ubuntu/Test
src_test.target  = sub-test
src_test.depends = sub-components 

#order is important
SUBDIRS += src_gestures src_toolkitlib src_components src_layouts src_performance_metrics src_test
