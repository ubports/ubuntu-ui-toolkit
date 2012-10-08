include( qtversioncheck.pri )

TEMPLATE = subdirs
SUBDIRS += demos modules/qt-components-ubuntu.pro tests/unit

test.target = test
test.commands = cd tests/unit; make test
test.depends =
QMAKE_EXTRA_TARGETS += test
