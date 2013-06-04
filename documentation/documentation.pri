install_docs.files = $$PWD/html
install_docs.path = /usr/share/ubuntu-ui-toolkit/doc
install_docs.depends = docs

install_qch.files = $$PWD/html/ubuntuuserinterfacetoolkit.qch
install_qch.path = /usr/share/qt5/doc/qch
install_qch.depends = docs

INSTALLS += install_docs install_qch


