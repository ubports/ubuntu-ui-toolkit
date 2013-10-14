TEMPLATE = subdirs

PROJECTNAME = ubuntu-ui-toolkit

SOURCECODE = `find .. -name *.qml`

message("")
message(" Project Name: $$PROJECTNAME ")
message(" Source Code: $$SOURCECODE ")
message("")
message(" run 'make pot' to generate the pot file from source code. ")
message(" run 'make mo' to generate the mo files from po files. ")
message(" run 'qmake; make install' to install the mo files. ")
message("")

## generate pot file 'make pot'
potfile.target = pot
potfile.commands = ./update-pot.sh
QMAKE_EXTRA_TARGETS += potfile

## generate mo files 'make mo'
mofiles.target = mo
mofiles.commands = ./generate_mo.sh
QMAKE_EXTRA_TARGETS += mofiles

## Installation steps for mo files. 'make install'
MO_FILES = $$system(ls locale/*/LC_MESSAGES/*.mo)

mo_files.path= /usr/share/locale/
for(mo_file, MO_FILES) {
  mo_name = $$replace(mo_file,.mo,)
  mo_files.files += $${mo_name}/LC_MESSAGES/ubuntu-ui-toolkit.mo
}
INSTALLS += mo_files

