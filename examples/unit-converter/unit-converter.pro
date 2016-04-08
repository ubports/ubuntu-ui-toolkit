TEMPLATE = subdirs

#load Ubuntu specific features
load(ubuntu-click)

# specify the manifest file, this file is required for click
# packaging and for the IDE to create runconfigurations
UBUNTU_MANIFEST_FILE=manifest.json.in

TARGET = unit-converter

QML_FILES += $$files(*.qml,true) \
             $$files(*.js,true)

CONF_FILES +=  $${TARGET}.apparmor \
               $${TARGET}.png

OTHER_FILES += $${CONF_FILES} \
               $${QML_FILES} \
               $${TARGET}.desktop

#specify where the qml/js files are installed to
qml_files.path = /
qml_files.files += $${QML_FILES}

#specify where the config files are installed to
config_files.path = /
config_files.files += $${CONF_FILES}

#install the desktop file, a translated version is
#automatically created in the build directory
desktop_file.path = /
desktop_file.files = $${TARGET}.desktop
desktop_file.CONFIG += no_check_exist

exists($$PWD/../examples.pro) {
	desktop_file.path = $$[QT_INSTALL_EXAMPLES]/ubuntu-ui-toolkit/examples/$${TARGET}
	config_files.path = $$[QT_INSTALL_EXAMPLES]/ubuntu-ui-toolkit/examples/$${TARGET}
	qml_files.path = $$[QT_INSTALL_EXAMPLES]/ubuntu-ui-toolkit/examples/$${TARGET}
}

INSTALLS+=config_files qml_files desktop_file
