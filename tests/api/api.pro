TEMPLATE = subdirs

SUBDIRS = Extinct

OTHER_FILES += test.sh

check.commands += ./test.sh || exit 1;
