COMPONENTS_PATH = ../../../modules/Ubuntu/Layouts
INCLUDEPATH += $$COMPONENTS_PATH/plugin
PRE_TARGETDEPS = $$COMPONENTS_PATH/libUbuntuLayouts.so
LIBS += -L$$COMPONENTS_PATH -lUbuntuLayouts
DEFINES += QUICK_TEST_SOURCE_DIR=\"\\\"$$_PRO_FILE_PWD_\\\"\"

components.target = $$PRE_TARGETDEPS
components.commands = cd $$COMPONENTS_PATH/plugin && $(QMAKE) && make
QMAKE_EXTRA_TARGETS += components
