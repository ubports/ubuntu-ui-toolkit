TEMPLATE = subdirs

SUBDIRS += ListItems \
           ComponentModule.pro \
           Pickers   \
           Popups    \
           Styles    \
           plugin    \
           Themes    \
           10        \
           11

plugins_qmltypes.path = $$installPath
plugins_qmltypes.files = plugins.qmltypes
# Silence spam on stderr due to fonts
# https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1256999
# https://bugreports.qt-project.org/browse/QTBUG-36243
plugins_qmltypes.extra = ALARM_BACKEND=memory $$[QT_INSTALL_BINS]/qmlplugindump -notrelocatable Ubuntu.Components 0.1 ../../ 2>/dev/null > $(INSTALL_ROOT)/$$installPath/plugins.qmltypes
INSTALLS += plugins_qmltypes
