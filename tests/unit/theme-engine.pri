include( ../../qtversioncheck.pri )

INCLUDEPATH += ../../../modules/Ubuntu/Components/plugin

#themeing engine sources required
SOURCES += ../../../modules/Ubuntu/Components/plugin/themeengine.cpp
SOURCES += ../../../modules/Ubuntu/Components/plugin/themesettings.cpp
SOURCES += ../../../modules/Ubuntu/Components/plugin/suffixtree.cpp
SOURCES += ../../../modules/Ubuntu/Components/plugin/qthmthemeloader.cpp
SOURCES += ../../../modules/Ubuntu/Components/plugin/qmlthemeloader.cpp
SOURCES += ../../../modules/Ubuntu/Components/plugin/style.cpp
SOURCES += ../../../modules/Ubuntu/Components/plugin/styleditem.cpp

OTHER_FILES += \
    ../../resources/test.qthm \
    ../../resources/base.qthm
