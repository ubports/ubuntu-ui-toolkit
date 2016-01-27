TEMPLATE = aux

MO_FILES = $$system(ls *.po)

install_mo_commands =
for(po_file, MO_FILES) {
  mo_file = $$replace(po_file,.po,.mo)
  system(msgfmt $$po_file -o $$mo_file)
  mo_name = $$replace(mo_file,.mo,)
  mo_targetpath = $(INSTALL_ROOT)/$$[QT_INSTALL_PREFIX]/share/locale/$${mo_name}/LC_MESSAGES
  mo_target = $${mo_targetpath}/ubuntu-ui-toolkit-gallery.mo
  !isEmpty(install_mo_commands): install_mo_commands += &&
  install_mo_commands += test -d $$mo_targetpath || mkdir -p $$mo_targetpath
  install_mo_commands += && cp $$PWD/$$mo_file $$mo_target
}

install.commands = $$install_mo_commands
QMAKE_EXTRA_TARGETS += install
