include( ../../qtversioncheck.pri )

INCLUDEPATH += ../../../modules/Ubuntu/Components/plugin

#themeing engine sources required
HEADERS += ../../../modules/Ubuntu/Components/plugin/style.h \
    ../../../modules/Ubuntu/Components/plugin/themeengine.h \
    ../../../modules/Ubuntu/Components/plugin/styleditem.h \
    ../../../modules/Ubuntu/Components/plugin/styleditem_p.h \
    ../../../modules/Ubuntu/Components/plugin/style_p.h \
    ../../../modules/Ubuntu/Components/plugin/themeengine_p.h \
    ../../../modules/Ubuntu/Components/plugin/qmlthemeloader_p.h \
    ../../../modules/Ubuntu/Components/plugin/qthmthemeloader_p.h \
    ../../../modules/Ubuntu/Components/plugin/themeloader_p.h

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
