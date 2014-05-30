# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2012, 2013, 2014 Canonical Ltd.
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

"""Tests for the Ubuntu UI Toolkit Gallery"""

import os
import shutil

import ubuntuuitoolkit
from ubuntuuitoolkit import tests


class GalleryTestCase(tests.QMLFileAppTestCase):
    """Base class for gallery test cases."""

    local_desktop_file_path = None

    def setUp(self):
        if self._application_source_exists():
            self.test_source_path = self._get_path_to_gallery_source()
        else:
            self.test_source_path = self._get_path_to_installed_gallery()
        assert os.path.exists(self.test_source_path)
        self.test_qml_file_path = self._get_test_qml_file_path()
        self.desktop_file_path = self._get_desktop_file_path()
        super(GalleryTestCase, self).setUp()

    def _get_path_to_gallery_source(self):
        return os.path.join(
            tests.get_path_to_source_root(), 'examples',
            'ubuntu-ui-toolkit-gallery')

    def _application_source_exists(self):
        return 'UBUNTU_UI_TOOLKIT_AUTOPILOT_FROM_SOURCE' in os.environ

    def _get_test_qml_file_path(self):
        return os.path.join(
            self.test_source_path,
            'ubuntu-ui-toolkit-gallery.qml')

    def _get_path_to_installed_gallery(self):
        return '/usr/lib/ubuntu-ui-toolkit/examples/ubuntu-ui-toolkit-gallery'

    def _get_desktop_file_path(self):
        desktop_file_path = os.path.join(
            self.test_source_path,
            'ubuntu-ui-toolkit-gallery.desktop')
        if self._application_source_exists():
            local_desktop_file_dir = tests.get_local_desktop_file_directory()
            if not os.path.exists(local_desktop_file_dir):
                os.makedirs(local_desktop_file_dir)
            local_desktop_file_path = os.path.join(
                local_desktop_file_dir, 'ubuntu-ui-toolkit-gallery.desktop')
            shutil.copy(desktop_file_path, local_desktop_file_path)
            # We can't delete the desktop file before we close the application,
            # so we save it on an attribute to be deleted on tear down.
            self.local_desktop_file_path = local_desktop_file_path
            return local_desktop_file_path
        else:
            return desktop_file_path

    def open_page(self, page):
        """Open a page of the widget gallery.

        :param page: The objectName of the element in the index list that opens
            the page.

        """
        list_view = self.main_view.select_single(
            ubuntuuitoolkit.QQuickListView, objectName="widgetList")
        list_view.click_element(page)
        element = self.main_view.select_single('Standard', objectName=page)
        element.selected.wait_for(True)

    def tearDown(self):
        super(GalleryTestCase, self).tearDown()
        # We can't delete the desktop file before we close the application,
        # so we save it on an attribute to be deleted on tear down.
        if self.local_desktop_file_path is not None:
            os.remove(self.local_desktop_file_path)
