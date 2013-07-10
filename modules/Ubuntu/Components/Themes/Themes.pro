TEMPLATE = subdirs
SUBDIRS += Ambiance

uri = Ubuntu.Components.Themes
installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)

qmldir_file.path = $$installPath
qmldir_file.files = qmldir

qml_files.path = $$installPath
qml_files.files = *.qml

INSTALLS += qmldir_file qml_files
