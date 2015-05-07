TEMPLATE = aux
CONFIG+=ubuntu_qml_module

uri = Ubuntu.Components.Themes.SuruDark
installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)

PARENT_THEME_FILE = parent_theme
QMLDIR_FILE = qmldir
ARTWORK_FILES += artwork/*.png \
                 artwork/*.svg \
                 artwork/*.sci

parent_theme_file.installPath = $$installPath
parent_theme_file.files = $$PARENT_THEME_FILE

qmldir_file.installPath = $$installPath
qmldir_file.files = $$QMLDIR_FILE

artwork_files.installPath = $$installPath/artwork
artwork_files.files = $$ARTWORK_FILES

UBUNTU_QML_MODULE_FILES += parent_theme_file qmldir_file artwork_files
