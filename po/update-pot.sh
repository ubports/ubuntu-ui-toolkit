#!/bin/sh
#
# Copyright 2012 Canonical Ltd.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; version 3.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#~  Usage:
#~    update-pot.sh [DOMAIN] [SOURCE_DIR] [PO_DIR]
#~
#~  Example:
#~    update-pot.sh ubuntu-ui-toolkit modules po

set -e
if [ -z "$3" ]; then
    grep -e '^#~' $0 | sed s/#~//
    exit 0
fi

DOMAIN=$1
SOURCE_DIR=$(readlink -f $2)
PO_DIR=$(readlink -f $3)

PROGNAME=$(basename $0)

# Create a list of files to scan
GETTEXT_FILES=$(mktemp --tmpdir $DOMAIN.lst.XXXXX)
trap 'rm -f "$GETTEXT_FILES"' EXIT
cd $SOURCE_DIR
find \( -name '*.cpp' -o -name '*.qml' -o -name '*.js' -o -name '*.desktop' \) \
    -a ! \( -path './build/*' \) | sort \
> $GETTEXT_FILES

# Generate pot from our list
xgettext \
    --output $PO_DIR/$DOMAIN.pot \
    --files-from $GETTEXT_FILES \
    --qt --c++ \
    --add-comments=TRANSLATORS \
    --keyword=tr \
    --keyword=tr:1,2 \
    --keyword=dtr:2 \
    --keyword=dtr:2,3 \
    --package-name $DOMAIN \
    --package-name $DOMAIN \
    --copyright-holder "Canonical Ltd"

echo "$PROGNAME: $PO_DIR/$DOMAIN.pot updated based on $SOURCE_DIR"
