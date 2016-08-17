#!/bin/bash
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

QDOC=$1
QHELPGENERATOR=$2
DOC_PATH=$3
if [ ! $SRC -o ! $DOC_PATH ]; then
    echo "Error - SRC and DOC_PATH must be defined"
    exit 1
fi

mkdir -p $DOC_PATH

# Offline docs for QtCreator
eval "$QDOC $SRC/ubuntu-ui-toolkit-qtcreator.qdocconf 2> $DOC_PATH/qdoc.log"
# FIXME: With Qt 5.2 this warning shows up, forcibly omit it from errors
grep -v "error: HTML file already exists; overwriting" $DOC_PATH/qdoc.log | grep -v "qdoc: warning: No documentation for 'global'" | grep -v "warning: Can't link to" > $DOC_PATH/qdoc.err
cat $DOC_PATH/qdoc.err
test ! -s $DOC_PATH/qdoc.err || exit 1
echo docs: Offline done.

eval "$QHELPGENERATOR -o '$DOC_PATH/ubuntuuserinterfacetoolkit.qch' '$DOC_PATH/offline/ubuntuuserinterfacetoolkit.qhp' | grep -v ' does not exist! Skipping it'"
echo docs: qch done: $DOC_PATH

# Online docs
eval "$QDOC $SRC/ubuntu-ui-toolkit-online.qdocconf 2> $DOC_PATH/qdoc.log"
grep -v "error: HTML file already exists; overwriting" $DOC_PATH/qdoc.log | grep -v "qdoc: warning: No documentation for 'global'" | grep -v "warning: Can't link to" > $DOC_PATH/qdoc.err
cat $DOC_PATH/qdoc.err
test ! -s $DOC_PATH/qdoc.err || exit 1
echo docs: Online done.

# Second qdoc pass, this time with indexes for cross-referencing.
# If we don't do this, bugs in docs from other modules (Qt, Unity) fail our build
eval "$QDOC $SRC/ubuntu-ui-toolkit-offline-indexes.qdocconf 2> $DOC_PATH/qdoc.log"
grep "warning: Can't link to" $DOC_PATH/qdoc.log > $DOC_PATH/qdoc.err
cat $DOC_PATH/qdoc.err
test ! -s $DOC_PATH/qdoc.err || exit 1
eval "$QDOC $SRC/ubuntu-ui-toolkit-online-indexes.qdocconf 2> $DOC_PATH/qdoc.log"
grep "warning: Can't link to" $DOC_PATH/qdoc.log > $DOC_PATH/qdoc.err
cat $DOC_PATH/qdoc.err
test ! -s $DOC_PATH/qdoc.err || exit 1
echo docs: Cross-referenced done.

sed -r -i 's@(</p>)<p/>@\1@g' $DOC_PATH/html/*.html
sed -r -i 's@(<a class="(next|prev)Page" href=".+">)(.+)(</a>)@<li>\1\3\4</li>@g' $DOC_PATH/html/*.html
sed -r -i 's@(href="qmlmodule-ubuntu-components0-ubuntu-components-0-1.html">Basic QML Types)@href="overview-ubuntu-sdk.html">Ubuntu User Interface Toolkit@g' $DOC_PATH/html/*.html
echo docs: HTML fixes done.
