TEMPLATE = aux

CONFIG+=ubuntu_qml_module

uri = Ubuntu.Components.Styles.12
installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)

qml_files.installPath = $$installPath
qml_files.files = *.qml

UBUNTU_QML_MODULE_FILES += qml_files
