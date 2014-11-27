include(../../variables.pri)

COMPONENTS_PATH = ../../../modules/Ubuntu/Components
INCLUDEPATH += $$COMPONENTS_PATH/plugin
INCLUDEPATH += ../../../modules/Ubuntu/Test/plugin
PRE_TARGETDEPS = $$COMPONENTS_PATH/libUbuntuComponents.so
PRE_TARGETDEPS += ../../../modules/Ubuntu/Test/libUbuntuTest.so
LIBS += -L$$COMPONENTS_PATH -lUbuntuComponents
LIBS += -L../../../modules/Ubuntu/Test -lUbuntuTest
DEFINES += QUICK_TEST_SOURCE_DIR=\"\\\"$$_PRO_FILE_PWD_\\\"\"
QMAKE_CXXFLAGS += -Werror

components.target = $$PRE_TARGETDEPS
components.commands = cd $$COMPONENTS_PATH/plugin && $(QMAKE) && make
QMAKE_EXTRA_TARGETS += components

DEFINES+=UBUNTU_COMPONENT_PATH='\\"$${ROOT_BUILD_DIR}/modules/Ubuntu/Components\\"'
DEFINES+=UBUNTU_QML_IMPORT_PATH='\\"$${ROOT_BUILD_DIR}/modules\\"'
DEFINES+=UBUNTU_SOURCE_ROOT='\\"$${ROOT_SOURCE_DIR}\\"'
DEFINES+=UBUNTU_BUILD_ROOT='\\"$${ROOT_BUILD_DIR}\\"'
