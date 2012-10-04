TEMPLATE = subdirs
SUBDIRS += demos modules/qt-components-ubuntu.pro \
    tests/unit/unit-tests.pro

test.target = test
test.commands = cd tests/unit; make test
test.depends =
QMAKE_EXTRA_TARGETS += test
