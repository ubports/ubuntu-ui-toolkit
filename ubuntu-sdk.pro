#include( documentation/documentation.pri )

BUILD_PATH_CONTENTS="SRC_DIR=\"$$PWD\"" \
                    "BUILD_DIR=\"$$OUT_PWD\""

write_file($$PWD/build_paths.inc,BUILD_PATH_CONTENTS)

requires(qtHaveModule(quick))
load(qt_parts)

SUBDIRS += po documentation app-launch-profiler ubuntu-ui-toolkit-launcher

#when standalone we always want tests to be built
!build_with_qt{
    sub_tests.CONFIG -= no_default_target
    sub_tests.CONFIG -= no_default_install
}

# additional 'make test' target required by continuous integration system
test.target = test
test.commands = make check
test.depends = license sub_src
QMAKE_EXTRA_TARGETS += test

license.target = license
license.commands = cd $$PWD; $$PWD/tests/license/checklicense.sh
QMAKE_EXTRA_TARGETS += license

check.target = check
check.commands = $$PWD/tests/checkresults.sh $$PWD/tests/test_tst_*.xml

#helper files
OTHER_FILES += \
    features/ubuntu_qml_module.prf \
    features/ubuntu_qml_plugin.prf \
    features/ubuntu_enable_testing.prf \
    features/coverage.prf \
    .qmake.conf
