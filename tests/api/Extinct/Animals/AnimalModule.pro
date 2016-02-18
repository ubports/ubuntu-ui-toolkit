CXX_MODULE = qml
TARGET = ExtinctAnimals
TARGETPATH = Extinct/Animals
IMPORT_VERSION = 0.1

include(plugin/plugin.pri)

QML_FILES += \
    Andrewsarchus.qml \
    Paratriisodon.qml \
    Gigantophis.qml \
    test.qml \
    digger.js \

load(ubuntu_qml_plugin)

