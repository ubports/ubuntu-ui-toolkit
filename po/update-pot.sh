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

set -e
cd $(dirname $0)
PO_DIR=$PWD

PROGNAME=$(basename $0)

# Create a list of files to scan
GETTEXT_FILES=$(mktemp --tmpdir uitk-gallery.lst.XXXXX)
trap 'rm -f "$GETTEXT_FILES"' EXIT
cd ..
find \( -name '*.cpp' -o -name '*.qml' -name '*.js' \) \
    -a ! \( -path './debian/*' -o -path './build/*' -o -path './.bzr/*' \) | sort \
> $GETTEXT_FILES

# Generate pot from our list
xgettext \
    --output $PO_DIR/ubuntu-ui-toolkit.pot \
    --files-from $GETTEXT_FILES \
    --qt --c++ \
    --add-comments=TRANSLATORS \
    --keyword=tr \
    --keyword=tr:1,2 \
    --package-name ubuntu-ui-toolkit \
    --copyright-holder "Canonical Ltd"

echo "$PROGNAME: $PO_DIR/ubuntu-ui-toolkit.pot updated"
