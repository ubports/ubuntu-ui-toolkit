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
import time

from autopilot import platform

import ubuntuuitoolkit
from ubuntuuitoolkit import tests


class PullToRefreshTestCase(tests.QMLFileAppTestCase):

    path = os.path.abspath(__file__)
    dir_path = os.path.dirname(path)
    test_qml_file_path = os.path.join(dir_path, 'test_pull_to_refresh.qml')

    def setUp(self):
        super().setUp()
        self.label = self.main_view.select_single(
            'Label', objectName='refreshedLabel')
        self.assertEqual(self.label.text, 'Not refreshed.')
        self.flickable_with_pull_to_refresh = self.main_view.select_single(
            ubuntuuitoolkit.QQuickFlickable,
            objectName='flickableWithPullToRefresh')

    def test_pull_to_refresh_must_refresh_model(self):
        # TODO disabled due bug #1372446
        self.skipTest(
            'Try to reproduce consistently, bug logged in #1372446')
        self.flickable_with_pull_to_refresh.pull_to_refresh()

        self.assertEqual(self.label.text, 'Refreshed.')

    def test_cancel_pull_to_refresh_must_not_refresh_model(self):
        if platform.model() != 'Desktop':
            # TODO remove the skip once bug http://pad.lv/1266601 is fixed.
            self.skipTest(
                'Autopilot is not yet able to do a drag on the devices '
                'without releasing the simulated finger.')
        self.flickable_with_pull_to_refresh._cancel_pull_to_refresh()
        # Sleep for some time to make sure that the list is not being
        # refreshed.
        time.sleep(3)
        self.assertEqual(self.label.text, 'Not refreshed.')
