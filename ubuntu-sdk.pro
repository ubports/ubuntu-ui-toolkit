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

license.target = license
license.commands = ./tests/license/checklicense.sh
QMAKE_EXTRA_TARGETS += license

DOC_PATH=${PWD}/documentation
docs.target = docs
docs.commands += qdoc $$DOC_PATH/ubuntu-ui-toolkit-qtcreator.qdocconf;
docs.commands += qhelpgenerator -o "$$DOC_PATH/html/ubuntuuserinterfacetoolkit.qch" "$$DOC_PATH/html/ubuntuuserinterfacetoolkit.qhp";
docs.commands += qdoc $$DOC_PATH/ubuntu-ui-toolkit-online.qdocconf;
docs.commands += $$DOC_PATH/fix-markup.sh $$DOC_PATH;
QMAKE_EXTRA_TARGETS += docs

