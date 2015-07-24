CXX_MODULE = qml
TARGET  = UbuntuTest
TARGETPATH = Ubuntu/Test
IMPORT_VERSION = 0.1

#QMAKE_DOCS = $$PWD/doc/qtquicklayouts.qdocconf

include(plugin/plugin.pri)

QML_FILES += UbuntuTestCase.qml

load(ubuntu_qml_plugin)

