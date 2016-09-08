CXX_MODULE = qml
TARGET  = UbuntuLayouts
TARGETPATH = Ubuntu/Layouts
IMPORT_VERSION = 0.1

#QMAKE_DOCS = $$PWD/doc/qtquicklayouts.qdocconf

include(plugin/plugin.pri)

load(ubuntu_qml_plugin)
