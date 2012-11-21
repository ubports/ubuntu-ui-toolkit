include( ../qtversioncheck.pri )

TEMPLATE = subdirs

OTHER_FILES += Ambiance/qmltheme/*.qmltheme

theme_files.path = /usr/share/themes/Ambiance/qmltheme
theme_files.files = $$OTHER_FILES

ARTWORK_FILES += \
    Ambiance/qmltheme/artwork/*.png \
    Ambiance/qmltheme/artwork/*.sci
theme_artworks.path = /usr/share/themes/Ambiance/qmltheme/artwork
theme_artworks.files = $$ARTWORK_FILES

INSTALLS += theme_files theme_artworks
