TEMPLATE = aux

PROJECTNAME = ubuntu-ui-toolkit

SOURCECODE = `find .. -name *.qml`

message("")
message(" Project Name: $$PROJECTNAME ")
message(" Source Code: $$SOURCECODE ")
message("")
message(" run 'make pot' to generate the pot file from source code. ")
message(" run 'qmake; make install' to install the mo files. ")
message("")

## generate pot file 'make pot'
potfile.target = pot
potfile.commands = cd $$PWD; $$PWD/update-pot.sh ubuntu-ui-toolkit $$PWD/../modules .;
QMAKE_EXTRA_TARGETS += potfile

## generate pot file for gallery
potfile.commands += cd $$PWD; $$PWD/update-pot.sh ubuntu-ui-toolkit-gallery $$PWD/../examples/ubuntu-ui-toolkit-gallery $$PWD/../examples/ubuntu-ui-toolkit-gallery/po;

## Installation steps for mo files. 'make install'
MO_FILES = $$files(*.po)

message($$MO_FILES)

install_mo_commands =
for(po_file, MO_FILES) {
  mo_file = $$replace(po_file,.po,.mo)
  system(msgfmt $$po_file -o $$mo_file)
  mo_name = $$replace(mo_file,.mo,)
  mo_targetpath = $(INSTALL_ROOT)$$[QT_INSTALL_PREFIX]/share/locale/$${mo_name}/LC_MESSAGES
  mo_target = $${mo_targetpath}/ubuntu-ui-toolkit.mo
  !isEmpty(install_mo_commands): install_mo_commands += &&
  install_mo_commands += test -d $$mo_targetpath || mkdir -p $$mo_targetpath
  install_mo_commands += && cp $${PWD}/$$mo_file $$mo_target
}

install.commands = $$install_mo_commands

QMAKE_EXTRA_TARGETS += install

