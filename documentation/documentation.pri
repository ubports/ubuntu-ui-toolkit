QDOC = /opt/qt5/bin/qdoc

doc.commands = $$QDOC $$PWD/ubuntu-sdk.qdocconf
doc.output = $$PWD/html

QMAKE_EXTRA_TARGETS += doc

html_files.path = /usr/share/qt-components-ubuntu/doc
html_files.files = $$doc.output

INSTALLS += html_files
