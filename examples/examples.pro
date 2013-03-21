TEMPLATE = subdirs

examples = jokes unit-converter
filetypes = qml png svg js qmltheme jpg wav
 
OTHER_FILES = ""

for(example, examples) {
  for(filetype, filetypes) {
    OTHER_FILES += $$example/*.$$filetype
  }
}

other_files.path = /usr/lib/ubuntu-ui-toolkit/examples
other_files.files = $$OTHER_FILES

INSTALLS += other_files

