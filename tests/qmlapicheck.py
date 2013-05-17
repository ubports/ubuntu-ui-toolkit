#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2013 Canonical Ltd.
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

import sys, fileinput

if len (sys.argv) < 2 or '-h' in sys.argv or '--help' in sys.argv:
    import os

    basename = os.path.basename(sys.argv[0])
    print ('Usage:\n  %s FILENAME [FILENAME2..N]\n\n'
           '    Generate a QML API file\n'
           'Example:\n'
           '    %s modules/Ubuntu/Components/*.qml > components.api.new\n'
           '    diff -Fqml -u components.api{,.new}\n'
           % (basename, basename))
    sys.exit(1)

keywords = ['signal', 'property', 'function']
in_block = 0
in_comment = False

for line in fileinput.input():
    # New file
    if fileinput.isfirstline():
        in_block = 0
        in_comment = False
        print('%s' % fileinput.filename())

    line = line.split('//')[0]
    if '/*' in line and not '*/' in line:
        in_comment = True
        continue
    if '*/' in line:
        in_comment = False
        line = line.split('*/')[1]
    if in_comment:
        continue

    # End of function/ signal/ Item block
    if '}' in line:
        in_block -= 1
        continue

    # Only root item "Item {" is inspected for API
    if in_block > 1:
        continue

    for word in line.split(' '):
        if word in keywords:
            signature = line.split(':')[0].split('{')[0].strip()
            print('    %s' % (signature))

    # Start of function/ signal/ Item block
    if '{' in line:
        in_block += 1

