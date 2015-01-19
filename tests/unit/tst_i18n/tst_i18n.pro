include(../test-include.pri)
QT += gui
DEFINES += SRCDIR=\\\"$$PWD/\\\"

system(msgfmt po/en_US.po -o localizedApp/share/locale/en/LC_MESSAGES/localizedApp.mo)

SOURCES += \
    src/tst_i18n.cpp

OTHER_FILES += \
    src/LocalizedApp.qml
