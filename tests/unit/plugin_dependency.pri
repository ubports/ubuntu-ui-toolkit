PLUGIN_SRC = $$PWD/../../modules/Ubuntu
PLUGIN_BLD = $$shadowed($$PWD)/../../modules/Ubuntu

COMPONENTS_PATH = ../../../modules/Ubuntu/Components
INCLUDEPATH += $$PLUGIN_SRC/Components/plugin
INCLUDEPATH += $$PLUGIN_SRC/Test/plugin
PRE_TARGETDEPS =  $$PLUGIN_BLD/Components/libUbuntuComponents.so
PRE_TARGETDEPS += $$PLUGIN_BLD/Test/libUbuntuTest.so
LIBS += -L$$PLUGIN_BLD/Components -lUbuntuComponents
LIBS += -L$$PLUGIN_BLD/Test -lUbuntuTest
DEFINES += QUICK_TEST_SOURCE_DIR=\"\\\"$$_PRO_FILE_PWD_\\\"\"
QMAKE_CXXFLAGS += -Werror

DEFINES+=UBUNTU_COMPONENT_PATH='\\"$${ROOT_BUILD_DIR}/modules/Ubuntu/Components\\"'
DEFINES+=UBUNTU_QML_IMPORT_PATH='\\"$${ROOT_BUILD_DIR}/modules\\"'
DEFINES+=UBUNTU_SOURCE_ROOT='\\"$${ROOT_SOURCE_DIR}\\"'
DEFINES+=UBUNTU_BUILD_ROOT='\\"$${ROOT_BUILD_DIR}\\"'
