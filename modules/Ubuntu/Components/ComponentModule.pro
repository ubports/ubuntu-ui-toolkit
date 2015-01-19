TEMPLATE=aux

CONFIG+=ubuntu_qml_module

uri = Ubuntu.Components
installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)

# qmldir file
qmldir_file.installPath = $$installPath
qmldir_file.files = qmldir

# qml source files
qml_files.installPath = $$installPath
qml_files.files = *.qml

# javascript files
js_files.installPath = $$installPath
js_files.files = *.js

#artwork files
artwork_files.installPath = $$installPath/artwork
artwork_files.files = $$system(find artwork -type f)

UBUNTU_QML_MODULE_FILES += qmldir_file qml_files js_files artwork_files

plugins_qmltypes.path = $$installPath
plugins_qmltypes.files = plugins.qmltypes
# Silence spam on stderr due to fonts
# https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1256999
# https://bugreports.qt-project.org/browse/QTBUG-36243
plugins_qmltypes.extra = ALARM_BACKEND=memory $$[QT_INSTALL_BINS]/qmlplugindump -notrelocatable Ubuntu.Components 0.1 ../../ 2>/dev/null > $(INSTALL_ROOT)/$$installPath/plugins.qmltypes
INSTALLS += plugins_qmltypes

install_subtargets.depends += plugins_qmltypes

