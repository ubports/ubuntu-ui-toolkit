#!/usr/bin/env python3
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
    sys.exit(
        'Usage:\n  env BUILTINS=foo,bar %s FILENAME [FILENAME2..N]\n\n'
        '  Generate a QML API file\n'
        'Example:\n'
        '  env BUILTINS=QQuick,QQml,U1db:: '
        '%s modules/Ubuntu/Components/qmldir plugins.qmltypes'
        ' > components.api.new\n'
        '\n'
        '  It is recommended to pass qmldir files over a list of qml files\n'
        '  because then internal components are discarded and errors in that\n'
        '  list can be found.\n'
        '  For testing one can pass qml files also directly or serve them\n'
        '  via standard input.\n'
        '  The variable BUILTINS is a comma-separated list of prefixes for\n'
        '  API that appears in qmltypes files but not part of the same\n'
        '  package.\n'
        '\n'
        'Use the following command to see changes in the API:\n'
        '  diff -Fqml -u components.api{,.new}\n' % (basename, basename))

builtins = os.getenv('BUILTINS', '').split(',')
inputfiles = []
classes = {}
for line in fileinput.input():
    if fileinput.filename()[-6:] == 'qmldir':
        if line[:8] == 'internal':
            # Internal components are not part of public API
            continue
        pieces = line.strip().split(' ')
        if len(pieces) > 2:
            filename = pieces[2]
            # We only work with QML
            if filename[-3:] == 'qml':
                # Filenames are relative to the qmldir
                # Foo 1.0 Foo.qml
                folder = os.path.dirname(fileinput.filename())
                fullpath = folder + '/' + filename
                classname = pieces[0]
                version = pieces[1]
                if fullpath not in inputfiles:
                    inputfiles.append(fullpath)
                    classes[fullpath] = [classname, version]
                else:
                    versions = classes[fullpath]
                    if classname not in versions:
                        versions.append(classname)
                    versions.append(version)
    else:
        inputfiles.append(fileinput.filename())
        fileinput.nextfile()


# Sort filenames to maintain a consistent order
# Get un/versioned files in the same order
def skipversion(filename):
    for v in ['10', '11']:
        filename = filename.replace(v + '/', '')
    return filename
inputfiles.sort(key=skipversion)

hook = fileinput.hook_encoded('utf-8')
for line in fileinput.input(inputfiles, openhook=hook):
    # New file
    if fileinput.isfirstline():
        in_block = 0
        in_comment = in_builtin_type = False
        block_meta = {}
        annotated_properties = {}
        if fileinput.filename()[-3:] == 'qml':
            filetype = 'qml'
            keywords = ['signal', 'property', 'function']
        elif fileinput.filename()[-8:] == 'qmltypes':
            filetype = 'qmltypes'
            keywords = ['Signal',
                        'Property',
                        'Method',
                        'prototype',
                        'exports']
        else:
            sys.exit('Unknown filetype %s' % fileinput.filename())
        if fileinput.filename() in classes:
            classname = ' '.join(classes[fileinput.filename()])
        else:
            classname = fileinput.filename()
        print(classname)

    line = line.split('//')[0]
    # alias properties only define their type through qdoc comments
    if '\\qmlproperty' in line:
        words = line.strip().split(' ')
        name = words[2]
        # Strip namespace
        if '::' in name:
            name = name.split('::')[1]
        type = words[1]
        annotated_properties[name] = type
    elif '\\internal' in line:
        # internal without type always relates to the next declared property
        annotated_properties['internal'] = 'internal'

    if '/*' in line and '*/' not in line:
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
    if '}' in line and '{' not in line:
        in_block -= 1
        block_meta = {}
        if in_block == 1 and in_builtin_type:
            in_builtin_type = False
        continue

    # Only root "Item {" is inspected for QML, otherwise all children
    if in_block == 1 or filetype == 'qmltypes':
        # Left hand side specifies a keyword, a type and a variable name
        declaration = line.split(':', 1)[0]
        words = declaration.strip().split(' ')
        # Skip types with prefixes considered builtin
        if filetype == 'qmltypes' and words[0] == 'name':
            found_builtin_type = False
            for builtin in builtins:
                if '"' + builtin in line:
                    found_builtin_type = True
                    break
            if found_builtin_type:
                in_builtin_type = True
                continue
            if 'prototype' in block_meta:
                print('    ' + block_meta['prototype'].strip())
                print('    ' + line.strip())
                continue

        block_meta[words[0]] = line
        # Omit prototype if it comes before the name since we may skip it
        if 'name' not in block_meta and words[0] == 'prototype':
            continue

        # Don't consider the qml variable name as a keyword
        if filetype == 'qml':
            words.pop()

        if filetype == 'qmltypes' and in_block > 1:
            keywords.append('name')
            keywords.append('Parameter')
        for word in words:
            if word in keywords:
                if filetype == 'qml':
                    separator = '{' if 'function' in declaration else ':'
                    signature = declaration.split(separator, 1)[0].strip()
                    if 'alias' in line:
                        no_mods = signature
                        for mod in ['readonly', 'default']:
                            no_mods = no_mods.replace(mod, '')
                        name = no_mods.strip().split(' ')[2]
                        if 'internal' in annotated_properties:
                            if name not in annotated_properties:
                                annotated_properties[name] = 'internal'
                            del annotated_properties['internal']
                        if name not in annotated_properties:
                            sys.exit(
                                '    %s\nError: Missing \\qmlproperty for %s'
                                % (signature, name))
                        real_type = annotated_properties[name]
                        signature = signature.replace('alias', real_type)
                elif filetype == 'qmltypes':
                    signature = line.strip()
                if not in_builtin_type:
                    print('    %s' % (signature))
                break

    # Start of function/ signal/ Item block
    if '{' in line and '}' not in line:
        in_block += 1
        block_meta = {}
        # The parent type can affect API
        if in_block == 1 and filetype == 'qml':
            print(line.split('{')[0].strip())
