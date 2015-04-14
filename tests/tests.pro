TEMPLATE = subdirs
SUBDIRS += unit unit_x11 autopilot

autopilot_module.path = /usr/lib/python3/dist-packages/ubuntuuitoolkit
autopilot_module.files = autopilot/ubuntuuitoolkit/*

SUBDIRS += launcher

INSTALLS += autopilot_module

check.commands += cd ..;
check.commands += tests/qmlapicheck.sh || exit 1;
check.commands += cd tests
