CXX_MODULE = qml
TARGET  = UbuntuMetrics
TARGETPATH = Ubuntu/Metrics
IMPORT_VERSION = 1.0
QT += qml UbuntuMetrics
SOURCES += plugin.cpp
load(ubuntu_qml_plugin)
