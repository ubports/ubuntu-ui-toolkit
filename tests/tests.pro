TEMPLATE = subdirs
SUBDIRS += unit autopilot

PYTHONDIR = $$system(python3 -c \"from distutils.sysconfig import get_python_lib; print(get_python_lib())\")

autopilot_module.path = $$PYTHONDIR/ubuntuuitoolkit
autopilot_module.files = autopilot/ubuntuuitoolkit/*

SUBDIRS += api

INSTALLS += autopilot_module

OTHER_FILES += qmlapicheck.sh

check.commands += $$PWD/packaging-sorting.sh || exit 1;
check.commands += $$PWD/qmlapicheck.sh || exit 1;
