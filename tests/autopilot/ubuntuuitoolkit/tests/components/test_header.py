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

"""Tests for the Ubuntu UI Toolkit Header component."""

import os

from autopilot.matchers import Eventually
from testtools.matchers import Equals

from ubuntuuitoolkit import tests


class HeaderTestCase(tests.QMLFileAppTestCase):

    path = os.path.abspath(__file__)
    dir_path = os.path.dirname(path)
    test_qml_file_path = os.path.join(
        dir_path, 'test_header.HeaderTestCase.qml')

    def setUp(self):
        super(HeaderTestCase, self).setUp()
        self.header = self.main_view.get_header()

    def test_click_header_overflow_action_must_close_popover(self):
        """Test that clicking an action from the popover must close it.

        Regression test for http://pad.lv/1326963

        """
        overflow_popover = self.main_view.select_single(
            'Popover',
            objectName='actions_overflow_popover')
        self.header.click_action_button('pushStackAction')
        self.assertThat(overflow_popover.visible, Eventually(Equals(False)))
