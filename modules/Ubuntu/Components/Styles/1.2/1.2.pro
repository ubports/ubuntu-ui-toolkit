TEMPLATE = aux

CONFIG+=ubuntu_qml_module

uri = Ubuntu.Components.Styles
installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)/1.2

qml_files.installPath = $$installPath
qml_files.files = *.qml

UBUNTU_QML_MODULE_FILES += qml_files
