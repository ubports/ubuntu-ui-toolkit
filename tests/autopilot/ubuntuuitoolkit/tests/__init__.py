# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2012-2015 Canonical Ltd.
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

"""Ubuntu UI Toolkit autopilot tests."""

import os
import tempfile

from autopilot.input import Pointer
from autopilot.matchers import Eventually
from testtools.matchers import Is, Not, Equals

import ubuntuuitoolkit
from ubuntuuitoolkit import base, fixture_setup


_DESKTOP_FILE_CONTENTS = ("""[Desktop Entry]
Type=Application
Exec=/usr/bin/whoami
Path=Not important
Name=Test app
Icon=Not important
""")


def _write_test_desktop_file():
    desktop_file_dir = get_local_desktop_file_directory()
    if not os.path.exists(desktop_file_dir):
        os.makedirs(desktop_file_dir)
    # Strip underscores which look like an APP_ID to Unity
    # See https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1329141
    chars = tempfile._RandomNameSequence.characters.strip("_")
    tempfile._RandomNameSequence.characters = chars
    desktop_file = tempfile.NamedTemporaryFile(
        suffix='.desktop', dir=desktop_file_dir, delete=False)
    desktop_file.write(_DESKTOP_FILE_CONTENTS.encode('utf-8'))
    desktop_file.close()
    return desktop_file.name


def get_local_desktop_file_directory():
    return os.path.join(os.environ['HOME'], '.local', 'share', 'applications')


def _get_module_include_path():
    return os.path.join(get_path_to_build_root(), 'modules')


def get_path_to_build_root():
    if "UITK_BUILD_ROOT" in os.environ:
        return os.environ["UITK_BUILD_ROOT"]
    return _guess_root_path()


def get_path_to_source_root():
    if "UITK_SOURCE_ROOT" in os.environ:
        return os.environ["UITK_SOURCE_ROOT"]
    return _guess_root_path()


def _guess_root_path():
    return os.path.abspath(
        os.path.join(
            os.path.dirname(__file__), '..', '..', '..', '..'))


class UbuntuUIToolkitWithFakeAppRunningTestCase(
        base.UbuntuUIToolkitAppTestCase):

    test_qml = ("""
import QtQuick 2.0
import Ubuntu.Components 1.1

MainView {
    width: units.gu(48)
    height: units.gu(80)
    objectName: "mainView"
}
""")

    def setUp(self):
        super().setUp()
        self.launch_application()

    def launch_application(self):
        fake_application = fixture_setup.FakeApplication(
            qml_file_contents=self.test_qml)
        self.useFixture(fake_application)

        local_modules_path = _get_module_include_path()
        if os.path.exists(local_modules_path):
            self.use_local_modules(local_modules_path)
        desktop_file_name = os.path.basename(
            fake_application.desktop_file_path)
        application_name, _ = os.path.splitext(desktop_file_name)
        self.app = self.launch_upstart_application(
            application_name,
            emulator_base=ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase)

    def use_local_modules(self, local_modules_path):
        env_vars = [
            'QML_IMPORT_PATH',
            'QML2_IMPORT_PATH',
            'UBUNTU_UI_TOOLKIT_THEMES_PATH'
        ]
        kwargs = {'global_': True}
        for env in env_vars:
            kwargs[env] = local_modules_path
        self.useFixture(fixture_setup.InitctlEnvironmentVariable(**kwargs))


class QMLStringAppTestCase(UbuntuUIToolkitWithFakeAppRunningTestCase):
    """Base test case for self tests that define the QML on an string."""

    def setUp(self):
        super().setUp()
        self.assertThat(
            self.main_view.visible, Eventually(Equals(True)))

        self.pointing_device = Pointer(self.input_device_class.create())

    @property
    def main_view(self):
        """
        Return the MainView of the app, selected by objectName.

        QML code used for testing must define the objectName
        of the MainView to be 'mainView'.
        """
        return self.app.select_single(objectName='mainView')


class FlickDirection:
    """Enum for flick or scroll direction."""

    UP, DOWN, LEFT, RIGHT = range(0, 4)


class QMLFileAppTestCase(base.UbuntuUIToolkitAppTestCase):
    """Base test case for self tests that launch a QML file."""

    test_qml_file_path = '/path/to/file.qml'
    desktop_file_path = None

    def setUp(self):
        super().setUp()
        self.pointing_device = Pointer(self.input_device_class.create())
        self.launch_application()

    def get_command_line(self, command_line):
        return command_line

    def launch_application(self):
        desktop_file_path = self._get_desktop_file_path()
        command_line = [
            base.get_toolkit_launcher_command(),
            "-I", _get_module_include_path(),
            self.test_qml_file_path,
            '--desktop_file_hint={0}'.format(desktop_file_path)
            ]
        self.app = self.launch_test_application(
            *self.get_command_line(command_line),
            emulator_base=ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase,
            app_type='qt')

        self.assertThat(
            self.main_view.visible, Eventually(Equals(True)))

    def _get_desktop_file_path(self):
        if self.desktop_file_path is None:
            desktop_file_path = _write_test_desktop_file()
            self.addCleanup(os.remove, desktop_file_path)
            return desktop_file_path
        else:
            self.desktop_file_path

    @property
    def main_view(self):
        """
        Return the MainView of the app, selected by objectName.

        QML code used for testing must define the objectName
        of the MainView to be 'mainView'.
        """
        return self.app.select_single(objectName='mainView')

    def getOrientationHelper(self):
        orientationHelper = self.main_view.select_many(
            "OrientationHelper")[0]
        self.assertThat(orientationHelper, Not(Is(None)))
        return orientationHelper

    def checkPageHeader(self, pageTitle):
        orientationHelper = self.getOrientationHelper()
        header = orientationHelper.select_single("AppHeader", title=pageTitle)
        self.assertThat(header, Not(Is(None)))
        self.assertThat(header.visible, Eventually(Equals(True)))
        return header

    def getObject(self, objectName):
        obj = self.app.select_single(objectName=objectName)
        self.assertThat(obj, Not(Is(None)))
        return obj

    def tap(self, objectName):
        obj = self.getObject(objectName)
        self.pointing_device.move_to_object(obj)
        self.pointing_device.click()
