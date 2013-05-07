TEMPLATE = subdirs

filetypes = qml png svg js qmltheme jpg qmlproject desktop wav

OTHER_FILES = ""

for(filetype, filetypes) {
  OTHER_FILES += *.$$filetype
}

desktop_files.path = /usr/share/applications
desktop_files.files = jokes.desktop

other_files.path = /usr/lib/ubuntu-ui-toolkit/examples/jokes
other_files.files = $$OTHER_FILES

INSTALLS += other_files desktop_files

