TEMPLATE=aux

CONFIG+=ubuntu_qml_module

uri = Ubuntu.Layouts
installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)

# qmldir
QMLDIR_FILE = qmldir

# define deployment for found deployables
qmldir_file.installPath = $$installPath
qmldir_file.files = $$QMLDIR_FILE

plugins_qmltypes.path = $$installPath
plugins_qmltypes.files = plugins.qmltypes
# Silence spam on stderr due to fonts
# https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1256999
# https://bugreports.qt-project.org/browse/QTBUG-36243
plugins_qmltypes.extra = $$[QT_INSTALL_BINS]/qmlplugindump -notrelocatable Ubuntu.Layouts 0.1 ../../ 2>/dev/null > $(INSTALL_ROOT)/$$installPath/plugins.qmltypes

UBUNTU_QML_MODULE_FILES += qmldir_file

INSTALLS += plugins_qmltypes
