QDOC = /opt/qt5/bin/qdoc

doc.commands = $$QDOC $$PWD/ubuntu-sdk.qdocconf
doc.files = $$PWD/html
doc.path = /usr/share/qt-components-ubuntu/doc

QMAKE_EXTRA_TARGETS += doc
INSTALLS += doc
