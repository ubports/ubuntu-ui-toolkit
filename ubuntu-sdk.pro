include( documentation/documentation.pri )

TEMPLATE = subdirs
SUBDIRS += modules/ubuntu-ui-toolkit.pro tests examples po/po.pro

tests/unit.depends = modules/ubuntu-ui-toolkit.pro

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
test_components.commands = cd tests/autopilot; autopilot run ubuntuuitoolkit
test_components.depends = modules/ubuntu-ui-toolkit.pro
QMAKE_EXTRA_TARGETS += test_components

qmluitests.target = qmluitests
qmluitests.commands = cd tests/unit_x11; make check
qmluitests.depends = modules/ubuntu-ui-toolkit.pro
QMAKE_EXTRA_TARGETS += qmluitests

license.target = license
license.commands = ./tests/license/checklicense.sh
QMAKE_EXTRA_TARGETS += license

DOC_PATH=$$system(pwd)/documentation
docs.target = docs
docs.commands += qdoc $$DOC_PATH/ubuntu-ui-toolkit-qtcreator.qdocconf 2> $$DOC_PATH/qdoc.err;
docs.commands += cat $$DOC_PATH/qdoc.err;
docs.commands += test ! -s $$DOC_PATH/qdoc.err || exit 1;
docs.commands += qhelpgenerator -o "$$DOC_PATH/html/ubuntuuserinterfacetoolkit.qch" "$$DOC_PATH/html/ubuntuuserinterfacetoolkit.qhp";
docs.commands += qdoc $$DOC_PATH/ubuntu-ui-toolkit-online.qdocconf;
docs.commands += $$DOC_PATH/fix-markup.sh $$DOC_PATH;
QMAKE_EXTRA_TARGETS += docs

