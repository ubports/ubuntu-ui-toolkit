# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2014, 2015 Canonical Ltd.
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
import json
import logging
import os
import shutil
import subprocess
import tempfile

import fixtures
from autopilot import display
from gi.repository import Gio

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
    'Exec': '{launcher} {qml_file_path}',
    'Icon': 'Not important'
}


logger = logging.getLogger(__name__)


class FakeApplication(fixtures.Fixture):

    def __init__(
            self, qml_file_contents=DEFAULT_QML_FILE_CONTENTS,
            desktop_file_dict=None, url_dispatcher_protocols=None):
        super().__init__()
        self._qml_file_contents = qml_file_contents
        if desktop_file_dict is None:
            self._desktop_file_dict = copy.deepcopy(DEFAULT_DESKTOP_FILE_DICT)
        else:
            self._desktop_file_dict = copy.deepcopy(desktop_file_dict)
        self.url_dispatcher_protocols = url_dispatcher_protocols

    def setUp(self):
        super().setUp()
        self.qml_file_path, self.desktop_file_path = (
            self._create_test_application())
        desktop_file_name = os.path.basename(self.desktop_file_path)
        self.application_name, _ = os.path.splitext(desktop_file_name)
        if self.url_dispatcher_protocols:
            self.url_dispatcher_file_path = (
                self._register_url_dispatcher_protocols(self.application_name))

    def _create_test_application(self):
        qml_file_path = self._write_test_qml_file()
        self.addCleanup(os.remove, qml_file_path)
        desktop_file_path = self._write_test_desktop_file(qml_file_path)
        self.addCleanup(os.remove, desktop_file_path)
        return qml_file_path, desktop_file_path

    def _write_test_qml_file(self):
        qml_file = self._named_temporary_file(suffix='.qml')
        qml_file.write(self._qml_file_contents)
        qml_file.close()
        return qml_file.name

    def _named_temporary_file(self, dir=None, mode='w+t',
                              delete=False, suffix=''):
        # Discard files with underscores which look like an APP_ID to Unity
        # See https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1329141
        chars = tempfile._RandomNameSequence.characters.strip("_")
        tempfile._RandomNameSequence.characters = chars
        return tempfile.NamedTemporaryFile(dir=dir, mode=mode,
                                           delete=delete, suffix=suffix)

    def _write_test_desktop_file(self, qml_file_path):
        desktop_file_dir = self._get_local_desktop_file_directory()
        if not os.path.exists(desktop_file_dir):
            os.makedirs(desktop_file_dir)
        desktop_file = self._named_temporary_file(suffix='.desktop',
                                                  dir=desktop_file_dir)
        self._desktop_file_dict['Exec'] = (
            self._desktop_file_dict['Exec'].format(
                launcher=base.get_toolkit_launcher_command(),
                qml_file_path=qml_file_path))
        desktop_file.write('[Desktop Entry]\n')
        for key, value in self._desktop_file_dict.items():
            desktop_file.write('{key}={value}\n'.format(key=key, value=value))
        desktop_file.close()
        return desktop_file.name

    def _get_local_desktop_file_directory(self):
        return os.path.join(
            os.environ.get('HOME'), '.local', 'share', 'applications')

    def _register_url_dispatcher_protocols(self, application_name):
        url_dispatcher_file_path = self._write_url_dispatcher_file(
            application_name)
        self.addCleanup(os.remove, url_dispatcher_file_path)
        self._update_url_dispatcher_directory(url_dispatcher_file_path)
        return url_dispatcher_file_path

    def _write_url_dispatcher_file(self, application_name):
        url_dispatcher_dir = self._get_local_url_dispatcher_directory()
        if not os.path.exists(url_dispatcher_dir):
            os.makedirs(url_dispatcher_dir)

        protocol_list = [
            {'protocol': protocol}
            for protocol in self.url_dispatcher_protocols]

        url_dispatcher_file_path = os.path.join(
            url_dispatcher_dir, application_name + '.url-dispatcher')
        with open(url_dispatcher_file_path, 'w') as url_dispatcher_file:
            url_dispatcher_file.write(json.dumps(protocol_list))

        return url_dispatcher_file_path

    def _get_local_url_dispatcher_directory(self):
        return os.path.join(
            os.environ.get('HOME'), '.config', 'url-dispatcher', 'urls')

    def _update_url_dispatcher_directory(self, url_dispatcher_file_path):
        # FIXME This should be updated calling
        # initctl start url-dispatcher-update-user, but it is not working.
        # https://bugs.launchpad.net/ubuntu/+source/url-dispatcher/+bug/1461496
        # --elopio - 2015-06-02
        subprocess.check_output(
            '/usr/lib/*/url-dispatcher/update-directory ' +
            url_dispatcher_file_path, shell=True)


class InitctlEnvironmentVariable(fixtures.Fixture):
    """Set the value of initctl environment variables."""

    def __init__(self, global_=False, **kwargs):
        super().__init__()
        # Added one level of indirection to be able to spy the calls to
        # environment during tests.
        self.environment = environment
        self.variables = kwargs
        self.global_ = global_

    def setUp(self):
        super().setUp()
        for variable, value in self.variables.items():
            self._add_variable_cleanup(variable)
            if value is None:
                self.environment.unset_initctl_env_var(
                    variable, global_=self.global_)
            else:
                self.environment.set_initctl_env_var(
                    variable, value, global_=self.global_)

    def _add_variable_cleanup(self, variable):
        if self.environment.is_initctl_env_var_set(
                variable, global_=self.global_):
            original_value = self.environment.get_initctl_env_var(
                variable, global_=self.global_)
            self.addCleanup(
                self.environment.set_initctl_env_var,
                variable,
                original_value,
                global_=self.global_)
        else:
            self.addCleanup(
                self.environment.unset_initctl_env_var,
                variable,
                global_=self.global_)


class FakeHome(fixtures.Fixture):

    # We copy the Xauthority file to allow executions using XVFB. If it is not
    # on the user's HOME directory, nothing will happen.
    should_copy_xauthority_file = True

    def __init__(self, directory=None):
        super().__init__()
        self.directory = directory

    def setUp(self):
        super().setUp()
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


class HideUnity7Launcher(fixtures.Fixture):

    """Hide the Unity7 launcher if it is visible and restore it on clean up."""

    _UNITYSHELL_GSETTINGS_SCHEMA = 'org.compiz.unityshell'
    _UNITYSHELL_GSETTINGS_PATH = (
        '/org/compiz/profiles/unity/plugins/unityshell/')
    _UNITYSHELL_LAUNCHER_KEY = 'launcher-hide-mode'
    _UNITYSHELL_LAUNCHER_HIDDEN_MODE = 1  # launcher hidden

    def setUp(self):
        super().setUp()
        self._hide_launcher()

    def _hide_launcher(self):
        if (self._UNITYSHELL_GSETTINGS_SCHEMA in
                Gio.Settings.list_relocatable_schemas()):
            unityshell_schema = Gio.Settings.new_with_path(
                self._UNITYSHELL_GSETTINGS_SCHEMA,
                self._UNITYSHELL_GSETTINGS_PATH)
            self._hide_launcher_from_schema(unityshell_schema)
        else:
            logger.warning('Unity Shell gsettings schema not found.')

    def _hide_launcher_from_schema(self, unityshell_schema):
        original_mode = self._get_launcher_mode(unityshell_schema)
        self.addCleanup(
            self._set_launcher_mode, unityshell_schema, original_mode)
        self._set_launcher_mode(
            unityshell_schema, self._UNITYSHELL_LAUNCHER_HIDDEN_MODE)

    def _get_launcher_mode(self, unityshell_schema):
        return unityshell_schema.get_int(self._UNITYSHELL_LAUNCHER_KEY)

    def _set_launcher_mode(self, unityshell_schema, mode):
        unityshell_schema.set_int(self._UNITYSHELL_LAUNCHER_KEY, mode)


class SimulateDevice(fixtures.Fixture):

    def __init__(self, app_width, app_height, grid_unit_px):
        super().__init__()
        self.app_width = app_width
        self.app_height = app_height
        self.grid_unit_px = grid_unit_px
        self._screen = None

    def setUp(self):
        super().setUp()
        if self._is_geometry_larger_than_display(
                self.app_width, self.app_height):
            scale_divisor = self._get_scale_divisor()
            self.grid_unit_px = self.grid_unit_px // scale_divisor
            self.app_width = self.app_width // scale_divisor
            self.app_height = self.app_height // scale_divisor
        self.useFixture(
            fixtures.EnvironmentVariable(
                'GRID_UNIT_PX', str(self.grid_unit_px)))

    def _get_scale_divisor(self):
        scale_divisor = 2
        while self._is_geometry_larger_than_display(
                self.app_width // scale_divisor,
                self.app_height // scale_divisor):
            scale_divisor = scale_divisor * 2
        return scale_divisor

    def _is_geometry_larger_than_display(self, width, height):
        screen = self._get_screen()
        screen_width = screen.get_screen_width()
        screen_height = screen.get_screen_height()
        return (width > screen_width) or (height > screen_height)

    def _get_screen(self):
        if self._screen is None:
            self._screen = display.Display.create()
        return self._screen
