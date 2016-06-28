CXX_MODULE = qml
TARGET  = UbuntuComponentsStyles
TARGETPATH = Ubuntu/Components/Styles
IMPORT_VERSION = 1.1

include(plugin/plugin.pri)

QML_FILES += 1.2/ComboButtonStyle.qml \
             1.2/PageHeadStyle.qml \
             1.2/PullToRefreshStyle.qml \
             1.3/PageHeadStyle.qml \
	     1.3/ActionBarStyle.qml \
	     1.3/PageHeaderStyle.qml \
	     1.3/ToolbarStyle.qml \
	     1.3/SectionsStyle.qml \

load(ubuntu_qml_plugin)

OTHER_FILES+=qmldir
