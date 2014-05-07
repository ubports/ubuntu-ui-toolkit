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

from autopilot import platform

from ubuntuuitoolkit.tests import gallery


class ScrollBarTestCase(gallery.GalleryTestCase):

    def setUp(self):
        super(ScrollBarTestCase, self).setUp()
        item = 'Navigation'
        self.loadItem(item)
        self.checkPageHeader(item)
        self.scrollbar = self.main_view.select_single(
            'Scrollbar', objectName="TemplateScrollbar")

    def move_mouse_to_thumb(self):
        # TODO we need a helper to move the interactive thumb.
        # --elopio 2014-05-06
        thumb = self.scrollbar.select_single(
            objectName='interactiveScrollbarThumb')
        self.pointing_device.move_to_object(thumb)

    def test_scrollbar_must_start_without_interactive_thumb(self):
        self.assertEqual(self.scrollbar.interactive, False)

    def test_move_mouse_to_thumb_make_scrollbar_interactive(self):
        if platform.model() != 'Desktop':
            self.skipTest(
                'The interactive thumb is activated only by the move of a '
                'mouse')

        self.move_mouse_to_thumb()

        self.assertEqual(self.scrollbar.interactive, True)

    def test_drag_thumb_down_must_make_bottom_visible(self):
        bottom_section = self.main_view.select_single(className='PageStack')
        flickable = self.main_view.select_single(
            'QQuickFlickable', objectName='TemplateFlickable')
        self.assertEqual(flickable.is_child_visible(bottom_section), False)

        self.move_mouse_to_thumb()
        x, y = self.pointing_device.position()
        self.pointing_device.drag(x, y, x, self.main_view.height)

        self.assertEqual(flickable.is_child_visible(bottom_section), True)
