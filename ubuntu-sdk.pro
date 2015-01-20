include( documentation/documentation.pri )

TEMPLATE = subdirs
SUBDIRS += modules/ubuntu-ui-toolkit.pro tests examples po

tests.depends = sub-modules-ubuntu-ui-toolkit-pro
examples.depends = sub-modules-ubuntu-ui-toolkit-pro

# additional 'make test' target required by continuous integration system
test.target = test
test.commands = make check
test.depends = license sub-modules-ubuntu-ui-toolkit-pro
QMAKE_EXTRA_TARGETS += test

test_performance.target = test-performance
test_performance.commands = cd $$shadowed($$_PRO_FILE_PWD_)/tests/unit/tst_performance; make check
test_performance.depends = sub-modules-ubuntu-ui-toolkit-pro sub-tests
QMAKE_EXTRA_TARGETS += test_performance

test_api.target = test-api
test_api.commands = cd $$shadowed($$_PRO_FILE_PWD_)/tests/unit/tst_components; make check
test_api.depends = sub-modules-ubuntu-ui-toolkit-pro sub-tests
QMAKE_EXTRA_TARGETS += test_api

test_components.target = test-components
test_components.commands = cd $$shadowed($$_PRO_FILE_PWD_)/tests/autopilot;
test_components.commands += env UITK_BUILD_ROOT="$${ROOT_BUILD_DIR}" UITK_SOURCE_ROOT="$${ROOT_SOURCE_DIR}" UBUNTU_UI_TOOLKIT_AUTOPILOT_FROM_SOURCE=1
test_components.commands += python3 -m autopilot.run run ubuntuuitoolkit
test_components.depends = sub-modules-ubuntu-ui-toolkit-pro
QMAKE_EXTRA_TARGETS += test_components

test_components2.target = test-components2
test_components2.commands  = cd $$PWD/tests/autopilot;
test_components2.commands += env UITK_BUILD_ROOT="$${ROOT_BUILD_DIR}" UITK_SOURCE_ROOT="$${ROOT_SOURCE_DIR}" UBUNTU_UI_TOOLKIT_AUTOPILOT_FROM_SOURCE=1
test_components2.commands += python2 -m autopilot.run run ubuntuuitoolkit
test_components2.depends = sub-modules-ubuntu-ui-toolkit-pro
QMAKE_EXTRA_TARGETS += test_components2

qmluitests.target = qmluitests
qmluitests.commands = cd $$shadowed($$_PRO_FILE_PWD_)/tests/unit_x11; make check
qmluitests.depends = sub-modules-ubuntu-ui-toolkit-pro sub-tests
QMAKE_EXTRA_TARGETS += qmluitests

license.target = license
license.commands = cd $$PWD; $$PWD/tests/license/checklicense.sh
QMAKE_EXTRA_TARGETS += license

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
