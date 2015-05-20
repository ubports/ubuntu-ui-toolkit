TEMPLATE = subdirs
SUBDIRS += unit unit_x11 autopilot

autopilot_module.path = /usr/lib/python3/dist-packages/ubuntuuitoolkit
autopilot_module.files = autopilot/ubuntuuitoolkit/*

SUBDIRS += launcher

SUBDIRS += apicheck

INSTALLS += autopilot_module

OTHER_FILES += qmlapicheck.sh
