LAYOUT_SRC = $$PWD/../../src/Ubuntu/Layouts
LAYOUT_BLD = $$shadowed($$PWD)/../../qml/Ubuntu/Layouts

INCLUDEPATH += $$LAYOUT_SRC/plugin
PRE_TARGETDEPS = $$LAYOUT_BLD/libUbuntuLayouts.so
LIBS += -L$$LAYOUT_BLD -lUbuntuLayouts
DEFINES += QUICK_TEST_SOURCE_DIR=\"\\\"$$_PRO_FILE_PWD_\\\"\"
