TEMPLATE = aux

filetypes = qml png svg js jpg qmlproject desktop

OTHER_FILES = ""

for(filetype, filetypes) {
  OTHER_FILES += *.$$filetype
}


desktop_files.path = $$[QT_INSTALL_EXAMPLES]/ubuntu-ui-toolkit/examples/unit-converter
desktop_files.files = unit-converter.desktop

other_files.path = $$[QT_INSTALL_EXAMPLES]/ubuntu-ui-toolkit/examples/unit-converter
other_files.files = $$OTHER_FILES

INSTALLS += other_files desktop_files

