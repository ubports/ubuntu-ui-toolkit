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
cd modules || exit 1
for i in $(ls Ubuntu/Components/*.qml 2>/dev/null); do
    echo modules/$i '->' /usr/lib/$ARCH/qt5/qml/Ubuntu/Components/
    adb push $i /usr/lib/$ARCH/qt5/qml/Ubuntu/Components/
done
for i in $(ls Ubuntu/Components/*.js 2>/dev/null); do
    echo modules/$i '->' /usr/lib/$ARCH/qt5/qml/Ubuntu/Components/
    adb push $i /usr/lib/$ARCH/qt5/qml/Ubuntu/Components/
done
echo modules/Ubuntu/Components/qmldir '->' /usr/lib/$ARCH/qt5/qml/Ubuntu/Components/
adb push Ubuntu/Components/qmldir /usr/lib/$ARCH/qt5/qml/Ubuntu/Components/
cd ..
for i in 10 11 ListItems Pickers Popups Colors Styles Themes artwork; do
    adb push modules/Ubuntu/Components/$i/ /usr/lib/$ARCH/qt5/qml/Ubuntu/Components/$i || exit 1
done
# Autopilot tests should always match the Toolkit
adb push tests/autopilot/ubuntuuitoolkit/ /usr/lib/python2.7/dist-packages/ubuntuuitoolkit || exit 1
adb push examples/ubuntu-ui-toolkit-gallery/ /usr/lib/ubuntu-ui-toolkit/examples/ubuntu-ui-toolkit-gallery || exit 1
# For launching the gallery easily
adb push examples/ubuntu-ui-toolkit-gallery/*.desktop /usr/share/applications/ || exit 1
