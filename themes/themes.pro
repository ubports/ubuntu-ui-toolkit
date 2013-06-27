include( ../common.pri )

TEMPLATE = subdirs

THEME_FILES = Ambiance/qmltheme/*.qmltheme
QML_FILES = $$system(ls Ambiance/qmltheme/*.qml)
QML_FILES += $$system(ls Ambiance/qmltheme/*.js)
QMLDIR_FILE = Ambiance/qmltheme/qmldir

OTHER_FILES += $$THEME_FILES $$QML_FILES $$QMLDIR_FILE

theme_files.path = /usr/share/themes/Ambiance/qmltheme
theme_files.files = $$THEME_FILES

ARTWORK_FILES += \
    Ambiance/qmltheme/artwork/*.png \
    Ambiance/qmltheme/artwork/*.svg \
    Ambiance/qmltheme/artwork/*.sci
theme_artworks.path = /usr/share/themes/Ambiance/qmltheme/artwork
theme_artworks.files = $$ARTWORK_FILES

qmldir_file.path = /usr/share/themes/Ambiance/qmltheme
qmldir_file.files = $$QMLDIR_FILE

qml_files.path = /usr/share/themes/Ambiance/qmltheme
qml_files.files = $$QML_FILES

INSTALLS += theme_files theme_artworks qmldir_file qml_files
