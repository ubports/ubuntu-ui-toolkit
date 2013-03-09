QDOC = /usr/lib/*/qt5/bin/qdoc
system($$QDOC $$PWD/ubuntu-ui-toolkit-online.qdocconf)

install_docs.files = $$PWD/html
install_docs.path = /usr/share/qt-components-ubuntu/doc

INSTALLS += install_docs
