include( ../qtversioncheck.pri )

TEMPLATE = subdirs

OTHER_FILES += demo/qmltheme/*.qmltheme

theme_files.path = /usr/share/themes/demo/qmltheme
theme_files.files = $$OTHER_FILES

ARTWORK_FILES += demo/artwork/*.png
theme_artworks.path = /usr/share/themes/demo/artwork
theme_artworks.files = $$ARTWORK_FILES

INSTALLS += theme_files theme_artworks
