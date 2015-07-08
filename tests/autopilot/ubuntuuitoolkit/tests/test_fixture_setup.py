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

import os
import subprocess
import tempfile

from unittest import mock
import testscenarios
import testtools
from autopilot import (
    display,
    introspection,
    platform,
    testcase as autopilot_testcase
)
from autopilot.matchers import Eventually
from testtools.matchers import Contains, Equals, FileExists, Not

from ubuntuuitoolkit import base, environment, fixture_setup, tests


class FakeApplicationTestCase(testtools.TestCase):

    def assert_desktop_file_contents(
            self, desktop_file_contents, expected_contents_dict):
        desktop_file_lines = desktop_file_contents.splitlines()
        self.assertEqual('[Desktop Entry]', desktop_file_lines[0])

        contents_dict = dict(
            [line.split('=') for line in desktop_file_lines[1:]])
        self.assertDictEqual(expected_contents_dict, contents_dict)

    def test_qml_file_must_be_created_with_specified_contents(self):
        fake_application = fixture_setup.FakeApplication(
            qml_file_contents='Test')
        self.useFixture(fake_application)

        with open(fake_application.qml_file_path, 'r+t') as qml_file:
            qml_file_contents = qml_file.read()

        self.assertEqual('Test', qml_file_contents)

    def test_qml_file_with_default_contents(self):
        fake_application = fixture_setup.FakeApplication()
        self.useFixture(fake_application)

        with open(fake_application.qml_file_path, 'r+t') as qml_file:
            qml_file_contents = qml_file.read()

        self.assertEqual(
            fixture_setup.DEFAULT_QML_FILE_CONTENTS, qml_file_contents)

    def test_desktop_file_must_be_created_with_specified_values(self):
        test_desktop_file_dict = {
            'test key 1': 'test value 1',
            'test key 2': 'test value 2',
            'Exec': 'test',
        }
        fake_application = fixture_setup.FakeApplication(
            desktop_file_dict=test_desktop_file_dict)
        self.useFixture(fake_application)

        with open(fake_application.desktop_file_path, 'r+t') as desktop_file:
            desktop_file_contents = desktop_file.read()

        self.assert_desktop_file_contents(
            desktop_file_contents, test_desktop_file_dict)

    def test_desktop_file_with_launch_command(self):
        test_desktop_file_dict = {'Exec': '{launcher} application'}

        qmlscene = 'ubuntuuitoolkit.base.get_toolkit_launcher_command'
        with mock.patch(qmlscene) as mock_qmlscene:
            mock_qmlscene.return_value = 'test_qmlscene_command'
            fake_application = fixture_setup.FakeApplication(
                desktop_file_dict=test_desktop_file_dict)
            self.useFixture(fake_application)

        with open(fake_application.desktop_file_path, 'r+t') as desktop_file:
            desktop_file_contents = desktop_file.read()

        self.assertThat(
            desktop_file_contents,
            Contains('Exec=test_qmlscene_command application'))

    def test_desktop_file_with_qml_file_path(self):
        test_desktop_file_dict = {'Exec': 'qmlscene {qml_file_path}'}

        fake_application = fixture_setup.FakeApplication(
            desktop_file_dict=test_desktop_file_dict)
        self.useFixture(fake_application)

        with open(fake_application.desktop_file_path, 'r+t') as desktop_file:
            desktop_file_contents = desktop_file.read()

        self.assertThat(
            desktop_file_contents,
            Contains(
                'Exec=qmlscene {}'.format(fake_application.qml_file_path)))

    def test_desktop_file_with_default_contents(self):
        get_launcher = 'ubuntuuitoolkit.base.get_toolkit_launcher_command'
        with mock.patch(get_launcher) as mock_launcher:
            mock_launcher.return_value = 'testlauncher'
            fake_application = fixture_setup.FakeApplication()
            self.useFixture(fake_application)

        with open(fake_application.desktop_file_path, 'r+t') as desktop_file:
            desktop_file_contents = desktop_file.read()

        expected_desktop_file_dict = {
            'Type': 'Application',
            'Name': 'test',
            'Icon': 'Not important',
            'Exec': 'testlauncher {}'.format(fake_application.qml_file_path),
        }
        self.assert_desktop_file_contents(
            desktop_file_contents, expected_desktop_file_dict)

    def test_desktop_file_must_be_created_in_local_directory(self):
        fake_application = fixture_setup.FakeApplication()
        self.useFixture(fake_application)

        expected_desktop_file_directory = os.path.join(
            os.environ.get('HOME'), '.local', 'share', 'applications')
        self.assertEqual(
            expected_desktop_file_directory,
            os.path.dirname(fake_application.desktop_file_path))

    def test_fake_application_files_must_be_removed_after_test(self):
        fake_application = fixture_setup.FakeApplication(
            url_dispatcher_protocols=['testprotocol'])

        def inner_test():
            class TestWithFakeApplication(testtools.TestCase):
                def test_it(self):
                    self.useFixture(fake_application)
            return TestWithFakeApplication('test_it')

        inner_test().run()
        self.assertThat(fake_application.qml_file_path, Not(FileExists()))
        self.assertThat(fake_application.desktop_file_path, Not(FileExists()))
        self.assertThat(
            fake_application.url_dispatcher_file_path, Not(FileExists()))


class LaunchFakeApplicationTestCase(autopilot_testcase.AutopilotTestCase):

    def test_launch_fake_application_with_qmlscene(self):
        fake_application = fixture_setup.FakeApplication()
        self.useFixture(fake_application)

        application = self.launch_test_application(
            base.get_qmlscene_launch_command(),
            fake_application.qml_file_path,
            '--desktop_file_hint={0}'.format(
                fake_application.desktop_file_path),
            app_type='qt')

        # We can select a component from the application.
        application.select_single('Label', objectName='testLabel')

    def test_launch_fake_application_from_url_dispatcher(self):
        if platform.model() == 'Desktop':
            self.skipTest('Not yet working on desktop')

        path = os.path.abspath(__file__)
        dir_path = os.path.dirname(path)
        qml_file_path = os.path.join(
            dir_path, 'test_fixture_setup.LaunchFakeApplicationTestCase.qml')
        with open(qml_file_path) as qml_file:
            qml_file_contents = qml_file.read()

        fake_application = fixture_setup.FakeApplication(
            qml_file_contents=qml_file_contents,
            url_dispatcher_protocols=['testprotocol'])
        self.useFixture(fake_application)

        self.useFixture(fixture_setup.InitctlEnvironmentVariable(
            QT_LOAD_TESTABILITY=1))

        self.addCleanup(
            subprocess.check_output,
            ['ubuntu-app-stop', fake_application.application_name])

        subprocess.check_output(
            ['url-dispatcher', 'testprotocol://test'])

        pid = int(subprocess.check_output(
            ['ubuntu-app-pid', fake_application.application_name]).strip())

        application = introspection.get_proxy_object_for_existing_process(
            pid=pid)

        # We can select a component from the application.
        application.select_single('Label', objectName='testLabel')


class InitctlEnvironmentVariableTestCase(testscenarios.TestWithScenarios):

    scenarios = [
        ('global_variable', {'global_': True}),
        ('local_variable', {'global_': False})
    ]

    def set_original_value(self, value):
        self.addCleanup(
            environment.unset_initctl_env_var, 'testenvvarforfixture',
            global_=self.global_)
        environment.set_initctl_env_var('testenvvarforfixture',
                                        value, global_=self.global_)

    def create_fixture(self, value):
        self.initctl_env_var = fixture_setup.InitctlEnvironmentVariable(
            testenvvarforfixture=value, global_=self.global_)

    def assertValueIs(self, expected_value):
        self.assertEqual(
            expected_value,
            environment.get_initctl_env_var(
                'testenvvarforfixture', global_=self.global_))

    def assertVariableIsNotSet(self):
        self.assertFalse(
            environment.is_initctl_env_var_set(
                'testenvvarforfixture', global_=self.global_))

    def assertTestIsSuccessful(self, expected_value, test_name):
        result = testtools.TestResult()

        class TestWithInitctlEnvVar(testtools.TestCase):
            def setUp(inner):
                super().setUp()
                inner.useFixture(self.initctl_env_var)

            def test_value_set(inner):
                self.assertValueIs(expected_value)

            def test_variable_not_set(inner):
                self.assertVariableIsNotSet()

        TestWithInitctlEnvVar(test_name).run(result)
        self.assertTrue(
            result.wasSuccessful(), 'Failed to set the environment variable.')

    def test_use_initctl_environment_variable_to_set_unexisting_variable(self):
        """Test the initctl env var fixture when the var is unset.

        During the test, the new value must be in place.
        After the test, the variable must be unset again.

        """
        self.create_fixture('test value')
        self.assertTestIsSuccessful('test value', 'test_value_set')
        self.assertVariableIsNotSet()

    def test_use_initctl_environment_variable_to_set_existing_variable(self):
        """Test the initctl env var fixture when the var is unset.

        During the test, the new value must be in place.
        After the test, the old value must be set again.

        """
        self.set_original_value('original test value')
        self.create_fixture('new test value')
        self.assertTestIsSuccessful('new test value', 'test_value_set')
        self.assertValueIs('original test value')

    def test_use_initctl_environment_variable_to_unset_existing_variable(self):
        """Test the initctl env var fixture to unset a variable.

        During the test, the variable must be unset.
        After the test, the old value must be set again.

        """
        self.set_original_value('original test value')
        self.create_fixture(None)
        self.assertTestIsSuccessful(None, 'test_variable_not_set')
        self.assertValueIs('original test value',)

    def test_use_initctl_environment_variable_to_unset_nonexisting_variable(
            self):
        """Test the initctl env var fixture to unset a variable.

        During the test, the variable must be unset.
        After the test, the variable must remain unset.

        """
        self.create_fixture(None)
        self.assertTestIsSuccessful(None, 'test_variable_not_set')
        self.assertVariableIsNotSet()


class FakeHomeTestCase(testtools.TestCase):

    def test_fake_home_fixture_patches_initctl_env_var(self):
        original_home = environment.get_initctl_env_var('HOME')
        fake_home = original_home + 'fake'
        result = testtools.TestResult()

        def inner_test():
            class TestWithFakeHome(testtools.TestCase):
                def test_it(self):
                    fake_home_fixture = fixture_setup.FakeHome(fake_home)
                    fake_home_fixture.should_copy_xauthority_file = False
                    self.useFixture(fake_home_fixture)
                    self.assertEqual(
                        fake_home, environment.get_initctl_env_var('HOME'))
            return TestWithFakeHome('test_it')

        inner_test().run(result)

        self.assertTrue(
            result.wasSuccessful(),
            'Failed to fake the home: {}'.format(result.errors))
        self.assertEqual(
            original_home,
            environment.get_initctl_env_var('HOME'))

    def test_fake_home_fixture_patches_env_var(self):
        original_home = os.environ.get('HOME')
        fake_home = tempfile.gettempdir()
        result = testtools.TestResult()

        def inner_test():
            class TestWithFakeHome(testtools.TestCase):
                def test_it(self):
                    fake_home_fixture = fixture_setup.FakeHome(fake_home)
                    fake_home_fixture.should_copy_xauthority_file = False
                    self.useFixture(fake_home_fixture)
                    self.assertEqual(
                        fake_home, os.environ.get('HOME'))
            return TestWithFakeHome('test_it')

        inner_test().run(result)

        self.assertTrue(
            result.wasSuccessful(),
            'Failed to fake the home: {}'.format(result.failures))
        self.assertEqual(original_home, os.environ.get('HOME'))

    def test_fake_home_fixture_must_create_default_directory(self):
        original_home = os.environ.get('HOME')
        self.useFixture(fixture_setup.FakeHome())

        home_parent_directory, _ = os.path.split(os.environ.get('HOME'))
        self.assertEqual(
            home_parent_directory,
            os.path.join(original_home, 'autopilot', 'fakeenv'))

    def test_fake_home_fixture_must_copy_xauthority(self):
        # Fake the home first so we don't write the xauthority on the real
        # home.
        self.useFixture(fixture_setup.FakeHome())

        open(os.path.join(os.environ.get('HOME'), '.Xauthority'), 'w').close()

        self.useFixture(fixture_setup.FakeHome())
        self.assertTrue(
            os.path.exists(
                os.path.join(os.environ.get('HOME'), '.Xauthority')))


class HideUnity7LauncherTestCase(
        tests.UbuntuUIToolkitWithFakeAppRunningTestCase):

    def setUp(self):
        if platform.model() != 'Desktop':
            self.skipTest('Unity 7 runs only on desktop.')
        self.useFixture(fixture_setup.HideUnity7Launcher())
        super().setUp()

    def test_maximized_application_must_use_all_screen_width(self):
        application = self.process_manager.get_running_applications()[0]
        window = application.get_windows()[0]

        # Maximize window.
        screen = display.Display.create()
        screen_width = screen.get_screen_width()
        screen_height = screen.get_screen_height()
        window.resize(screen_width, screen_height)

        def get_window_width():
            _, _, window_width, _ = window.geometry
            return window_width

        self.assertThat(
            get_window_width,
            Eventually(Equals(screen_width)))


class FakeDisplay(object):

    """Fake display with fixed widht and height for use in tests."""

    def __init__(self, width, height):
        super().__init__()
        self.width = width
        self.height = height

    def get_screen_width(self):
        return self.width

    def get_screen_height(self):
        return self.height


class SimulateDeviceTestCase(autopilot_testcase.AutopilotTestCase):

    scenarios = [
        ('Device equal to screen', {
            'device_width': 100, 'device_height': 100, 'device_grid_unit': 20,
            'screen_width': 100, 'screen_height': 100,
            'expected_width': 100, 'expected_height': 100,
            'expected_grid_unit': 20}),
        ('Device smaller than screen', {
            'device_width': 100, 'device_height': 90, 'device_grid_unit': 20,
            'screen_width': 110, 'screen_height': 100,
            'expected_width': 100, 'expected_height': 90,
            'expected_grid_unit': 20}),
        ('Device wider than screen', {
            'device_width': 200, 'device_height': 90, 'device_grid_unit': 20,
            'screen_width': 110, 'screen_height': 100,
            'expected_width': 100, 'expected_height': 45,
            'expected_grid_unit': 10}),
        ('Device taller than screen', {
            'device_width': 100, 'device_height': 180, 'device_grid_unit': 20,
            'screen_width': 110, 'screen_height': 100,
            'expected_width': 50, 'expected_height': 90,
            'expected_grid_unit': 10}),
        ('Device bigger than screen', {
            'device_width': 200, 'device_height': 180, 'device_grid_unit': 20,
            'screen_width': 110, 'screen_height': 100,
            'expected_width': 100, 'expected_height': 90,
            'expected_grid_unit': 10}),
    ]

    def test_simulate_device_fixture_with_size_smaller_than_screen(self):
        """Test the simulation of a device that fits the screen.

        All the attributes of the fixture must remain the same.

        """

        simulate_device_fixture = fixture_setup.SimulateDevice(
            self.device_width, self.device_height, self.device_grid_unit)
        fake_display = FakeDisplay(self.screen_width, self.screen_height)
        simulate_device_fixture._screen = fake_display

        self.useFixture(simulate_device_fixture)

        self.assertEqual(
            int(os.environ.get('GRID_UNIT_PX')), self.expected_grid_unit)
        self.assertEqual(
            simulate_device_fixture.app_width, self.expected_width)
        self.assertEqual(
            simulate_device_fixture.app_height, self.expected_height)
