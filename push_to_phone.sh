#!/bin/bash
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
DEST=/usr/lib/$ARCH/qt5/qml/Ubuntu/Components
RUN=$XDG_RUNTIME_DIR/$(basename $0)
STONE=/tmp/$(basename $0)

# Ask early so the script can run through smoothly
echo Type your phone\'s PIN or password to continue:
read -s PW

# Make the image writable
phablet-config writable-image || exit 1

# Prepare copy script to be run on the device
rm -Rf $RUN
mkdir -p $RUN
echo '#!/bin/sh' > $RUN/copy.sh
echo echo Updating Ubuntu.Components... >> $RUN/copy.sh
echo cd $STONE >> $RUN/copy.sh
echo DEST=$DEST >> $RUN/copy.sh

cd modules || exit 1
# Copy selectively to avoid pushing binaries (arch conflict) and sources (unneeded)
for i in $(ls Ubuntu/Components/*.qml Ubuntu/Components/*.js Ubuntu/Components/qmldir 2>/dev/null); do
    echo modules/$i '->' $STONE/c
    adb push $i $STONE/c/$i || exit 1
done
cd ..
echo cp -R c/Ubuntu/Components/* "\$DEST" '|| exit 1' >> $RUN/copy.sh

for i in 10 11 ListItems Pickers Popups Styles Themes artwork; do
    adb push modules/Ubuntu/Components/$i/ $STONE/$i || exit 1
    echo cp -R $i/ "\$DEST"/$i >> $RUN/copy.sh '|| exit 1' || exit 1
done

# Autopilot tests should always match the Toolkit
adb push tests/autopilot/ubuntuuitoolkit/ $STONE/ap || exit 1
echo cp -R ap/ /usr/lib/python2.7/dist-packages/ubuntuuitoolkit '|| exit 1' >> $RUN/copy.sh || exit 1
echo cp -R ap/ /usr/lib/python3/dist-packages/ubuntuuitoolkit '|| exit 1' >> $RUN/copy.sh || exit 1
adb push examples/ubuntu-ui-toolkit-gallery/ $STONE/ex >> $RUN/copy.sh || exit 1
echo cp -R ex/ /usr/lib/ubuntu-ui-toolkit/examples/ubuntu-ui-toolkit-gallery '|| exit 1'

# For launching the gallery easily
echo cp ex/*.desktop /usr/share/applications/ '|| exit 1' >> $RUN/copy.sh || exit 1

echo echo ...OK >> $RUN/copy.sh
chmod +x $RUN/copy.sh
adb push $RUN/copy.sh $STONE/copy.sh || exit 1
adb shell "echo $PW | sudo --stdin $STONE/copy.sh"
