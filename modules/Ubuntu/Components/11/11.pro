TEMPLATE = aux

CONFIG+=ubuntu_qml_module

uri = Ubuntu.Components.11
installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)

qml_files.installPath = $$installPath
qml_files.files = *.qml

UBUNTU_QML_MODULE_FILES += qml_files
