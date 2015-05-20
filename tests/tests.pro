TEMPLATE = subdirs
SUBDIRS += unit unit_x11 autopilot

autopilot_module.path = /usr/lib/python3/dist-packages/ubuntuuitoolkit
autopilot_module.files = autopilot/ubuntuuitoolkit/*

SUBDIRS += launcher

INSTALLS += autopilot_module

#check.commands += cd $${ROOT_BUILD_DIR};
#check.commands += $${ROOT_SOURCE_DIR}/tests/qmlapicheck.sh || exit 1;
#check.commands += cd $${ROOT_BUILD_DIR}/tests

OTHER_FILES += qmlapicheck.sh \
               qmlapicheck.py
