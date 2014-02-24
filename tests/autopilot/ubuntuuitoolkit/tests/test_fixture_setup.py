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

import os

try:
 # Python 3.
 from unittest import mock
except ImportError:
 # Python 2 add-on: python-mock.
 import mock
import testtools
from autopilot import testcase as autopilot_testcase
from testtools.matchers import Contains, Not, FileExists

from ubuntuuitoolkit import base, fixture_setup


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

    def test_desktop_file_with_qmlscene_launch_command(self):
        test_desktop_file_dict = {'Exec': '{qmlscene} application'}

        qmlscene = 'ubuntuuitoolkit.base.get_qmlscene_launch_command'
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
        qmlscene = 'ubuntuuitoolkit.base.get_qmlscene_launch_command'
        with mock.patch(qmlscene) as mock_qmlscene:
            mock_qmlscene.return_value = 'qmlscene'
            fake_application = fixture_setup.FakeApplication()
            self.useFixture(fake_application)

        with open(fake_application.desktop_file_path, 'r+t') as desktop_file:
            desktop_file_contents = desktop_file.read()

        expected_desktop_file_dict = {
            'Type': 'Application',
            'Name': 'test',
            'Icon': 'Not important',
            'Exec': 'qmlscene {}'.format(fake_application.qml_file_path),
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
        fake_application = fixture_setup.FakeApplication()

        def inner_test():
            class TestWithFakeApplication(testtools.TestCase):
                def test_it(self):
                    self.useFixture(fake_application)
            return TestWithFakeApplication('test_it')

        inner_test().run()
        self.assertThat(fake_application.qml_file_path, Not(FileExists()))
        self.assertThat(fake_application.desktop_file_path, Not(FileExists()))


class LaunchFakeApplicationTestCase(autopilot_testcase.AutopilotTestCase):

    def test_launch_fake_application_with_qmlscene(self):
        fake_application = fixture_setup.FakeApplication()
        self.useFixture(fake_application)

        self.application = self.launch_test_application(
            base.get_qmlscene_launch_command(),
            fake_application.qml_file_path,
            '--desktop_file_hint={0}'.format(
                fake_application.desktop_file_path),
            app_type='qt')

        # We can select a component from the application.
        self.application.select_single('Label', objectName='testLabel')
