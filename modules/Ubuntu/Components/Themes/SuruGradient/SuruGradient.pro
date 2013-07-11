TEMPLATE = subdirs

uri = Ubuntu.Components.Themes.SuruGradient
installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)

PARENT_THEME_FILE = parent_theme
QMLDIR_FILE = qmldir
QML_FILES = *.qml
ARTWORK_FILES += artwork/*.png \
                 artwork/*.svg \
                 artwork/*.sci

parent_theme_file.path = $$installPath
parent_theme_file.files = $$PARENT_THEME_FILE

qmldir_file.path = $$installPath
qmldir_file.files = $$QMLDIR_FILE

qml_files.path = $$installPath
qml_files.files = $$QML_FILES

artwork_files.path = $$installPath/artwork
artwork_files.files = $$ARTWORK_FILES


INSTALLS += parent_theme_file qmldir_file qml_files artwork_files
OTHER_FILES += $$PARENT_THEME_FILE $$QMLDIR_FILE $$QML_FILES $$ARTWORK_FILES
