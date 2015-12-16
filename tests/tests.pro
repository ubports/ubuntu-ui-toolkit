TEMPLATE = subdirs
SUBDIRS += unit unit_x11 autopilot

autopilot_module.path = $$[QT_INSTALL_PREFIX]/lib/python3/dist-packages/ubuntuuitoolkit
autopilot_module.files = autopilot/ubuntuuitoolkit/*

SUBDIRS += api

INSTALLS += autopilot_module

OTHER_FILES += qmlapicheck.sh

check.commands += ./qmlapicheck.sh || exit 1;
