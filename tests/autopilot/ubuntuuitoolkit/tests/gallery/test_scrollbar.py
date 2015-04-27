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

from autopilot import platform

from ubuntuuitoolkit import ubuntu_scenarios
from ubuntuuitoolkit.tests import gallery


class ScrollBarTestCase(gallery.GalleryTestCase):

    scenarios = ubuntu_scenarios.get_device_simulation_scenarios()

    def setUp(self):
        super().setUp()
        self.open_page('navigationElement')
        self.scrollbar = self.main_view.wait_select_single(
            'Scrollbar', objectName="TemplateScrollbar")

    def move_mouse_to_thumb(self):
        # TODO we need a helper to move the interactive thumb.
        # --elopio 2014-05-06
        thumb = self.get_mouse_thumb()
        self.pointing_device.move_to_object(thumb)

    def get_mouse_thumb(self):
        return self.scrollbar.select_single(
            objectName='interactiveScrollbarThumb')

    def test_scrollbar_must_start_without_interactive_thumb(self):
        self.assertEqual(self.scrollbar.interactive, False)

    def test_move_mouse_to_thumb_must_make_scrollbar_interactive(self):
        if platform.model() != 'Desktop':
            self.skipTest(
                'The interactive thumb is activated by the move of a mouse')

        self.move_mouse_to_thumb()

        self.assertEqual(self.scrollbar.interactive, True)

    def test_drag_thumb_down_must_make_bottom_visible(self):
        if platform.model() != 'Desktop':
            self.skipTest(
                'The interactive thumb is activated by the move of a mouse')

        bottom_section = self.main_view.select_single(className='PageStack')
        flickable = self.main_view.select_single(
            'QQuickFlickable', objectName='TemplateFlickable')
        self.assertEqual(flickable.is_child_visible(bottom_section), False)

        self.move_mouse_to_thumb()
        mouse_thumb = self.get_mouse_thumb()
        x, y, width, height = mouse_thumb.globalRect
        start_x = stop_x = x + (width // 2)
        start_y = y + (height // 0.8)
        stop_y = self.main_view.y + self.main_view.height

        self.pointing_device.drag(start_x, start_y, stop_x, stop_y)

        self.assertEqual(flickable.is_child_visible(bottom_section), True)
