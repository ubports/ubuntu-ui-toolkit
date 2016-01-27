CXX_MODULE = qml
TARGET  = UbuntuPerformanceMetrics
TARGETPATH = Ubuntu/PerformanceMetrics
IMPORT_VERSION = 0.1

#QMAKE_DOCS = $$PWD/doc/qtquicklayouts.qdocconf

include(plugin/plugin.pri)

QML_FILES +=  BarGraph.qml \
             PerformanceOverlay.qml \

load(ubuntu_qml_plugin)
