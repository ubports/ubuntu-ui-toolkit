TEMPLATE=aux

CONFIG+=ubuntu_qml_module

uri = Ubuntu.Components.Themes.Ambiance
installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)/1.3

QML_FILES = *.qml

qml_files.installPath = $$installPath
qml_files.files = $$QML_FILES

UBUNTU_QML_MODULE_FILES += qml_files

