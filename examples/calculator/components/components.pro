TEMPLATE = aux

filetypes = qml png svg js qmltheme jpg qmlproject desktop wav

OTHER_FILES = ""

for(filetype, filetypes) {
  OTHER_FILES += *.$$filetype
}

other_files.path = /usr/lib/ubuntu-ui-toolkit/examples/calculator/components
other_files.files = $$OTHER_FILES

INSTALLS += other_files

