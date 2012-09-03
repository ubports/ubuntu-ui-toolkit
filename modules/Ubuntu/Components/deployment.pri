# deployment rules for the plugin
installPath = $$[QT_INSTALL_IMPORTS]/$$replace(uri, \\., /)
target.path = $$installPath
INSTALLS += target

# Components
QML_FILES = $$system(ls *.qml)
JS_FILES = $$system(ls *.js)
QDOC_FILES = $$system(ls *.qdoc)

# Components/artwork
ARTWORK_FILES = $$system(find artwork -type f)

# Components/Listitems
LISTITEMS_FILES = $$system(find ListItems -type f)

# qmldir
QMLDIR_FILE = qmldir

# make found deployables visible in Qt Creator
OTHER_FILES += $$QML_FILES $$JS_FILES $$QDOC_FILES $$ARTWORK_FILES $$LISTITEMS_FILES $$QMLDIR_FILE

# define deployment for found deployables
qmldir_file.path = $$installPath
qmldir_file.files = $$QMLDIR_FILE
qml_files.path = $$installPath
qml_files.files = $$QML_FILES
js_files.path = $$installPath
js_files.files = $$JS_FILES
qdoc_files.path = $$installPath
qdoc_files.files = $$QDOC_FILES
artwork_files.path = $$installPath/artwork
artwork_files.files = $$ARTWORK_FILES
listitems_files.path = $$installPath/ListItems
listitems_files.files = $$LISTITEMS_FILES

INSTALLS += qmldir_file qml_files js_files qdoc_files artwork_files listitems_files
