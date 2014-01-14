TEMPLATE = subdirs
SUBDIRS += unit unit_x11

autopilot_module.path = /usr/lib/python3/dist-packages/ubuntuuitoolkit
autopilot_module.files = autopilot/ubuntuuitoolkit/*

INSTALLS += autopilot_module
