uri = Ubuntu.Layouts
installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)

# qmldir
QMLDIR_FILE = qmldir

# make found deployables visible in Qt Creator
OTHER_FILES += $$QMLDIR_FILE

# define deployment for found deployables
qmldir_file.path = $$installPath
qmldir_file.files = $$QMLDIR_FILE

plugins_qmltypes.path = $$installPath
plugins_qmltypes.files = plugins.qmltypes
plugins_qmltypes.extra = $$[QT_INSTALL_BINS]/qmlplugindump -notrelocatable Ubuntu.Layouts 0.1 ../../ > $(INSTALL_ROOT)/$$installPath/plugins.qmltypes

INSTALLS += qmldir_file plugins_qmltypes
