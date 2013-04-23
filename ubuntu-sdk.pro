include( common.pri )
include( documentation/documentation.pri )

TEMPLATE = subdirs
SUBDIRS += demos themes modules/ubuntu-ui-toolkit.pro tests/unit examples

tests/unit.depends = modules/ubuntu-ui-toolkit.pro

# additional 'make test' target required by continuous integration system
test.target = test
test.commands = make check
test.depends = license
QMAKE_EXTRA_TARGETS += test

license.target = license
license.commands = ./tests/license/checklicense.sh
QMAKE_EXTRA_TARGETS += license
