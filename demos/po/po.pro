TEMPLATE = subdirs

system(./generate_mo.sh)

MO_FILES = $$system(ls *.mo)

install_mo_commands =
for(mo_file, MO_FILES) {
  mo_name = $$replace(mo_file,.mo,)
  mo_targetpath = $(INSTALL_ROOT)/usr/share/locale/$${mo_name}/LC_MESSAGES
  mo_target = $${mo_targetpath}/componentshowcase.mo
  !isEmpty(install_mo_commands): install_mo_commands += &&
  install_mo_commands += test -d $$mo_targetpath || mkdir -p $$mo_targetpath
  install_mo_commands += && cp $$mo_file $$mo_target
}

install.commands = $$install_mo_commands
QMAKE_EXTRA_TARGETS += install
