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
import shutil
import tempfile

import fixtures

from ubuntuuitoolkit import base, environment


DEFAULT_QML_FILE_CONTENTS = ("""
import QtQuick 2.0
import Ubuntu.Components 1.1

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


class InitctlEnvironmentVariable(fixtures.Fixture):
    """Set the value of initctl environment variables."""

    def __init__(self, **kwargs):
        super(InitctlEnvironmentVariable, self).__init__()
        self.variables = kwargs

    def setUp(self):
        super(InitctlEnvironmentVariable, self).setUp()
        for variable, value in self.variables.items():
            self._add_variable_cleanup(variable)
            environment.set_initctl_env_var(variable, value)

    def _add_variable_cleanup(self, variable):
        if environment.is_initctl_env_var_set(variable):
            original_value = environment.get_initctl_env_var(variable)
            self.addCleanup(
                environment.set_initctl_env_var, variable,
                original_value)
        else:
            self.addCleanup(environment.unset_initctl_env_var, variable)


class FakeHome(fixtures.Fixture):

    # We copy the Xauthority file to allow executions using XVFB. If it is not.
    should_copy_xauthority_file = True

    def __init__(self, directory=None):
        super(FakeHome, self).__init__()
        self.directory = directory

    def setUp(self):
        super(FakeHome, self).setUp()
        self.directory = self._make_directory_if_not_specified()
        if self.should_copy_xauthority_file:
            self._copy_xauthority_file(self.directory)
        # We patch both environment variables so it works on the desktop and on
        # the phone.
        self.useFixture(
            InitctlEnvironmentVariable(HOME=self.directory))
        self.useFixture(
            fixtures.EnvironmentVariable('HOME', newvalue=self.directory))

    def _make_directory_if_not_specified(self):
        if self.directory is None:
            parent_directory = os.path.join(
                os.environ.get('HOME'), 'autopilot', 'fakeenv')
            if not os.path.exists(parent_directory):
                os.makedirs(parent_directory)
            temp_dir_fixture = fixtures.TempDir(parent_directory)
            self.useFixture(temp_dir_fixture)
            return temp_dir_fixture.path
        else:
            return self.directory

    def _copy_xauthority_file(self, directory):
        """Copy the .Xauthority file if it exists in the user's home."""
        xauthority_file_path = os.path.join(
            os.environ.get('HOME'), '.Xauthority')
        if os.path.isfile(xauthority_file_path):
            shutil.copyfile(
                xauthority_file_path,
                os.path.join(directory, '.Xauthority'))
