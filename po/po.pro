TEMPLATE = subdirs

PROJECTNAME = $$system(basename ../*.qmlproject)
PROJECTNAME = $$replace(PROJECTNAME,.qmlproject,)

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
#potfile.commands = xgettext -o $${PROJECTNAME}.pot --package-name $${PROJECTNAME} --qt --c++ --add-comments=TRANSLATORS --keyword=tr $${SOURCECODE}
potfile.commands = ./update-pot.sh
QMAKE_EXTRA_TARGETS += potfile

## generate mo files 'make mo'
mofiles.target = mo
#mofiles.commands = msgfmt *.po
mofiles.commands = ./generate_mo.sh
QMAKE_EXTRA_TARGETS += mofiles

## Installation steps for mo files. 'make install'
MO_FILES = $$system(ls *.mo)

install_mo_commands =
for(mo_file, MO_FILES) {
  mo_name = $$replace(mo_file,.mo,)
  mo_targetpath = $(INSTALL_ROOT)/usr/share/locale/$${mo_name}/LC_MESSAGES
  mo_target = $${mo_targetpath}/$${PROJECTNAME}.mo
  !isEmpty(install_mo_commands): install_mo_commands += &&
  install_mo_commands += test -d $$mo_targetpath || mkdir -p $$mo_targetpath
  install_mo_commands += && cp $$mo_file $$mo_target
}

install.commands = $$install_mo_commands
QMAKE_EXTRA_TARGETS += install

