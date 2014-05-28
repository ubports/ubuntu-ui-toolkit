#!/bin/sh
#
# Copyright 2014 Canonical Ltd.
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
# Author: Christian Dywan <christian.dywan@canonical.com>

ARCH=arm-linux-gnueabihf

# Make the image writable
phablet-config writable-image || exit 1
# Copy selectively to avoid pushing binaries (arch conflict) and sources (unneeded)
for i in / /ListItems/ /Pickers/ /Popups/ /Colors/ /Styles/ /Themes/ /artwork/; do
    OLDPWD=$PWD
    cd modules
    for j in $(ls Ubuntu/Components$i*.qml 2>/dev/null); do
        adb push $j /usr/lib/$ARCH/qt5/qml/$j || exit 1
    done
    for j in $(ls Ubuntu/Components$i*.js 2>/dev/null); do
        adb push $j /usr/lib/$ARCH/qt5/qml/$j || exit 1
    done
    cd $OLDPWD
done
# Autopilot tests should always match the Toolkit
adb push tests/autopilot/ubuntuuitoolkit/ /usr/lib/python2.7/dist-packages/ubuntuuitoolkit || exit 1
adb push examples/ubuntu-ui-toolkit-gallery/ /usr/lib/ubuntu-ui-toolkit/examples/ubuntu-ui-toolkit-gallery || exit 1
# For launching the gallery easily
adb push examples/ubuntu-ui-toolkit-gallery/*.desktop /usr/share/applications/ || exit 1
