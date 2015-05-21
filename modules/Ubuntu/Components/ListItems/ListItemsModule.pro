TEMPLATE=aux

CONFIG+=ubuntu_qml_module

uri = Ubuntu.Components.ListItems
installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)

# qmldir file
qmldir_file.installPath = $$installPath
qmldir_file.files = qmldir

UBUNTU_QML_MODULE_FILES += qmldir_file

