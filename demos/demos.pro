TEMPLATE = subdirs

SUBDIRS += po

filetypes = qml png svg js qmltheme jpg

OTHER_FILES = ""

for(filetype, filetypes) {
  OTHER_FILES += *.$$filetype
}

OTHER_FILES += launch_componentshowcase launch_componentshowcase_phone

desktop_files.path = /usr/share/applications
desktop_files.files = qt-components-ubuntu-demos.desktop qt-components-ubuntu-phone-demo.desktop

other_files.path = /usr/lib/qt-components-ubuntu/demos
other_files.files = $$OTHER_FILES

INSTALLS += other_files desktop_files

