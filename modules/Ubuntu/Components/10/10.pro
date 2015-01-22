TEMPLATE = aux

CONFIG+=ubuntu_qml_module

uri = Ubuntu.Components.10
installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)

qml_files.installPath = $$installPath
qml_files.files = *.qml \
                  *.txt

UBUNTU_QML_MODULE_FILES += qml_files
#INSTALLS += qml_files
