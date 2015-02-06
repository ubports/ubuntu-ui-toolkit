TEMPLATE=aux

CONFIG+=ubuntu_qml_module

uri = Ubuntu.PerformanceMetrics
installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)

# Components
QML_FILES = *.qml

# qmldir
QMLDIR_FILE = qmldir

# define deployment for found deployables
qmldir_file.installPath = $$installPath
qmldir_file.files = $$QMLDIR_FILE
qml_files.installPath = $$installPath
qml_files.files = $$QML_FILES

plugins_qmltypes.path = $$installPath
plugins_qmltypes.files = plugins.qmltypes
plugins_qmltypes.extra = $$[QT_INSTALL_BINS]/qmlplugindump -notrelocatable Ubuntu.PerformanceMetrics 0.1 ../../ > $(INSTALL_ROOT)/$$installPath/plugins.qmltypes

INSTALLS += plugins_qmltypes
UBUNTU_QML_MODULE_FILES += qmldir_file qml_files
