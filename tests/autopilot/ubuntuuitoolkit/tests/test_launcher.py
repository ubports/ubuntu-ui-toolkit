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
from testtools.matchers import Equals
from autopilot.matchers import Eventually
from ubuntuuitoolkit import tests


class LauncherTouchTestCase(tests.QMLFileAppTestCase):
    path = os.path.abspath(__file__)
    dir_path = os.path.dirname(path)
    test_qml_file_path = os.path.join(
        dir_path, 'test_launcher.touch.qml')

    def get_command_line(self, command_line):
        command_line.append('-touch')
        return command_line

    def test_apparent_touch_screen(self):
        textfield = self.main_view.select_single(objectName="textarea")
        # Trigger handlers as a courtesy to a watching person
        self.pointing_device.click_object(textfield)
        self.assertThat(textfield.text, Eventually(Equals("Yes")))


class LauncherQtTestTestCase(tests.QMLFileAppTestCase):
    path = os.path.abspath(__file__)
    dir_path = os.path.dirname(path)
    test_qml_file_path = os.path.join(
        dir_path, 'test_launcher.testcase.qml')

    def test_can_run_qt_test_case(self):
        textfield = self.main_view.select_single(objectName="textarea")
        self.assertThat(textfield.text,
                        Eventually(Equals("Lorem ipsum dolor sit amet")))
