TEMPLATE = subdirs

# specify the manifest file, this file is required for click
# packaging and for the IDE to create runconfigurations
UBUNTU_MANIFEST_FILE=manifest.json.in

exists($$PWD/../examples.pro) {
    desktop_file.path = $$[QT_INSTALL_EXAMPLES]/ubuntu-ui-toolkit/examples/$${TARGET}
    config_files.path = $$[QT_INSTALL_EXAMPLES]/ubuntu-ui-toolkit/examples/$${TARGET}
    qml_files.path = $$[QT_INSTALL_EXAMPLES]/ubuntu-ui-toolkit/examples/$${TARGET}
    config_files.files += $${UBUNTU_MANIFEST_FILE}
} else {
    #load Ubuntu specific features
    load(ubuntu-click)
    qml_files.path = /
    config_files.path = /
    desktop_file.path = /
}

TARGET = calculator

SUBDIRS += components

QML_FILES += $$files(*.qml,true) \
             $$files(*.js,true) \
             components

CONF_FILES +=  $${TARGET}.apparmor \
               $${TARGET}.png \
	       $${TARGET}.wav

OTHER_FILES += $${CONF_FILES} \
               $${QML_FILES} \
               $${TARGET}.desktop \
            ``

#specify where the qml/js files are installed to
qml_files.files += $${QML_FILES}

#specify where the config files are installed to
config_files.files += $${CONF_FILES}

#install the desktop file, a translated version is
#automatically created in the build directory
desktop_file.files = $${TARGET}.desktop
desktop_file.CONFIG += no_check_exist

INSTALLS+=config_files qml_files desktop_file
