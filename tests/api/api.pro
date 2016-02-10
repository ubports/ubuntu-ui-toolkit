TEMPLATE = subdirs

SUBDIRS = Extinct

OTHER_FILES += test.sh

check.commands += $$PWD/test.sh || exit 1;
