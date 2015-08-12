TEMPLATE = aux

CONFIG+=ubuntu_qml_module

uri = Ubuntu.Components.Themes.Ambiance
installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)

QMLDIR_FILE = qmldir
ARTWORK_FILES += artwork/*.png \
                 artwork/*.svg \
                 artwork/*.sci

qmldir_file.installPath = $$installPath
qmldir_file.files = $$QMLDIR_FILE

artwork_files.installPath = $$installPath/artwork
artwork_files.files = $$ARTWORK_FILES

UBUNTU_QML_MODULE_FILES += qmldir_file artwork_files

