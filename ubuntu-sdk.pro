include( documentation/documentation.pri )

TEMPLATE = subdirs
SUBDIRS += modules/ubuntu-ui-toolkit.pro tests examples po/po.pro

tests.depends = modules/ubuntu-ui-toolkit.pro

# additional 'make test' target required by continuous integration system
test.target = test
test.commands = make check
test.depends = license modules/ubuntu-ui-toolkit.pro
QMAKE_EXTRA_TARGETS += test

test_performance.target = test-performance
test_performance.commands = cd tests/unit/tst_performance; make check
test_performance.depends = modules/ubuntu-ui-toolkit.pro
QMAKE_EXTRA_TARGETS += test_performance

test_api.target = test-api
test_api.commands = cd tests/unit/tst_components; make check
test_api.depends = modules/ubuntu-ui-toolkit.pro
QMAKE_EXTRA_TARGETS += test_api

test_components.target = test-components
test_components.commands = cd tests/autopilot; python3 -m autopilot.run run ubuntuuitoolkit
test_components.depends = modules/ubuntu-ui-toolkit.pro
QMAKE_EXTRA_TARGETS += test_components

test_components2.target = test-components2
test_components2.commands = cd tests/autopilot; python2 -m autopilot.run run ubuntuuitoolkit
test_components2.depends = modules/ubuntu-ui-toolkit.pro
QMAKE_EXTRA_TARGETS += test_components2

qmluitests.target = qmluitests
qmluitests.commands = cd tests/unit_x11; make check
qmluitests.depends = modules/ubuntu-ui-toolkit.pro
QMAKE_EXTRA_TARGETS += qmluitests

license.target = license
license.commands = ./tests/license/checklicense.sh
QMAKE_EXTRA_TARGETS += license

DOC_SRC = .
equals(QT_MAJOR_VERSION, 5):lessThan(QT_MINOR_VERSION, 2) {
    DOC_SRC = documentation
}

DOC_PATH=$$system(pwd)/documentation
docs.target = docs
docs.commands = SRC=$$DOC_SRC ./documentation/docs.sh $$DOC_PATH;
QMAKE_EXTRA_TARGETS += docs

