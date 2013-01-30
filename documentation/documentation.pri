QDOC = /usr/bin/qdoc
system($$QDOC $$PWD/ubuntu-sdk.qdocconf)

install_docs.files = $$PWD/html
install_docs.path = /usr/share/qt-components-ubuntu/doc

INSTALLS += install_docs
