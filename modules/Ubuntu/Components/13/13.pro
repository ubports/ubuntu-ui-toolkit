TEMPLATE = aux

CONFIG+=ubuntu_qml_module

uri = Ubuntu.Components.11
installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)

qml_files.installPath = $$installPath
qml_files.files = *.qml ListItems/*.qml Pickers/*.qml Popups/*.qml

# javascript files
js_files.installPath = $$installPath
js_files.files = *.js ListItems/*.js Pickers/*.js Popups/*.js

UBUNTU_QML_MODULE_FILES += qml_files js_files
