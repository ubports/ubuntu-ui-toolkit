TEMPLATE = aux

filetypes = qml png svg js jpg qmlproject desktop

OTHER_FILES = theme/parent_theme

for(filetype, filetypes) {
  OTHER_FILES += *.$$filetype
  OTHER_FILES += theme/*.$$filetype
}

desktop_files.path = $$[QT_INSTALL_EXAMPLES]/ubuntu-ui-toolkit/examples/
desktop_files.files = customtheme.desktop

other_files.path = $$[QT_INSTALL_EXAMPLES]/ubuntu-ui-toolkit/examples/customtheme
other_files.files = $$OTHER_FILES

INSTALLS += other_files desktop_files
