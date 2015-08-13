include(../../test-include.pri)
QT += gui
DEFINES += SRCDIR=\\\"$$PWD/\\\"

DOMAIN = ubuntu-ui-toolkit
mo.target = mo
mo.commands = set -e;
mo.commands += echo Generating localization;
mo.commands += msgfmt $${PWD}/po/en_US.po -o $${PWD}/$${DOMAIN}/share/locale/en/LC_MESSAGES/$${DOMAIN}.mo;
QMAKE_EXTRA_TARGETS += mo
PRE_TARGETDEPS += mo

SOURCES += \
    src/tst_i18n_RelativeTime.cpp

OTHER_FILES += \
    src/RelativeTime.qml
