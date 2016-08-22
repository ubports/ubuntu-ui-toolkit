QT -= core gui
TEMPLATE = lib
TARGET = umlttng
CONFIG += plugin
LIBS += -llttng-ust
SOURCES = tracepoints.c lttng.c
target.path = $$[QT_INSTALL_PLUGINS]/ubuntu/metrics
INSTALLS += target
