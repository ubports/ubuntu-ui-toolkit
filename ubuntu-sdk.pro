include( qtversioncheck.pri )

TEMPLATE = subdirs
SUBDIRS += demos modules/qt-components-ubuntu.pro tests/unit

# additional 'make test' target required by continuous integration system
test.target = test
test.commands = make check
QMAKE_EXTRA_TARGETS += test
