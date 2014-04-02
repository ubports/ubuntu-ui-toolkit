COMPONENTS_PATH = ../../../modules/Ubuntu/Components
INCLUDEPATH += $$COMPONENTS_PATH/plugin
INCLUDEPATH += ../../../modules/Ubuntu/Test/plugin
PRE_TARGETDEPS = $$COMPONENTS_PATH/libUbuntuComponents.so
PRE_TARGETDEPS += ../../../modules/Ubuntu/Test/libUbuntuTest.so
LIBS += $$COMPONENTS_PATH/libUbuntuComponents.so
LIBS += ../../../modules/Ubuntu/Test/libUbuntuTest.so
DEFINES += QUICK_TEST_SOURCE_DIR=\"\\\"$$_PRO_FILE_PWD_\\\"\"
QMAKE_CXXFLAGS += -Werror

components.target = $$PRE_TARGETDEPS
components.commands = cd $$COMPONENTS_PATH/plugin && $(QMAKE) && make
QMAKE_EXTRA_TARGETS += components

