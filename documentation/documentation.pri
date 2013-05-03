QDOC = /usr/lib/*/qt5/bin/qdoc
system($$QDOC $$PWD/ubuntu-ui-toolkit-online.qdocconf)
system($$QDOC $$PWD/ubuntu-ui-toolkit-qtcreator.qdocconf)
system(qhelpgenerator "$$PWD/html/ubuntuuserinterfacetoolkit.qhp")

install_docs.files = $$PWD/html
install_docs.path = /usr/share/ubuntu-ui-toolkit/doc

install_qch.files = $$PWD/html/ubuntuuserinterfacetoolkit.qch
install_qch.path = /usr/share/qt5/doc/qch

INSTALLS += install_docs install_qch


