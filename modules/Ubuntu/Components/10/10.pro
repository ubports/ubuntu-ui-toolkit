TEMPLATE = subdirs

uri = Ubuntu.Components.10
installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)

qml_files.path = $$installPath
qml_files.files = *.qml

INSTALLS += qml_files
