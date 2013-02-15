QDOC = /usr/lib/*/qt5/bin/qdoc
system($$QDOC $$PWD/ubuntu-sdk.qdocconf)

install_docs.files = $$PWD/html
install_docs.path = /usr/share/ubuntu-ui-toolkit/doc

INSTALLS += install_docs
