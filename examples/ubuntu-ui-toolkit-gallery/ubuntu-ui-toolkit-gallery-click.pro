TEMPLATE = subdirs

#load Ubuntu specific features
load(ubuntu-click)

# specify the manifest file, this file is required for click
# packaging and for the IDE to create runconfigurations
UBUNTU_MANIFEST_FILE=manifest.json.in

TARGET = ubuntu-ui-toolkit-gallery

QML_FILES += $$files(*.qml,true) \
             $$files(*.js,true)

CONF_FILES +=  ubuntu-ui-toolkit-gallery.apparmor \
               ubuntu-ui-toolkit-gallery.png

OTHER_FILES += $${CONF_FILES} \
               $${QML_FILES} \
               ubuntu-ui-toolkit-gallery.desktop

#specify where the qml/js files are installed to
qml_files.path = /
qml_files.files += $${QML_FILES}

#specify where the config files are installed to
config_files.path = /
config_files.files += $${CONF_FILES}

#install the desktop file, a translated version is
#automatically created in the build directory
desktop_file.path = /
desktop_file.files = ubuntu-ui-toolkit-gallery.desktop
desktop_file.CONFIG += no_check_exist

INSTALLS+=config_files qml_files desktop_file
