include(../test-include.pri)
QT += gui
DEFINES += SRCDIR=\\\"$$PWD/\\\"

LOCALIZED_APP_DOMAIN = localizedApp
RELATIVE_TIME_DOMAIN = relativeTime
mo.target = mo
mo.commands = set -e;
mo.commands += echo Generating localization;
mo.commands += msgfmt po/en_US.po -o $${LOCALIZED_APP_DOMAIN}/share/locale/en/LC_MESSAGES/$${LOCALIZED_APP_DOMAIN}.mo;
mo.commands += msgfmt po/en_US.po -o $${RELATIVE_TIME_DOMAIN}/share/locale/en/LC_MESSAGES/$${RELATIVE_TIME_DOMAIN}.mo;
QMAKE_EXTRA_TARGETS += mo
PRE_TARGETDEPS += mo

SOURCES += \
    src/tst_i18n_LocalizedApp.cpp \
    src/tst_i18n_RelativeTime.cpp

OTHER_FILES += \
    src/LocalizedApp.qml \
    src/RelativeTime.qml
