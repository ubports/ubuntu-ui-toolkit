# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2014 Canonical Ltd.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; version 3.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

import copy
import os
import tempfile

import fixtures

from ubuntuuitoolkit import base


DEFAULT_QML_FILE_CONTENTS = ("""
import QtQuick 2.0
import Ubuntu.Components 0.1

MainView {
    width: units.gu(48)
    height: units.gu(60)

    Label {
        objectName: 'testLabel'
        text: 'Test application.'
    }
}
""")
DEFAULT_DESKTOP_FILE_DICT = {
    'Type': 'Application',
    'Name': 'test',
    'Exec': '{qmlscene} {qml_file_path}',
    'Icon': 'Not important'
}


class FakeApplication(fixtures.Fixture):

    def __init__(
            self, qml_file_contents=DEFAULT_QML_FILE_CONTENTS,
            desktop_file_dict=None):
        super(FakeApplication, self).__init__()
        self._qml_file_contents = qml_file_contents
        if desktop_file_dict is None:
            self._desktop_file_dict = copy.deepcopy(DEFAULT_DESKTOP_FILE_DICT)
        else:
            self._desktop_file_dict = copy.deepcopy(desktop_file_dict)

    def setUp(self):
        super(FakeApplication, self).setUp()
        self.qml_file_path, self.desktop_file_path = (
            self._create_test_application())

    def _create_test_application(self):
        qml_file_path = self._write_test_qml_file()
        self.addCleanup(os.remove, qml_file_path)
        desktop_file_path = self._write_test_desktop_file(qml_file_path)
        self.addCleanup(os.remove, desktop_file_path)
        return qml_file_path, desktop_file_path

    def _write_test_qml_file(self):
        qml_file = tempfile.NamedTemporaryFile(
            mode='w+t', suffix='.qml', delete=False)
        qml_file.write(self._qml_file_contents)
        qml_file.close()
        return qml_file.name

    def _write_test_desktop_file(self, qml_file_path):
        desktop_file_dir = self._get_local_desktop_file_directory()
        if not os.path.exists(desktop_file_dir):
            os.makedirs(desktop_file_dir)
        desktop_file = tempfile.NamedTemporaryFile(
            mode='w+t', suffix='.desktop', dir=desktop_file_dir, delete=False)
        self._desktop_file_dict['Exec'] = (
            self._desktop_file_dict['Exec'].format(
                qmlscene=base.get_qmlscene_launch_command(),
                qml_file_path=qml_file_path))
        desktop_file.write('[Desktop Entry]\n')
        for key, value in self._desktop_file_dict.items():
            desktop_file.write('{key}={value}\n'.format(key=key, value=value))
        desktop_file.close()
        return desktop_file.name

    def _get_local_desktop_file_directory(self):
        return os.path.join(
            os.environ.get('HOME'), '.local', 'share', 'applications')
