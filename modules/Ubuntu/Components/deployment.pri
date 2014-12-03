uri = Ubuntu.Components
installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)

# Components
QML_FILES = $$system(ls *.qml)
JS_FILES = $$system(ls *.js)

# Components/artwork
ARTWORK_FILES = $$system(find artwork -type f)

# Components/Listitems
LISTITEMS_FILES = $$system(find ListItems -maxdepth 1 -type f)
LISTITEMS_ARTWORK_FILES = $$system(find ListItems/artwork -type f)

# Components/Popups
POPUPS_FILES = $$system(find Popups -maxdepth 1 -type f)

#Components/Pickers
PICKER_FILES = $$system(find Pickers -maxdepth 1 -type f)

#Style API
STYLES_FILES = $$system(find Styles -maxdepth 1 -type f)

# qmldir
QMLDIR_FILE = qmldir

# make found deployables visible in Qt Creator
OTHER_FILES += $$QML_FILES $$JS_FILES $$ARTWORK_FILES $$LISTITEMS_FILES $$POPUPS_FILES $$PICKERS_FILES $$STYLES_FILES $$QMLDIR_FILE $$LISTITEMS_ARTWORK_FILES

# define deployment for found deployables
qmldir_file.path = $$installPath
qmldir_file.files = $$QMLDIR_FILE
qml_files.path = $$installPath
qml_files.files = $$QML_FILES
js_files.path = $$installPath
js_files.files = $$JS_FILES
artwork_files.path = $$installPath/artwork
artwork_files.files = $$ARTWORK_FILES
listitems_files.path = $$installPath/ListItems
listitems_files.files = $$LISTITEMS_FILES
listitems_artwork_files.path = $$installPath/ListItems/artwork
listitems_artwork_files.files = $$LISTITEMS_ARTWORK_FILES
popups_files.path = $$installPath/Popups
popups_files.files = $$POPUPS_FILES
picker_files.path = $$installPath/Pickers
picker_files.files = $$PICKER_FILES
styles_files.path = $$installPath/Styles
styles_files.files = $$STYLES_FILES

plugins_qmltypes.path = $$installPath
plugins_qmltypes.files = plugins.qmltypes
# Silence spam on stderr due to fonts
# https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1256999
# https://bugreports.qt-project.org/browse/QTBUG-36243
plugins_qmltypes.extra = ALARM_BACKEND=memory $$[QT_INSTALL_BINS]/qmlplugindump -notrelocatable Ubuntu.Components 0.1 ../../ 2>/dev/null > $(INSTALL_ROOT)/$$installPath/plugins.qmltypes

INSTALLS += qmldir_file qml_files js_files artwork_files listitems_files listitems_artwork_files popups_files picker_files styles_files plugins_qmltypes
