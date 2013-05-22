include( common.pri )
include( documentation/documentation.pri )

TEMPLATE = subdirs
SUBDIRS += themes modules/ubuntu-ui-toolkit.pro tests examples

tests/unit.depends = modules/ubuntu-ui-toolkit.pro

# additional 'make test' target required by continuous integration system
test.target = test
test.commands = make check
test.depends = license
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
test_components.commands = cd tests/autopilot; autopilot run UbuntuUiToolkit
test_components.depends = modules/ubuntu-ui-toolkit.pro
QMAKE_EXTRA_TARGETS += test_components

license.target = license
license.commands = ./tests/license/checklicense.sh
QMAKE_EXTRA_TARGETS += license

DOC_PATH=${PWD}/documentation
docs.target = docs
docs.commands += qdoc $$DOC_PATH/ubuntu-ui-toolkit-qtcreator.qdocconf;
docs.commands += qhelpgenerator -o "$$DOC_PATH/html/ubuntuuserinterfacetoolkit.qch" "$$DOC_PATH/html/ubuntuuserinterfacetoolkit.qhp"
QMAKE_EXTRA_TARGETS += docs

