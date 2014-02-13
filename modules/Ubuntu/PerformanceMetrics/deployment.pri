uri = Ubuntu.PerformanceMetrics
installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)

# Components
QML_FILES = $$system(ls *.qml)

# qmldir
QMLDIR_FILE = qmldir

# make found deployables visible in Qt Creator
OTHER_FILES += $$QMLDIR_FILE
OTHER_FILES += $$QML_FILES $$QMLDIR_FILE

# define deployment for found deployables
qmldir_file.path = $$installPath
qmldir_file.files = $$QMLDIR_FILE
qml_files.path = $$installPath
qml_files.files = $$QML_FILES

plugins_qmltypes.path = $$installPath
plugins_qmltypes.files = plugins.qmltypes
plugins_qmltypes.extra = $$[QT_INSTALL_BINS]/qmlplugindump -notrelocatable Ubuntu.PerformanceMetrics 0.1 ../../ > $(INSTALL_ROOT)/$$installPath/plugins.qmltypes

INSTALLS += qmldir_file qml_files plugins_qmltypes
