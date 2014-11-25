install_docs.files = $$shadowed($$_PRO_FILE_PWD_)/documentation/html
install_docs.path = /usr/share/ubuntu-ui-toolkit/doc
install_docs.depends = docs

install_qch.files = $$shadowed($$_PRO_FILE_PWD_)/documentation/ubuntuuserinterfacetoolkit.qch
install_qch.path = /usr/share/qt5/doc/qch/
install_qch.depends = docs

INSTALLS += install_qch install_docs


