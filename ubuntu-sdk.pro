include( documentation/documentation.pri )

TEMPLATE = subdirs
SUBDIRS += modules/ubuntu-ui-toolkit.pro tests examples po

BUILD_PATH_CONTENTS="SRC_DIR=\"$$PWD\"" \
                    "BUILD_DIR=\"$$OUT_PWD\""

write_file($$PWD/build_paths.inc,BUILD_PATH_CONTENTS)

tests.depends = sub-modules-ubuntu-ui-toolkit-pro
examples.depends = sub-modules-ubuntu-ui-toolkit-pro

# additional 'make test' target required by continuous integration system
test.target = test
test.commands = make check
test.depends = license sub-modules-ubuntu-ui-toolkit-pro
QMAKE_EXTRA_TARGETS += test

license.target = license
license.commands = cd $$PWD; $$PWD/tests/license/checklicense.sh
QMAKE_EXTRA_TARGETS += license

check.target = check
check.commands = $$PWD/tests/checkresults.sh $$PWD/tests/test_tst_*.xml

DOC_SRC = $$PWD
equals(QT_MAJOR_VERSION, 5):lessThan(QT_MINOR_VERSION, 2) {
    DOC_SRC = $$PWD/documentation
}

DOC_PATH=$$shadowed($$_PRO_FILE_PWD_)/documentation
docs.target = docs
docs.commands = cd $$PWD; SRC=$$PWD/documentation BLD=$$ROOT_BUILD_DIR/documentation $$PWD/documentation/docs.sh $$DOC_PATH
QMAKE_EXTRA_TARGETS += docs

#helper files
OTHER_FILES += \
    features/ubuntu_qml_module.prf \
    features/coverage.prf \
    .qmake.conf
