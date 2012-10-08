TEMPLATE = subdirs
SUBDIRS += demos modules/qt-components-ubuntu.pro tests/unit
#zsombi: the test cases are out as it doesn't link...
#SUBDIRS += tests/unit/unit-tests.pro

test.target = test
test.commands = cd tests/unit; make test
test.depends =
QMAKE_EXTRA_TARGETS += test
