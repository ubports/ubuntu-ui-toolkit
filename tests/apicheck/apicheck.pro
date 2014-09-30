TEMPLATE = app
QT += qml quick declarative declarative-private gui qml-private
# For setSharedOpenGLContext
QT += core-private gui-private testlib quick-private
CONFIG += no_keywords
SOURCES += apicheck.cpp
installPath = $$[QT_INSTALL_LIBS]/ubuntu-ui-toolkit
apicheck.path = $$installPath
apicheck.files = apicheck
INSTALLS += apicheck

check.commands += cd ../..;
check.commands += sh tests/apicheck/apicheck.sh || exit 1;
check.commands += cd tests/apicheck;
