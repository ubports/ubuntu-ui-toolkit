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

import sys
import fileinput
import os

if len(sys.argv) < 2 or '-h' in sys.argv or '--help' in sys.argv:
    basename = os.path.basename(sys.argv[0])
    print (
        'Usage:\n  env BUILTINS=foo,bar %s FILENAME [FILENAME2..N]\n\n'
        '  Generate a QML API file\n'
        'Example:\n'
        '  env BUILTINS=QQuick,QQml,U1db:: '
        '%s modules/Ubuntu/Components/*.qml plugins.qmltypes'
        ' > components.api.new\n'
        '  diff -Fqml -u components.api{,.new}\n' % (basename, basename))
    sys.exit(1)

builtins = os.getenv('BUILTINS', '').split(',')
for line in fileinput.input():
    # New file
    if fileinput.isfirstline():
        in_block = 0
        in_comment = in_builtin_type = False
        annotated_type = None
        if fileinput.filename()[-3:] == 'qml':
            filetype = 'qml'
            keywords = ['signal', 'property', 'function']
        elif fileinput.filename()[-8:] == 'qmltypes':
            filetype = 'qmltypes'
            keywords = ['Signal',
                        'Property',
                        'Method',
                        'prototype:',
                        'exports:']
        else:
            print('Unknown filetype %s' % fileinput.filename())
            sys.exit(1)
        print('%s' % fileinput.filename())

    line = line.split('//')[0]
    # alias properties only define their type through qdoc comments
    if '\\qmlproperty' in line:
        annotated_type = line
    elif '\\internal' in line:
        annotated_type = 'internal internal internal'

    if '/*' in line and not '*/' in line:
        in_comment = True
        continue
    if '*/' in line:
        in_comment = False
        line = line.split('*/')[1]
    if in_comment:
        continue

    if '{' in line and '}' in line:
        if filetype == 'qmltypes' and not in_builtin_type:
            print('    ' + line.strip())
        continue

    # End of function/ signal/ Item block
    if '}' in line:
        in_block -= 1
        if in_block == 1 and in_builtin_type:
            in_builtin_type = False
        continue

    # Only root "Item {" is inspected for QML, otherwise all children
    if in_block == 1 or filetype == 'qmltypes':
        words = line.strip().split(' ')
        # Skip types with prefixes considered builtin
        if len(words) > 1 and words[0] == 'name:':
            found_builtin_type = False
            for builtin in builtins:
                if words[1].startswith('"' + builtin):
                    found_builtin_type = True
                    break
            if found_builtin_type:
                in_builtin_type = True
                continue

        if filetype == 'qmltypes' and in_block > 1:
            keywords.append('name:')
            keywords.append('Parameter')
        for word in words:
            if word in keywords:
                if filetype == 'qml':
                    signature = line.split(':')[0].split('{')[0].strip()
                    if 'alias' in line:
                        if not annotated_type:
                            print('    %s' % (signature))
                            print('Error: Missing \\qmlproperty annotation')
                            sys.exit(1)
                        real_type = annotated_type.strip().split(' ')[1]
                        signature = signature.replace('alias', real_type)
                    annotated_type = None
                elif filetype == 'qmltypes':
                    signature = line.strip()
                if not in_builtin_type:
                    print('    %s' % (signature))
                break

    # Start of function/ signal/ Item block
    if '{' in line:
        in_block += 1
        # The parent type can affect API
        if in_block == 1 and filetype == 'qml':
            print(line.split('{')[0].strip())
