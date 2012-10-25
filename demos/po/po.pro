TEMPLATE = subdirs

system(./generate_mo.sh)

MO_FILES = $$system(ls *.mo)

install_mo_commands =
for(mo_file, MO_FILES) {
  mo_name = $$replace(mo_file,.mo,)
  mo_targetpath = /usr/share/locale/$${mo_name}/LC_MESSAGES
  mo_target = $${mo_targetpath}/gallery.mo
  !isEmpty(install_mo_commands): install_mo_commands += &&
  install_mo_commands += mkdir -p \"$$mo_targetpath\"
  install_mo_commands += && cp \"$$mo_file\" \"$$mo_target\"
}

install_mo.commands = $$install_mo_commands
QMAKE_EXTRA_TARGETS += install_mo
