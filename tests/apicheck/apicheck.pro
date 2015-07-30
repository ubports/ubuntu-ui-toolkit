TEMPLATE = app
QT += qml quick qml-private
# For setSharedOpenGLContext
QT += core-private gui-private testlib quick-private
CONFIG += no_keywords
SOURCES += apicheck.cpp
installPath = $$[QT_INSTALL_LIBS]/ubuntu-ui-toolkit
apicheck.path = $$installPath
apicheck.files = apicheck
INSTALLS += apicheck

check.commands += $${ROOT_SOURCE_DIR}/tests/qmlapicheck.sh || exit 1;
check.commands += $${ROOT_SOURCE_DIR}/tests/apicheck/test/test.sh || exit 1;
