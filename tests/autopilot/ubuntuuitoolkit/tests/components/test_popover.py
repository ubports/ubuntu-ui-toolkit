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

"""Tests for the Ubuntu UI Toolkit Header component."""

import os

from ubuntuuitoolkit import (
    QQuickFlickable,
    tests
)


class DialogScrollTestCase(tests.QMLFileAppTestCase):

    path = os.path.abspath(__file__)
    dir_path = os.path.dirname(path)
    test_qml_file_path = os.path.join(
        dir_path, 'test_popover.qml')

    def test_dialog_small(self):
        button = self.main_view.select_single(objectName='button_small')
        self.pointing_device.click_object(button)
        dialog = self.main_view.select_single(objectName='dialog_small')
        flickable = dialog.select_single('QQuickFlickable')
        # There's nothing to reveal
        self.assertTrue(flickable.atYEnd)

    def test_dialog_scrollable(self):
        button = self.main_view.select_single(objectName='button_huge')
        self.pointing_device.click_object(button)
        dialog = self.main_view.select_single(objectName='dialog_huge')
        flickable = dialog.select_single(QQuickFlickable)
        buttlet = dialog.select_single(objectName='buttlet49')
        # We can scroll
        self.assertFalse(flickable.atYEnd)
        flickable.swipe_child_into_view(buttlet)
        self.assertEqual(flickable.is_child_visible(buttlet), True)
